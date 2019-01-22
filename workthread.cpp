#include "workthread.h"
#include <QDebug>
#include "logger.h"

const unsigned int IsplotMax = Cconfiglefile::getconfigfileInstance()->getSysconfig("SPARKPLOT","SPARKPLOT_MAX").toUInt();
//#入库，出库线程反复调用的时间间隔(秒)
const unsigned int IspotTimeInterval = Cconfiglefile::getconfigfileInstance()->getSysconfig("REFREASHTIME","THREADINTERVIAL").toUInt();
//----------------------------------------------
//#车在库中停顿的时间(单位s)
const unsigned int IspotStoptime = Cconfiglefile::getconfigfileInstance()->getSysconfig("SPLOTTIME","EACHSPLOTTIME").toUInt();
//#读卡时间1s
const unsigned int IreadCardTime =  Cconfiglefile::getconfigfileInstance()->getSysconfig("SPLOTTIME","READCARDTIME").toUInt();
// #栏杆抬起,放下分别2s
const unsigned int Ioperatehandrail = Cconfiglefile::getconfigfileInstance()->getSysconfig("SPLOTTIME","OPERATEHANDRAIL").toUInt();
//#汽车行驶速度1s/1splot
const unsigned int Icardspeed = Cconfiglefile::getconfigfileInstance()->getSysconfig("SPLOTTIME","CARDSPEED").toUInt();
//#汽车出库，入库到行道上
const unsigned int Icardoperate = Cconfiglefile::getconfigfileInstance()->getSysconfig("SPLOTTIME","CARDOPERATE").toUInt();


//workThread::workThread(QObject* parent,void(*p)(QString ID,QString username,unsigned int IspotNum,unsigned int istatus) ):isStop(false)
//{
//    sem = new QSemaphore(1);
//    sem->release();
//    checkbox = (QCheckBox*)parent;
//    callback = p;
//}


workThread::workThread(QObject* parent ,QObject* parent1,QObject* parent2):isStop(false)
{
    sem = new QSemaphore(1);
    sem->release();
    checkbox = (QCheckBox*)parent;
    TextEdit1 =(QTextEdit*)parent1;
    TextEdit2=(QTextEdit*)parent2;
}

void workThread::run()
{
    unsigned int intervalTime = Cconfiglefile::getconfigfileInstance()->getSysconfig("REFREASHTIME","THREADINTERVIAL").toUInt();
    connect(this,SIGNAL(timeReady()),this,SLOT(threadProcess()));
    QThread::sleep(5*intervalTime);
    while(!isStop)
    {
        if(sem == nullptr) break;
        while(!sem->tryAcquire(1))
        {
            QThread::msleep(50);
        }
        emit timeReady();
        QThread::sleep(intervalTime);
    }
}
workThread::~workThread()
{
    if(sem!=nullptr)
    {
        delete sem;
        sem=nullptr;
    }
    checkbox=nullptr;
    quit();
    wait();
}

void workThread::stopThread()
{
    isStop = true;
}

