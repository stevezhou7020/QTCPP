#ifndef PARKLOT_H
#define PARKLOT_H

#include <QMainWindow>
#include "common.h"
#include "cconfiglefile.h"
#include "parkplotinfo.h"
#include "workthread.h"
#include "comeinplotthread.h"
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include "cdisplaysplotinfo.h"


namespace Ui {
class parklot;
}

class parklot : public QMainWindow
{
    Q_OBJECT
public:
    explicit parklot(QWidget *parent = nullptr);
    ~parklot();
    /*启动线程*/
    void startWorkThread();
//   static void runOuttingRelationFunc(QString sID, QString susername, unsigned int IspotNum,unsigned int istatus);
private slots:
    void on_safequit_clicked();
    //导入入库资源
    void on_pushButton_clicked();
    //仿真正式开始
    void on_startBtn_clicked();
public:
    /*临时存储*/
    QList<queueInfo*> tempComeInInfo;
private:
    Ui::parklot *ui;
    bool stoptimer;
    QString username;
    QString ID;
    unsigned int istatus;/*0 出库成功 1 出库失败*/
private:
    /*启动线程*/
     workThread* myworkThread;
     /*入库线程*/
     ComeInPlotThread* mycomInparkThread;
    /*刷新停车场信息*/
     CDisplaySplotInfo* displaysplotinfo;
};

#endif // PARKLOT_H
