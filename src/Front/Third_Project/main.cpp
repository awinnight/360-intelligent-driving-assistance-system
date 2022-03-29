#include <iostream>
#include <map>
#include "CSocketBase.h"
#include "ThreadPool.h"
using namespace std;

//pthread_mutex_t mylockA = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t mylockB = PTHREAD_MUTEX_INITIALIZER;
#define MAXSIZE     1024   //数据缓冲区
#define FDSIZE      1000   //客户端连接数量
#define EPOLLEVENTS 100   

#define MAX_CLIENT    10  //客户端最大数

int cap_num;
int wfd;
int epolfd;
map<int, int> heartmap;

//处理接收到的连接
static void handle_accpet(int epollfd, int listenfd);
//读处理
void do_read(int epollfd, int fd, char* buf, ThreadPool* tp);
//添加事件
void add_event(int epollfd, int fd, int state);
//修改事件
void modify_event(int epollfd, int fd, int state);
//删除事件
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

    /* 开始epoll流程 */
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];// 并发100个事件
    //创建一个描述符
    epollfd = epoll_create(FDSIZE);
    //添加监听描述符事件
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
        //获取已经准备好的描述符事件
        ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
        int i;
        int fd;
        //进行选好遍历
        for (i = 0; i < ret; i++)
        {
            fd = events[i].data.fd;
            //根据描述符的类型和事件类型进行处理
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
    pthread_join(thread, NULL);		//阻塞等待线程结束
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
        //添加一个客户描述符和事件
        add_event(epollfd, new_fd, EPOLLIN);
    }
}


void do_read(int epollfd, int fd, char* buf, ThreadPool* tp)
{
    int nread;

    nread = read(fd, buf, sizeof(COM_PAGE));   //每次读取固定长度数据
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
        //移出在线列表
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