/////-----------------------------------------------需要优化-------------------------
/// ---------------1.根据需求需要将出车位的车位让出来，让进来的车可以选此车位
/// ---------------2.倘若出库失败，车库车后面有几量车跟随，这些车如何安排让道，如何控制进库车和这些车争道问题
/// ---------------3.根据需求1，如果车位在车道及时让出来，进库车马上能识别有车位，进库车成功进来，出库车出去失败，出库车之前的车位被占用又没有空余的车位，导致出库车一直占道，如何处理？？？
///
/// --------------------------由于时间原因，我采用简化处理的：
/// ----------------1》车只要进库就分配车位给他，直到成功出库才释放车位。
/// ----------------2》出库车出库失败，他自己以及后面几量跟随的车都默认回到自己的车位，此时进车不与出车发生争道冲突。
///
void workThread::threadProcess()
{
    /*检查入库队列的时间，等于0，就将数据拷贝到出库队列*/
    qDebug()<<"threadProcess";
    QList<queueInfo*> & comeInfoList = myQueueCacheInfo::getQueueCashInfoInstance()->getOutFreeQueueInfoList();
    if(comeInfoList.size() == 0){
        qDebug()<<"threadProcess end ----"; sem->release();
        myQueueCacheInfo::getQueueCashInfoInstance()->releaseLock_out();
        return;
    }
    QList<queueInfo*> & outtingInfoList = myQueueCacheInfo::getQueueCashInfoInstance()->outtingInfoList;
    /*判断入库缓存，如果时间达到，复制到出库缓存，修改相关值*/
    QList<queueInfo*>::iterator comeIniter = comeInfoList.begin();
    /*这一秒变化过没有*/
    bool bLabstatus = false;
    bool bcheckststus = (checkbox->checkState() == Qt::Checked);
    for( ;comeIniter != comeInfoList.end();)
    {
        if(comeIniter.i->t()== nullptr|| !comeIniter.i->v){
             ++comeIniter;continue;
        }
        queueInfo* tempqueueInfo = comeIniter.i->t();
        if(!tempqueueInfo->isUsed) { ++comeIniter;continue;}
        if(tempqueueInfo->lefttime <= IspotTimeInterval)
        {
            //车在车库中等待的时间
            comeIniter.i->t()->lefttime = IspotStoptime;
            /*多加了IspotTimeInterval 为了下面循环容易判断*/
            comeIniter.i->t()->IoutPlottime = Icardoperate + Icardspeed * (IsplotMax-tempqueueInfo->plotIDNumber)/2 +\
                    IreadCardTime + Ioperatehandrail + Icardspeed+IspotTimeInterval;
            outtingInfoList.push_front(tempqueueInfo);
            /*因为删除会破坏迭代器*/
            QList<queueInfo*>::iterator iter_e=comeIniter++;
            comeInfoList.erase(iter_e);
            continue;
        }
        /*每个时间减去IspotTimeInterval*/
        tempqueueInfo->lefttime -= IspotTimeInterval;
         ++comeIniter;
    }

    /*判断出库缓存  相对于来说要难很多 */

    /*首先判断是否有需要出库的，同时将时间都减小IspotTimeInterval*/
    unsigned int IneedoutqueueNum = 0;
    QList<queueInfo*>::iterator tempoutIterator=outtingInfoList.begin();
    for(; tempoutIterator != outtingInfoList.end();++tempoutIterator)
    {
        queueInfo* tempoutqueueInfo = tempoutIterator.i->t();
        if(tempoutqueueInfo->isOuttingplot != 0){
            continue;
        }
        if(tempoutqueueInfo->lefttime <= IspotTimeInterval)
        {
            tempoutqueueInfo->isOuttingplot = 1;/*开始出库*/
            continue;
        }
        /*小心越界 加continue */
        tempoutqueueInfo->lefttime -= IspotTimeInterval;
    }
    QList<queueInfo*>::iterator outtingInfoIterator = outtingInfoList.begin();
    while(outtingInfoIterator != outtingInfoList.end())
    {
        queueInfo* tempoutqueueInfo = outtingInfoIterator.i->t();
        if(!tempoutqueueInfo || !tempoutqueueInfo->isUsed || tempoutqueueInfo->isOuttingplot == 0) { ++outtingInfoIterator; continue;}
        //如果剩余时间小于时间间隔 代表出库成功

        /*代表可以触发出库操作，但是不一定能出库成功*/
        /*代表有2个同时触发出库，距离出口远的先出库，没有出库的剩余时间再加上 IspotTimeInterval 没有出库的需要设置 isOuttingplot */
        if(IneedoutqueueNum >= 2)
        {
            /*距离出口远的先行 判断争道冲突的条件：IoutPlottime是否一样 */
            for(QList<queueInfo*>::iterator  item = outtingInfoList.begin();item!=outtingInfoList.end();++item)
            {
                queueInfo* tempIteminfo = item.i->t();
                if(tempIteminfo->isOuttingplot != 1 || item != outtingInfoIterator) continue;
                /*争道冲突*/
                if(tempIteminfo->IoutPlottime == tempoutqueueInfo->IoutPlottime)
                {
                    if(tempIteminfo->IoutPlottime > tempoutqueueInfo->IoutPlottime)
                    {
                        tempoutqueueInfo->isOuttingplot = 0;
                        tempoutqueueInfo->lefttime += IspotTimeInterval;
                    }
                    else{
                        /*包含 同道默认后面这个后出库*/
                        tempIteminfo->isOuttingplot = 0;
                        tempIteminfo->lefttime += IspotTimeInterval;
                    }
                }
            }
        }

        if(tempoutqueueInfo->isOuttingplot != 0)//包含 出库成功 或者 等待刷卡
        {
            /*出库失败退回到原来的车位，或者 返回离出口最近的一个空车位 */
            //出库时间比他小的都没有问题，出库时间大的就有麻烦，默认都原路返回，然后
            if(tempoutqueueInfo->IoutPlottime <= IreadCardTime + Ioperatehandrail + Icardspeed + IspotTimeInterval)
            {
                //开始检测卡
                //查看前面是否有车正在出
                if((tempoutqueueInfo->isOuttingplot == 1) && tempoutqueueInfo->IoutPlottime == IreadCardTime + Ioperatehandrail + Icardspeed +IspotTimeInterval )
                {
                    qDebug("checkstatus : %d ",bcheckststus);
                    if(bLabstatus || bcheckststus)// checkbox->checkState() 有延时
                    {
                        tempoutqueueInfo->IoutPlottime += (2*Ioperatehandrail);
                        qDebug()<<"增加时间： " << " ID: " << tempoutqueueInfo->usercardinfo.ID << " IoutPlottime : "<<tempoutqueueInfo->IoutPlottime ;
                        ++outtingInfoIterator;
                        continue;
                    }
                    else
                    {
                        bLabstatus = true;/*下一秒，这秒只能一个识别卡*/
                        if(!Cconfiglefile::getconfigfileInstance()->check_cardInfo(tempoutqueueInfo->usercardinfo.ID,tempoutqueueInfo->usercardinfo.username))
                        {   //出库卡检测失败  现在只有他一辆车在过道上
                            //过道上有几辆车，后面的车全部退回之前的车库,全部重新开始出库---------有缺陷
                            for(QList<queueInfo*>::iterator  item = outtingInfoList.begin();item!=outtingInfoList.end();++item)
                            {
                                queueInfo* tempIteminfo = item.i->t();
                                if(tempIteminfo->isOuttingplot != 1 ) continue;
                                if(tempIteminfo->IoutPlottime >= tempoutqueueInfo->IoutPlottime)
                                {
                                    tempIteminfo->isOuttingplot = 0;
                                    tempIteminfo->lefttime = Icardspeed * (IsplotMax-tempIteminfo->plotIDNumber)/2+Icardoperate;
                                    tempIteminfo->IoutPlottime = Icardoperate + Icardspeed * (IsplotMax-tempIteminfo->plotIDNumber)/2 +\
                                            IreadCardTime + Ioperatehandrail + Icardspeed+IspotTimeInterval;
                                }
                            }
                            logger::record(logger::Level::Error) << "检测卡失败 用户名：" <<  tempoutqueueInfo->usercardinfo.username<<"车牌号: "\
                                                                <<tempoutqueueInfo->usercardinfo.ID;
                            tempoutqueueInfo->isOuttingplot= 4;/*检测卡失败*/
//                            callback(tempoutqueueInfo->usercardinfo.ID,tempoutqueueInfo->usercardinfo.username,tempoutqueueInfo->plotIDNumber,1);
                            ++outtingInfoIterator;
                            refreashTextEdit(tempoutqueueInfo,1);
                            continue;//出库失败，不往下执行
                        }
                        tempoutqueueInfo->isOuttingplot = 3;//检测卡成功
                    }
                   // qDebug()<<" ID: " << tempoutqueueInfo->usercardinfo.ID << " IoutPlottime : "<<tempoutqueueInfo->IoutPlottime;
                }
                else if(tempoutqueueInfo->isOuttingplot == 3  && tempoutqueueInfo->IoutPlottime == Ioperatehandrail + Icardspeed + IspotTimeInterval)
                {
                     bLabstatus = true;
                    // myQueueCacheInfo::getQueueCashInfoInstance()->outQueuePushBack(tempoutqueueInfo->IoutPlottime);
                     qDebug()<<"检测卡成功" << " ID: " << tempoutqueueInfo->usercardinfo.ID << " IoutPlottime : "<<tempoutqueueInfo->IoutPlottime ;
                     qDebug("检测卡成功 checkstatus : %d ",bcheckststus);
                     if(!bcheckststus )
                     {
                         checkbox->setCheckState(Qt::Checked);
                         checkbox->repaint();
                         QThread::msleep(10);
                     }
                      qDebug("检测卡成功 checkstatus 2 : %d ",bcheckststus);
                }
                else if(tempoutqueueInfo->isOuttingplot == 3 && tempoutqueueInfo->IoutPlottime == IspotTimeInterval)
                {
                    bLabstatus = true;
                    /*释放车位资源*/
                    /*出库成功*/
                    qDebug()<<"出库成功"<< " ID: " << tempoutqueueInfo->usercardinfo.ID << " IoutPlottime : "<<\
                              tempoutqueueInfo->IoutPlottime << "plotIDNumber "<<tempoutqueueInfo->plotIDNumber;
                    logger::record(logger::Level::Info) << "出库成功 用户名：" <<  tempoutqueueInfo->usercardinfo.username<<"车牌号: "\
                                                        <<tempoutqueueInfo->usercardinfo.ID;
                    /*刷新控件*/
                    refreashTextEdit(tempoutqueueInfo);

                    parkplotInfo::getparkplotInstance()->releaseParkSpot(tempoutqueueInfo->plotIDNumber);
                    if(bcheckststus)
                    {
                        checkbox->setCheckState(Qt::Unchecked);
                        checkbox->repaint();
                        QThread::msleep(10);
                    }
                    //回掉函数更新控件上的值
//                    callback(tempoutqueueInfo->usercardinfo.ID,tempoutqueueInfo->usercardinfo.username,tempoutqueueInfo->plotIDNumber,0);
                    outtingInfoIterator = outtingInfoList.erase(outtingInfoIterator);
                    delete tempoutqueueInfo;
                    tempoutqueueInfo = nullptr;
                    continue;
                }
            }
            tempoutqueueInfo->IoutPlottime -= IspotTimeInterval;
        }
        ++outtingInfoIterator;
    }
    myQueueCacheInfo::getQueueCashInfoInstance()->releaseLock_out();
    sem->release();
    qDebug()<<"------- threadProcess end -----------";
}

