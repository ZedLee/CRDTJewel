//
//  cliBlMgr.cpp
//  JwlAlg
//
//  Created by 李卓 on 21/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#include "cliBlMgr.h"
#include "CRDTBoard.h"
#include "Singleton.h"
#include "commonFunc.h"
#include "log4z.h"

void cliBlMgr::parseMsg(string msg)
{
    m_jsMsg = nlohmann::json::parse(msg);
    //m_jsRet = m_jsMsg;
    
    switch (m_jsMsg[REQUEST_TYPE].get<int>())
    {
        case USER_CONNECT:
            break;
            
        case USER_LOGOUT:
            break;
            
        case USER_FIND_GAME_MATCH:
            replyGameMacth();
            break;
            
        case USER_GAME_SETUP:
            replyGameSetting();
            break;
        case BOARDCAST_MSG_INGAME:
            replyBoardcastPlayerOperation();
            break;
            
    }
}

nlohmann::json cliBlMgr::getRet()
{
    return m_jsRet;
}

string cliBlMgr::getStrRet()
{
    return m_jsRet.dump();
}

void cliBlMgr::requestConnect()
{
    m_jsRet[REQUEST_TYPE] = USER_CONNECT;
    m_jsRet[USER_SOCKET_UUID] = m_pZmqClient->getSockID();
    m_pZmqClient->sentReq(m_jsRet.dump());
}

void cliBlMgr::requestGameMatch()
{
    m_jsRet[USER_SOCKET_UUID] = m_pZmqClient->getSockID();
    m_jsRet[REQUEST_TYPE] = USER_FIND_GAME_MATCH;
    m_pZmqClient->sentReq(m_jsRet.dump());
}

void cliBlMgr::requestLogout()
{
    m_jsRet[USER_SOCKET_UUID] = m_pZmqClient->getSockID();
    m_jsRet[REQUEST_TYPE] = USER_LOGOUT;
    m_pZmqClient->sentReq(m_jsRet.dump());
}

void cliBlMgr::requestBoardcast2Replicas(string GameID, string Msg)
{
    m_jsRet[USER_SOCKET_UUID] = m_pZmqClient->getSockID();
    m_jsRet[REQUEST_TYPE] = BOARDCAST_MSG_INGAME;
    m_jsRet[GAME_UUID] = GameID;
    m_jsRet[MASSAGE_CONTENT] = Msg;
}

void cliBlMgr::requestGameSetting(string GameID)
{
    //nlohmann::json jsRet;
    m_jsRet[REQUEST_TYPE] = USER_GAME_SETUP;
    m_jsRet[USER_SOCKET_UUID] = m_pZmqClient->getSockID();
    m_jsRet[GAME_UUID] = GameID;
    m_pZmqClient->sentReq(m_jsRet.dump());

}

void cliBlMgr::replyGameMacth()
{
    if (m_jsMsg[GAME_MATCH_STATE].get<int>() == GAME_MATCH_SUCESSED)
    {
        m_pZmqClient->createSubConnect(m_jsMsg[GAME_UUID]);
        
    }
}

void cliBlMgr::replyGameSetting()
{
    
    if (m_jsMsg[GAME_MATCH_STATE].get<int>() == GAME_SUBSCRIBER_READY)
    {
        CRDTBoard *pCB = Singleton<CRDTBoard>::GetInstance();
        if (!pCB->getGameID().empty())
        {
            LOGI("游戏已经开始");
            return;
        }
        LOGI("成功进入游戏。。。");
        vector<string> replicaID;
        replicaID.push_back(m_jsMsg[USER_SOCKET_UUID]);
        replicaID.push_back(m_jsMsg[OTHER_PLAYER_ID]);
        
        
        pCB->setGameID(m_jsMsg[GAME_UUID].get<string>());
        pCB->gameStart(replicaID);
    }
    
}

void cliBlMgr::replyBoardcastPlayerOperation()
{
    CRDTBoard *pCB = Singleton<CRDTBoard>::GetInstance();
    nlohmann::json jsContent = m_jsMsg[MASSAGE_CONTENT];
    vector<string> strPos;
    split(jsContent[GAME_MOVING_POINT].get<string>(), strPos);
    PlayerMove move;
    move.p.x = str2int(strPos[0]);
    move.p.y = str2int(strPos[1]);
    move.d = jsContent[GAME_MOVING_DIRECTION].get<Direction>();
    map<string, int> vc = jsContent[GAME_VECTOR_CLOCK].get<map<string, int>>();
    
    
    pCB->operationMerge(move, vc, m_jsMsg[USER_SOCKET_UUID].get<string>());

}
