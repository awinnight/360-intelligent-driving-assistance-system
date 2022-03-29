#pragma once
#include <iostream>
#include <string>
#include <string.h>
using namespace std;

class VideoTable
{
    public:
        VideoTable(int UID, int FrameCount, int CurrentFrame,string x,string CurrentTime, string FirstFrame, string VideoUrl);
        ~VideoTable();
        int getUID() const;
        int getFrameCount() const;
        int getCurrentFrame() const;
        string getX() const;
        string getCurrentTime() const;
        string getFirstFrame() const;
        string getVideoUrl() const;

    private:
        int UID;
        int FrameCount;
        int CurrentFrame;
        string X;
        string CurrentTime;
        string FirstFrame;
        string VideoUrl;
};

