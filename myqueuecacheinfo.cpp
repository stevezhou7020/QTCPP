#include "myqueuecacheinfo.h"

/*单例模式*/
myQueueCacheInfo* myQueueCacheInfo::myqueuecacheinfo = new myQueueCacheInfo();

myQueueCacheInfo::~myQueueCacheInfo()
{
    for(QList<queueInfo*>::iterator item = comeInInfoList.begin(); item != comeInInfoList.end() ; ++item)
    {
        queueInfo* temp = item.i->t();
        if(temp == nullptr) continue;
        delete temp;
    }
    comeInInfoList.clear();

    for(QList<queueInfo*>::iterator item = outtingInfoList.begin(); item != outtingInfoList.end() ; ++item)
    {
        queueInfo* temp = item.i->t();
        if(temp == nullptr) continue;
        delete temp;
    }
    outtingInfoList.clear();

    if(myqueuecacheinfo != nullptr)
    {
        delete myqueuecacheinfo;
        myqueuecacheinfo = nullptr;
    }
}

myQueueCacheInfo* myQueueCacheInfo::getQueueCashInfoInstance()
{
    return myqueuecacheinfo;
}

myQueueCacheInfo::myQueueCacheInfo()
{
    outtingInfoList.clear();
}

QList<queueInfo*>& myQueueCacheInfo::getOutFreeQueueInfoList()
{
    while(1)
    {
        if(lock_out.tryLockForWrite())
        {
             return comeInInfoList;
        }
        QThread::msleep(50);
    }
}

void myQueueCacheInfo::releaseLock_out()
{
    lock_out.unlock();
}
//int  myQueueCacheInfo::outQueueSize()
//{
//    return outingqueue.size();
//}
//unsigned int  myQueueCacheInfo::outQueuePop()
//{
//    unsigned int temp;
//    while(1)
//    {
//        if(lock.tryLockForWrite())
//        {
//            temp = outingqueue.first();
//            outingqueue.pop_front();
//            lock.unlock();
//            return temp;
//        }
//        QThread::msleep(10);
//    }
//}
//void myQueueCacheInfo::outQueuePushBack(unsigned int seconds)
//{
//    while(1)
//    {
//        if(lock.tryLockForWrite())
//        {
//            outingqueue.push_back(seconds);
//            lock.unlock();
//            break;
//        }
//        QThread::msleep(10);
//    }
//}

