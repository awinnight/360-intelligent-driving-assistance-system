#pragma once
#include <stdio.h>
#define LOGIN_IN 1
#define LOGIN_OUT 2
#define FRAME_COUNT 7500			//Ĭ����Ƶ�ɼ�5����Ϊһ�ݣ�5*60*25=7500֡

//�ͻ��� ���͸� ��������ҵ���
#define LOGIN 1
#define REGISTER 2
#define VIDEO_INFO 3
#define VIDEO_RECORD 4
#define PICTURE 5
#define VIDEO_LIST_REQUEST 13		//��Ƶ�б������
#define PICTURE_LIST_REQUEST 14		//ͼƬ�б������

//������ ���͸� �ͻ��˵���Ӧ�� ���ж�ҵ���Ƿ�ɹ���ɡ�
#define LOGIN_ECHO 6				//��½��Ӧ��
#define REGISTER_ECHO 7				//ע����Ӧ��
#define VINFO_ECHO 8				//��Ƶ�ɼ���Ӧ��
#define VRECORD_ECHO 9				//��Ƶ��¼��Ӧ��
#define VIDEO_LIST 10				//��Ƶ�б���Ӧ��
#define PICTURE_LIST 11				//ͼƬ�б���Ӧ��
#define PINFO_ECHO 12				//ͼƬ�ռ���Ӧ��


//ͨ�����ݰ���ͷ
typedef struct Communicate_Head
{
	int Cfd;			//�û��ͻ���fd
	int UID;			//�û�ID  
	int Type;			//������		�����궨��define����½��Ϊ1��ע���Ϊ2����Ƶ�ɼ�������Ƶ��Ϣ����Ϊ3����Ƶ���ż�¼��Ϊ4��ͼƬ��Ϊ5��
	size_t Len;			//���峤��
	char CRC[32];		//CRCУ����
	int Page_ID;		//���ݰ�ID || ���ݰ���ˮ��
	int Operation;		//������		��ĿǰĬ�϶��壺ҵ��ʧ��Ϊ0��ҵ��ɹ�Ϊ1�����������Ͽ�ǰ������֪ͨ�ͻ��ˣ�����10��

}COM_HEAD;

//ͨ�����ݰ�
typedef struct Communicate_Page
{
	struct Communicate_Head Head;			//���ݰ���ͷ
	char Page_Content[1024];				//���ݰ�����

}COM_PAGE;

//��¼��   ͨ�����ݰ�����֮һ
typedef struct Login_Info_Page
{
	int Cfd;					//�û��ͻ���fd
	char UAccount[15];			//�û��˻�
	char UPwd[12];				//�û�����

}LOG_PAGE;

//��Ƶ�ɼ���Ϣ��   ͨ�����ݰ�����֮һ
typedef struct Video_Info_Page
{
	int UID;			//�û�ID
	int FrameCount;				//��Ƶʵ����֡��
	double X;					//��Ƶ���Ž���   �ٷֱ���С����ʾ����80%->0.8
	char VCurrtime[32];			//��Ƶ¼��ʱ��
	char FirstFrame_Url[64];	//��Ƶͷ֡·��
	char Video_Url[64];			//��Ƶ�洢·��   || ��Ƶ����

}VINFO_PAGE;

//��Ƶ���ż�¼��
typedef struct Video_Record_Page
{
	int VID;			//��ƵID
	int UID;			//�û�ID
	int CurrFrame;		//���Ž����ĵ�ǰ֡
	double X;			//��Ƶ���Ž���   �ٷֱ���С����ʾ����80%->0.8  
	char VCurrTime[32];						//���Ž���ʱ��

}VRECORD_PAGE;

//��Ƶ�б���Ϣ��       �������� ���͸� �ͻ��ˡ�
typedef struct Video_List_Page
{
	int VID;				//��ƵID
	double X;					//��Ƶ���Ž���   �ٷֱ���С����ʾ����80%->0.8,100%->1.0
	char VCurrTime[32];			//���Ž���ʱ��
	char FirstFrame_Url[64];	//��Ƶͷ֡·��
	char Video_Url[64];			//��Ƶ�洢·�� || ��Ƶ����

}VLIST_PAGE;

//����ͼƬ��
typedef struct Special_Picture_Page
{
	int UID;			//�û�ID
	int VID;			//��ƵID   ����ģʽ�¿�Ϊ�ջ���-1��ʾ
	int flag;				//0ΪͼƬ��һ��С����1ΪͼƬ����С��
	int PageNum;			//С�����
	char SpecialPicture[24];			//����ͼƬ����
	char PictureUrl[64];				//ͼƬ·��
	char PicBuf[916];					//��ȡʱ����ʱ��

}PICTURE_PAGE;

//ͼƬ�б���Ϣ��
typedef struct Picture_List_Page
{
	int UID;			//�û�ID
	int VID;			//��ƵID   ����ģʽ�¿�Ϊ�ջ���-1��ʾ
	int PID;			//ͼƬID
	char PictureUrl[64];				//ͼƬ·��
}PLIST_PAGE;

//ע�������¼����Ӧ�����ذ�ͷ