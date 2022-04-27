/*
 * @Description:
 * @Version:
 * @Autor: guanzhou
 * @email: guanzhou.cn@gmail.com
 * @Date: 2021-08-08 15:34:11
 * @LastEditors: guanzhou
 * @LastEditTime: 2021-08-11 23:59:20
 */
#include "qhttptool.h"

QHttpTool::QHttpTool()
{
    sIP = csIP;
    sPort = csPort;
    sRouting = csRouting;
    initHttpRequest();

    mMultiPart = nullptr;

    QObject::connect(&qNetworkAccessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(interfaceRequestFinished(QNetworkReply*)));
}

QHttpTool::QHttpTool(string ip,string port,string routing)
{
    initHttpRequest();
    sIP = ip;
    sPort = port;
    sRouting = routing;

    mMultiPart = nullptr;

    QObject::connect(&qNetworkAccessManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(interfaceRequestFinished(QNetworkReply*)));
}

QHttpTool::~QHttpTool()
{
    qNetworkAccessManager.disconnect();
}

void QHttpTool::initHttpRequest()
{
//    qHttpRequest.setRawHeader("Content-Type", "charset='utf-8'");
//    qHttpRequest.setRawHeader("Content-Type",  "application/json");
}

int QHttpTool::setIP(string ip)
{
    sIP = ip;
    return NET_OK;
}

int QHttpTool::setPort(string port)
{
    sPort = port;
    return NET_OK;
}

int QHttpTool::setPort(int port)
{
    sPort = std::to_string(port);
    return NET_OK;
}

int QHttpTool::setRouting(string routing)
{
    sRouting = routing;
    return NET_OK;
}

int QHttpTool::insertData(QJsonObject data)
{
    for(auto p = data.begin();p!=data.end();p++){
        qData.insert(p.key(),p.value());
    }
    return NET_OK;
}

int QHttpTool::setData(QJsonObject data)
{
    qData = data;
    return NET_OK;
}

int QHttpTool::setFilePath(QString path)
{
    sFilePath = path;
    return NET_OK;
}

int QHttpTool::insertFilePath(QString path)
{
    if(!lFilePaths.contains(path))
        lFilePaths.push_back(path);
    return NET_OK;
}

int QHttpTool::setFileType(int type)
{
    mFileType = type;
}
int QHttpTool::clearFilePaths()
{
    lFilePaths.clear();
    return NET_OK;
}

int QHttpTool::clearData()
{
    sFilePath.clear();
    lFilePaths.clear();
    qData.empty();
    return NET_OK;
}

int QHttpTool::GET(int requestCode)
{
    QString qParams = "?";
    bool flag = true;

    for(auto p = qData.begin();p!=qData.end();p++){
        if(flag){
            flag = false;
        }else{
            qParams += "&";
        }
        qParams += p.key();
        qParams += "=";
        qParams += p.value().toString();
    }

    QString url = QString::fromStdString(sIP);
    if(sPort != "0"){
        url += ":";
        url += QString::fromStdString(sPort);
    }
    url += QString::fromStdString(sRouting);
    if(!qData.isEmpty())
        url += qParams;

#ifdef Debug
    std::cout<<"url:\t"<<url.toStdString()<<std::endl;
#endif

    QNetworkRequest workrequest;
    workrequest.setRawHeader("Content-Type",  "application/json");
    QNetworkAccessManager* nam = new QNetworkAccessManager();
    workrequest.setUrl(QUrl(url));
    QObject::connect(nam,SIGNAL(finished(QNetworkReply*)),this,SLOT(interfaceRequestFinished(QNetworkReply*)));
    QNetworkReply* reply = nam->get(workrequest);

    if(requestCode){
        qRequestMap.insert(reply,requestCode);
    }

    return NET_OK;
}

int QHttpTool::POST(int requestCode)
{
    if(!sFilePath.isNull()||sFilePath!=""){
        return fPOST(requestCode);
    }else if(!lFilePaths.isEmpty()){
        return fPOSTs(requestCode);
    }

    QByteArray data ;
    data.append(QJsonDocument(qData).toJson()) ;

    auto test = QJsonDocument(qData).toJson();

    QString url = QString::fromStdString(sIP);
    if(sPort!="0"){
        url += ":";
        url += QString::fromStdString(sPort);
    }
    url += QString::fromStdString(sRouting);

#ifdef DEBUG
    std::cout<<"url:\t"<<url.toStdString()<<std::endl;
#endif
    QNetworkRequest workrequest;
    workrequest.setRawHeader("Content-Type",  "application/json");
    workrequest.setUrl(QUrl(url));
    QNetworkAccessManager* nam = new QNetworkAccessManager();
    QObject::connect(nam,SIGNAL(finished(QNetworkReply*)),this,SLOT(interfaceRequestFinished(QNetworkReply*)));
    QNetworkReply* reply = nam->post(workrequest,data);

    clearData();

    if(requestCode){
        qRequestMap.insert(reply,requestCode);
    }

    return NET_OK;
}

