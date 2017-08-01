//
//  Board.cpp
//  JwlAlg
//
//  Created by 李卓 on 07/07/2017.
//  Copyright © 2017 李卓. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "Board.h"
#include <time.h>
#include <math.h>


Board::~Board()
{
    
}

ostream &operator<<( ostream &output, const Board o)
{
    output<<endl;
    output<<endl<<"    ";
    for (int i = 0;i < o.m_iWidth; i++)
    {
        output<<i<<" ";
    }
    output<<endl<<endl;
    for (int i = 0;i < o.m_iLength; i++)
    {
        output<<i<<"   ";
        for (int j=0;j<o.m_iWidth;j++)
        {
            output<<o.m_VecBoard[i][j]<<" ";
        }
        output<<endl;
    }
    output<<endl;
    return output;
}

int Board::init()
{
    //initialize 2D-Array size
    vector<int> vecRow(m_iWidth, 0);
    m_VecBoard.assign(m_iLength, vecRow);
    //m_iRandomSeed = (unsigned int)time(0);
    m_lScore = 0;
    generateBoard();
    return 0;
}

void Board::generateBoard()
{
    
    srand(m_iRandomSeed);
    for (int i = 0;i < m_iLength; i++)
    {
        for (int j=0;j<m_iWidth;j++)
        {
            m_VecBoard[i][j] = rand() % m_iColour + 1;
            
            while (isRemovable4FirstGeneration(i, j))
            {
                //srand(m_iRandomSeed);
                m_VecBoard[i][j] = rand() % m_iColour + 1;
            }
        }
        
    }
    cout<<*this;
    
}

bool Board::isRemovable4FirstGeneration(int y, int x)
{
    bool bRetRow = false, bRetCol = false;
    // because first generation is ordered, therefore, check left and up is enough
    if (x - 2 >= 0)
    {
        bRetRow = (m_VecBoard[y][x] == m_VecBoard[y][x - 1] && m_VecBoard[y][x - 1] == m_VecBoard[y][x - 2]) ? true : false;
    }
    if (y - 2 >= 0)
    {
        bRetCol = (m_VecBoard[y][x] == m_VecBoard[y - 1][x] && m_VecBoard[y - 1][x] == m_VecBoard[y - 2][x]) ? true : false;
    }
    return bRetRow || bRetCol;
}

void Board::setRandomSeed(unsigned int val)
{
    m_iRandomSeed = val;
}

bool Board::findMatch(vector<MyPoint> &vecPoint)
{
    
    for(int i = 0; i < m_iLength; ++i)
    {
        for(int j = 0; j < m_iWidth; ++j)
        {
            if (findMatch(i, j, vecPoint)) return true;
        }
    }
    
    return false;
}

bool Board::findMatch(int y, int x, vector<MyPoint> &vecPoint)
{
    bool bRet = false;
    int iCurrColor = m_VecBoard[y][x];
    int iCombo = 0;
    
    vecPoint.push_back(MyPoint(x, y));
    
    //pure horizontal check
    for(int i = x + 1; i < m_iWidth; i++)
    {
        if(m_VecBoard[y][i] != iCurrColor) break;
        vecPoint.push_back(MyPoint(i, y));
        iCombo++;
    }
    if(iCombo >= 2)
    {
        bRet = true;
    }
    else
    {
        vecPoint.clear();
    }
    
    
    //pure vertical check
    if (false == bRet)
    {
        iCombo = 0;
        vecPoint.push_back(MyPoint(x, y));
        for(int i = y + 1; i < m_iLength; i++)
        {
            if(m_VecBoard[i][x] != iCurrColor) break;
            vecPoint.push_back(MyPoint(x, i));
            iCombo++;
        }
        if(iCombo >= 2)
        {
            bRet = true;
        }
        else
        {
            vecPoint.clear();
        }
    }
    //cross situation
    else if (true == bRet)
    {
        iCombo = 0;
        for (auto val : vecPoint)
        {
            vector<MyPoint> vecCol;
            for (int i = val.y + 1; i < m_iLength; i++)
            {
                if(m_VecBoard[i][x] != iCurrColor) break;
                vecCol.push_back(MyPoint(x, i));
                iCombo++;
            }
            if(iCombo >= 2)
            {
                vecPoint.insert(vecPoint.end(), vecCol.begin(), vecCol.end());
                break;
            }
        }
    }
    
    return bRet;
}

