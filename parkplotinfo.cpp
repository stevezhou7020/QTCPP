#include "parkplotinfo.h"

parkplotInfo::parkplotInfo()
{
    IparkplotNum = Cconfiglefile::getconfigfileInstance()->getSysconfig("SPARKPLOT","SPARKPLOT_MAX").toUInt();
    myparkplotmemory = new myParkplotInfoMemory(IparkplotNum);
    lock = new QReadWriteLock[IparkplotNum];
}
parkplotInfo::~parkplotInfo()
{
    if(parkplotInstance!=nullptr)
    {
        delete  parkplotInstance;
        parkplotInstance =nullptr;
    }
    if(nullptr!=myparkplotmemory)
    {
        delete myparkplotmemory;
        myparkplotmemory=nullptr;
    }

    if(nullptr!=lock)
    {
        delete lock;
        lock=nullptr;
    }
}

parkplotInfo* parkplotInfo::parkplotInstance = new parkplotInfo();

 parkplotInfo* parkplotInfo::getparkplotInstance()
{
    return parkplotInstance;
}

/*获取下一块没被占用的停车位信息*/
myparkspotInfo* parkplotInfo::getNextParkspotInfo()
{
    unsigned int count = 2;
    /*循环找一次*/
    while(count-- >0)
    {
        /*总是返回离入库口最近的车位*/
        for(unsigned int i = 0; i< IparkplotNum; i++)
        {
            if(1 == myparkplotmemory->ParkSpotInfo[i].type) continue;
            /*获取这块内存的写权限*/
            if(lock[i].tryLockForWrite())
            {
                do{
                    myparkplotmemory->ParkSpotInfo[i].type = 1;
                    myparkplotmemory->ParkSpotInfo[i].plotIDNumber = i;
                    myparkplotmemory->unUsedNumber--;
                }while(0);
                lock[i].unlock();
                return &myparkplotmemory->ParkSpotInfo[i];
            }
        }
        QThread::msleep(50);
    }
    /*没有找到空余的停车位*/
    return nullptr;
}


void parkplotInfo::releaseParkSpot(unsigned int IDNumber)
{
    if(IDNumber < IparkplotNum)
    {
        while(true)
        {
            if(lock[IDNumber].tryLockForWrite(50))
            {
                do{
                    myparkplotmemory->ParkSpotInfo[IparkplotNum].releaseParkspotInfo();
                    myparkplotmemory->unUsedNumber++;
                }while(0);
                lock[IDNumber].unlock();
                break;
            }
            QThread::msleep(100);
        }
    }
}




/*获取所有停车位信息*/
const myParkplotInfoMemory* parkplotInfo::getAllParkplotMemory()
{
    return myparkplotmemory;
}

/*获取停车位空闲数目*/
unsigned int parkplotInfo::getUnusedSplotNum()
{
    return myparkplotmemory->unUsedNumber;
}
/*获取停车位占用数目*/
unsigned int parkplotInfo::getUsedSplotNum()
{
    return IparkplotNum - myparkplotmemory->unUsedNumber;
}
/*获取停车位的总数*/
unsigned int parkplotInfo::getAllSplotNum()
{
    return IparkplotNum;
}
