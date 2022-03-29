#include "PictureModel.h"
PictureModel* PictureModel::picturemodel = nullptr;
PictureModel* PictureModel::getinstace()
{
    if (PictureModel::picturemodel == nullptr)
    {
        PictureModel::picturemodel = new PictureModel();
    }
    return PictureModel::picturemodel;
}

int PictureModel::selectPicture(int UID)
{   int row;
    int col;
    char** datares;
    char sql[256];
    sprintf(sql, "select * from PictureInfo where UID=%d;",
        UID);
    int res = this->DB->getDataByTable3(sql, datares, row, col);
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

int PictureModel::insertPicture(int UID, int VID, char* PName, char* PUrl)
{
    int row;
    int col;
    char** datares;
    char sql[256];
    sprintf(sql, "insert into PictureInfo values(null,%d,%d,'%s','%s');",
        UID,VID,PName,PUrl);
    int res = this->DB->getDataByTable1(sql, datares, row, col);
    if (res == 0)
    {
        return 0;
    }
    else {
        //    cout<<"failed!"<<endl;
        return -1;
    }
}

PictureModel::PictureModel()
{
    this->DB = SignalDB::getinstance();
}