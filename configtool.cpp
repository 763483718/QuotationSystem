#include "configtool.h"

ConfigTool::ConfigTool()
{
//    mFlag = 0;
//    mBlockSize = 0;
//    mLastBlockSize = 0;
//    mLastRow = 1;
//    mLastNumber = 0;
    init();
//    readConfig();

//    printf("mBlockSize:\t%d\n",mBlockSize);
//    printf("mLastBlockSize:\t%d\n",mLastBlockSize);
//    printf("mLastRow:\t%d\n",mLastRow);
//    printf("mLastNumber:\t%d\n",mLastNumber);

}

ConfigTool::~ConfigTool()
{
    writeConfig();
//    printf("===========================================\n");
//    printf("mBlockSize:\t%d\n",mBlockSize);
//    printf("mLastBlockSize:\t%d\n",mLastBlockSize);
//    printf("mLastRow:\t%d\n",mLastRow);
//    printf("mLastNumber:\t%d\n",mLastNumber);
}

QList<QString> ConfigTool::getLocalVersions()
{
    return lVersions;
}

int ConfigTool::getBlockSize()
{
    return mBlockSize;
}

int ConfigTool::getLastBlockSize()
{
    return mLastBlockSize;
}

int ConfigTool::getLastRow()
{
    return mLastRow;
}

int ConfigTool::getFlag()
{
    return mFlag;
}

int ConfigTool::getLastNumber()
{
    return mLastNumber;
}

void ConfigTool::init()
{
    mFlag = 0;
    mBlockSize = 0;
    mLastBlockSize = 0;
    mLastRow = 1;
    mLastNumber = 0;
    lVersions.clear();

    readConfig();
}

void ConfigTool::backup()
{
    mOldFlag = mFlag;
    mOldBlockSize = mBlockSize;
    mOldLastBlockSize = mLastBlockSize;
    mOldLastRow = mLastRow;
    mOldLastNumber = mLastNumber;
    lOldVersions = lVersions;
}

void ConfigTool::recover()
{
    mFlag = mOldFlag;
    mBlockSize = mOldBlockSize;
    mLastBlockSize = mOldLastBlockSize;
    mLastRow = mOldLastRow;
    mLastNumber = mOldLastNumber;
    lVersions = lOldVersions;
}

void ConfigTool::updateOneVersion(int index, QString version)
{
    if(index == lVersions.size()){
        mBlockSize++;
        lVersions.push_back(version);
        return;
    }else if(index > lVersions.size()){
        return;
    }
    lVersions.replace(index,version);
}

void ConfigTool::clearVersions()
{
    lVersions.clear();
}

void ConfigTool::setFlag(int flag)
{
    mFlag = flag;
}
void ConfigTool::setBlockSize(int size)
{
    while(size > mBlockSize){
        lVersions.push_back("");
        mBlockSize++;
    }
    while(size < mBlockSize){
        lVersions.removeLast();
        mBlockSize--;
    }
    mBlockSize = size;
}

void ConfigTool::setLastBlockSize(int size)
{
    mLastBlockSize = size;
}

void ConfigTool::setLastRow(int row)
{
    mLastRow = row;
}

void ConfigTool::setLastNumber(int number)
{
    mLastNumber = number;
}

void ConfigTool::readConfig()
{
    QFile loadFile(ConfigPath);
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        writeConfig();
        return;
    }

     QByteArray allData = loadFile.readAll();
     loadFile.close();

     QJsonParseError jsonError;
     QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &jsonError));
     if(jsonError.error != QJsonParseError::NoError)
     {
    //         qDebug() << "json error!" << jsonError.errorString();
         return;
     }

     QJsonObject rootObj = jsonDoc.object();

     //开始解析json
     if(rootObj.contains("BLOCKSIZE")){
        mBlockSize = rootObj.value("BLOCKSIZE").toInt();
     }else{
         mBlockSize = 0;
     }

     if(rootObj.contains("LASTBLOCKSIZE")){
        mLastBlockSize = rootObj.value("LASTBLOCKSIZE").toInt();
     }else{
         mLastBlockSize = 0;
     }

     if(rootObj.contains("LASTROW")){
         mLastRow = rootObj.value("LASTROW").toInt();
     }else{
         mLastRow = 0;
     }

     if(rootObj.contains("LASTNUMBER")){
         mLastNumber = rootObj.value("LASTNUMBER").toInt();
     }else{
         mLastNumber = 0;
     }

     if(rootObj.contains("FLAG")){
         mFlag = rootObj.value("FLAG").toInt();
     }else{
         mFlag = 0;
     }

     if(rootObj.contains("VERSIONS")){
        QJsonArray arr = rootObj.value("VERSIONS").toArray();
        for(int i = 0;i < arr.size();i++){
            QString version = arr.at(i).toString();
            lVersions.append(version);
        }
     }

}

void ConfigTool::writeConfig()
{
    QFile loadFile(ConfigPath);
    if(!loadFile.open(QIODevice::WriteOnly)) {
        return;
    }
    QJsonObject rootObj;
    rootObj.insert("BLOCKSIZE",mBlockSize);
    rootObj.insert("LASTBLOCKSIZE",mLastBlockSize);
    rootObj.insert("LASTROW",mLastRow);
    rootObj.insert("LASTNUMBER",mLastNumber);
    rootObj.insert("FLAG",mFlag);

//    printf("LASTNUMBER: \t%d\n",mLastNumber);

    QJsonArray arr;
    int i = 0;
    for(QString version:lVersions){
        arr.insert(i,version);
        i++;
    }
    rootObj.insert("VERSIONS",arr);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(rootObj);

    loadFile.write(jsonDoc.toJson());
    loadFile.close();
}




















