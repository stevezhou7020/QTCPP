#include "comeinplotthread.h"
#include "logger.h"

unsigned int intervalTime = Cconfiglefile::getconfigfileInstance()->getSysconfig("REFREASHTIME","THREADINTERVIAL").toUInt();
//#入库，出库线程反复调用的时间间隔(秒)
unsigned int IspotTimeInterval = Cconfiglefile::getconfigfileInstance()->getSysconfig("REFREASHTIME","THREADINTERVIAL").toUInt();
// #栏杆抬起,放下分别2s
unsigned int Ioperatehandrail = Cconfiglefile::getconfigfileInstance()->getSysconfig("SPLOTTIME","OPERATEHANDRAIL").toUInt();
//#汽车行驶速度1s/1splot
unsigned int Icardspeed = Cconfiglefile::getconfigfileInstance()->getSysconfig("SPLOTTIME","CARDSPEED").toUInt();
//#汽车出库，入库到行道上
unsigned int Icardoperate = Cconfiglefile::getconfigfileInstance()->getSysconfig("SPLOTTIME","CARDOPERATE").toUInt();


ComeInPlotThread::ComeInPlotThread(QObject* parent,QObject* parent1,QObject* parent2,QObject* parent3)
{
   sem = new QSemaphore(1);
   sem->release();
   isStop = false;
   number=5;
   checkbox = (QCheckBox*)parent;
   TextEdit1 =(QTextEdit*)parent1;
   TextEdit2=(QTextEdit*)parent2;
   TextCheckView=(QTextEdit*)parent3;
   readyInUsers.clear();
}

void ComeInPlotThread::setQueueInfo(QList<queueInfo*>* ptempqueueinfo)
{
    tempqueueInfo = ptempqueueinfo;
}

ComeInPlotThread::~ComeInPlotThread()
{
    if(sem!=nullptr)
    {
        delete sem;
        sem=nullptr;
    }
    readyInUsers.clear();
    quit();
    wait();
}
void ComeInPlotThread::run()
{
    connect(this,SIGNAL(comeinplottimeReady()),this,SLOT(comeinplotthreadProcess()));
    while(!isStop)
    {
        if(sem == nullptr) break;
        while(!sem->tryAcquire(1))
        {
            QThread::msleep(500);
        }
        QThread::sleep(intervalTime);
        emit comeinplottimeReady();
    }
}

void ComeInPlotThread::stopThread()
{
    isStop = true;
}

