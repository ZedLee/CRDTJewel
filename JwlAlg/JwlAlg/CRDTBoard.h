//
//  CRDTBoard.hpp
//  JwlAlg
//
//  Created by 李卓 on 12/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#ifndef CRDTBoard_h
#define CRDTBoard_h

#include <stdio.h>
#include "Board.h"
#include "zmqClient.h"
#include <mutex>


class PlayerMove
{
public:
    MyPoint p;
    Direction d;
    PlayerMove(MyPoint point, Direction direction) : p(point), d(direction) {}
    PlayerMove() {}
    bool operator ==(PlayerMove m) { return m.p.x == p.x && m.p.y == p.y && m.d == d ? true : false; }
    PlayerMove operator = (PlayerMove m1) {this->p.x = m1.p.x; this->p.y = m1.p.y; this->d = m1.d; return *this;}
};

//operation based better move wins CRDT game board
class CRDTBoard
{
    //CRDTs 历史状态操作原则
    //每次提交修改前将上一次状态存储于 m_mapHistoricalState
    
private:
    //game board
    Board m_Board;
    
    string m_strID;
    
    //CRDTs properties
    //vectoc clock, key is replica id, value is updates counting
    map<string, int> m_mapVectorClock;
    
    //historical move
    //key is a pair stand for specific move commit by specific replica
    //value stand for the operation comited by replica
    map<pair<string, int>, PlayerMove> m_mapHistoricalMove;
    
    //historical state
    //key is uncomfirmed move, value is game board state before this move
    map<int, vector<vector<int>>> m_mapHistoricalState;
    
    zmqClient* m_pZmqCli;
    
    bool m_bGameSettingUp;
    
    mutex m_mutexSetUp;
    
    string m_strGameID;
    
    mutex m_mutexGameID;
    
    //operation can not merge with post by user simultanously
    mutex m_mutexOperation;
    
public:
    
    CRDTBoard();
    
    void initVectorClock(vector<string> replicasID);
    
    void operationMerge(PlayerMove move, map<string, int> vc, string srcID);
    
    bool validateSwap(PlayerMove);
    
    void setFlagSettingUp(bool bFlag);
    
    bool getFlagSettingUp();
    
    void setGameID(string gid);
    
    string getGameID();
    
    void gameRequest();
    
    void gameStart(vector<string> replicasID);
    
    //未调用
    void gameOver();
    
protected:
    
    //the last argument is better move, if conflict exist
    //if conflict exist, return true, otherwise retuen false
    //only support two player at present
    bool dectectConflict(vector<vector<int>> state, PlayerMove m1, PlayerMove m2, PlayerMove& mRet);
    
    int boardcastOperation(PlayerMove move);
    
    void consoleGameInterface();
    
};

#endif /* CRDTBoard_h */
