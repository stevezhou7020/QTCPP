#ifndef COMEINPLOTTHREAD_H
#define COMEINPLOTTHREAD_H
#include <QThread>
#include "cconfiglefile.h"
#include <QSemaphore>
#include "myqueuecacheinfo.h"
#include "parkplotinfo.h"
#include <QCheckBox>
#include<QTextEdit>

class ComeInPlotThread : public QThread
{
    Q_OBJECT
public:
     explicit ComeInPlotThread(QObject* parent = nullptr,QObject* parent1 = nullptr,QObject* parent2 = nullptr,QObject* parent3 = nullptr);
    ~ComeInPlotThread() Q_DECL_OVERRIDE;
    void setQueueInfo(QList<queueInfo*>* queueinfo);
    void refreashTextEdit(queueInfo* tempoutqueueInfo);
 public:
     virtual void run() Q_DECL_OVERRIDE;
 signals:
     void comeinplottimeReady();
 public slots:
     void comeinplotthreadProcess();
 private:
     QCheckBox* checkbox;
     QTextEdit* TextEdit1;
     QTextEdit* TextEdit2;
     QTextEdit* TextCheckView;
     bool isStop;
     QMutex   m_mutex;
     QSemaphore* sem;
     QList<queueInfo*>* tempqueueInfo;
     QString readyInUsers;
     int number;
 public:
     void stopThread();
};

#endif // COMEINPLOTTHREAD_H