void workThread::refreashTextEdit(queueInfo* tempoutqueueInfo,int type)
{
    QString data;
    QTextEdit *ptextEdit;
    if(tempoutqueueInfo->plotIDNumber%2 == 0)
    {
        data = TextEdit1->placeholderText();
        ptextEdit = TextEdit1;
    }else{
        data = TextEdit2->placeholderText();
        ptextEdit = TextEdit2;
    }

    QString resultData;
    if(!data.isEmpty())
    {
        QStringList templist = data.split("\r\n");
        for(int i = 0; i< templist.size();++i)
        {
            if(templist[i].isEmpty()|| templist[i].isNull()) continue;
            if(templist[i].toStdString().find(("出库检测失败")) != std::string::npos){
                resultData.append(templist[i]);
                continue;
            }
            if((templist[i].toStdString().find(tempoutqueueInfo->usercardinfo.ID) == std::string::npos \
                    || templist[0].toStdString().find(tempoutqueueInfo->usercardinfo.username) == std::string::npos))
            {
                resultData.append(templist[i]);
                if(type==1)
                {
                    resultData.append("(出库检测失败)");
                }
                 resultData.append(tr("\r\n"));
                qDebug()<<" resultdata: "<< resultData;
            }
        }
        ptextEdit->clear();
        ptextEdit->setPlaceholderText(resultData);
        ptextEdit->repaint();
        ptextEdit->show();
        QThread::msleep(10);
    }
}
