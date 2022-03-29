#include <iostream>
#include <map>
#include "CSocketBase.h"
#include "ThreadPool.h"
using namespace std;

//pthread_mutex_t mylockA = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t mylockB = PTHREAD_MUTEX_INITIALIZER;
#define MAXSIZE     1024   //���ݻ�����
#define FDSIZE      1000   //�ͻ�����������
#define EPOLLEVENTS 100   

#define MAX_CLIENT    10  //�ͻ��������

int cap_num;
int wfd;
int epolfd;
map<int, int> heartmap;

//������յ�������
static void handle_accpet(int epollfd, int listenfd);
//������
void do_read(int epollfd, int fd, char* buf, ThreadPool* tp);
//����¼�
void add_event(int epollfd, int fd, int state);
//�޸��¼�
void modify_event(int epollfd, int fd, int state);
//ɾ���¼�
void delete_event(int epollfd, int fd, int state);
void* produce(void* arg);
void* consume(void* arg);

/* socket->bind->listen->accept->send/recv->close*/
int main(int argc, char** argv)
{
    char recv_buf[2048];
   // recv_buf = (char*)malloc(sizeof(COM_PAGE));
    memset(&recv_buf,0,sizeof(COM_PAGE));
    int ret;
    void* pret;
    socklen_t addr_len;
    int client_num = -1;
    signal(SIGCHLD, SIG_IGN);

    CSocketBase* SerSocket = new CSocketBase();
    SerSocket->Socket_Start();

    ThreadPool* tp = new ThreadPool(100, 5, 60);

    /* ��ʼepoll���� */
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];// ����100���¼�
    //����һ��������
    epollfd = epoll_create(FDSIZE);
    //��Ӽ����������¼�
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = SerSocket->sock_fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, SerSocket->sock_fd, &ev);

    pthread_t thread,hthread;
    int pthret = pthread_create(&thread, NULL, produce, NULL);
    if (pthret != 0)
    {
        perror("thread create failed!");
        exit(1);
    }
    pthret = pthread_create(&hthread, NULL, consume, NULL);
    if (pthret != 0)
    {
        perror("hthread create failed!");
        exit(1);
    }
    /* accept */
    while (1)
    {
        //��ȡ�Ѿ�׼���õ��������¼�
        ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
        int i;
        int fd;
        //����ѡ�ñ���
        for (i = 0; i < ret; i++)
        {
            fd = events[i].data.fd;
            //���������������ͺ��¼����ͽ��д���
            if ((fd == SerSocket->sock_fd) && (events[i].events & EPOLLIN))
            {
                handle_accpet(epollfd, SerSocket->sock_fd);
            }
            else if (events[i].events & EPOLLIN)
            {
                //memset(recv_buf, 0, sizeof(recv_buf));
                wfd = fd;
                epolfd = epollfd;
                do_read(epollfd, fd, recv_buf, tp);
            }
        }

    }
    pthread_join(thread, NULL);		//�����ȴ��߳̽���
    pthread_join(hthread, NULL);
    close(SerSocket->sock_fd);
    exit(0);
}


void handle_accpet(int epollfd, int listenfd)
{
    int client_num = 0;
    socklen_t addr_len = sizeof(struct sockaddr);
    struct sockaddr_in client_addr;
    int new_fd = accept(listenfd, (struct sockaddr*)&client_addr, &addr_len);
    if (-1 == new_fd)
    {
        fprintf(stderr, "accept error:%s\n\a", strerror(errno));
        close(listenfd);
        exit(1);
    }
    else
    {
        client_num++;
        fprintf(stderr, "Server get connetion form client%d: %s\n", client_num, inet_ntoa(client_addr.sin_addr));
        //���һ���ͻ����������¼�
        add_event(epollfd, new_fd, EPOLLIN);
    }
}


void do_read(int epollfd, int fd, char* buf, ThreadPool* tp)
{
    int nread;

    nread = read(fd, buf, sizeof(COM_PAGE));   //ÿ�ζ�ȡ�̶���������
    if (nread == -1)
    {
        perror("read error:");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    }
    else if (nread == 0)
    {
        fprintf(stderr, "client close.\n");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
        //�Ƴ������б�
    }
    else
    {
        COM_PAGE dataPack;
        int ret;
        memcpy(&dataPack, buf, sizeof(COM_PAGE));
        if (dataPack.Head.Type==0)
        {
            heartmap[fd] = 0;
            cout<<"heart!!!" << endl;
        }
        else
        {
            heartmap[fd] = 0;
            CTaskChild* tc = new CTaskChild(buf);
            CTask* task = tc;
            tp->add_task(task);    
        }
    }

}

void add_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void delete_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

void modify_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

void* produce(void* args)
{
    char shmbuff[2048] = {0};
    ShareM smr(2345, sizeof(COM_PAGE), 10);
    while (1)
    {
        //pthread_mutex_lock(&mylockA);
        smr.ShmRead(shmbuff);
        if (strlen(shmbuff)>0)
        {
            int wnum = ::write(wfd, shmbuff, sizeof(shmbuff));
            cout << "111111111111111111111111111111222222222222222222222222222-----"<<wnum<< endl;
        }
        else
        {

        }
    }

}

void* consume(void* arg)
{
    map<int, int>::iterator hit;
    while (true)
    {
        sleep(10);
        for (hit=heartmap.begin();hit!=heartmap.end();hit++)
        {
            hit->second++;
            if (hit->second==6)
            {
                fprintf(stderr, "client close.\n");
                close(wfd);
                delete_event(epolfd,wfd, EPOLLIN);
            }
        }
        cout<<"787878787" << heartmap.size() << endl;
    }
}


