#ifndef CDISPLAYSPLOTINFO_H
#define CDISPLAYSPLOTINFO_H
#include <QThread>
#include "myqueuecacheinfo.h"
#include "QCheckBox"
#include <QTextEdit>
#include <QSemaphore>
#include <QDateTime>
#include "parkplotinfo.h"

class CDisplaySplotInfo : public QThread
{
    Q_OBJECT
public:
    explicit CDisplaySplotInfo(QObject* parent = nullptr);
    ~CDisplaySplotInfo() Q_DECL_OVERRIDE;
    void stopThread();
public:
    virtual void run() Q_DECL_OVERRIDE;
signals:
    void timeReady();
public slots:
    void threadProcess();
private:
    void refreashTextEdit(queueInfo* tempqueueinfo);
private:
    QMutex   m_mutex;
    QSemaphore* sem;
    QTextEdit* parkinfoview;
    bool isStop;
    unsigned int starttimeSeconds;
};

#endif // CDISPLAYSPLOTINFO_H
