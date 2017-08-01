//
//  commonFunc.hpp
//  JwlAlg
//
//  Created by 李卓 on 19/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#ifndef commonFunc_h
#define commonFunc_h

#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

//根据时空间生成唯一ID
string generateUUID();

//字符串分割
int split(const string& str, vector<string>& ret_, string sep = ",");


#endif /* commonFunc_h */
