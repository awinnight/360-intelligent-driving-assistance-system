#include "CSocketBase.h"

CSocketBase::CSocketBase()
{

}

CSocketBase::~CSocketBase()
{

}

//�����׽���
int CSocketBase::Socket_Create()
{
	/* socket */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);			//AF_INET:IPV4;SOCK_STREAM:TCP
    if (sock_fd==-1)
    {
        fprintf(stderr, "socket error:%s\n\a", strerror(errno));
        exit(1);
        return FAILED;
    }
    return SUCCESS;
}

//��ַ����        ****************************************************************�����ϵ�ַ�࣬��ַ��������ļ���ȡIP��Port***********************************************
int CSocketBase::Addr_Reuse()
{
    //����Socket���ԣ�SO_REUSEADDR��������bind�����б��ص�ַ�ظ�ʹ��   
    int iSockopt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,
        (const char*)&iSockopt, sizeof(int)) < 0)
    {
        close(sock_fd);
        exit(1);
        return FAILED;
    }
    server_port = 8888;
    /* set server sockaddr_in */
    memset(&server_addr, 0, sizeof(struct sockaddr_in));//clear
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY:This machine all IP
    server_addr.sin_port = htons(server_port);

    return SUCCESS;
}

//��
int CSocketBase::Socket_Bind()
{
    /* bind */
    int ret = bind(sock_fd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr));
    if (-1 == ret)
    {
        fprintf(stderr, "bind error:%s\n\a", strerror(errno));
        close(sock_fd);
        exit(1);
        return FAILED;
    }
    return SUCCESS;
}

//����
int CSocketBase::Socket_Listen()
{
    /* listen */
    int ret = listen(sock_fd, LISTEN_NUM);
    if (-1 == ret)
    {
        fprintf(stderr, "listen error:%s\n\a", strerror(errno));
        close(sock_fd);
        exit(1);
        return FAILED;
    }
    return SUCCESS;
}

//Socket�࿪ʼҵ��
void CSocketBase::Socket_Start()
{
    int Cret,Rret,Bret,Lret;
    Cret = Socket_Create();
    Rret = Addr_Reuse();
    Bret = Socket_Bind();
    Lret = Socket_Listen();
    if (Cret * Rret * Bret * Lret == 1)
    {
        printf("server start!!!!!\n");
    }
    else
    {
        printf("socket create failed!!!\n");
    }
    
}
