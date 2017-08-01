//
//  onlineUser.h
//  short
//
//  Created by 李卓 on 21/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#ifndef onlineUser_h
#define onlineUser_h
#include <map>
#include <string>
#include <mutex>
#include <vector>

using namespace std;

//取消游戏匹配，游戏结束等均未完成
struct user
{
    string strUserName;
    string strReqSockID;
    string strPubSockFilter;
    bool bFindGame = false;
    // if user comfirm to start game, set bGameLock true
    bool bGameLock = false;
    bool bInGame = false;
};

class onlineUser
{
private:
    map<string, user> m_mapOnlineUser;
    
    mutex m_mapOnlineUserMutex;
    
    map<string, pair<user, user>> m_matchedUser;
    
    mutex m_mapMatchedUserMutex;
    
public:
    inline int getOnlineUserSize()
    {
        unique_lock<mutex> lock(m_mapOnlineUserMutex);
        return (int) m_mapOnlineUser.size();
    }
    
    inline void addOnlineUser(user userInfo)
    {
        unique_lock<mutex> lock(m_mapOnlineUserMutex);
        if (m_mapOnlineUser.find(userInfo.strReqSockID) == m_mapOnlineUser.end())
        {
            m_mapOnlineUser[userInfo.strReqSockID] = userInfo;
        }
    }
    
    inline void removeUser(user userInfo)
    {
        unique_lock<mutex> lock(m_mapOnlineUserMutex);
        if (m_mapOnlineUser.find(userInfo.strReqSockID) != m_mapOnlineUser.end())
        {
            m_mapOnlineUser.erase(userInfo.strReqSockID);
        }
    }
    
    inline void removeUser(string userID)
    {
        unique_lock<mutex> lock(m_mapOnlineUserMutex);
        if (m_mapOnlineUser.find(userID) != m_mapOnlineUser.end())
        {
            m_mapOnlineUser.erase(userID);
        }
    }
    
    inline bool getUserInfo(string onlineID, user &userInfo)
    {
        unique_lock<mutex> lock(m_mapOnlineUserMutex);
        if (m_mapOnlineUser.find(onlineID) != m_mapOnlineUser.end())
        {
            userInfo = m_mapOnlineUser[onlineID];
            return true;
        }
        
        return false;
    }
    
    inline vector<user> findGameMatch(string user1ID = "")
    {
        vector<user> vecRet;
        vecRet.clear();
        unique_lock<mutex> lock(m_mapOnlineUserMutex);
        if (!user1ID.empty())
        {
            m_mapOnlineUser[user1ID].bFindGame = true;
            vecRet.push_back(m_mapOnlineUser[user1ID]);
        }
        for (auto user : m_mapOnlineUser)
        {
            if (user.second.strReqSockID.compare(user1ID) == 0)
            {
                continue;
            }
            if (user.second.bFindGame == true && user.second.bGameLock == false)
            {
                vecRet.push_back(user.second);
            }
            if (vecRet.size() == 2)
            {
                break;
            }
        }
        
        if (vecRet.size() < 2)
        {
            vecRet.clear();
        }
        else
        {
            m_mapOnlineUser[vecRet[0].strReqSockID].bGameLock = true;
            m_mapOnlineUser[vecRet[1].strReqSockID].bGameLock = true;
        }
        
        return vecRet;
    }
    
    inline void insertMatchedUser(string GameID, string UserID1, string UserID2)
    {
        user u1, u2;
        {
            unique_lock<mutex> lock(m_mapOnlineUserMutex);
            u1 = m_mapOnlineUser[UserID1];
            u2 = m_mapOnlineUser[UserID2];
        }
        {
            unique_lock<mutex> lock(m_mapMatchedUserMutex);
            m_matchedUser[GameID] = make_pair(u1, u2);
        }
    }
    
    inline bool IsMatchUserReady(string GameID)
    {
        unique_lock<mutex> lock(m_mapMatchedUserMutex);
        auto pairRet = m_matchedUser[GameID];
        return pairRet.first.bInGame && pairRet.second.bInGame;
    }
    
    inline void setOneMatchUserReady(string GameID, string UserID)
    {
        {
            unique_lock<mutex> lock(m_mapMatchedUserMutex);
            auto pairRet = m_matchedUser[GameID];
            if (pairRet.first.strReqSockID.compare(UserID) == 0)
            {
                m_matchedUser[GameID].first.bInGame = true;
            }
            else if (pairRet.second.strReqSockID.compare(UserID) == 0)
            {
                m_matchedUser[GameID].second.bInGame = true;
            }
        }
        
        {
            unique_lock<mutex> lck(m_mapOnlineUserMutex);
            m_mapOnlineUser[UserID].bInGame = true;
        }
    }
    
    inline pair<user, user> getMathedUser(string GameID)
    {
        unique_lock<mutex> lock(m_mapMatchedUserMutex);
        return m_matchedUser[GameID];
    }
    
    inline void cancelGameMatch(string userID)
    {
        unique_lock<mutex> lock(m_mapOnlineUserMutex);
        m_mapOnlineUser[userID].bFindGame = false;
    }
    
    inline void gameOver(string GameID, string UserID1, string UserID2)
    {
        {
            unique_lock<mutex> lock(m_mapOnlineUserMutex);
            m_mapOnlineUser[UserID1].bFindGame = false;
            m_mapOnlineUser[UserID1].bInGame = false;
            m_mapOnlineUser[UserID1].bGameLock = false;
        
            m_mapOnlineUser[UserID2].bFindGame = false;
            m_mapOnlineUser[UserID2].bInGame = false;
            m_mapOnlineUser[UserID2].bGameLock = false;
        }
        
        {
            unique_lock<mutex> lck(m_mapMatchedUserMutex);
            m_matchedUser.erase(GameID);
        }
    }
    
    
};

#endif /* onlineUser_h */
