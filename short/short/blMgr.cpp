//
//  blMgr.cpp
//  short
//
//  Created by 李卓 on 19/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#include "blMgr.h"
#include "blMacro.h"
#include "onlineUser.h"
#include "Singleton.h"
#include "commonFunc.h"

void blMgr::parseMsg(string msg)
{
    m_jsMsg = nlohmann::json::parse(msg);
    m_jsRet = m_jsMsg;
    
    switch (m_jsMsg[REQUEST_TYPE].get<int>())
    {
        case USER_CONNECT:
            userConnect();
            break;
            
        case USER_LOGOUT:
            userLogout();
            break;
            
        case USER_FIND_GAME_MATCH:
            findGameMatch();
            break;
            
        case USER_GAME_SETUP:
            userSubSockWaiting();
            break;
            
        case BOARDCAST_MSG_INGAME:
            boardcastPlayerOperation();
            break;
            
    }
}

nlohmann::json blMgr::getRet()
{
    return m_jsRet;
}

string blMgr::getStrRet()
{
    return m_jsRet.dump();
}

int blMgr::userConnect()
{
    user userInfo;
    onlineUser *pOnlineUser = Singleton<onlineUser>::GetInstance();
    userInfo.strReqSockID = m_jsMsg[USER_SOCKET_UUID];
    pOnlineUser->addOnlineUser(userInfo);
    
    m_jsRet[REQUEST_ACKNOWLEDGE] = ACKNOWLEDGE_SUCESSED;
    
    m_pZmqSvr->sendMsg(m_jsMsg[USER_SOCKET_UUID], m_jsRet.dump());
    
    return 0;
}

int blMgr::userLogout()
{
    onlineUser *pOnlineUser = Singleton<onlineUser>::GetInstance();
    pOnlineUser->removeUser(m_jsMsg[USER_SOCKET_UUID].get<string>());
    m_jsRet[REQUEST_ACKNOWLEDGE] = ACKNOWLEDGE_SUCESSED;
    
    m_pZmqSvr->sendMsg(m_jsMsg[USER_SOCKET_UUID], m_jsRet.dump());
    
    return 0;
}

int blMgr::findGameMatch()
{
    onlineUser *pOnlineUser = Singleton<onlineUser>::GetInstance();
    vector<user> matchUser = pOnlineUser->findGameMatch(m_jsMsg[USER_SOCKET_UUID]);
    
    m_jsRet[REQUEST_ACKNOWLEDGE] = ACKNOWLEDGE_SUCESSED;
    
    if (matchUser.size() == 0)
    {
        m_jsRet[GAME_MATCH_STATE] = GAME_MATCH_WAIT;
        m_pZmqSvr->sendMsg(m_jsRet[USER_SOCKET_UUID], m_jsRet.dump());
    }
    else if (matchUser.size() == 2)
    {
        m_jsRet[GAME_MATCH_STATE] = GAME_MATCH_SUCESSED;
        m_jsRet[GAME_UUID] = generateUUID();
        pOnlineUser->insertMatchedUser(m_jsRet[GAME_UUID].get<string>(),
                                       matchUser[0].strReqSockID, matchUser[1].strReqSockID);
        for (user val : matchUser)
        {
            m_jsRet[USER_SOCKET_UUID] = val.strReqSockID;
            m_pZmqSvr->sendMsg(m_jsRet[USER_SOCKET_UUID], m_jsRet.dump());
        }
    }
    
    return 0;
}

int blMgr::userSubSockWaiting()
{
    onlineUser *pOnlineUser = Singleton<onlineUser>::GetInstance();
    if (pOnlineUser->IsMatchUserReady(m_jsMsg[GAME_UUID]))
    {
        m_jsRet[GAME_MATCH_STATE] = GAME_SUBSCRIBER_READY;
        auto pairUser = pOnlineUser->getMathedUser(m_jsMsg[GAME_UUID]);
        
        m_jsRet[USER_SOCKET_UUID] = pairUser.first.strReqSockID;
        m_jsRet[OTHER_PLAYER_ID] = pairUser.second.strReqSockID;
        m_pZmqSvr->sendMsg(pairUser.first.strReqSockID, m_jsRet.dump());
        
        m_jsRet[USER_SOCKET_UUID] = pairUser.second.strReqSockID;
        m_jsRet[OTHER_PLAYER_ID] = pairUser.first.strReqSockID;
        m_pZmqSvr->sendMsg(pairUser.second.strReqSockID, m_jsRet.dump());
    }
    else
    {
        pOnlineUser->setOneMatchUserReady(m_jsMsg[GAME_UUID], m_jsMsg[USER_SOCKET_UUID]);
        m_jsRet[GAME_MATCH_STATE] = GAME_SUBSCRIBER_WAITING;
        m_pZmqSvr->publisherSendMsg(m_jsMsg[GAME_UUID].get<string>(), m_jsRet.dump());
    }
    
    return 0;
}

int blMgr::boardcastPlayerOperation()
{
    m_pZmqSvr->publisherSendMsg(m_jsRet[GAME_UUID], m_jsRet.dump());
    return 0;
}


