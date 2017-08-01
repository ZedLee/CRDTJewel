//
//  zmqClient.cpp
//  JwlAlg
//
//  Created by 李卓 on 14/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#include "zmqClient.h"
#include <string>
#include "commonFunc.h"
#include "cliBlMgr.h"
#include "log4z.h"
#include "json.hpp"

using namespace std;


zmqClient::zmqClient()
{
    m_pContext = new zmq::context_t(1);
    
    m_pIOThreadPool = new ThreadPool(5);
    
    m_strClienID = generateUUID();
    
    createReqConnect();
}

zmqClient::~zmqClient()
{
    if (m_pReqSock != NULL)
    {
        m_pReqSock->close();
        delete m_pReqSock;
    }
    if (m_pSubSock != NULL)
    {
        m_pSubSock->close();
        delete m_pSubSock;
    }
    if (m_pContext != NULL) delete m_pContext;
}

std::string zmqClient::getSockID()
{
    return m_strClienID;
}

void zmqClient::createReqConnect()
{
    m_pReqSock = new zmq::socket_t(*m_pContext, ZMQ_DEALER);
    string strAddr = SERVER_ADDR;
    strAddr.append(SERVER_REP_PORT);
    m_pReqSock->setsockopt(ZMQ_IDENTITY, m_strClienID.c_str(), m_strClienID.size());
    m_pReqSock->connect(strAddr);
    m_pIOThreadPool->enqueue(std::bind(&zmqClient::waitReqReply, this));
    //m_pIOThreadPool->enqueue(std::bind(&zmqClient::sentTest, this));
}

void zmqClient::sentReq(std::string msg)
{
    unique_lock<mutex> lck(m_mutexSendQueue);
    m_sendQueue.push(msg);
}

void zmqClient::sentTest()
{
    int request_nbr = 0;
    while (true)
    {
        char request_string[16] = {};
        sprintf(request_string, "request #%d", ++request_nbr);
        sentReq(request_string);
        sleep(1);
        
    }
}

void zmqClient::waitReqReply()
{
    
    zmq::pollitem_t items[] = {{*m_pReqSock, 0, ZMQ_POLLIN, 0}};

    try
    {
        while (true)
        {
            
            zmq::poll(items, 1, 20);
            if (items[0].revents & ZMQ_POLLIN)
            {
                //s_dump(*m_pReqSock);
                string msg = s_recv(*m_pReqSock);
                //cout<<msg<<endl;
                LOGI(msg);
                //execute bussiness logic if recieve any message
                cliBlMgr l_mrg(this);
                l_mrg.parseMsg(msg);
            }
            
            {
                unique_lock<mutex> lck(m_mutexSendQueue);
                while (m_sendQueue.size() != 0)
                {
                    string sendMsg = m_sendQueue.front();
                    m_sendQueue.pop();
                    m_pReqSock->send(sendMsg.c_str(), sendMsg.size());
                }
            }

        }
    }
    catch (std::exception &e) {}

}

void zmqClient::disconnectSubSock()
{
    if (m_pSubSock != NULL)
    {
        {
            unique_lock<mutex> lck(m_mutexSubSockFlag);
            m_bSubSockReady = true;
        }
        m_pSubSock->close();
        delete m_pSubSock;
        m_pSubSock = NULL;
    }
}

void zmqClient::recvSubTask(std::string filter)
{
    m_pSubSock = new zmq::socket_t(*m_pContext, ZMQ_SUB);
    m_pSubSock->connect(string(SERVER_ADDR).append(SERVER_PUB_PORT));
    m_pSubSock->setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.size());
    LOGI("create subscriber socket success...");
    
    
    while (true)
    {
        // Read envelope with address
        std::string address = s_recv (*m_pSubSock);
        
        //  Read message contents
        std::string contents = s_recv (*m_pSubSock);
        if (m_bSubSockReady == false)
        {
            unique_lock<mutex> lck(m_mutexSubSockFlag);
            m_bSubSockReady = true;
        }
        LOGI("Recieve massage from publisher" + contents);
        if (contents.compare("GAME_OVER") == 0) break;
        cliBlMgr l_mrg(this);
        l_mrg.parseMsg(contents);
        
        //let thread pool to execute bussiness logic
        
    }
    this->disconnectSubSock();
}

//当函数成功返回，同等于成功进入游戏房间
bool zmqClient::createSubConnect(string filter)
{
    m_pIOThreadPool->enqueue(std::bind(&zmqClient::recvSubTask, this, filter));
    
    cliBlMgr l_cbm;
    int iTimes = 0;
    
    while (!m_bSubSockReady)
    {
        iTimes++;
        s_sleep(1000);
        l_cbm.requestGameSetting(filter);
        if (iTimes > 6)
        {
            return false;
        }
    }
    
    return true;
}
