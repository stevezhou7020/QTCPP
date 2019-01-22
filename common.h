#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <qqueue.h>
#include <QVector>
#include <QMap>
#include "cconfiglefile.h"

#define ID_NUMBER_LENGTH       12
#define CARD_USERNAME          24


/*用户卡片信息*/
struct userCardInfo
{
    char ID[ID_NUMBER_LENGTH];
    char username[CARD_USERNAME];
    userCardInfo(){
        memset(ID,0,sizeof(ID));
        memset(username,0,sizeof(username));
    }
};

/*停车位信息*/
struct parkspotInfo
{
    unsigned char type;/*0为空 1为被占用*/
    /*被占用时的ID*/
    char ID[ID_NUMBER_LENGTH];
    char username[CARD_USERNAME];
    /*还有多久出停车位的时间*/
   // unsigned int outtime;
    /*停车位的数字编号*/
    unsigned int plotIDNumber;
    /*清空缓存*/
    void releaseParkspotInfo()
    {
        type = 0;
        //outtime = 0;
        memset(ID,0,ID_NUMBER_LENGTH*sizeof(char));
        memset(username,0,CARD_USERNAME*sizeof(char));
        /*取MAX*/
        plotIDNumber = -1;
    }
    parkspotInfo(){
        type = 0;
        //outtime = 0;
        memset(ID,0,ID_NUMBER_LENGTH*sizeof(char));
        memset(username,0,CARD_USERNAME*sizeof(char));
        /*取MAX*/
        plotIDNumber = -1;
    }
};
typedef  struct parkspotInfo myparkspotInfo;

/*停车位共享内存结构体*/
struct myParkplotInfoMemory
{
    myparkspotInfo* ParkSpotInfo;
    /*剩余的停车位数目*/
    unsigned int unUsedNumber;

    myParkplotInfoMemory()= delete;
    explicit myParkplotInfoMemory(unsigned int num)
    {
        ParkSpotInfo = new myparkspotInfo[num];
        memset(ParkSpotInfo,0,num*sizeof(myparkspotInfo));
        unUsedNumber = num;
    }
    ~myParkplotInfoMemory()
    {
        delete[] ParkSpotInfo;
        ParkSpotInfo =nullptr;
        unUsedNumber=0;
    }
};

struct queueInfo
{
    struct userCardInfo usercardinfo;
    /*入库剩下的时间，触发出库操作时间*/
    unsigned int lefttime;
    /*出库需要多久时间*/
    unsigned int IoutPlottime;
    /*停车位编号*/
    unsigned int plotIDNumber;
    /*使用状态*/
    bool isUsed;
    /*触发出库操作标志 1 开始出库 3刷卡成功 4 刷卡失败*/
    unsigned int  isOuttingplot;
    queueInfo()
    {
        lefttime = 0;
        plotIDNumber = -1;
        IoutPlottime=0;
        isUsed=false;
        isOuttingplot=0;
    }
};

#endif // COMMON_H
