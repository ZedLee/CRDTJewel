//
//  zmqSvr.hpp
//  short
//
//  Created by 李卓 on 17/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#ifndef zmqSvr_h
#define zmqSvr_h
#include <stdio.h>
#include <string>
#include <map>
#include "zhelpers.hpp"
#include "zmsg.hpp"
#include "ThreadPool.h"
#include <mutex>
#include <condition_variable>

using namespace std;


#define SERVER_ADDR "tcp://10.9.135.227:"
//#define LOCAL_ADDR "tcp://localhost:"
#define SERVER_REP_PORT "10888"
#define SERVER_PUB_PORT "10889"
//#define SERVER_WORKER_PORT "10890"
#define INPROC_BACKEND_ADDR "inproc://backend"

#define WORKER_THREAD_NUMBER 3
#define IO_THREAD_NUMBER 8


class zmqSvr
{
private:
    zmq::context_t *m_pContext;
    
    zmq::socket_t *m_pPubSock;
    
    mutex m_mutexPubSock;
    
    zmq::socket_t *m_pFrontEndSock;
    
    zmq::socket_t *m_pBackEndSock;
    
    std::vector<zmq::socket_t*> m_vecWorkerQueue;
    
    ThreadPool * m_pIOThreadPool;
    
    mutex m_contextMutex;
    
    mutex m_sendQueueMutex;
    
    condition_variable m_cvSendQueue;
    
    queue<pair<string, string>> m_queueSendMsg;
    
public:
    zmqSvr();
    
    ~zmqSvr();
    
    void sendMsg(string strid, string strmsg);
    
    void publisherSendMsg(string filter,string msg);

protected:
    
    void createRecvWorker();
    
    void createTunnel();
    
};

#endif /* zmqSvr_h */