void Board::swap(MyPoint p1, MyPoint p2)
{
    int iTmp = m_VecBoard[p1.y][p1.x];
    m_VecBoard[p1.y][p1.x] = m_VecBoard[p2.y][p2.x];
    m_VecBoard[p2.y][p2.x] = iTmp;
}

bool Board::validateSwap(MyPoint p1, Direction d)
{
    bool bRet = false;
    MyPoint p2(p1.x, p1.y);
    vector<MyPoint> vecRetPoints;

    switch (d) {
        case Direction::LEFT :
            p2.x--;
            break;
        case Direction::RIGHT :
            p2.x++;
            break;
        case Direction::DOWN :
            p2.y++;
            break;
        case Direction::UP :
            p2.y--;
            break;
        case Direction::UNKNOWN :
            return bRet;
    }
    
    //check boundary
    if (p2.x < 0 || p2.x >= m_iWidth || p2.y < 0 || p2.y >= m_iLength)
    {
        return bRet;
    }
    
    //noneffective move
    if (m_VecBoard[p1.y][p1.x] == m_VecBoard[p2.y][p2.x])
    {
        return bRet;
    }
    
    swap(p1, p2);
    if (!findMatch(vecRetPoints))
    {
        //if nothing match then swap back
        swap(p1, p2);
        return bRet;
    }
    else
    {
        //reset random seed for re generation
        m_iRandomSeed = m_VecBoard[p1.y][p1.x];
        m_vecLastMatch = vecRetPoints;
        
        srand(m_iRandomSeed);
        
        m_lScore += remove(vecRetPoints);
        regenerateBoard(vecRetPoints);
        bRet = true;
    }
 
    return bRet;
}

int Board::remove(vector<MyPoint> vecPoint)
{
    
    for (auto point : vecPoint)
    {
        m_VecBoard[point.y][point.x] = 0;
    }
    //how much scores player get in this move
    return pow(1.5, vecPoint.size()) * 20;
}

void Board::regenerateBoard(vector<MyPoint> vecPoint)
{
    //regenerate game board    
    for (auto point : vecPoint)
    {
        for (int i = point.y; i > 0; i--)
        {
            m_VecBoard[i][point.x] = m_VecBoard[i - 1][point.x];
        }
        m_VecBoard[0][point.x] = rand() % m_iColour + 1;
    }
    
    //check the element in new board is removable or not
    vector<MyPoint> vecRetPoints;
    if (findMatch(vecRetPoints))
    {
        cout<<"new board have effective match"<<endl;
        cout<<*this<<endl;
        m_lScore += remove(vecRetPoints);
        regenerateBoard(vecRetPoints);
    }
}

long Board::getScore()
{
    return m_lScore;
}

bool Board::isRemovable()
{
    for(int i = 2; i < m_iLength - 1; ++i)
    {
        for(int j = 2; j < m_iWidth - 1; ++j)
        {
            bool bRow = false, bCol = false;
            
            swap(MyPoint(j, i), MyPoint(j + 1, i));
            bRow = isRemovable4FirstGeneration(i, j);
            swap(MyPoint(j, i), MyPoint(j + 1, i));
            
            swap(MyPoint(j, i), MyPoint(j, i + 1));
            bCol = isRemovable4FirstGeneration(i, j);
            swap(MyPoint(j, i), MyPoint(j, i + 1));
            
            if (bRow || bCol) return true;
        }
    }
    return false;
}
