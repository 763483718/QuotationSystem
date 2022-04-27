#pragma once

#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHttpPart>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include <mutex>
#include <iostream>
#include <string>
#include <functional>

using namespace std;

enum NETSTATA{
    NET_OK,
    NET_ERROR
};

const static string csIP = "http://4h4m732737.zicp.vip";//"http://10.12.41.108";//http://manxiuleidun.vaiwan.com/swagger-ui.html//http://4h4m732737.zicp.vip
const static string csPort = "0";
const static string csRouting = "/user/findU";

const int DOWNLOADFILE_CODE = 900;

class QHttpTool : public QObject
{
    Q_OBJECT
public:
    QHttpTool();
    ~QHttpTool();
    QHttpTool(string ip,string port,string routing);

    int setIP(string ip);
    int setPort(string port);
    int setPort(int port);
    int setRouting(string routing);

    int insertData(QJsonObject data);
    int setData(QJsonObject data);
    int setFilePath(QString path);
    int insertFilePath(QString path);
    int setFileType(int type);
    int clearFilePaths();
    int clearData();

    int GET(int requestCode = 0);
    int POST(int requestCode = 0);
    int DOWNLOAD(QString url);

    int bindCallback(std::function <void(QJsonObject, int, QJsonValue)> httpCallback);
    int bindCallback(std::function <void(QJsonObject, int, QByteArray)> httpCallback);

    std::mutex mSendingMutex;

public slots:
    void interfaceRequestFinished(QNetworkReply* reply);
    void interfaceRequestError(QNetworkReply::NetworkError e);

protected:
    void requestFinished(QNetworkReply* reply,const QByteArray data,const int statusCode);

private:
    void initHttpRequest();
    /**
     * @description: 自定义网络回调函数
     * @param {
     *          QJsonObject：请求时的信息 包含statusCode（int）、errorStr(QString)、requestCode(int)、服务器传的其他内容
     *          int 请求时的requestCode
     *          QJsonValue 服务器返回数据或发送失败的错误信息 错误时存请求的url
     *        }
     * @return {*}
     * @Date: 2021-08-11 00:23:07
     * @author: guanzhou
     * @LastEditors: guanzhou
     * @LastEditTime: Do not edit
     */
    std::function<void(QJsonObject, int, QJsonValue)> fHttpCallback;
    std::function<void(QJsonObject, int, QByteArray)> fFileCallback;

    int fPOST(int requestCode);
    int fPOSTs(int requestCode);

    QNetworkRequest qHttpRequest;
    QNetworkAccessManager qNetworkAccessManager;
    QHttpMultiPart* mMultiPart;

    QMap<QNetworkReply*,int> qRequestMap;

    string sIP;
    string sPort;
    string sRouting;

    int mFileType;      //1表示文件 2表示图片
    QJsonObject qData;
    QString sFilePath;
    QList<QString> lFilePaths;

    QFile* pFile;
};
