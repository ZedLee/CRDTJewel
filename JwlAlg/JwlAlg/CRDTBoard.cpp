//
//  CRDTBoard.cpp
//  JwlAlg
//
//  Created by 李卓 on 12/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#include "CRDTBoard.h"
#include "commonFunc.h"
#include "Singleton.h"
#include "json.hpp"
#include "blMacro.h"
#include "cliBlMgr.h"

CRDTBoard::CRDTBoard()
{
    m_bGameSettingUp = false;
    m_pZmqCli = Singleton<zmqClient>::GetInstance();
    m_strID = m_pZmqCli->getSockID();
    //m_Board.init();
}


void CRDTBoard::initVectorClock(vector<string> replicasID)
{
    if (replicasID.size() == 0) return;
    for (auto val : replicasID)
    {
        m_mapVectorClock[val] = 0;
    }
}

void CRDTBoard::operationMerge(PlayerMove move, map<string, int> vc, string srcID)
{
    //becaues zeroMQ pub/sub architecture, drop msg if this replica is source replica
    if (0 == srcID.compare(m_strID))
    {
        return;
    }
    
    unique_lock<mutex> lck(m_mutexOperation);
    
    //set CRDT properties
    m_mapVectorClock[srcID]++;
    
    m_mapHistoricalMove[make_pair(srcID, vc[srcID])] = move;
    
    //detect conflict by vector clock
    if (vc[srcID] > m_mapVectorClock[srcID] && vc[m_strID] < m_mapVectorClock[m_strID])
    {
        //conflict may exist
        
        //historical state already deleted, means this state result in conflict
        //in the last moves, and conflict already resolved
        // so, drop this message
        if (m_mapHistoricalState.end() == m_mapHistoricalState.find(vc[m_strID]))
        {
            return;
        }
        
        PlayerMove pmRet;
        bool bConflict = dectectConflict(m_mapHistoricalState[vc[m_strID]],
                        m_mapHistoricalMove[make_pair(m_strID, vc[m_strID])], move, pmRet);
        
        if (bConflict == true)
        {
            m_Board.setBoardState(m_mapHistoricalState[vc[m_strID]]);
            m_Board.validateSwap(pmRet.p, pmRet.d);
        }
    }
    //no conflict
    else
    {
        m_Board.validateSwap(move.p, move.d);
        
        //garbage collection mechanism
        //delete redundent state in m_mapHistoricalState
        map<int, vector<vector<int>>>::iterator itFind = m_mapHistoricalState.find(vc[m_strID]);
        if (itFind != m_mapHistoricalState.end())
        {
            m_mapHistoricalState.erase(itFind);
        }
    }
    
    cout<<m_Board<<endl;
}

void CRDTBoard::setFlagSettingUp(bool bFlag)
{
    unique_lock<mutex> lck(m_mutexSetUp);
    m_bGameSettingUp = bFlag;
}

bool CRDTBoard::getFlagSettingUp()
{
    unique_lock<mutex> lck(m_mutexSetUp);
    return m_bGameSettingUp;
}

void CRDTBoard::gameRequest()
{
    cliBlMgr cbm;
    cbm.requestGameMatch();
}


void CRDTBoard::gameStart(vector<string> replicasID)
{
    initVectorClock(replicasID);
    //generate random seed by first 4 letter of game ID
    const char *szGameID = m_strGameID.c_str();
    unsigned int uiRandomSeed = 0;
    for (int i = 0; i < 4; i++)
    {
        uiRandomSeed += szGameID[i];
    }
    m_Board.setRandomSeed(uiRandomSeed);
    m_Board.init();
    
    //a new thread for simple interface
    std::thread gameThread(std::bind(&CRDTBoard::consoleGameInterface,this));
    gameThread.join();
    
}

void CRDTBoard::gameOver()
{
    m_mapVectorClock.clear();
    setGameID("");
    setFlagSettingUp(false);
    m_mapHistoricalMove.clear();
    m_mapHistoricalState.clear();
    
}

