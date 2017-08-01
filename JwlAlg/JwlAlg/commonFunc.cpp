//
//  commonFunc.cpp
//  JwlAlg
//
//  Created by 李卓 on 19/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#include "commonFunc.h"
#include <uuid/uuid.h>


string generateUUID()
{
    uuid_t uuid;
    char szID[64];
    
    uuid_generate(uuid);
    uuid_unparse(uuid, szID);
    string strRet = szID;

    return strRet;
    
}

int split(const string& str, vector<string>& ret_, string sep)
{
    if (str.empty())
    {
        return 0;
    }
    
    string tmp;
    string::size_type pos_begin = str.find_first_not_of(sep);
    string::size_type comma_pos = 0;
    
    while (pos_begin != string::npos)
    {
        comma_pos = str.find(sep, pos_begin);
        if (comma_pos != string::npos)
        {
            tmp = str.substr(pos_begin, comma_pos - pos_begin);
            pos_begin = comma_pos + sep.length();
        }
        else
        {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }
        
        if (!tmp.empty())
        {
            ret_.push_back(tmp);
            tmp.clear();
        }
    }
    return 0;
}

int str2int(string str)
{
    return std::stoi(str);
    
}
