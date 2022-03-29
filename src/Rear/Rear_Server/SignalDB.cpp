#include "SignalDB.h"
extern map<int, VideoTable*> Vlmap;
extern map<int, PictureTable*> Plmap;

SignalDB* SignalDB::db = nullptr;
SignalDB* SignalDB::getinstance()
{
    if (SignalDB::db == nullptr)
    {
        SignalDB::db = new SignalDB();
    }
    return SignalDB::db;
}

void SignalDB::ReleaseInstance()
{
    if (SignalDB::db != nullptr)
    {
        SignalDB::db = nullptr;
    }
}

int SignalDB::openDB(const char* path)
{
    int res = sqlite3_open(path, &sqldb);
    if (res != SQLITE_OK)
    {
        cout << sqlite3_errcode(sqldb) << endl;
        cout << sqlite3_errmsg(sqldb) << endl;
    }
    return res;
}

void SignalDB::closeDB()
{
    int res = sqlite3_close(sqldb);
    if (res != SQLITE_OK)
    {
        cout << sqlite3_errcode(sqldb) << endl;
        cout << sqlite3_errmsg(sqldb) << endl;
    }
    else {
        cout << "close OK" << endl;
    }
}

int SignalDB::getDataByTable(const char* sql, char**& presult, int& row, int& col)
{
    cout << "video list!!!!" << endl;
    char* errmsg;
    int res = sqlite3_get_table(sqldb, sql, &presult, &row, &col, &errmsg);
    if (res == SQLITE_OK)
    {
        if (col == 0)
        {
            cout << "Null!" << endl;
            return -2;
        }
        else {
            for (int j = col; j < col * (row + 1); j++) {
                if (j % col == col - 1)
                {
                    Vlmap[atoi(presult[j - col + 1])] = new VideoTable(atoi(presult[j - col + 2]), atoi(presult[j - col + 3]), atoi(presult[j - col + 4]),
                        presult[j - col + 5], presult[j - col + 6], presult[j - col + 7], presult[j - col + 8]);
                }
            }
            cout << "Vlmap size:"<<Vlmap.size() << endl;
            return 0;
        }
    }
    else {
        return -1;
    }
}

int SignalDB::getDataByTable1(const char* sql, char**& presult, int& row, int& col)
{
    cout << "picture list!!!!" << endl;
    char* errmsg;
    int res = sqlite3_get_table(sqldb, sql, &presult, &row, &col, &errmsg);
    if (res == SQLITE_OK)
    {
        return 0;
    }
    else {
        return -1;
    }
}

int SignalDB::getDataByTable2(const char* sql, char**& presult, int& row, int& col)
{
    char* errmsg;
    int res = sqlite3_get_table(sqldb, sql, &presult, &row, &col, &errmsg);
    if (res == SQLITE_OK)
    {
        if (col == 0)
        {
            cout << "Null!" << endl;
            return -2;
        }
        else {
            //cout<<"select succed!!" << endl;
            int id = atoi(presult[col]);
            return id;
        }

    }
    else {
        cout << sqlite3_errcode(sqldb) << endl;
        cout << sqlite3_errmsg(sqldb) << endl;
        return -1;
    }
}

int SignalDB::getDataByTable3(const char* sql, char**& presult, int& row, int& col)
{
    cout << "picture select!!!!" << endl;
    char* errmsg;
    int res = sqlite3_get_table(sqldb, sql, &presult, &row, &col, &errmsg);
    if (res == SQLITE_OK)
    {
        if (col == 0)
        {
            cout << "Null!" << endl;
            return -2;
        }
        else {
            for (int j = col; j < col * (row + 1); j++) {
                if (j % col == col - 1)
                {
                    Plmap[atoi(presult[j - col + 1])] = new PictureTable(atoi(presult[j - col + 1]),atoi(presult[j - col + 2]), atoi(presult[j - col + 3]),
                    presult[j - col + 4], presult[j - col + 5]);
                }
            }
            cout << "Plmap size:" << Plmap.size() << endl;
            return 0;
        }
    }
    else {
        return -1;
    }
}

int SignalDB::getDataByTable4(const char* sql, char**& presult, int& row, int& col)
{
    char* errmsg;
    int res = sqlite3_get_table(sqldb, sql, &presult, &row, &col, &errmsg);
    if (res == SQLITE_OK)
    {
        if (col == 0)
        {
            cout << "Null!" << endl;
            return -2;
        }
        else {

            return 0;
        }

    }
    else {
        cout << sqlite3_errcode(sqldb) << endl;
        cout << sqlite3_errmsg(sqldb) << endl;
        return -1;
    }
}

SignalDB::SignalDB()
{
    openDB("/opt/Third.db");
}
