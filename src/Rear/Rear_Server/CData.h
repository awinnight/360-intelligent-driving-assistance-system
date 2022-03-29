#pragma once
#include <stdio.h>
#define LOGIN_IN 1
#define LOGIN_OUT 2
#define FRAME_COUNT 7500			//默认视频采集5分钟为一份，5*60*25=7500帧

//客户端 发送给 服务器的业务包
#define LOGIN 1
#define REGISTER 2
#define VIDEO_INFO 3
#define VIDEO_RECORD 4
#define PICTURE 5
#define VIDEO_LIST_REQUEST 13		//视频列表请求包
#define PICTURE_LIST_REQUEST 14		//图片列表请求包

//服务器 发送给 客户端的相应包 【判断业务是否成功完成】
#define LOGIN_ECHO 6				//登陆响应包
#define REGISTER_ECHO 7				//注册响应包
#define VINFO_ECHO 8				//视频采集响应包
#define VRECORD_ECHO 9				//视频记录响应包
#define VIDEO_LIST 10				//视频列表响应包
#define PICTURE_LIST 11				//图片列表响应包
#define PINFO_ECHO 12				//图片收集响应包


//通信数据包包头
typedef struct Communicate_Head
{
	int Cfd;			//用户客户端fd
	int UID;			//用户ID  
	int Type;			//功能码		【看宏定义define：登陆包为1；注册包为2；视频采集包（视频信息包）为3；视频播放记录包为4；图片包为5】
	size_t Len;			//包体长度
	char CRC[32];		//CRC校验码
	int Page_ID;		//数据包ID || 数据包流水号
	int Operation;		//操作码		【目前默认定义：业务失败为0；业务成功为1；心跳包：断开前服务器通知客户端，返回10】

}COM_HEAD;

//通信数据包
typedef struct Communicate_Page
{
	struct Communicate_Head Head;			//数据包包头
	char Page_Content[1024];				//数据包包体

}COM_PAGE;

//登录包   通信数据包包体之一
typedef struct Login_Info_Page
{
	int Cfd;					//用户客户端fd
	char UAccount[15];			//用户账户
	char UPwd[12];				//用户密码

}LOG_PAGE;

//视频采集信息包   通信数据包包体之一
typedef struct Video_Info_Page
{
	int UID;			//用户ID
	int FrameCount;				//视频实际总帧数
	double X;					//视频播放进度   百分比用小数表示，如80%->0.8
	char VCurrtime[32];			//视频录制时间
	char FirstFrame_Url[64];	//视频头帧路径
	char Video_Url[64];			//视频存储路径   || 视频名称

}VINFO_PAGE;

//视频播放记录包
typedef struct Video_Record_Page
{
	int VID;			//视频ID
	int UID;			//用户ID
	int CurrFrame;		//播放结束的当前帧
	double X;			//视频播放进度   百分比用小数表示，如80%->0.8  
	char VCurrTime[32];						//播放结束时间

}VRECORD_PAGE;

//视频列表信息包       【服务器 发送给 客户端】
typedef struct Video_List_Page
{
	int VID;				//视频ID
	double X;					//视频播放进度   百分比用小数表示，如80%->0.8,100%->1.0
	char VCurrTime[32];			//播放结束时间
	char FirstFrame_Url[64];	//视频头帧路径
	char Video_Url[64];			//视频存储路径 || 视频名称

}VLIST_PAGE;

//特征图片包
typedef struct Special_Picture_Page
{
	int UID;			//用户ID
	int VID;			//视频ID   倒车模式下可为空或以-1表示
	int flag;				//0为图片第一个小包，1为图片后续小包
	int PageNum;			//小包序号
	char SpecialPicture[24];			//特征图片名称
	char PictureUrl[64];				//图片路径
	char PicBuf[916];					//截取时本地时间

}PICTURE_PAGE;

//图片列表信息包
typedef struct Picture_List_Page
{
	int UID;			//用户ID
	int VID;			//视频ID   倒车模式下可为空或以-1表示
	int PID;			//图片ID
	char PictureUrl[64];				//图片路径
}PLIST_PAGE;

//注册包、登录包响应包发回包头