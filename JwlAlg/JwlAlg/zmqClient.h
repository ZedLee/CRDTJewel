//
//  zmqClient.hpp
//  JwlAlg
//
//  Created by 李卓 on 14/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#ifndef zmqClient_h
#define zmqClient_h

#include <stdio.h>
#include "zhelpers.hpp"
#include <mutex>
#include <string>
#include "ThreadPool.h"
#include <queue>


#define SERVER_ADDR "tcp://localhost:"
#define SERVER_REP_PORT "10888"
#define SERVER_PUB_PORT "10889"

class zmqClient
{
private:
    zmq::context_t *m_pContext;
    
    zmq::socket_t *m_pReqSock;
    
    zmq::socket_t *m_pSubSock;
    
    ThreadPool * m_pIOThreadPool;
    
    std::string m_strClienID;
    
    std::queue<std::string> m_sendQueue;
    
    std::mutex m_mutexSendQueue;
    
    bool m_bSubSockReady = false;
    
    std::mutex m_mutexSubSockFlag;
    
    
public:
    
    zmqClient();
    
    ~zmqClient();
    
    void sentReq(std::string msg);
    
    void sentReqUnsafe(std::string msg) {m_pReqSock->send(msg.c_str(), msg.size());}
    
    void recvRep();
    
    bool createSubConnect(std::string filter);
    
    void disconnectSubSock();
    
    std::string getSockID();
    
protected:
    //long connection when initialization
    void createReqConnect();
    
    void waitReqReply();
    
    void sentTest();
    
    void recvSubTask(std::string filter);
};


#endif /* zmqClient_h */
