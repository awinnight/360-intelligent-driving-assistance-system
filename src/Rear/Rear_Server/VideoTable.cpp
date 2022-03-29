#include "VideoTable.h"

VideoTable::VideoTable(int UID, int FrameCount, int CurrentFrame,string x, string CurrentTime, string FirstFrame, string VideoUrl)
{
	this->UID = UID;
	this->FrameCount = FrameCount;
	this->CurrentFrame = CurrentFrame;
	this->X = x;
	this->CurrentTime= CurrentTime;
	this->FirstFrame= FirstFrame;
	this->VideoUrl= VideoUrl;
}

int VideoTable::getUID() const
{
	return this->UID;
}
int VideoTable::getFrameCount() const
{
	return this->FrameCount;
}
int VideoTable::getCurrentFrame() const
{
	return this->CurrentFrame;
}
string VideoTable::getCurrentTime() const
{
	return this->CurrentTime;
}
string VideoTable::getFirstFrame() const
{
	return this->FirstFrame;
}
string VideoTable::getVideoUrl() const
{
	return this->VideoUrl;
}

string VideoTable::getX() const
{
	return this->X;
}
