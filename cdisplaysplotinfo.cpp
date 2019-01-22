#include "cdisplaysplotinfo.h"

CDisplaySplotInfo::CDisplaySplotInfo(QObject* parent)
{
    parkinfoview = (QTextEdit*)parent;
    sem = new QSemaphore(1);
    sem->release();
    isStop = false;
    starttimeSeconds = 1;
}
CDisplaySplotInfo::~CDisplaySplotInfo()
{
    if(sem!=nullptr)
    {
        delete sem;
        sem=nullptr;
    }
    quit();
    wait();
}
void CDisplaySplotInfo::run()
{
    connect(this,SIGNAL(timeReady()),this,SLOT(threadProcess()));
    while(!isStop)
    {
        if(sem == nullptr) break;
        while(!sem->tryAcquire(1))
        {
            QThread::msleep(10);
        }
        emit timeReady();
        starttimeSeconds++;
        QThread::sleep(1);
    }
}
void CDisplaySplotInfo::stopThread()
{
    isStop = true;
}
void CDisplaySplotInfo::threadProcess()
{
    unsigned int num=parkplotInfo::getparkplotInstance()->getUnusedSplotNum();
    qDebug()<<"剩余停车位:"<<num;
    char cNum[10];
    std::sprintf(cNum,"%d秒",starttimeSeconds);
    char Plot[4];
    std::sprintf(Plot,"%d",num);
    QString tempdata;
    tempdata.append(tr("运行时间(秒):")).append(QString(cNum)).append(tr( " 剩余停车位: ")).append(Plot);
    parkinfoview->clear();
    parkinfoview->setPlaceholderText(tempdata);
    parkinfoview->repaint();
    parkinfoview->show();
    sem->release();
}
