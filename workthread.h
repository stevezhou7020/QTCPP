#ifndef WORKTHREAD_H
#define WORKTHREAD_H
#include <QThread>
#include <QSemaphore>
#include "myqueuecacheinfo.h"
#include "parkplotinfo.h"
#include "QCheckBox"
#include "common.h"
#include <QTextEdit>



/*每隔1S中看入队缓存是否时间到，可以转达出库缓存*/
class workThread :public QThread
{
    Q_OBJECT
public:
//    workThread(QObject* parent = nullptr,void (*p)(QString ID,QString username,unsigned int IspotNum,unsigned int istatus)= nullptr);
    explicit workThread(QObject* parent = nullptr,QObject* parent1 = nullptr,QObject* parent2 = nullptr);
    ~workThread() Q_DECL_OVERRIDE;
    void stopThread();
public:
    virtual void run() Q_DECL_OVERRIDE;

signals:
    void timeReady();
public slots:
    void threadProcess();
private:
    void refreashTextEdit(queueInfo* tempqueueinfo,int type=0);//0 正常出库 1 出库检测失败
private:
    QMutex   m_mutex;
    QSemaphore* sem;
    QCheckBox* checkbox;
    QTextEdit* TextEdit1;
    QTextEdit* TextEdit2;
    bool isStop;
//    void (*callback)(QString ID,QString usename,unsigned int, unsigned int istatus);//0 出库成功 1失败
};

#endif // WORKTHREAD_H
