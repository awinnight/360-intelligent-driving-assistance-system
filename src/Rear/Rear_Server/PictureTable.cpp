#include "PictureTable.h"

PictureTable::PictureTable(int PID,int UID, int VID, string PName, string PUrl)
{
	this->PID = PID;
	this->UID = UID;
	this->VID = VID;
	this->PName = PName;
	this->PUrl = PUrl;
}
int PictureTable::getPID() const
{
	return this->PID;
}
int PictureTable::getUID() const
{
	return this->UID;
}
int PictureTable::getVID() const
{
	return this->VID;
}
string PictureTable::getPName() const
{
	return this->PName;
}
string PictureTable::getPUrl() const
{
	return this->PUrl;
}