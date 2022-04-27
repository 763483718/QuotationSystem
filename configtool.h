#ifndef CONFIGTOOL_H
#define CONFIGTOOL_H


#include <mutex>

#include <QList>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

const QString ConfigPath = ".\\config.txt";

class ConfigTool
{
public:
    ConfigTool();
    ~ConfigTool();

    //获取本地数据版本号
    QList<QString> getLocalVersions();

    //获取本地块的数量
    int getBlockSize();

    //获取最后一个块中的产品个数
    int getLastBlockSize();

    //获取最后一个块中的行号
    int getLastRow();

    int getFlag();

    int getLastNumber();

    //初始化
    void init();

    //备份
    void backup();
    //恢复
    void recover();

    //更新某个块的版本号
    void updateOneVersion(int index,QString version);

    void clearVersions();

    void setFlag(int flag);

    void setBlockSize(int size);

    void setLastBlockSize(int size);

    void setLastRow(int row);

    void setLastNumber(int number);

    void writeConfig();

    std::mutex configMutex;
private:
    void readConfig();

    int mFlag;              //身份 1为管理员 0为普通人员
    int mBlockSize;         //共有块的数量（包括未满的）
    int mLastBlockSize;     //最后一个块中的产品数量
    int mLastRow;           //最后一个块的最后行号
    int mLastNumber;        //最后一个编号
    QList<QString> lVersions;     //版本号


    //备份
    int mOldFlag;
    int mOldBlockSize;
    int mOldLastBlockSize;
    int mOldLastRow;
    int mOldLastNumber;
    QList<QString> lOldVersions;
};

#endif // CONFIGTOOL_H