void ComeInPlotThread::comeinplotthreadProcess()
{
    if(number == 0)
    {
        QList<queueInfo*>::iterator item = tempqueueInfo->begin();
        for(;item!= tempqueueInfo->end(); ++item)
        {
            queueInfo* myqueueInfo = item.i->t();
            if(readyInUsers.toStdString().find(myqueueInfo->usercardinfo.username) == std::string::npos)
            {
                 readyInUsers.append(myqueueInfo->usercardinfo.username);
                 qDebug()<<"add data to queue: ID: "<< item.i->t()->usercardinfo.ID << " username: "<< item.i->t()->usercardinfo.username;

                 /*验证合法性*/
                 if(!Cconfiglefile::getconfigfileInstance()->check_cardInfo(myqueueInfo->usercardinfo.ID,myqueueInfo->usercardinfo.username))
                 {
                     //非法车辆
                     QString tempdata;
                     tempdata.append(tr("非法车辆 用户名: ")).append(myqueueInfo->usercardinfo.username).append(tr(" 车牌号: ")).append(myqueueInfo->usercardinfo.ID).append(tr("\r\n"));
                     TextCheckView->setPlaceholderText(TextCheckView->placeholderText()+tempdata);
                     TextCheckView->repaint();
                     TextCheckView->show();
                     QThread::msleep(10);
                     logger::record(logger::Level::Error) << "非法车辆 用户名：" <<  myqueueInfo->usercardinfo.username<<"车牌号: "\
                                                         <<myqueueInfo->usercardinfo.ID;
                     tempqueueInfo->erase(item);
                     sem->release(); 
                     return;
                 }
                /*是否有多余的车位*/
                 myparkspotInfo* myparkinfo;
                 while((myparkinfo = parkplotInfo::getparkplotInstance()->getNextParkspotInfo()) == nullptr)
                 {
                     QString tempdata;
                     tempdata.append(tr("没有多余的车位请等待 用户名: ")).append(myqueueInfo->usercardinfo.username).append(tr(" 车牌号: ")).append(myqueueInfo->usercardinfo.ID).append(tr("\r\n"));
                     QString checkString = TextCheckView->placeholderText();
                     if(checkString.toStdString().find(tempdata.toStdString()) == std::string::npos)
                     {
                         TextCheckView->setPlaceholderText(TextCheckView->placeholderText()+tempdata);
                         TextCheckView->repaint();
                         TextCheckView->show();
                         QThread::msleep(10);
                     }
                     logger::record(logger::Level::Warning) << "没有多余的车位请等待 用户名：" <<  myqueueInfo->usercardinfo.username<<"车牌号: "\
                                                         <<myqueueInfo->usercardinfo.ID;
                    QThread::sleep(3*intervalTime);
                 }
                 myqueueInfo->isUsed=true;
                 //停车场占用
                 strcpy(myparkinfo->ID,myqueueInfo->usercardinfo.ID);
                 strcpy(myparkinfo->username,myqueueInfo->usercardinfo.username);
                 myparkinfo->type=1;
                 myqueueInfo->lefttime = Icardspeed*(myparkinfo->plotIDNumber/2)+Ioperatehandrail+Icardoperate+IspotTimeInterval;
                 myqueueInfo->plotIDNumber = myparkinfo->plotIDNumber;

                 myQueueCacheInfo::getQueueCashInfoInstance()->getOutFreeQueueInfoList().push_back(myqueueInfo);
                 myQueueCacheInfo::getQueueCashInfoInstance()->releaseLock_out();

                 QString tempdata;
                 logger::record(logger::Level::Info) << "入库成功 用户名：" <<  myqueueInfo->usercardinfo.username<<"车牌号: "\
                                                     <<myqueueInfo->usercardinfo.ID;
                 tempdata.append(tr("入库成功 用户名: ")).append(myqueueInfo->usercardinfo.username).append(tr(" 车牌号: ")).append(myqueueInfo->usercardinfo.ID).append(tr("\r\n"));
                 TextCheckView->clear();
                 TextCheckView->setPlaceholderText(tempdata);
                 TextCheckView->repaint();
                 TextCheckView->show();
                 QThread::msleep(10);
                 refreashTextEdit(myqueueInfo);
                 if(checkbox->checkState() == Qt::Unchecked)
                 {
                     checkbox->setCheckState(Qt::Checked);
                     checkbox->repaint();
                     QThread::msleep(10);
                     number=5;
                 }  
                 break;
            }  
        }
        if(item == tempqueueInfo->end())
        {
            TextCheckView->clear();
            TextCheckView->repaint();
            QThread::msleep(10);
            readyInUsers.clear();
            isStop=true;
        }
    }
   else
   {
       if(number-- ==3)
       {
           if(checkbox->checkState() == Qt::Checked)
           {
               checkbox->setCheckState(Qt::Unchecked);
               checkbox->repaint();

               TextCheckView->clear();
               TextCheckView->repaint();
               QThread::msleep(10);
           }
       }
    }
    sem->release();
    qDebug()<< "结束入库的线程--------";
}


void ComeInPlotThread::refreashTextEdit(queueInfo* tempoutqueueInfo)
{

    QTextEdit *ptextEdit;
    if(tempoutqueueInfo->plotIDNumber%2 == 0)
    {
        ptextEdit = TextEdit1;
    }else{
        ptextEdit = TextEdit2;
    }
    QString resultData;
    char Plot[6];
    std::sprintf(Plot,"%d",10000+tempoutqueueInfo->plotIDNumber);
    resultData.append(tempoutqueueInfo->usercardinfo.ID).append(" | ").append(tempoutqueueInfo->usercardinfo.username)\
            .append(tr(" | 车位号:")).append(Plot).append("\r\n");
    ptextEdit->setPlaceholderText(ptextEdit->placeholderText()+resultData);
    ptextEdit->repaint();
    ptextEdit->show();

}
