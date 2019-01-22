#include "parklot.h"
#include "ui_parklot.h"
#include "logger.h"

parklot::parklot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::parklot),
    stoptimer(false)
{
    ui->setupUi(this);
    startWorkThread();
    tempComeInInfo.clear();
    ui->startBtn->setDisabled(true);
    ui->safequit->setDisabled(true);
}

parklot::~parklot()
{
    delete ui;
    if(myworkThread!=nullptr){
        delete myworkThread;
        myworkThread =nullptr;
    }
    if(mycomInparkThread != nullptr)
    {
        delete mycomInparkThread;
        mycomInparkThread = nullptr;
    }

    if(displaysplotinfo != nullptr)
    {
        delete displaysplotinfo;
        displaysplotinfo = nullptr;
    }
    for(QList<queueInfo*>::iterator item = tempComeInInfo.begin(); item != tempComeInInfo.end() ; ++item)
    {
        queueInfo* temp = item.i->t();
        if(temp == nullptr) continue;
            delete temp;
    }
    tempComeInInfo.clear();
}


/*开始初始化线程*/
void parklot::startWorkThread()
{
    logger::record(logger::Level::Info) << "startWorkThread, done.";
    /*处理入库队列成功到出库成功or失败的所有过程 加日志 */
    if(nullptr==myworkThread)
    {
//        myworkThread = new workThread(this->ui->checkOutBox,runOuttingRelationFunc);
         myworkThread = new workThread(this->ui->checkOutBox,this->ui->textEdit1,this->ui->textEdit2);
    }


    if(nullptr==mycomInparkThread)
    {
        mycomInparkThread = new ComeInPlotThread(this->ui->checkInBox,this->ui->textEdit1,this->ui->textEdit2,this->ui->checkInListView);
    }

    if(nullptr==displaysplotinfo)
    {
        displaysplotinfo = new CDisplaySplotInfo(this->ui->parkInfoView);
    }

}

void ChangeOutBoxState()
{

}

void parklot::on_safequit_clicked()
{
    myworkThread->stopThread();
    myworkThread->quit();
    displaysplotinfo->stopThread();
    mycomInparkThread->stopThread();

    displaysplotinfo->quit();
    displaysplotinfo->wait();

    mycomInparkThread->quit();
    mycomInparkThread->wait();
    myworkThread->wait();
    this->close();
}
/*刷新控件上的值*/
//void parklot::runOuttingRelationFunc(QString ID, QString username,unsigned int IspotNum, unsigned int istatus)
//{//istatus=0 出库成功，出库检测失败1
//    qDebug()<<" ID: " << ID << " username " << username;
//    if(istatus == 0)
//    {
//        if(IspotNum%2 == 0){

//        }else{

//        }
//    }
//}
static QDir getCurrentPath()
{
   QDir dir = QDir::currentPath();
   dir.cdUp();
   dir.cdUp();
   dir.cdUp();
   dir.cdUp();
   dir.cd("OOT");
   return dir;
}
void parklot::on_pushButton_clicked()
{
    QStringList  fileNameList;
    QString fileName0;
    QFileDialog* fd = new QFileDialog(this);//创建对话框
    fd->resize(240,320);    //设置显示的大小
    fd->setWindowTitle(tr("select file"));
    fd->setDirectory(getCurrentPath().path());
    fd->setNameFilter(tr("csv(*.csv)")); //设置文件过滤器
    fd->setViewMode(QFileDialog::List);  //设置浏览模式，有 列表（list） 模式和 详细信息（detail）两种方式
    if(fd->exec() == QDialog::Accepted )   //如果成功的执行
    {
        QStringList fileNameList = fd->selectedFiles();      //返回文件列表的名称

        QFile file(fileNameList[0]);
        qDebug()<<"filename :"<<fileNameList[0];
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::about(this,tr("文件错误"),tr("导入文件错误,可能不存在或者没有权限读，请重新导入"));
            return;
        }
        QTextStream * out = new QTextStream(&file);//文本流
        QStringList tempOption = out->readAll().split("\r\n");//每行以\r\n区分
        /*处理csv中的数据*/
        for(int i = 1 ; i < tempOption.count(); i++)
        {
             if(tempOption.at(i).isEmpty()){continue;}
             QStringList tempbar = tempOption.at(i).split(",");//一行中的单元格以，区分
             if(tempbar.isEmpty()){continue;}
             qDebug("size : %d",tempbar.size());
             if(tempbar.size() != 5)
             {
                 QMessageBox::about(this,tr("文件格式错误"),tr("导入文件错误，请重新导入"));
                 return;
             }

             QString tempcardID = tempbar[1];
             QString tempusername = tempbar[4];

             queueInfo* myqueueInfo = new queueInfo();

             strncpy(myqueueInfo->usercardinfo.ID,tempcardID.toStdString().c_str(),tempcardID.toStdString().size());
             strncpy(myqueueInfo->usercardinfo.username,tempusername.toStdString().c_str(),tempusername.toStdString().size());

             qDebug()<<"usernae ID "<<tempcardID << " " << " username : "<< tempusername;
             tempComeInInfo.push_back(myqueueInfo);
             //myQueueCacheInfo::getQueueCashInfoInstance()->comeInInfoList.push_back(myqueueInfo);
        }
        file.close();//操作完成后记得关闭文件
        ui->startBtn->setEnabled(true);
        ui->pushButton->setEnabled(false);
    }
    qDebug()<<" comin info size : "<< tempComeInInfo.size();
    fd->close();
    delete fd;
}

void parklot::on_startBtn_clicked()
{
    /*启动线程*/
    ui->startBtn->setDisabled(true);
    mycomInparkThread->setQueueInfo(&tempComeInInfo);

    if(!mycomInparkThread->isRunning())
    {
       mycomInparkThread->start();
    }

    if(!myworkThread->isRunning())
    {
       myworkThread->start();
    }   
    if(!displaysplotinfo->isRunning())
    {
       displaysplotinfo->start();
    }

    ui->safequit->setEnabled(true);
    ui->pushButton->setEnabled(false);

}
