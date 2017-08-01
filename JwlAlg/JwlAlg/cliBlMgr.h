//
//  cliBlMgr.hpp
//  JwlAlg
//
//  Created by 李卓 on 21/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#ifndef cliBlMgr_h
#define cliBlMgr_h

#include <stdio.h>
#include <json.hpp>
#include <zmqClient.h>
#include <string>
#include "Singleton.h"
#include "blMacro.h"

using namespace std;

class cliBlMgr
{
private:
    nlohmann::json m_jsMsg;
    
    nlohmann::json m_jsRet;
    
    zmqClient *m_pZmqClient;
    
public:
    cliBlMgr(zmqClient *pZmqClient) : m_pZmqClient(pZmqClient) {}
    
    cliBlMgr() {m_pZmqClient = Singleton<zmqClient>::GetInstance();}
    
    void parseMsg(string msg);
    
    nlohmann::json getRet();
    
    string getStrRet();
    
//business logic
public:
    
    void requestConnect();
    
    void requestGameMatch();
    
    void requestLogout();
    
    void requestBoardcast2Replicas(string GameID, string Msg);
    
    void requestGameSetting(string GameID);
    
//business logic for process reply message
protected:
    
    void replyGameMacth();
    
    void replyGameSetting();
    
    void replyBoardcastPlayerOperation();
    
};


#endif /* cliBlMgr_h */
