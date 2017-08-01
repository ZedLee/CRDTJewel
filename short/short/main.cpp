//
//  main.cpp
//  short
//
//  Created by 李卓 on 10/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#include <zmq.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "zhelpers.hpp"
#include "ThreadPool.h"
#include "zmqSvr.h"
#include "Singleton.h"
#include "onlineUser.h"
#include "log4z.h"

using namespace zsummer::log4z;

void envPubSvr()
{
    //  Prepare our context and publisher
    zmq::context_t context(1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:5563");
    
    while (1) {
        //  Write two messages, each with an envelope and content
        s_sendmore (publisher, "A");
        s_send (publisher, "We don't want to see this");
        s_sendmore (publisher, "B");
        s_send (publisher, "We would like to see this");
        sleep (1);
    }
}

int main () {
    //envPubSvr();
    ILog4zManager::getRef().start();
    ILog4zManager::getRef().setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, true);
    
    Singleton<onlineUser>::InitInstance();
    
    
    zmqSvr zt;
    while (1)
    {
        sleep(2000);
    }
    
    return 0;  
}
