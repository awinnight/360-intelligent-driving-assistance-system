#pragma once

#include "SignalDB.h"
#include <iostream>
using namespace std;

class UserModel
{
public:
    static UserModel* usermodel;
    static UserModel* getinstace();
    int selectUserByA(char *acc);
    int selectUserByAP(char* acc,char* pwd);
    int insertUserByANP(char* acc,char* pwd);
private:
    UserModel();
    SignalDB* DB;
};



