#include "VideoModel.h"

VideoModel* VideoModel::videomodel = nullptr;
VideoModel* VideoModel::getinstace()
{
    if (VideoModel::videomodel == nullptr)
    {
        VideoModel::videomodel = new VideoModel();
    }
    return VideoModel::videomodel;
}

int VideoModel::selectVideoByDate(int UID)
{
    int row;
    int col;
    char** datares;
    char sql[256];
    sprintf(sql, "select * from VideoInfo where UID=%d;",
        UID);
    int res = this->DB->getDataByTable(sql, datares, row, col);
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

int VideoModel::insertVideoByDFFNH(int UID, int FrameCount, int CurrentFrame,char *x,char* CurrentTime, char* FirstFrame, char* VideoUrl)
{
    int row;
    int col;
    char** datares;
    char sql[256];
    sprintf(sql, "insert into VideoInfo values(null,%d,%d,%d,'%s','%s','%s','%s');",
        UID, FrameCount, CurrentFrame,x,CurrentTime, FirstFrame, VideoUrl);
    int res = this->DB->getDataByTable(sql, datares, row, col);
    if (res == 0)
    {
        return 0;
    }
    else {
        //    cout<<"failed!"<<endl;
        return -1;
    }
}

int VideoModel::updateVideoByID(int VID, int CurrFrame, char* x, char* VCurrTime)
{
    int row;
    int col;
    char** datares;
    char sql[256];
    sprintf(sql, "UPDATE VideoInfo SET CurrentFrame=%d,X='%s',CurrentTime='%s' where VID=%d;", CurrFrame, x, VCurrTime, VID);
    int res = this->DB->getDataByTable4(sql, datares, row, col);
    if (res == 0)
    {
        return 0;
    }
    else {
        //    cout<<"failed!"<<endl;
        return -1;
    }
}

int VideoModel::SelectTableForCount()
{
    int row;
    int col;
    char** datares;
    char sql[256] = "select count(*) from VideoInfo;";

    int res = this->DB->getDataByTable4(sql, datares, row, col);
    if (res == 0)
    {
        return 0;
    }
    else {
        //    cout<<"failed!"<<endl;
        return -1;
    }
}

VideoModel::VideoModel()
{
    this->DB = SignalDB::getinstance();
}