int QHttpTool::fPOSTs(int requestCode)
{
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QString url = QString::fromStdString(sIP);
    if(sPort!="0"){
        url += ":";
        url += QString::fromStdString(sPort);
    }
    url += QString::fromStdString(sRouting);

    //添加数据
    for(auto item = qData.begin();item != qData.end();item++){
        QHttpPart dataPart;
        QString key = item.key();
        QString value1 = item.value().toString();
        dataPart.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("text/plain"));
        dataPart.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data; name=\"" + item.key() +"\""));
        dataPart.setBody(item.value().toString().toUtf8());
        multiPart->append(dataPart);
    }

    int count = 0;
    for(auto item = lFilePaths.begin();item != lFilePaths.end();item++){
        QHttpPart filePart;
        QFile* file = new QFile(*item);

        if(mFileType == 1){
            string path = ".\\request\\" + std::to_string(requestCode + count) + ".xlsx";
            file->copy(QString::fromStdString(path));
            delete file;
            file = new QFile(QString::fromStdString(path));

//            string path1 = ".\\" + std::to_string(count) + ".xlsx";
//            file->copy(QString::fromStdString(path1));
            count++;

            file->open(QIODevice::ReadOnly);
            filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
            filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\""+file->fileName()+"\""));
        }else{
            QFileInfo fInfo(*item);
            string path = ".\\request\\" +fInfo.fileName().toStdString();
//            cout<<"base:\t"<<fInfo.completeBaseName().toStdString()<<"\t"<<path<<endl;
            file->copy(QString::fromStdString(path));
            delete file;
            file = new QFile(QString::fromStdString(path));
            file->open(QIODevice::ReadOnly);
            filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
            filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\""+fInfo.fileName()+"\""));
        }

        filePart.setBodyDevice(file);
        file->setObjectName("file");
        file->setParent(multiPart);
        multiPart->append(filePart);
    }

    QNetworkRequest workrequest;
    workrequest.setUrl(QUrl(url));
    QNetworkAccessManager* nam = new QNetworkAccessManager();
    nam->setNetworkAccessible(QNetworkAccessManager::Accessible);
    QObject::connect(nam,SIGNAL(finished(QNetworkReply*)),this,SLOT(interfaceRequestFinished(QNetworkReply*)));
    QNetworkReply* reply = nam->post(workrequest,multiPart);

    multiPart->setObjectName("multiPart");
    multiPart->setParent(reply);

    clearData();

    if(requestCode){
        qRequestMap.insert(reply,requestCode);
    }
    return NET_OK;
}

int QHttpTool::fPOST(int requestCode)
{
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    //添加数据
    for(auto item = qData.begin();item != qData.end();item++){
        QHttpPart dataPart;
        dataPart.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data; name=\"num\""));
        dataPart.setBody(item.value().toString().toUtf8());
        multiPart->append(dataPart);
    }

    QString url = QString::fromStdString(sIP);
    if(sPort!="0"){
        url += ":";
        url += QString::fromStdString(sPort);
    }
    url += QString::fromStdString(sRouting);

    cout<<"filefilefilefile11";
    cout<<sFilePath.toStdString()<<endl;

    QHttpPart filePart1;
    QFile* file = new QFile(sFilePath);
    file->open(QIODevice::ReadOnly);
    filePart1.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
    filePart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\""+file->fileName()+"\""));

    filePart1.setBodyDevice(file);
    file->setParent(multiPart);

    multiPart->append(filePart1);

    QNetworkRequest workrequest;
    workrequest.setUrl(QUrl(url));
    QNetworkAccessManager* nam = new QNetworkAccessManager();
    nam->setNetworkAccessible(QNetworkAccessManager::Accessible);
    QObject::connect(nam,SIGNAL(finished(QNetworkReply*)),this,SLOT(interfaceRequestFinished(QNetworkReply*)));
    QNetworkReply* reply1 = nam->post(workrequest,multiPart);

//    QObject::connect(reply1, SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(interfaceRequestError(QNetworkReply::NetworkError err)));

    multiPart->setObjectName("multiPart");
    multiPart->setParent(reply1);

    clearData();

    if(requestCode){
        qRequestMap.insert(reply1,requestCode);
    }


    return NET_OK;
}

