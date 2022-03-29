#include "CTask.h"

CTask::CTask()
{
}

CTask::~CTask()
{
}

CTaskChild::CTaskChild(char* buf)
{
	smw=new ShareM(1234, sizeof(COM_PAGE), 10);
	buff = (char*)malloc(sizeof(COM_PAGE));
	memset(buff,0, sizeof(COM_PAGE));
	
	memcpy(buff, buf, sizeof(COM_PAGE));
	cout<<"////" <<buff<<"---"<<strlen(buff)<< endl;

}

CTaskChild::~CTaskChild()
{
	delete this;
}

void CTaskChild::run()
{
	smw->ShmWrite(buff);
	cout<<strlen(buff)<<"[[[" << sizeof(buff) << endl;
}