bool CRDTBoard::dectectConflict(vector<vector<int>> state, PlayerMove m1, PlayerMove m2, PlayerMove& mRet)
{
    if (m1 == m2) return false;
    Board l_boardMove1, l_boardMove2;
    l_boardMove1.setBoardState(state);
    l_boardMove1.validateSwap(m1.p, m1.d);
    
    l_boardMove2.setBoardState(state);
    l_boardMove2.validateSwap(m2.p, m2.d);
    
    for (auto valBoard1 : l_boardMove1.m_vecLastMatch)
    {
        for (auto valBoard2 : l_boardMove2.m_vecLastMatch)
        {
            if (valBoard1.x == valBoard2.x)
            {
                mRet = l_boardMove1.m_lScore >= l_boardMove1.m_lScore ? m1 : m2;
                return true;
            }
        }
    }
    return false;
}

int CRDTBoard::boardcastOperation(PlayerMove move)
{
    nlohmann::json jsMsg;
    jsMsg[REQUEST_TYPE] = BOARDCAST_MSG_INGAME;
    jsMsg[GAME_UUID] = getGameID();
    jsMsg[USER_SOCKET_UUID] = m_strID;
    
    nlohmann::json jsContent;
    jsContent[GAME_MOVING_POINT] = to_string(move.p.x) + "," + to_string(move.p.y);
    jsContent[GAME_MOVING_DIRECTION] = move.d;
    jsContent[GAME_VECTOR_CLOCK] = m_mapVectorClock;
    jsContent[USER_SOCKET_UUID] = m_strID;
    
    jsMsg[MASSAGE_CONTENT] = jsContent;
    
    //m_pZmqCli->sentReq(jsMsg.dump());
    m_pZmqCli->sentReqUnsafe(jsMsg.dump());
    return 0;
}

void CRDTBoard::setGameID(string gid)
{
    unique_lock<mutex> lck(m_mutexGameID);
    m_strGameID = gid;
}

string CRDTBoard::getGameID()
{
    unique_lock<mutex> lck(m_mutexGameID);
    return m_strGameID;
}

bool CRDTBoard::validateSwap(PlayerMove move)
{
    unique_lock<mutex> lck(m_mutexOperation);
    //set CRDT properties
    m_mapVectorClock[m_strID]++;
    m_mapHistoricalMove[make_pair(m_strID, m_mapVectorClock[m_strID])] = move;
    m_mapHistoricalState[m_mapVectorClock[m_strID] - 1] = m_Board.m_VecBoard;
    
    if (m_Board.validateSwap(move.p, move.d))
    {
        //if move effective, boardcast to other replicas
        boardcastOperation(move);
    }
    
    return true;
}

void CRDTBoard::consoleGameInterface()
{
    bool bRun = true;
    //Board bT;
    //bT.init();

    while (bRun)
    {
        string input;
        cout<<"input quit for quit this game..."<<endl;
        cout<<"input position and direction, e.g. 2,2,right"<<endl;
        cin>>input;
        
        if (0 == input.compare("quit"))
        {
            cout<<"your scores is "<<m_Board.getScore()<<endl;
            cout<<"quit..."<<endl;
            bRun = false;
            
            gameOver();
            
            cliBlMgr cbm(Singleton<zmqClient>::GetInstance());
            cbm.requestLogout();
            //exit game by user
            exit(-1);
        }
        else
        {
            vector<string> ret;
            Direction d = Direction::UNKNOWN;
            split(input, ret, ",");
            if (ret.size() != 3)
            {
                continue;
            }
            if (0 == ret[2].compare("left"))
            {
                d = Direction::LEFT;
            }
            else if (0 == ret[2].compare("right"))
            {
                d = Direction::RIGHT;
            }
            else if (0 == ret[2].compare("down"))
            {
                d = Direction::DOWN;
            }
            else if (0 == ret[2].compare("up"))
            {
                d = Direction::UP;
            }
            PlayerMove move(MyPoint(atoi(ret[0].c_str()), atoi(ret[1].c_str())), d);
            this->validateSwap(move);
            cout<<m_Board;
        }
    }
}