int QHttpTool::DOWNLOAD(QString url)
{
    QNetworkRequest workrequest;
    workrequest.setUrl(QUrl(url));
    QNetworkAccessManager* nam = new QNetworkAccessManager();
    QObject::connect(nam,SIGNAL(finished(QNetworkReply*)),this,SLOT(interfaceRequestFinished(QNetworkReply*)));
    QNetworkReply* reply = nam->get(workrequest);

    qRequestMap.insert(reply,DOWNLOADFILE_CODE);

    return NET_OK;
}

int QHttpTool::bindCallback(std::function <void(QJsonObject, int, QJsonValue)> httpCallback)
{
    this->fHttpCallback = httpCallback;
    return NET_OK;
}

int QHttpTool::bindCallback(std::function <void(QJsonObject, int, QByteArray)> httpCallback)
{
    this->fFileCallback = httpCallback;
    return NET_OK;
}

void QHttpTool::requestFinished(QNetworkReply* reply,const QByteArray data,const int statusCode)
{
//    std::cout<<__func__<<"\turl:\t"<<reply->url().toString().toStdString()<<std::endl;
    int requestCode = qRequestMap.value(reply);

    if(statusCode == 200){
        //文件callback
        if(requestCode >= 1000 && requestCode < 10000){
            QJsonObject valueInfo;
            valueInfo.insert("statusCode",statusCode);

            fFileCallback(valueInfo,requestCode,data);
        }else if (requestCode == DOWNLOADFILE_CODE) {
            QJsonObject valueInfo;
            valueInfo.insert("statusCode",statusCode);
            valueInfo.insert("url",reply->url().toString());

            fFileCallback(valueInfo,requestCode,data);
        }else{
            //普通callback
            QJsonObject json = QJsonDocument::fromJson(data).object();
            QJsonObject valueInfo = json.value("info").toObject();
            valueInfo.insert("statusCode",statusCode);
            QJsonValue valueData = json.value("result");

            fHttpCallback(valueInfo,requestCode,valueData);
        }
    }else{
        std::cout<<__func__<<"\turl:\t"<<reply->url().toString().toStdString()<<std::endl;
        //文件
        if(requestCode >= 1000 && requestCode < 10000){
            QJsonObject valueInfo;
            valueInfo.insert("statusCode",statusCode);
            valueInfo.insert("errorStr",reply->errorString());
            valueInfo.insert("fileType",0);

            fFileCallback(valueInfo,requestCode,data);
        }else if (requestCode == DOWNLOADFILE_CODE){
            QJsonObject valueInfo;
            valueInfo.insert("statusCode",statusCode);
            valueInfo.insert("errorStr",reply->errorString());
            valueInfo.insert("fileType",1);

            fFileCallback(valueInfo,requestCode,data);
        }else{
            QJsonValue errorValue;
            QJsonObject errorObject;
            errorObject.insert("statusCode",statusCode);
            errorObject.insert("errorStr",reply->errorString());
            errorValue=reply->url().toString();

            fHttpCallback(errorObject,requestCode,errorValue);
        }

//        std::cout<<"error info:\t";
//        std::cout<<reply->errorString().toStdString()<<"\t";
//        std::cout<<reply->error()<<endl;
        std::cout<<"error info:\t";
        std::cout<<reply->errorString().toStdString()<<"\t";
        std::cout<<reply->error()<<"\t";
        std::cout<<reply->url().toString().toStdString()<<endl;

    }
    qRequestMap.remove(reply);
    auto multiPart = reply->findChild<QHttpMultiPart*>("multiPart");
    if(multiPart){
        auto file = multiPart->findChildren<QFile*>("file");
        for(auto f : file){
            f->close();
            f->remove();
        }
    }
    auto nam = reply->parent();
    if(nam){
        disconnect(nam,SIGNAL(finished(QNetworkReply*)),this,SLOT(interfaceRequestFinished(QNetworkReply*)));
        nam->deleteLater();
        nam = nullptr;
    }
}

void QHttpTool::interfaceRequestError(QNetworkReply::NetworkError e)
{
//    cout<<"2222"<<e<<endl;
}

void QHttpTool::interfaceRequestFinished(QNetworkReply* reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    auto requestCode = qRequestMap.value(reply);
#ifdef DEBUG
    std::cout<<"requestFinished\t"<<statusCode<<"\n";
#endif
    if(reply->error() == QNetworkReply::NoError){
        requestFinished(reply,reply->readAll(),statusCode);
    }else{
        auto error = reply->error();
        auto str = reply->errorString();
        requestFinished(reply,"",statusCode);
    }

//    auto multPart = reply->findChild<QHttpMultiPart*>("multPart");
//    if(multPart){
//        auto file = multPart->findChild<QFile*>("file");
//        file->deleteLater();
//        multPart->deleteLater();
//    }
//    auto nam = reply->findChild<QNetworkAccessManager*>("nam");
//    if(nam){
//        nam->deleteLater();
//    }
}
