//
//  blMgr.h
//  short
//
//  Created by 李卓 on 19/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#ifndef blMgr_h
#define blMgr_h

#include <stdio.h>
#include "json.hpp"
#include <string>
#include <map>
#include <set>
#include <mutex>
#include "zmqSvr.h"

using namespace std;


class blMgr
{
private:
    nlohmann::json m_jsMsg;
    
    nlohmann::json m_jsRet;
    
    zmqSvr *m_pZmqSvr;
    
public:
    blMgr(zmqSvr *pZmqSvr) : m_pZmqSvr(pZmqSvr) {}
    
    void parseMsg(string msg);
    
    nlohmann::json getRet();
    
    string getStrRet();
    
//business logic
protected:
    int userConnect();
    
    int userLogout();
    
    int findGameMatch();
    
    int userSubSockWaiting();
    
    int boardcastPlayerOperation();
    
};

#endif /* blMgr_h */
