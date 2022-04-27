#ifndef QEXCELTOOL_H
#define QEXCELTOOL_H

#include <QList>
#include <QImage>
#include <QDateTime>
#include <QFileInfo>
#include <QThread>
#include <QDir>

#include <iostream>
#include <direct.h>
#include <algorithm>
#include <mutex>
#include <sstream>
#include <iostream>
#include <iomanip>


#include "QHttpTool.h"
#include "ConfigTool.h"
#include "FormatException.h"
#include "xlsxdocument.h"
#include "products.h"
#include "OperationNumber.h"

typedef products QProduct;
typedef productItem QProductItem;

const static string TempPath = ".\\Cache\\Temp\\block";
const static string TempFolder = ".\\Cache\\Temp";
const static string CachePath = ".\\Cache\\block";
const static string CacheFolder = ".\\Cache";
const static string ImgFloder = ".\\img";
const static string RequestFloder = ".\\request";
const static int BlockSize = 50;

const int GET_VERSION_CODE = 800;
const int UPLOADIMAGE_CODE = 801;
const int GET_IMAGE_NAME_CODE = 802;
const int REPLACE_IMAGE_CODE = 803;
const int JUDGETYPE_CODE = 804;
const int BATCHADJUSTMENT_CODE = 805;

enum EXCEL_STATUS{
    EXCEL_OK,
    EXCEL_ERROR,
    EXCEL_FINISHED,
    EXCEL_NOHTTPTOOL,
    EXCEL_NOFILE,
    EXCEL_ISMODIFING,
    EXCEL_NONE
};

class EImage{
public:
    EImage(QImage image,QPoint p){
        img =image.copy();
        point = p;
    }
    EImage(QImage image,int row,int col){
        img =image.copy();
        point.setX(col);
        point.setY(row);
    }

    QImage img;
    QPoint point;
};

//QTXLSX_USE_NAMESPACE
class QExcelTool : public QObject
{
    Q_OBJECT
public:
    QExcelTool();
    ~QExcelTool();

    int setHttpTool(QHttpTool* httpTool);
    int open(QString fileName);                             //批量导入产品
    int close();
    int getImages();
    int getImages(QList<QImage> &images,QList<QPoint> &points);
    int saveImage(QString path);
    int getProducts(int block,QList<QProduct> &products);
    int getProducts(QList<int> ids,QList<QProduct> &products);
    int exportProducts(QList<QProduct> products,QString path, bool exportDeleted = true);
    int exportProducts(QList<int> ids,QString path, bool exportDeleted = true);
    int replaceImage(int number, QString src,QString dst);
    int saveErrorExcel(QString path);
    int batchAdjustment(float magnitude);
    int checkImages();
    int clearCache();

    //保存原来内容到/cache/temp路径下，如果上传失败，就恢复
    int updateProducts(int block,QList<QProduct> products,bool upload = true); //第20列：块序号 第21列：版本号
    int insertProduct(QProduct product);                    //第20列：块序号 第21列：版本号

    int CacheInit(int flag);                                //1为管理员 0为普通人员

    int getProductsAndBlocksSize(int &blockSize,int &productsSize);

    QProduct& at(int i);

    //用户不要使用这个指针
    static QExcelTool* pQExcelTool;
private:
    QList<QProduct> getProducts();
    QXlsx::Format getFormat();

    //回调函数
    static void interfaceCallback(QJsonObject info,int requestCode,QJsonValue data){
        switch(requestCode){
        case GET_VERSION_CODE:
            pQExcelTool->getVersionCallback(info,data);
            break;
        case UPLOADIMAGE_CODE:
            pQExcelTool->uploadImagesCallback(info,data);
            break;
        case GET_IMAGE_NAME_CODE:
            pQExcelTool->getImageNameCallback(info,data);
            break;
        case REPLACE_IMAGE_CODE:
            pQExcelTool->replaceImageCallback(info,data);
            break;
        case JUDGETYPE_CODE:
            pQExcelTool->judgeTypeCallback(info,data);
            break;
        case BATCHADJUSTMENT_CODE:
            pQExcelTool->batchAdjustmentCallback(info,data);
            break;
        default:
            break;
        }

        if(requestCode >= 10000 && requestCode < 20000){
            pQExcelTool->uploadCallback(info,requestCode,data);
        }else if(requestCode >= 20000){
            pQExcelTool->uploadFilesCallback(info,requestCode,data);
        }
    }

