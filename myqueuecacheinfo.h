#ifndef MYQUEUECACHEINFO_H
#define MYQUEUECACHEINFO_H
#include "common.h"
#include <QList>
#include <QQueue>
#include <QReadWriteLock>
#include <QThread>
/*单例模式*/
class myQueueCacheInfo
{
public:
    ~myQueueCacheInfo();
    myQueueCacheInfo(const myQueueCacheInfo&) =delete ;
    myQueueCacheInfo& operator=(const myQueueCacheInfo&) =delete;

    static myQueueCacheInfo* getQueueCashInfoInstance();

public:
    /**入库队列信息*/
    QList<queueInfo*> comeInInfoList;
    /*出库队列信息*/
    QList<queueInfo*>  outtingInfoList;

    QList<queueInfo*>& getOutFreeQueueInfoList();

     void releaseLock_out();
private:
    static myQueueCacheInfo* myqueuecacheinfo;
    /*出库抬栏杆队列*/
    QQueue<unsigned int> outingqueue;

    QReadWriteLock lock_out;
    myQueueCacheInfo();
};
#endif // MYQUEUECACHEINFO_H
