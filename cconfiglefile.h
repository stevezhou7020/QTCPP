#ifndef CCONFIGLEFILE_H
#define CCONFIGLEFILE_H

#include<QDir>
#include<QFile>
#include<QString>
#include<QTextStream>
#include<QStringList>
#include<QCoreApplication>
#include<QtDebug>
#include<QSettings>


/*单例模式*/
/*获取配置文件数据*/
class Cconfiglefile
{
public:
    ~Cconfiglefile();
    Cconfiglefile(const Cconfiglefile & conf)=delete;
    Cconfiglefile& operator=(const Cconfiglefile &)=delete;

    static const Cconfiglefile* getconfigfileInstance();

    /*判断出入库卡信息是否登记过*/
    bool check_cardInfo(const QString& cardId,const QString& username) const;
    /*获取系统的配置文件*/
    const QString getSysconfig(const QString & fieldname ,const QString subname) const;
private:
   Cconfiglefile();
   static Cconfiglefile* configinstance;
};

#endif // CCONFIGLEFILE_H
