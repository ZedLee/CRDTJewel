//
//  route.hpp
//  short
//
//  Created by 李卓 on 10/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#ifndef route_hpp
#define route_hpp

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

using namespace std;

string findMacth(map<string, map<string, int>> mapDistance,
                 map<string, pair<int, int>> mapWeight, int curDelivery,
                 int curRecycle, int maxCap,string &curPlace);

bool SortByDis( const pair<string, int> &v1, const pair<string, int> &v2);

//图，key为地点， value: nested map->first为地方， nested->seconde为两地点间距离
map<string, map<string, int>> mapDistance;

//图， key为地点， value: pair->first为需求货物重量， pair->second为回收货物重量
map<string, pair<int, int>> mapWeight;

vector<vector<string>> findShortestDistance(string src, int capability,
                                            map<string, pair<int, int>> mapWeight, map<string, map<string, int>> mapDistance)
{
    map<string, pair<int, int>> mapCpyWeight = mapWeight;
    string strCurrentPos;
    vector<vector<string>> vecRet;
    int iDilivery, iRecycle;
    
    while (mapCpyWeight.size() != 0)
    {
        strCurrentPos = src;
        vector<string> vecRoute(1, src);
        iDilivery = capability;
        iRecycle = 0;
        
        //每找到一个匹配地点就删除一个
        while (!findMacth(mapDistance, mapCpyWeight, iDilivery, iRecycle, capability, strCurrentPos).empty())
        {
            vecRoute.push_back(strCurrentPos);
            //重置该货车当前货物重量
            iDilivery = iDilivery - mapCpyWeight[strCurrentPos].first;
            iRecycle = iRecycle + mapCpyWeight[strCurrentPos].second;
            mapCpyWeight.erase(mapCpyWeight.find(strCurrentPos));
        }
        //将一条路线塞入结果vector中
        vecRet.push_back(vecRoute);
    }
    
    return vecRet;
}

//找出能匹配条件的点
string findMacth(map<string, map<string, int>> mapDistance,
                 map<string, pair<int, int>> mapWeight, int curDelivery,
                 int curRecycle, int maxCap,string &curPlace)
{
    vector<pair<string, int>> vec4sort;
    string strRet;
    
    //map转数组准备排序
    for (map<string, int>::iterator it = mapDistance[curPlace].begin();
            it != mapDistance[curPlace].end(); it++)
    {
        vec4sort.push_back(make_pair(it->first, it->second));
    }
    //stl算法标准库，排序
    sort(vec4sort.begin(), vec4sort.end(), SortByDis);
    for (auto val : vec4sort)
    {
        //找出载重量能满足的下一个最近目的地
        if (curDelivery - mapWeight[val.first].first >= 0
                && curDelivery - mapWeight[val.first].first + mapWeight[val.first].second + curRecycle <= maxCap)
        {
            strRet = val.first;
            curPlace = val.first;
        }
    }
    
    //如果没有匹配点，返回空值
    return strRet;
}

bool SortByDis( const pair<string, int> &v1, const pair<string, int> &v2)
{
    //升序排列
    return v1.second < v2.second;
}

#endif /* route_hpp */
