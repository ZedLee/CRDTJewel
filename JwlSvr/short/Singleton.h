//
//  Singleton.h
//  zmqc
//
//  Created by 李卓 on 21/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#ifndef Singleton_h
#define Singleton_h
#include <iostream>
#include <stdio.h>
#include <mutex>

using namespace std;

template <typename T>
class Singleton
{
public:
    //需在主线程中将单例先实例化好，否则会造成线程不安全
    template<typename... Args>
    static T* InitInstance(Args&&... args)
    {
        if(m_pInstance == nullptr)
        {
            m_pInstance = new T(std::forward<Args>(args)...);
        }
        return m_pInstance;
    
    }
    
    static T* GetInstance()
    {
        if (m_pInstance == nullptr)
            throw std::logic_error("the instance is not yet init...");
            
            return m_pInstance;
        }
    
    static void DestroyInstance()
    {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
    
private:
    Singleton(void);
    virtual ~Singleton(void);
    Singleton(const Singleton&);
    Singleton& operator = (const Singleton&);
    
    
private:
    static T* m_pInstance;
};

//模板特例化
template<class T> T* Singleton<T>::m_pInstance = nullptr;

#endif /* Singleton_h */
