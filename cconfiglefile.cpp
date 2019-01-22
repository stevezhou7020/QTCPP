#include "cconfiglefile.h"

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

Cconfiglefile* Cconfiglefile::configinstance = new Cconfiglefile();

Cconfiglefile::Cconfiglefile()
{

}
Cconfiglefile::~Cconfiglefile(){
    if(configinstance !=nullptr)
    {
        delete  configinstance;
        configinstance = nullptr;
    }
}
const Cconfiglefile* Cconfiglefile::getconfigfileInstance(){
    return configinstance;
}


bool Cconfiglefile::check_cardInfo(const QString& cardId,const QString& username) const
{
    QDir dir = getCurrentPath();
    QString filename("employee_checkin.csv");
    QFile file(dir.filePath(filename));
    qDebug()<<dir.filePath(filename) ;
    if(!file.open(QIODevice::ReadOnly)){
        qDebug("open file failed, maybe file not exist,or no rights to read");
        exit(0);
    }
    QTextStream * out = new QTextStream(&file);//文本流
    QStringList tempOption = out->readAll().split("\r\n");//每行以\r\n区分
    /*处理csv中的数据*/
    bool status = false;
    qDebug("count:%d ",tempOption.count());
    for(int i = 1 ; i < tempOption.count(); i++)
    {
         if(tempOption.at(i).isEmpty()){continue;}
         QStringList tempbar = tempOption.at(i).split(",");//一行中的单元格以，区分
         if(tempbar.isEmpty()){continue;}
         qDebug()<<tempbar[0]<< " "<<tempbar[1]<< " "<<tempbar[2]<< " "<<tempbar[3]<< " "<<tempbar[4]<<" "<<tempbar[5];
         if(QString("1") != tempbar[5]){ continue;}
         QString tempcardID = tempbar[1];
         QString tempusername = tempbar[4];
         if(tempcardID.compare(cardId) ==0 && tempusername.compare(username)==0)
         {
             status = true;
             break;
         }
    }
    file.close();//操作完成后记得关闭文件
    return status;
}
const QString Cconfiglefile::getSysconfig(const QString & fieldname ,const QString subname) const
{
    QDir dir = getCurrentPath();
    QString filename("sysconfig.ini");
    QFile file(dir.filePath(filename));

   // qDebug()<<dir.filePath(filename) ;
    if(!file.exists())
    {
        qDebug("file not exist!!");
        exit(0);
    }
    QSettings configIniRead(dir.filePath(filename), QSettings::IniFormat);
    //将读取到的ini文件保存在QString中
    QString temp="/"+fieldname+"/"+subname;
  //  qDebug()<< " configIniRead.value : "<< configIniRead.value(temp).toString();
    return configIniRead.value(temp).toString();
}
