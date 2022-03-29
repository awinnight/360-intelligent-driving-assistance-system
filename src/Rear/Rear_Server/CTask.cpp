#include "CTask.h"
extern map<string, map<int, PICTURE_PAGE>> BigMap;
extern map<int, VideoTable*> Vlmap;
extern map<int, PictureTable*> Plmap;

CTask::CTask()
{
}

CTask::~CTask()
{
}

CTaskChild::CTaskChild(char* buf)
{
    smw = new ShareM(2345, sizeof(COM_PAGE), 10);
	buff = (char*)malloc(sizeof(COM_PAGE));
	memset(buff, 0, sizeof(COM_PAGE));
	memcpy(buff, buf, sizeof(COM_PAGE));

    lock = 1;
}

CTaskChild::~CTaskChild()
{
	delete this;
}

void CTaskChild::run()
{
    char shmbuff[2048] = {0};
    COM_PAGE dataPack;
    int ret;
    memcpy(&dataPack, buff, sizeof(COM_PAGE));
    int funcode = dataPack.Head.Type;
    int optcode = dataPack.Head.Operation;
    cout << funcode << endl;
    cout << optcode << endl;
    this->usermodel = UserModel::getinstace();
    this->vimodel = VideoModel::getinstace();
    this->picmodel = PictureModel::getinstace();

    switch (funcode)
    {
    case LOGIN:                                                             //客户端登录
    {   
        //获取用户名、密码
        LOG_PAGE  login = { 0 };
        COM_PAGE  echo = { 0 };
        echo.Head.Cfd = dataPack.Head.Cfd;
        echo.Head.Len = dataPack.Head.Len;
        echo.Head.Page_ID = dataPack.Head.Page_ID;
        echo.Head.UID = dataPack.Head.UID;
        strcpy(echo.Head.CRC, dataPack.Head.CRC);
        echo.Head.Type = LOGIN_ECHO;

        memcpy(&login, dataPack.Page_Content, sizeof(LOG_PAGE));
        printf("read message is : name=%s, password=%s, sockfd=%d \n", login.UAccount, login.UPwd, login.Cfd);
        ret=usermodel->selectUserByAP(login.UAccount, login.UPwd); 
        if (ret>0)
        {
            cout << ret << endl;            //查询结果
            printf("login module!\n");

            echo.Head.Operation = 1;
            echo.Head.UID = ret;
            strcpy(echo.Page_Content, "echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);
        }
        else
        {
            cout << ret << endl;            //查询结果
            printf("login failed!\n");
            
            echo.Head.Operation = 0;
            echo.Head.UID = ret;
            strcpy(echo.Page_Content, "echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);

        }
        
    }
    break;
    case VIDEO_INFO:                                                        //视频采集包
    {
        COM_PAGE  echo = { 0 };
        echo.Head.Cfd = dataPack.Head.Cfd;
        echo.Head.Len = dataPack.Head.Len;
        echo.Head.Page_ID = dataPack.Head.Page_ID;
        echo.Head.UID = dataPack.Head.UID;
        strcpy(echo.Head.CRC, dataPack.Head.CRC);
        echo.Head.Type = VINFO_ECHO;

        VINFO_PAGE vinfo = { 0 };
        bzero(&vinfo, sizeof(VINFO_PAGE));
        memcpy(&vinfo, dataPack.Page_Content, sizeof(VINFO_PAGE));

        char xx[12] = {0};
        sprintf(xx,"%f",vinfo.X);
        ret = vimodel->insertVideoByDFFNH(vinfo.UID,vinfo.FrameCount,1,xx,vinfo.VCurrtime,vinfo.FirstFrame_Url,vinfo.Video_Url);
        if (ret==-1)
        {
            printf("collect succeed!\n");
            echo.Head.Operation = 1;
            strcpy(echo.Page_Content, "video echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);
        }
        else
        {
            printf("collect failed!\n");
            echo.Head.Operation = 0;
            strcpy(echo.Page_Content, "video echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);
        }

    }
    break;
    case VIDEO_LIST_REQUEST:                                                //视频列表请求包
    {
        map<int, VideoTable*>::iterator vit;

        COM_PAGE  echo = { 0 };
        echo.Head.Cfd = dataPack.Head.Cfd;
        echo.Head.Len = dataPack.Head.Len;
        echo.Head.Page_ID = dataPack.Head.Page_ID;
        echo.Head.UID = dataPack.Head.UID;
        strcpy(echo.Head.CRC, dataPack.Head.CRC);
        echo.Head.Type = VIDEO_LIST;
        VLIST_PAGE vlist = { 0 };
        bzero(&vlist, sizeof(VLIST_PAGE));
        memcpy(&vlist, dataPack.Page_Content, sizeof(VLIST_PAGE));

        ret = vimodel->selectVideoByDate(dataPack.Head.UID);
        if (ret==0)
        {
            printf("get video list succeed!Listmap size:%d\n",Vlmap.size());
            for (vit=Vlmap.begin();vit!=Vlmap.end();vit++)
            {
                vlist.VID = vit->first;
                vlist.X = atof(vit->second->getX().c_str());
                strcpy(vlist.VCurrTime,vit->second->getCurrentTime().c_str());
                strcpy(vlist.FirstFrame_Url, vit->second->getFirstFrame().c_str());
                strcpy(vlist.Video_Url, vit->second->getVideoUrl().c_str());
                echo.Head.Operation = 1;

                memcpy(&echo.Page_Content,&vlist,sizeof(VLIST_PAGE));
                memset(shmbuff, 0, sizeof(shmbuff));
                memcpy(shmbuff, &echo, sizeof(COM_PAGE));
                smw->ShmWrite(shmbuff);
            }
        }
        else
        {
            printf("get video list failed!\n");
            echo.Head.Operation = 0;
            strcpy(echo.Page_Content, "echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);
        }
        Vlmap.clear();
    }
    break;
    case VIDEO_RECORD:
    {
        COM_PAGE  echo = { 0 };
        echo.Head.Cfd = dataPack.Head.Cfd;
        echo.Head.Len = dataPack.Head.Len;
        echo.Head.Page_ID = dataPack.Head.Page_ID;
        echo.Head.UID = dataPack.Head.UID;
        strcpy(echo.Head.CRC, dataPack.Head.CRC);
        echo.Head.Type = VRECORD_ECHO;

        VRECORD_PAGE vrecord = { 0 };
        bzero(&vrecord, sizeof(VRECORD_PAGE));
        memcpy(&vrecord, dataPack.Page_Content, sizeof(VRECORD_PAGE));
        char xx[12] = {0};
        sprintf(xx,"%f",vrecord.X);
        ret = vimodel->updateVideoByID(vrecord.VID,vrecord.CurrFrame,xx,vrecord.VCurrTime);
        if (ret==0)
        {
            printf("updata succeed!\n");
            echo.Head.Operation = 1;
            strcpy(echo.Page_Content, "echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);
        }
        else
        {
            printf("updata failed!\n");
            echo.Head.Operation = 0;
            strcpy(echo.Page_Content, "echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);
        }

    }
    break;

    case REGISTER:                                                          // 注册
    {
        //获取用户名、密码
        LOG_PAGE  reg = { 0 };
        COM_PAGE  echo = { 0 };
        echo.Head.Cfd = dataPack.Head.Cfd;
        echo.Head.Len = dataPack.Head.Len;
        echo.Head.Page_ID = dataPack.Head.Page_ID;
        echo.Head.UID = dataPack.Head.UID;
        strcpy(echo.Head.CRC, dataPack.Head.CRC);
        echo.Head.Type = REGISTER_ECHO;

        memcpy(&reg, dataPack.Page_Content, sizeof(LOG_PAGE));
        printf("read message is : name=%s, password=%s, sockfd=%d \n", reg.UAccount, reg.UPwd, reg.Cfd);
        ret = usermodel->insertUserByANP(reg.UAccount, reg.UPwd);
        if (ret == -1)
        {
            cout << ret << endl;            //查询结果
            printf("register module!\n");
            echo.Head.Operation = 1;
            strcpy(echo.Page_Content, "echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);
        }
        else
        {
            cout << ret << endl;            //查询结果
            printf("register failed!\n");
            echo.Head.Operation = 0;
            strcpy(echo.Page_Content, "echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);
        }
    }
    break;
    case PICTURE:                                               //图片包
    {
        PICTURE_PAGE picture = {0};
        COM_PAGE  echo = { 0 };
        echo.Head.Cfd = dataPack.Head.Cfd;
        echo.Head.Len = dataPack.Head.Len;
        echo.Head.Page_ID = dataPack.Head.Page_ID;
        echo.Head.UID = dataPack.Head.UID;
        strcpy(echo.Head.CRC, dataPack.Head.CRC);
        echo.Head.Type = PINFO_ECHO;

        bzero(&picture, sizeof(PICTURE_PAGE));
        memcpy(&picture, dataPack.Page_Content, sizeof(PICTURE_PAGE));
        map<int, PICTURE_PAGE> ::iterator it1 = BigMap[picture.SpecialPicture].begin();
        std::cout << "-----------------------" << endl;
        if (dataPack.Head.Operation == 0)                   //接收图片包
        {
            if (picture.flag == 0)              //图片第一个小包
            {
                //  totalmap.insert(make_pair(picture.picuture_name, smallmap));
                BigMap[picture.SpecialPicture][picture.PageNum] = picture;
                std::cout << "picture name=====" << picture.SpecialPicture << endl;
            }
            else if (picture.flag == 1)             //图片后续小包
            {
                //smallmap[picture.picturenum] = picture;
                BigMap[picture.SpecialPicture][picture.PageNum] = picture;
                std::cout << picture.SpecialPicture<< "===========" << picture.PageNum << endl;
                std::cout << "SmallMap size========" << BigMap[picture.SpecialPicture].size() << endl;
                
            }
            
        }
        else if (dataPack.Head.Operation == 1)                 //组装图片包
        {
            lock = 0;
            std::cout << "wait!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
            std::cout << "222222222222" << endl;
            map<string, map<int, PICTURE_PAGE>> ::iterator it2;
            std::cout << "333333333333333333" << endl;
            for (it2 = BigMap.begin(); it2 != BigMap.end(); it2++)
            {
                if (it2->first == picture.SpecialPicture)
                {
                    std::cout << "555555555555" << endl;
                    break;
                }
            }
            std::cout << "bigmap size========" << BigMap.size() << endl;
            int y = 0;
            FILE* fp;
            char PicName[64];
            sprintf(PicName, "%s%s","/opt/Picture/", it2->first.c_str());
            fp = fopen(PicName, "wb+");
            std::cout << "smallmap size========" << BigMap[picture.SpecialPicture].size() << endl;
            while (it1 != BigMap[picture.SpecialPicture].end())
            {
                std::cout << "loading..........." << endl;
                if (it2->first == it1->second.SpecialPicture)
                {
                    if (it1->second.flag == 0)
                    {
                        BigMap[picture.SpecialPicture].erase(it1++);
                        cout << "no des page========" << endl;
                    }
                    w_size = fwrite(it1->second.PicBuf, 1, 912, fp);
                    BigMap[picture.SpecialPicture].erase(it1++);
                    x += w_size;
                    std::cout << "xxxxxxxxxxxxxxxxx========" << x << endl;
                    std::cout << "yyyyyyyyyyyyyyyyy========" << y << endl;
                    std::cout << "small mapsize===========" << BigMap[picture.SpecialPicture].size() << endl;

                }
                else
                {
                    it1++;
                    std::cout << "................." << endl;
                    y++;
                }

            }
            fclose(fp);
            ret = this->picmodel->insertPicture(picture.UID,picture.VID,picture.SpecialPicture,picture.PictureUrl);

            echo.Head.Operation = 1;
            strcpy(echo.Page_Content, "echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);
            lock = 1;
         
        }
        else
        {
            std::cout << "get picture failed!!!" << endl;
            echo.Head.Operation = 0;
            strcpy(echo.Page_Content,"echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);
        }

    }
    break;
    
    case PICTURE_LIST_REQUEST:                                                //图片列表请求包
    {
        map<int, PictureTable*>::iterator pit;
        COM_PAGE  echo = { 0 };
        echo.Head.Cfd = dataPack.Head.Cfd;
        echo.Head.Len = dataPack.Head.Len;
        echo.Head.Page_ID = dataPack.Head.Page_ID;
        echo.Head.UID = dataPack.Head.UID;
        strcpy(echo.Head.CRC, dataPack.Head.CRC);
        echo.Head.Type = PICTURE_LIST;
        PLIST_PAGE plist = { 0 };
        bzero(&plist, sizeof(PLIST_PAGE));
        plist.UID = dataPack.Head.UID;
        ret = picmodel->selectPicture(dataPack.Head.UID);
        if (ret == 0)
        {
            printf("get picture list succeed!Plmap size:%d\n", Plmap.size());
            for (pit = Plmap.begin(); pit != Plmap.end(); pit++)
            {
                plist.PID = pit->first;
                plist.VID = pit->second->getVID();
                strcpy(plist.PictureUrl,pit->second->getPUrl().c_str());
                echo.Head.Operation = 1;

                memcpy(&echo.Page_Content, &plist, sizeof(PLIST_PAGE));
                memset(shmbuff, 0, sizeof(shmbuff));
                memcpy(shmbuff, &echo, sizeof(COM_PAGE));
                smw->ShmWrite(shmbuff);
            }
        }
        else
        {
            printf("get picture list failed!\n");
            echo.Head.Operation = 0;
            strcpy(echo.Page_Content, "echo");
            memset(shmbuff, 0, sizeof(shmbuff));
            memcpy(shmbuff, &echo, sizeof(COM_PAGE));
            smw->ShmWrite(shmbuff);
        }
        Plmap.clear();
    }
    break;

    }
}
