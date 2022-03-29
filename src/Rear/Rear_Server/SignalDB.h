#pragma once

#include <sqlite3.h>
#include <iostream>
#include <map>
#include "VideoTable.h"
#include "PictureTable.h"
using namespace std;

class SignalDB
{
public:
    static SignalDB* db;
    static SignalDB* getinstance();
    static void ReleaseInstance();
    int openDB(const char* path);
    void closeDB();
    int getDataByTable(const char* sql, char**& presult, int& row, int& col);         
    int getDataByTable1(const char* sql, char**& presult, int& row, int& col);
    int getDataByTable2(const char* sql, char**& presult, int& row, int& col);                 //照片查询
    int getDataByTable3(const char* sql, char**& presult, int& row, int& col);                 
    int getDataByTable4(const char* sql, char**& presult, int& row, int& col);                 //查询数据库表记录行数
private:
    SignalDB();
    sqlite3* sqldb;
};




