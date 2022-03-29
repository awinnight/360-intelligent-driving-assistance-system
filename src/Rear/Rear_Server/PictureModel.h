#pragma once
#include "SignalDB.h"
#include <iostream>
using namespace std;

class PictureModel
{
public:
    static PictureModel* picturemodel;
    static PictureModel* getinstace();
    int selectPicture(int UID);
    int insertPicture(int UID,int VID,char * PName,char * PUrl);

private:
    PictureModel();
    SignalDB* DB;
};

