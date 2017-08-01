//
//  zmqSvr.cpp
//  short
//
//  Created by 李卓 on 17/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#include "zmqSvr.h"
#include "ThreadPool.h"
#include "blMgr.h"
#include "log4z.h"

zmqSvr::zmqSvr()
{
    m_pIOThreadPool = new ThreadPool(IO_THREAD_NUMBER);
    m_pContext = new zmq::context_t(1);
    
    //initialize publisher socket
    m_pPubSock = new zmq::socket_t(*m_pContext, ZMQ_PUB);
    m_pPubSock->bind(string(SERVER_ADDR).append(SERVER_PUB_PORT));
    
    //used 5 thread in pool
    m_pIOThreadPool->enqueue(std::bind(&zmqSvr::createTunnel, this));
    
    cout<<"zmq server initialized..."<<endl;
    
}

zmqSvr::~zmqSvr()
{
    for (auto val : m_vecWorkerQueue)
    {
        val->close();
        delete val;
    }
    m_vecWorkerQueue.clear();
    
    if (m_pPubSock != NULL)
    {
        m_pPubSock->close();
        delete m_pPubSock;
    }
    if (m_pFrontEndSock != NULL)
    {
        m_pFrontEndSock->close();
        delete m_pFrontEndSock;
    }
    if (m_pBackEndSock != NULL)
    {
        m_pBackEndSock->close();
        delete m_pBackEndSock;
    }
    
    if (m_pContext != NULL) delete m_pContext;
    
}

void zmqSvr::publisherSendMsg(string filter,string msg)
{
    LOGI("send msg to subsriber[" + filter + "]: " + msg);
    unique_lock<mutex> lck(m_mutexPubSock);
    s_sendmore(*m_pPubSock, filter.c_str());
    s_send(*m_pPubSock, msg.c_str());
}

void zmqSvr::createTunnel()
{
    //initialize front end socket
    m_pFrontEndSock = new zmq::socket_t(*m_pContext, ZMQ_ROUTER);
    m_pFrontEndSock->bind(string(SERVER_ADDR).append(SERVER_REP_PORT));
    
    //initialize back end worker socket
    m_pBackEndSock = new zmq::socket_t(*m_pContext, ZMQ_DEALER);
    m_pBackEndSock->bind(INPROC_BACKEND_ADDR);
    
    for (int i = 0; i < WORKER_THREAD_NUMBER; i++)
    {
        m_pIOThreadPool->enqueue(std::bind(&zmqSvr::createRecvWorker, this));
    }
    try
    {
        zmq::proxy(*m_pFrontEndSock, *m_pBackEndSock, nullptr);
    }
    catch (std::exception &e) {}
    
}

void zmqSvr::sendMsg(string strid, string strmsg)
{
    unique_lock<mutex> lck(m_sendQueueMutex);
    m_queueSendMsg.push(make_pair(strid, strmsg));
    m_cvSendQueue.notify_one();
}

//由于zmq负载均衡机制，每个任务会平均发放给各个worker
//所以收发必须在同一个worker中
void zmqSvr::createRecvWorker()
{
    zmq::socket_t *workerSock;
    {
        unique_lock<mutex> lck(m_contextMutex);
        workerSock = new zmq::socket_t(*m_pContext, ZMQ_DEALER);
        workerSock->connect(INPROC_BACKEND_ADDR);
        m_vecWorkerQueue.push_back(workerSock);
    }
    
    zmq::pollitem_t items[] = {{*workerSock, 0, ZMQ_POLLIN, 0}};
    try
    {
        while (true)
        {
            zmq::poll(items, 1, 10);
            if (items[0].revents & ZMQ_POLLIN)
            {
                zmq::message_t identity;
                zmq::message_t msg;
                workerSock->recv(&identity);
                workerSock->recv(&msg);
                
                string strid = s_read_msg(&identity);
                string strmsg = s_read_msg(&msg);

                LOGI("Recieve Message [" + strid + "]: " + strmsg);
                
                //sendMsg(strid, strmsg);
                blMgr l_blMgr(this);
                l_blMgr.parseMsg(strmsg);
            }
            
            {
                unique_lock<mutex> lck(m_sendQueueMutex);
                int iSizeQueue = (int) m_queueSendMsg.size();
                int iSendNum = 0;
                if (iSizeQueue <= WORKER_THREAD_NUMBER)
                {
                    iSendNum = iSizeQueue > 0 ? 1 : 0;
                }
                else
                {
                    iSendNum = iSizeQueue / WORKER_THREAD_NUMBER + 1;
                }
                for (int i = 0; i < iSendNum; i++)
                {
                    pair<string, string> pairMsg = m_queueSendMsg.front();
                    m_queueSendMsg.pop();
                    zmq::message_t copied_id(pairMsg.first.c_str(), pairMsg.first.size());
                    zmq::message_t copied_msg(pairMsg.second.c_str(), pairMsg.second.size());
                    workerSock->send(copied_id, ZMQ_SNDMORE);
                    workerSock->send(copied_msg);
                }
            }
        }
    }
    catch (std::exception &e) {}
    
}


