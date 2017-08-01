//
//  Board.h
//  JwlAlg
//
//  Created by 李卓 on 07/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#ifndef Board_h
#define Board_h

#include <vector>
#include <map>
#include <string>

using namespace std;

class MyPoint
{
public:
    int x;
    int y;
    MyPoint(int x, int y) : x(x), y(y) {}
    MyPoint() { x = -1; y = -1; }
};

enum Direction
{
    UNKNOWN = -1,
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
};

//未完成：
//1. 生成或重新生成棋盘后未有判断是否有可消除的点
class Board
{
    friend class CRDTBoard;
    
private:
    vector<vector<int>> m_VecBoard;
    
    int m_iLength;
    
    int m_iWidth;
    
    int m_iColour;
    
    unsigned int m_iRandomSeed;
    
    long m_lScore = 0;
    
    vector<MyPoint> m_vecLastMatch;
    
public:
    
    Board(int l = 8, int w = 8, int c = 6, unsigned int r = 0) : m_iLength(l), m_iWidth(w), m_iColour(c), m_iRandomSeed((unsigned int)time(0)) {}
    
    ~Board();
    
    //validate users' swap
    //if valid, swap, remove and return true
    //if not, do nothing and return false
    bool validateSwap(MyPoint p1, Direction d);
    
    //output board's state
    friend ostream &operator<<( ostream &output, const Board o);
    
    long getScore();
    
    void setRandomSeed(unsigned int val);
    
    void setBoardState(vector<vector<int>> b) { m_VecBoard = b; };
    
    int init();
    
protected:
    
    void generateBoard();
    
    //only use in void generateBoard();
    bool isRemovable4FirstGeneration(int y, int x);
    
    //remove same and continuous elements in board
    //return how much scores players get in this move
    int remove(vector<MyPoint> vecPoint);
    
    //swap in board
    void swap(MyPoint p1, MyPoint p2);
    
    //input an empty vector
    //if found any match, positon of points matched will store in this vertor
    bool findMatch(vector<MyPoint> &vecPoint);
    
    bool findMatch(int y, int x, vector<MyPoint> &vecPoint);
    
    void regenerateBoard(vector<MyPoint> vecPoint);
    
    //judge regenerate board if removable or not
    bool isRemovable();
    
};


#endif /* Board_h */