    static void interfaceFileCallback(QJsonObject info,int requestCode,QByteArray data){
        if(requestCode == DOWNLOADFILE_CODE){
            pQExcelTool->downloadFileCallback(info,requestCode,data);
        }else{
            pQExcelTool->downloadEXCELCallback(info,requestCode,data);
        }
    }
    void batchAdjustmentCallback(QJsonObject info,QJsonValue data);

    void judgeTypeCallback(QJsonObject info,QJsonValue data);

    void replaceImageCallback(QJsonObject info,QJsonValue data);

    void getImageNameCallback(QJsonObject info,QJsonValue data);

    void uploadImagesCallback(QJsonObject info,QJsonValue data);

    void downloadEXCELCallback(QJsonObject info,int requestCode,QByteArray data);

    void downloadFileCallback(QJsonObject info,int requestCode,QByteArray data);

    void getVersionCallback(QJsonObject info,QJsonValue data);

    void uploadCallback(QJsonObject info,int requestCode,QJsonValue data);

    void uploadFilesCallback(QJsonObject info,int requestCode,QJsonValue data);
//===========================================================================================
    int init();

    int exportProducts(bool exportDeleted = true, bool errorMode = false);

    //重新对product排序
    int resortProducts(const QList<int> &srcIds, QList<QProduct> &products);

    //批量更新
    int updateRepeatProducts(int block,QList<QProduct> products,bool upload = true);

    //更新缓存
    int updateFiles();

    //下载单个块文件
    int downloadBlockFile(int block, int flag);

    //上传单个文件
    int uploadBlockFile(int block);
    //批量上传文件
    int uploadBlockFiles(int block);

    //异常处理
    int exceptionProcess(int row,QString msg);
    int exceptionProcess(QProduct product);

    //处理重复产品型号
    int processRepeatProduct();

    //将mRow移动到下一个编号位置
    int nextProduct(int id = -1);
    int nextCacheProduct(int id = -1);

    //回滚
    int recover();

    //flag == true 表示读产品第一行
    //flag == false 表示读型号行
    int readLine(QProduct &product, int flag = false);
    int readProduct(QProduct &product);

    int readCacheLine(QProduct &product, int flag = false);
    int readCacheProduct(QProduct &product);

    int findProductByIds(int block,QList<int> ids,QList<QProduct> &products);

    int findRowById(int id);

    int writeProduct(int number,bool exportDeleted = true,bool errorMode = false);
    int writeProduct(int number,QProduct product,QXlsx::Document* pXlsx,int &row,bool exportDeleted = true,bool errorMode = false);

    int process();

    int adjustBlock(int block,float magnitude);

    int insertProductProcess(QProduct product);

    int saveToCache();      //第20列：块序号 第21列：版本号
    int uploadImages();     //上传图片

    ConfigTool mConfigTool;

    QHttpTool* pHttpTool;

    QXlsx::Document* pXlsxCache;
    QXlsx::Document* pXlsxError;
    QXlsx::Document* pXlsx;
    QList<QProduct> lProducts;

    QList<EImage> lImages;

    int mRow;
    int mPreRow;
    int mNextRow;
    int mMaxRow;

    int mErrorRow;

    int mCacheRow;

    int mImageIndex;

    QList<QString> lFilePaths;
    QList<QString> lImagePaths;

    //正在修改的块需要上锁：正在修改的块号放入list，修改list需要锁
    std::mutex mModifyMutex;
    QList<int> lModifyBlocks;
    bool isDownloading = false;
    bool requestDone = false;
    bool bIsError = false;
    bool finishOne = false;
    bool repeatMode = false;

    QList<QString> lOldVersion;

public:
    signals:
    void signals_finished(int OperationNumber,int block);

};

#endif // QEXCELTOOL_H
