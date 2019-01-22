#ifndef PARKPLOTINFO_H
#define PARKPLOTINFO_H
/*主要用来处理 停车场相关信息*/
#include "common.h"
#include "cconfiglefile.h"
#include <QReadWriteLock>
#include <QThread>

/*单例模式*/
class parkplotInfo
{
public:
    static parkplotInfo* getparkplotInstance();

    parkplotInfo(const parkplotInfo * info)=delete;
    parkplotInfo& operator=(const parkplotInfo & info)=delete;

    ~parkplotInfo();
    /*获取下一块没被占用的停车位信息*/
    myparkspotInfo* getNextParkspotInfo();
    /*获取所有停车位信息*/
    const myParkplotInfoMemory* getAllParkplotMemory();
    /*释放车位资源*/
    void releaseParkSpot(unsigned int IDNumber);

    /****************以下方法只是作为显示数据时候调用，不能用作多线程流程处理(结果有延时)*******************/
    /*获取停车位的总数*/
    unsigned int getAllSplotNum();
    /*获取停车位空闲数目*/
    unsigned int getUnusedSplotNum();
    /*获取停车位占用数目*/
    unsigned int getUsedSplotNum() ;
private:
    parkplotInfo();
    myParkplotInfoMemory* myparkplotmemory;
    static  parkplotInfo* parkplotInstance;
    unsigned int IparkplotNum{0};
    QReadWriteLock* lock;
};

#endif // PARKPLOTINFO_H
