//
//  main.cpp
//  JwlAlg
//
//  对对碰核心算法
//
//  Created by 李卓 on 07/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#include <iostream>
#include "Board.h"
#include <string>

#include "ThreadPool.h"
#include <zmq.hpp>
#include "zhelpers.hpp"
#include "zmqClient.h"
#include "commonFunc.h"
#include "Singleton.h"
#include "cliBlMgr.h"
#include "log4z.h"
#include "CRDTBoard.h"

using namespace zsummer::log4z;


void gameInterface()
{
    bool bRun = true;
    Board bT;
    bT.init();
    
    while (bRun)
    {
        string input;
        cout<<"input quit for quit this game..."<<endl;
        cout<<"input position and direction, e.g. 2,2,right"<<endl;
        cin>>input;
        
        if (0 == input.compare("quit"))
        {
            cout<<"your scores is "<<bT.getScore()<<endl;
            cout<<"quit..."<<endl;
            bRun = false;
            
            cliBlMgr cbm(Singleton<zmqClient>::GetInstance());
            cbm.requestLogout();
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
            bT.validateSwap(MyPoint(atoi(ret[0].c_str()), atoi(ret[1].c_str())), d);
            cout<<bT;
        }
    }
}


int main(int argc, const char * argv[])
{
    
    ILog4zManager::getRef().start();
    ILog4zManager::getRef().setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, true);
    cout<<endl;
    
    Singleton<zmqClient>::InitInstance();
    zmqClient* pZmqCli = Singleton<zmqClient>::GetInstance();
    
    Singleton<CRDTBoard>::InitInstance();
    CRDTBoard *pCRDTboard = Singleton<CRDTBoard>::GetInstance();
    
    cliBlMgr cbm(pZmqCli);
    cbm.requestConnect();
    sleep(1);
    pCRDTboard->gameRequest();
    
    
    
    while (true)
    {
        sleep(200);
    }
    
    return 0;
}
