#include "UserModel.h"

UserModel* UserModel::usermodel = nullptr;
UserModel* UserModel::getinstace()
{
    if (UserModel::usermodel == nullptr)
    {
        UserModel::usermodel = new UserModel();
    }
    return UserModel::usermodel;
}

int UserModel::selectUserByA(char* acc)
{
    int row;
    int col;
    char** datares;
    char sql[256];
    sprintf(sql, "select * from UserInfo where UAccount='%s';",
        acc);
    int res = this->DB->getDataByTable2(sql, datares, row, col);
    //this->DB->closeDB();
    if (res == 0)
    {
        if (row == 0)
        {
            cout << "failed!" << endl;
            return -1;
        }
        else {
            cout << "succeed!" << endl;
            return 0;
        }
    }
    else {
        //    cout<<"failed!"<<endl;
        return -1;
    }
}

int UserModel::selectUserByAP(char* acc, char* pwd)
{
    int row;
    int col;
    char** datares;
    char sql[256];
    sprintf(sql, "select * from UserInfo where UAccount='%s' and UPwd='%s';",
        acc, pwd);
    int res = this->DB->getDataByTable2(sql, datares, row, col);
    //this->DB->closeDB();
    if (res> 0)
    {
        return res;

    }
    else {
        //    cout<<"failed!"<<endl;
        return -1;
    }
}

int UserModel::insertUserByANP(char* acc,char* pwd)
{
    int row;
    int col;
    char** datares;
    char sql[256];
    sprintf(sql, "insert into UserInfo values(null,'%s','%s');",
        acc, pwd);
    int res = this->DB->getDataByTable2(sql, datares, row, col);
    if (res == 0)
    {
        return 0;
    }
    else {
        return -1;
    }
}

UserModel::UserModel()
{
    this->DB = SignalDB::getinstance();
}
