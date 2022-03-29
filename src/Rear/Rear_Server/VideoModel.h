#pragma once
#include "SignalDB.h"
#include <iostream>
using namespace std;

class VideoModel
{
public:
    static VideoModel* videomodel;
    static VideoModel* getinstace();
    int selectVideoByDate(int UID);
    int insertVideoByDFFNH(int UID,int FrameCount,int CurrentFrame, char* x,char * CurrentTime,char * FirstFrame,char *VideoUrl);
    int updateVideoByID(int VID,int CurrFrame,char *x,char * VCurrTime);
    int SelectTableForCount();
private:
    VideoModel();
    SignalDB* DB;
};

