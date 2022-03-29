#pragma once
#include <iostream>
#include <string>
#include <string.h>
using namespace std;

class PictureTable
{
public:
	PictureTable(int PID,int UID,int VID,string PName,string PUrl);
	~PictureTable();
	int getPID() const;
	int getUID() const;
	int getVID() const;
	string getPName() const;
	string getPUrl() const;

private:
	int PID;
	int UID;
	int VID;
	string PName;
	string PUrl;
};

