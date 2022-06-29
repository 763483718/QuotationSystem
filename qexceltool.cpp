#include "QExcelTool.h"

bool softFunction(EImage a,EImage b)
{
    return a.point.y()<b.point.y();
}

QExcelTool* QExcelTool::pQExcelTool = nullptr;

QExcelTool::QExcelTool()
{
    pXlsx = nullptr;
    pXlsxError = nullptr;
    pXlsxCache = nullptr;

    mRow = 1;
    mPreRow = 1;
    mNextRow = 1;

    mErrorRow = 2;

    mImageIndex = -1;

    pHttpTool = nullptr;

    init();

//    QObject::connect(this,SIGNAL(signals_finished(int,int)),this,SLOT(test(int,int)));
}

QExcelTool::~QExcelTool()
{
    if(pXlsx){
        pXlsx->save();
        pXlsx->deleteLater();
    }
    if(pXlsxError){
//        pXlsxError->saveAs(".\\ErrorTemp.xlsx");
        pXlsxError->save();
        pXlsxError->deleteLater();
    }
    if(pXlsxCache){
        pXlsxCache->save();
        pXlsxCache->deleteLater();
    }
    if(pHttpTool){
        delete pHttpTool;
    }
    QFile file(".\\ErrorTemp.xlsx");
    if(file.exists()){
        file.remove();
    }
    //删除http缓存
    QDir dir(QString::fromStdString(".\\request"));
    if(dir.exists()){
        dir.removeRecursively();
    }
    mkdir(".\\request");
}

int QExcelTool::setHttpTool(QHttpTool *httpTool)
{
    pHttpTool = httpTool;
    return EXCEL_OK;
}

int QExcelTool::init()
{
    //创建文件夹
    if (0 != access(ImgFloder.c_str(), 0))
    {
        // if this folder not exist, create a new one.
        mkdir(ImgFloder.c_str());   // 返回 0 表示创建成功，-1 表示失败
    }

    if (0 != access(CacheFolder.c_str(), 0))
    {
        // if this folder not exist, create a new one.
        mkdir(CacheFolder.c_str());   // 返回 0 表示创建成功，-1 表示失败
    }

    if (0 != access(TempFolder.c_str(), 0))
    {
        // if this folder not exist, create a new one.
        mkdir(TempFolder.c_str());   // 返回 0 表示创建成功，-1 表示失败
    }

    if(0 != access(RequestFloder.c_str(),0))
    {
        mkdir(RequestFloder.c_str());
    }
    return EXCEL_OK;
}

QList<QProduct> QExcelTool::getProducts()
{
    return lProducts;
}

int QExcelTool::getProducts(int block,QList<QProduct> &products)
{
    std::unique_lock<std::mutex> lk_config(mConfigTool.configMutex);
    int blockNum = mConfigTool.getBlockSize();
    mCacheRow = mConfigTool.getLastRow() + 1;
    lk_config.unlock();

    if(blockNum <= block){
        return EXCEL_ERROR;
    }
    products.clear();

    string path = CachePath + std::to_string(block);
    path += ".xlsx";

    if(pXlsxCache){
        pXlsxCache->save();
        pXlsxCache->deleteLater();
        pXlsxCache = nullptr;
    }
    pXlsxCache = new QXlsx::Document(QString::fromStdString(path));
    mMaxRow = pXlsxCache->dimension().rowCount();
    mRow = 1;
    mPreRow = 1;
    mNextRow = 1;

    while(mRow <= mMaxRow){
        auto res = nextCacheProduct();

        if(res == EXCEL_ERROR){
            return EXCEL_ERROR;
        }

        QProduct product;
        readCacheProduct(product);

        products.append(product);

        if(res == EXCEL_FINISHED){
            break;
        }
    }
    pXlsxCache->save();
    pXlsxCache->deleteLater();
    pXlsxCache = nullptr;
    return EXCEL_OK;
}

int QExcelTool::resortProducts(const QList<int> &srcIds, QList<QProduct> &products)
{
    cout << "productsID:\t";
    for(auto p : products)
    {
        cout << p.getp_id() << "\t";
    }
    cout << endl;

    for(int i = 0; i < srcIds.size(); i++)
    {
        if(products.at(i).getp_id() != srcIds[i])
        {
            for(int k = i + 1; k < products.size(); k++)
            {
                if(products.at(k).getp_id() == srcIds[i])
                {
                    auto temp = products[k];
                    products[k] = products[i];
                    products[i] = temp;
                    break;
                }
            }
        }
    }
    return EXCEL_OK;
}

int QExcelTool::getProducts(QList<int> ids, QList<QProduct> &products)
{
    //读config
    std::unique_lock<std::mutex> lk_config(mConfigTool.configMutex);
    int blockNum = mConfigTool.getBlockSize();
    lk_config.unlock();

    if(blockNum == 0){
        return EXCEL_NONE;
    }

    if(ids.size() == 0){
        return EXCEL_NONE;
    }

    auto ids_records = ids;

    qSort(ids.begin(), ids.end());

    int block = (ids[0] - 1) / BlockSize;
    QList<int> idsInOneBlock;
    for(auto id : ids){
        if((id - 1)/BlockSize == block){
            idsInOneBlock.push_back(id);
        }else{
            if(idsInOneBlock.empty()){
                continue;
            }else{
                int ret = findProductByIds(block,idsInOneBlock,products);
                if(ret != EXCEL_OK){
                    return ret;
                }
            }
            block = (id-1)/50;
            idsInOneBlock.clear();
            idsInOneBlock.push_back(id);
        }
    }
    if(!idsInOneBlock.isEmpty()){
        int ret = findProductByIds(block,idsInOneBlock,products);
        if(ret != EXCEL_OK){
            return ret;
        }
    }
    resortProducts(ids_records, products);
    return EXCEL_OK;
}

int QExcelTool::checkImages()
{
    //读config
    std::unique_lock<std::mutex> lk_config(mConfigTool.configMutex);
    int blockNum = mConfigTool.getBlockSize();
    lk_config.unlock();

    //判断是否缺文件 文件内图片是否存在
    for(int i = 0;i < blockNum;i++){
        string path = CachePath + std::to_string(i);
        path += ".xlsx";
        QFile file(QString::fromStdString(path));
        if(!file.exists()){
            return EXCEL_NOFILE;
        }

        //判断图片
        if(pXlsxCache){
            pXlsxCache->save();
            pXlsxCache->deleteLater();
        }
        pXlsxCache = new QXlsx::Document(QString::fromStdString(path));
        int maxRow = pXlsxCache->dimension().lastRow();
        for(int row = 2;row <= maxRow;row++){
            auto imagePath = pXlsxCache->read(row,2);
            if(imagePath.isNull()){
                continue;
            }
            QString path = imagePath.toString();
            QFile file(path);
            if(!file.exists()){
                QString url = QString::fromStdString(csIP) + path.mid(1);
                url.replace("img","images");
                url.replace("\\","/");
                pHttpTool->DOWNLOAD(url);
            }
        }
        pXlsxCache->deleteLater();
        pXlsxCache = nullptr;
    }
    return EXCEL_OK;
}

int QExcelTool::exportProducts(QList<QProduct> products,QString path, bool exportDeleted)
{
    if(products.size() == 0){
        return EXCEL_OK;
    }
    lProducts = products;

    if(pXlsxCache){
        pXlsxCache->save();
        pXlsxCache->deleteLater();
        pXlsxCache = nullptr;
    }

    QFile oldFile(path);
    if(oldFile.exists()){
        oldFile.remove();
    }
    QFile file(".\\Cache.xlsx");
    file.copy(path);

    pXlsxCache = new QXlsx::Document(path);
    pXlsxCache->setRowHeight(1,100);

    mCacheRow = 2;

    if(exportProducts(exportDeleted,true) != EXCEL_OK){
        return EXCEL_ERROR;
    }

    pXlsxCache->save();
    pXlsxCache->deleteLater();
    pXlsxCache = nullptr;

    return EXCEL_OK;
}

int QExcelTool::exportProducts(bool exportDeleted,bool errorMode)
{
    if(!pXlsxCache){
        return EXCEL_ERROR;
    }
    while(lProducts.size() != 0){
        int num = lProducts.at(0).getp_id();
        writeProduct(num,exportDeleted,errorMode);
        lProducts.removeAt(0);
    }
    return EXCEL_OK;
}

int QExcelTool::exportProducts(QList<int> ids,QString path, bool exportDeleted)
{
    return EXCEL_OK;
}

int QExcelTool::clearCache()
{
    QDir dir(QString::fromStdString(CacheFolder));
    if(dir.exists()){
        dir.removeRecursively();
    }else{
        return EXCEL_NOFILE;
    }
    return EXCEL_OK;
}

int QExcelTool::findProductByIds(int block, QList<int> ids, QList<QProduct> &products)
{
    if(pXlsxCache){
        pXlsxCache->save();
        pXlsxCache->deleteLater();
        pXlsxCache = nullptr;
    }
    string path = CachePath + std::to_string(block) + ".xlsx";
    QFileInfo fInfo(QString::fromStdString(path));
    if(!fInfo.exists()){
        return EXCEL_NOFILE;
    }

    pXlsxCache = new QXlsx::Document(QString::fromStdString(path));
    mMaxRow = pXlsxCache->dimension().rowCount();
    mRow = 1;

    for(auto id : ids){
//        nextCacheProduct(id);
        findRowById(id);
        QProduct product;
        readCacheProduct(product);
        products.push_back(product);
    }

    pXlsxCache->deleteLater();
    pXlsxCache = nullptr;

    return EXCEL_OK;
}

int QExcelTool::replaceImage(int number,QString src, QString dst)
{
    QString path = dst;

    auto sp = src.split("\\");
    src = sp.last();

    sp = dst.split("\\");
    dst = sp.last();

    QJsonObject obj;
    obj.insert("flagNum",QString::fromStdString(std::to_string(number)));
    obj.insert("newName",dst);
    obj.insert("oldName",src);
//cout << __func__ << "\tnumber:\t" << number << endl;
//cout << __func__ << "\tnewName:\t" << dst.toStdString() << endl;
//cout << __func__ << "\toldName:\t" << src.toStdString() << endl;
    pQExcelTool = this;

    std::unique_lock<std::mutex> lk_http(pHttpTool->mSendingMutex);
    pHttpTool->clearData();
    pHttpTool->setFileType(2);
    pHttpTool->setPort("0");
    pHttpTool->setData(obj);
    pHttpTool->setRouting("/product/PicRepeat");
    pHttpTool->bindCallback(QExcelTool::interfaceCallback);

//cout << __func__ << "\tpath:\t" << path.toStdString() << endl;
    pHttpTool->insertFilePath(path);

    pHttpTool->POST(REPLACE_IMAGE_CODE);

    lk_http.unlock();

    return EXCEL_OK;
}

int QExcelTool::saveErrorExcel(QString path)
{
    if(pXlsxError != nullptr){
        pXlsxError->save();
        pXlsxError->deleteLater();
        pXlsxError = nullptr;
    }
    QFile oldFile(path);
    if(oldFile.exists()){
        oldFile.remove();
    }

    QFile file(".\\ErrorTemp.xlsx");
    if(file.exists()){
        file.copy(path);
        file.remove();
    }else{
        return EXCEL_NOFILE;
    }
    return EXCEL_OK;
}

int QExcelTool::batchAdjustment(float magnitude)
{
    if(pHttpTool == nullptr){
        return EXCEL_NOHTTPTOOL;
    }
    pQExcelTool = this;

    QJsonObject obj;
//    obj.insert("adjustPrice",QString::fromStdString(std::to_string(magnitude)));
    obj.insert("adjustPrice",magnitude);

    std::unique_lock<std::mutex> lk_http(pHttpTool->mSendingMutex);
    pHttpTool->clearData();
    pHttpTool->setData(obj);
    pHttpTool->setPort("0");
    pHttpTool->setRouting("/product/adjustBlock");
    pHttpTool->bindCallback(QExcelTool::interfaceCallback);
    pHttpTool->POST(BATCHADJUSTMENT_CODE);
    lk_http.unlock();

    return EXCEL_OK;
}

int QExcelTool::open(QString fileName)
{
    mRow = 1;
    mPreRow = 1;
    mNextRow = 1;

    mErrorRow = 2;

    mImageIndex = -1;

    lImagePaths.clear();

    if(pXlsx != nullptr){
//        pXlsx->save();
        pXlsx->deleteLater();
        lProducts.clear();
        lImages.clear();
    }

    try {
        pXlsx = new QXlsx::Document(fileName);
        getImages();
    } catch (std::exception e) {
        return EXCEL_ERROR;
    }
    mMaxRow = pXlsx->dimension().rowCount();
//    std::cout<<"mMaxRow:\t"<<mMaxRow<<std::endl;
    auto ret = process();

    if(lProducts.size() > 0){
        return processRepeatProduct();
    }

//    saveToCache();
//    uploadImages();

//    pXlsx->save();
//    pXlsx->deleteLater();
//    lProducts.clear();
//    lImages.clear();

//    pXlsxError->save();
//    pXlsxError->deleteLater();
//    pXlsxError = nullptr;

    return ret;
}

int QExcelTool::saveToCache()
{
    if(lProducts.size() == 0){
        return EXCEL_OK;
    }

    //读config
    std::unique_lock<std::mutex> lk_config(mConfigTool.configMutex);
    //block的数量
    int blockNum = mConfigTool.getBlockSize();
    int lastBlockNum = mConfigTool.getLastBlockSize();
    int number = mConfigTool.getLastNumber();
    mCacheRow = mConfigTool.getLastRow() + 1;
    lOldVersion = mConfigTool.getLocalVersions();
    mConfigTool.backup();
    lk_config.unlock();


    //设置块锁
    bool isModifing = false;
    std::unique_lock<std::mutex> lk_modify(mModifyMutex);
//    bIsError = false;
    if(lModifyBlocks.indexOf(blockNum - 1) >= 0){
        isModifing = true;
    }else{
        lModifyBlocks.push_back(blockNum - 1);
    }
    lk_modify.unlock();
    if(isModifing){
        return EXCEL_ISMODIFING;
    }

    if(pXlsxCache){
        pXlsxCache->save();
        pXlsxCache->deleteLater();
        pXlsxCache = nullptr;
    }

    string path;

    bool isCreated = false;
    if(blockNum == 0 || lastBlockNum == BlockSize){

        blockNum++;
        lastBlockNum = 0;
        mCacheRow = 2;

        QFile file(".\\Cache.xlsx");

        path = CachePath + std::to_string(blockNum - 1);
        path += ".xlsx";

        file.copy(QString::fromStdString(path));
        isCreated = true;
    }else{
        path = CachePath + std::to_string(blockNum - 1);
        path += ".xlsx";
    }

    pXlsxCache = new QXlsx::Document(QString::fromStdString(path));
    pXlsxCache->setRowHeight(1,100);
    //块序号
    pXlsxCache->write(1,20,blockNum - 1);

    if(!isCreated){
        string newPath = TempPath + std::to_string(blockNum - 1);
        newPath += ".xlsx";

        QFile oldFile(QString::fromStdString(path));
    //    oldFile.remove(QString ::fromStdString(newPath));
        oldFile.copy(QString ::fromStdString(newPath));


    }else{
        //块锁
        std::unique_lock<std::mutex> lk_modify2(mModifyMutex);
        lModifyBlocks.removeAll(blockNum - 2);
        lModifyBlocks.push_back(blockNum - 1);
        lk_modify2.unlock();
    }

    lFilePaths.clear();
    while(lProducts.size() != 0){
        int range = 0;
        bool outRange = false;

        if(lProducts.size() <= BlockSize - lastBlockNum){
            range = lProducts.size();
            lastBlockNum = lastBlockNum + lProducts.size();
        }else{
            range = BlockSize - lastBlockNum;
            outRange = true;
        }

        for(int i = 0;i < range;i++){
            number++;
            writeProduct(number);
            lProducts.removeAt(0);
        }

        //写行号
        pXlsxCache->write(1,22,mCacheRow - 1);
        //写版本号到excel文件和config文件
        QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
        pXlsxCache->write(1,21,timestamp);

        std::unique_lock<std::mutex> lk(mConfigTool.configMutex);
        mConfigTool.updateOneVersion(blockNum - 1,timestamp);
        mConfigTool.setBlockSize(blockNum);
        mConfigTool.setLastBlockSize(lastBlockNum);
        mConfigTool.setLastNumber(number);
        mConfigTool.setLastRow(mCacheRow - 1);
        mConfigTool.writeConfig();
        lk.unlock();

        pXlsxCache->save();
//        pXlsxCache->destroyed();
        pXlsxCache->deleteLater();
        pXlsxCache = nullptr;

#if 0
//        uploadBlockFile(blockNum - 1);
#else
        string path = CachePath + std::to_string(blockNum - 1) + ".xlsx";
        lFilePaths.push_back(QString::fromStdString(path));
#endif

        if(outRange){

            blockNum++;
            lastBlockNum = 0;
            mCacheRow = 2;

            QFile file(".\\Cache.xlsx");
            path = CachePath + std::to_string(blockNum - 1);
            path += ".xlsx";
            file.copy(QString::fromStdString(path));

            pXlsxCache = new QXlsx::Document(QString::fromStdString(path));
            pXlsxCache->setRowHeight(1,100);

            //块锁
            std::unique_lock<std::mutex> lk_modify2(mModifyMutex);
            lModifyBlocks.push_back(blockNum - 1);
            lk_modify2.unlock();

        }else{
            //块锁
            std::unique_lock<std::mutex> lk_modify2(mModifyMutex);
            requestDone = true;
            lk_modify2.unlock();
        }
    }

    uploadBlockFiles(blockNum - 1);

    return EXCEL_OK;
}

int QExcelTool::uploadImages()
{
    if(pHttpTool == nullptr){
        return EXCEL_NOHTTPTOOL;
    }

    std::lock_guard<std::mutex> lk_modify(mModifyMutex);

    if(lImagePaths.isEmpty()){
        return EXCEL_ERROR;
    }
    pQExcelTool = this;

    std::unique_lock<std::mutex> lk_http(pHttpTool->mSendingMutex);
    pHttpTool->clearData();
    pHttpTool->setFileType(2);
    pHttpTool->setPort("0");
    pHttpTool->setRouting("/product/PicUpload");
    pHttpTool->bindCallback(QExcelTool::interfaceCallback);
    for(auto item : lImagePaths){
        pHttpTool->insertFilePath(item);
        cout<< __func__ << "\timagePath:\t"<<item.toStdString()<<endl;
    }
    pHttpTool->POST(UPLOADIMAGE_CODE);
    lk_http.unlock();

    return EXCEL_OK;
}

int QExcelTool::writeProduct(int number,QProduct product,QXlsx::Document* pXlsx,int &row,bool exportDeleted,bool errorMode)
{
    if(!exportDeleted && product.getp_isDelete()){
        return EXCEL_OK;
    }
    QXlsx::Format format = getFormat();

    //写编号
    pXlsx->write(row,1,number,format);

    if(!errorMode){
        //写图片路径
        pXlsx->write(row,2,product.getp_imagePath(),format);
    }else{
        if(!product.getp_imagePath().isNull()&&product.getp_imagePath()!=""){
            QImage img(product.getp_imagePath());
            pXlsx->insertImage(row - 1 ,1,img);
        }
    }

    //写品牌
    if(!product.getp_brand().isNull()){
        pXlsx->write(row,3,product.getp_brand(),format);
    }

    //写中文名
    if(!product.getp_name().isNull()){
        pXlsx->write(row,4,product.getp_name(),format);
    }

    //写孔位
    if(product.getp_hole() != ""){
        pXlsx->write(row,12,product.getp_hole(),format);
    }

    //配套件
    if(!product.getp_fitting().isNull()){
        pXlsx->write(row,13,product.getp_fitting(),format);
    }

    //写备注
    if(!product.getp_remark().isNull()){
        pXlsx->write(row,14,product.getp_remark(),format);
    }

    if(!errorMode){
        //产品是否已经删除
        pXlsx->write(row,16,product.getp_isDelete(),format);
    }

    auto items = product.getp_productItem();
    for(int i = 0;i < product.getItemsSize();i++){
        if(!exportDeleted && items.at(i).getp_itemIsDelete()){
            continue;
        }
        pXlsx->setRowHeight(row,100);
        //型号
        if(!items.at(i).getp_type().isNull()){
            pXlsx->write(row,5,items.at(i).getp_type(),format);
        }

        //价格&来源&材料
        if(!items.at(i).getp_source().isNull() && items.at(i).getp_source()!=""){
            std::ostringstream out_str;
            out_str << std::setiosflags(ios::fixed) << std::setprecision(3) << items.at(i).getp_price();
            string price = out_str.str();

            pXlsx->write(row,6,QString::fromStdString(price),format);
            pXlsx->write(row,7,items.at(i).getp_source(),format);
            if(!items.at(i).getp_material().isNull()){
                pXlsx->write(row,8,items.at(i).getp_material(),format);
            }
        }

        //颜色
        if(!items.at(i).getp_color().isNull() && items.at(i).getp_color() != ""){
            pXlsx->write(row,9,items.at(i).getp_color(),format);
        }

        //mpq
        if(items.at(i).getp_MPQ() != -1){
            pXlsx->write(row,11,items.at(i).getp_MPQ(),format);
        }

        //moq
        if(items.at(i).getp_MOQ() != -1){
            pXlsx->write(row,10,items.at(i).getp_MOQ(),format);
        }

        if(!errorMode){
            //是否已经删除
            pXlsx->write(row,15,items.at(i).getp_itemIsDelete(),format);
        }
        row++;
    }
    return EXCEL_OK;
}


int QExcelTool::writeProduct(int number, bool exportDeleted,bool errorMode)
{
    if(lProducts.size() == 0){
        return EXCEL_ERROR;
    }

    QProduct product = lProducts.at(0);

    return writeProduct(number,product,pXlsxCache,mCacheRow,exportDeleted,errorMode);
}

int QExcelTool::process()
{
    bool formatError = false;
    while(mRow <= mMaxRow){
//        std::cout<<"mNext:\t"<<mNextRow<<std::endl;
        try {
            auto res = nextProduct();
            if(res == EXCEL_ERROR){
                return EXCEL_ERROR;
            }

            QProduct product;
            readProduct(product);

            if(!product.getp_image().isNull()){
                QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
                string path = ".\\img\\" + timestamp.toStdString() + ".jpg";
                product.saveImage(QString::fromStdString(path));
                product.setp_imagePath(QString::fromStdString(path));
                lImagePaths.append(QString::fromStdString(path));
                QThread::msleep(5);
            }
            lProducts.append(product);
            if(res == EXCEL_FINISHED){
                if(formatError){
                    return EXCEL_ERROR;
                }
                return EXCEL_OK;
            }
        } catch (FormatException e) {
            exceptionProcess(e.getRow(),e.getMsg());
            mRow = mNextRow - 1;
            formatError = true;
        }
    }
    if(formatError){
        return EXCEL_ERROR;
    }
    return EXCEL_OK;
}

QProduct& QExcelTool::at(int i)
{
    return lProducts[i];
}

int QExcelTool::close()
{
    if(pXlsx){
        pXlsx->deleteLater();
        pXlsx = nullptr;
    }
    return EXCEL_OK;
}

int QExcelTool::getImages()
{
    QList<int> rows;
    QList<int> cols;

    lImages.clear();
    QList<QImage> images = pXlsx->getImages(rows,cols);
    for(int i = 0;i < rows.size();i++){
        lImages.append(EImage(images.at(i),rows.at(i),cols.at(i)));
    }
    std::sort(lImages.begin(),lImages.end(),softFunction);

    return EXCEL_OK;
}

int QExcelTool::getImages(QList<QImage> &images,QList<QPoint> &points)
{
    QList<int> rows;
    QList<int> cols;

    images = pXlsx->getImages(rows,cols);
    for(int i = 0;i < rows.size();i++){
        points.append(QPoint(rows.at(i),cols.at(i)));
    }

    return EXCEL_OK;
}

int QExcelTool::saveImage(QString path)
{
    if(lImages.size() == 0){
        return EXCEL_ERROR;
    }
    if(path[path.length()-1]!="\\"){
        path += "\\";
    }
    for(int i = 0; i < lImages.size();i++){
        QString name = path + QString::fromStdString(std::to_string(i));
        name += ".jpg";
        lImages.at(i).img.save(name);
//        printf("row:\t%d\n",lImages.at(i).point.x());
    }
    return EXCEL_OK;
}

int QExcelTool::updateRepeatProducts(int block, QList<QProduct> products,bool upload)
{
    cout<<"filefilefilefileupdate:\t"<<block<<endl;
    QList<QProduct> src;
    getProducts(block,src);

    for(auto product:products){
        auto id = product.getp_id();
        int index = (id - 1) % BlockSize;

        src[index].replaceItems(product);
    }
    if(upload){
        return updateProducts(block,src);
    }
    return updateProducts(block,src,false);
}

int QExcelTool::updateProducts(int block, QList<QProduct> products,bool upload)
{
    //读取config
    std::unique_lock<std::mutex> lk_config(mConfigTool.configMutex);
    int blockNum = mConfigTool.getBlockSize();
    int lastBlockNum = mConfigTool.getLastBlockSize();
    mCacheRow = 2;
    lOldVersion = mConfigTool.getLocalVersions();
    mConfigTool.backup();
    lk_config.unlock();

    if(blockNum <= block){
        return EXCEL_ERROR;
    }else if(blockNum - 1 == block){
        if(lastBlockNum != products.size()){
            return EXCEL_ERROR;
        }
    }else{
        if(products.size() != BlockSize){
            return EXCEL_ERROR;
        }
    }

    //设置块锁
    bool isModifing = false;
    std::unique_lock<std::mutex> lk_modify(mModifyMutex);
    if(lModifyBlocks.indexOf(block) >= 0){
        isModifing = true;
    }else{
        lModifyBlocks.push_back(block);
        requestDone = true;
        finishOne = true;
    }
    lk_modify.unlock();
    if(isModifing){
        return EXCEL_ISMODIFING;
    }

    lProducts.clear();
    lProducts = products;

    string path = CachePath + std::to_string(block);
    path += ".xlsx";
    string newPath = TempPath + std::to_string(block);
    newPath += ".xlsx";

    QFile oldFile(QString::fromStdString(path));
//    oldFile.remove(QString ::fromStdString(newPath));
    oldFile.copy(QString ::fromStdString(newPath));
    oldFile.remove();

    if(pXlsxCache){
        pXlsxCache->save();
        pXlsxCache->deleteLater();
        pXlsxCache = nullptr;
    }
//    pXlsxCache = new QXlsx::Document(".\\Cache.xlsx");
//    pXlsxCache->saveAs(QString::fromStdString(path));
//    pXlsxCache->deleteLater();
    QFile file(".\\Cache.xlsx");
    file.copy(QString::fromStdString(path));

    pXlsxCache = new QXlsx::Document(QString::fromStdString(path));
    pXlsxCache->setRowHeight(1,100);

    //写块号
    pXlsxCache->write(1,20,block);

    while(lProducts.size() != 0){
        int number = lProducts.at(0).getp_id();
        int ret = writeProduct(number);
        if(ret == EXCEL_ERROR){
            return ret;
        }
        lProducts.removeAt(0);
    }

    //写行号
    pXlsxCache->write(1,22,mCacheRow - 1);
    //写版本号到excel文件和config文件
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    pXlsxCache->write(1,21,timestamp);

    pXlsxCache->save();

    std::unique_lock<std::mutex> lk_config2(mConfigTool.configMutex);
    if(block == blockNum - 1){
        mConfigTool.setLastRow(mCacheRow - 1);
    }
    mConfigTool.updateOneVersion(block,timestamp);
    mConfigTool.writeConfig();
    lk_config2.unlock();

    pXlsxCache->save();
    pXlsxCache->deleteLater();
    pXlsxCache = nullptr;

    //上传
//    path = CachePath + std::to_string(block) + ".xlsx";
//    lFilePaths.push_back(QString::fromStdString(path));

    if(upload){
        return uploadBlockFile(block);
    }else{
        std::unique_lock<std::mutex> lk_modify2(mModifyMutex);
        lModifyBlocks.removeAll(block);
        lk_modify2.unlock();
    }

    return EXCEL_OK;
}

int QExcelTool::insertProduct(QProduct product)
{
    lProducts.clear();
    lProducts.append(product);
    lImagePaths.clear();
    if(!product.getp_image().isNull()){
        lImagePaths.append(product.getp_imagePath());
    }else{
        return EXCEL_ERROR;
    }
    return processRepeatProduct();
}

int QExcelTool::adjustBlock(int block, float magnitude)
{
    string path = CachePath + std::to_string(block) + ".xlsx";
    QFileInfo fileInfo(QString::fromStdString(path));
    if(!fileInfo.exists()){
        return EXCEL_NOFILE;
    }

    //修改
    if(pXlsxCache){
        pXlsxCache->save();
        pXlsxCache->deleteLater();
        pXlsxCache = nullptr;
    }
    pXlsxCache = new QXlsx::Document(QString::fromStdString(path));
    pXlsxCache->setRowHeight(1,100);

    int maxRow = pXlsxCache->dimension().lastRow();
    for(int row = 2;row <= maxRow; row++){
        auto item = pXlsxCache->read(row,6);
        if(!item.isNull()){
            float price = item.toFloat();
            price *= (100 + magnitude)*0.01;
            pXlsxCache->write(row,6,price);
        }
    }
    pXlsxCache->save();
    pXlsxCache->deleteLater();
    pXlsxCache = nullptr;

    return EXCEL_OK;
}

int QExcelTool::insertProductProcess(QProduct product)
{
    //读config
    std::unique_lock<std::mutex> lk_config(mConfigTool.configMutex);
    int blockNum = mConfigTool.getBlockSize();
    int lastBlockNum = mConfigTool.getLastBlockSize();
    int number = mConfigTool.getLastNumber();
    mCacheRow = mConfigTool.getLastRow() + 1;
    lOldVersion = mConfigTool.getLocalVersions();
    mConfigTool.backup();
    lk_config.unlock();

    //设置块锁
    bool isModifing = false;
    std::unique_lock<std::mutex> lk_modify(mModifyMutex);
    if(lModifyBlocks.indexOf(blockNum - 1) >= 0){
        isModifing = true;
    }else{
        lModifyBlocks.push_back(blockNum - 1);
        requestDone = true;
    }
    lk_modify.unlock();
    if(isModifing){
        return EXCEL_ISMODIFING;
    }

    string path;
    bool isCreated = false;
    //如果没有块或者当前块已满
    if(blockNum == 0 || lastBlockNum == BlockSize){
        pXlsxCache = new QXlsx::Document(".\\Cache.xlsx");

        blockNum++;
        lastBlockNum = 0;
        mCacheRow = 2;

        path = CachePath + std::to_string(blockNum - 1);
        path += ".xlsx";

        pXlsxCache->saveAs(QString::fromStdString(path));
        pXlsxCache->deleteLater();
        pXlsxCache = nullptr;
        isCreated = true;
    }else{
        path = CachePath + std::to_string(blockNum - 1);
        path += ".xlsx";
    }

    if(!isCreated){
        string newPath = TempPath + std::to_string(blockNum - 1);
        newPath += ".xlsx";

        QFile oldFile(QString::fromStdString(path));
    //    oldFile.remove(QString ::fromStdString(newPath));
        oldFile.copy(QString ::fromStdString(newPath));
    }else{
        //块锁
        std::unique_lock<std::mutex> lk_modify2(mModifyMutex);
        lModifyBlocks.removeAll(blockNum - 2);
        lModifyBlocks.push_back(blockNum - 1);
        requestDone = true;
        lk_modify2.unlock();
    }

    pXlsxCache = new QXlsx::Document(QString::fromStdString(path));
    pXlsxCache->setRowHeight(1,100);
    //写块号
    pXlsxCache->write(1,20,blockNum - 1);

    //上传图片
    lImagePaths.clear();
    if(!product.getp_image().isNull()){
//        QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
//        string path = ".\\img\\" + timestamp.toStdString() + ".jpg";
//        product.saveImage(QString::fromStdString(path));
//        product.setp_imagePath(QString::fromStdString(path));
        lImagePaths.append(product.getp_imagePath());

        uploadImages();
    }

    //写本地文件
    lProducts.push_front(product);
    number++;
    writeProduct(number);
    lastBlockNum++;

    //写行号
    pXlsxCache->write(1,22,mCacheRow - 1);
    //写版本号到excel文件和config文件
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    pXlsxCache->write(1,21,timestamp);

    mConfigTool.updateOneVersion(blockNum - 1,timestamp);

    pXlsxCache->save();
    pXlsxCache->deleteLater();
    pXlsxCache = nullptr;

    mConfigTool.setBlockSize(blockNum);
    mConfigTool.setLastBlockSize(lastBlockNum);
    mConfigTool.setLastNumber(number);
    mConfigTool.setLastRow(mCacheRow - 1);
    mConfigTool.writeConfig();

    //上传
    lFilePaths.clear();
    path = CachePath + std::to_string(blockNum - 1) + ".xlsx";
    lFilePaths.push_back(QString::fromStdString(path));
    uploadBlockFile(blockNum - 1);

    return EXCEL_OK;
}

int QExcelTool::getProductsAndBlocksSize(int &blockSize,int &productsSize)
{
    blockSize = mConfigTool.getBlockSize();
    productsSize = mConfigTool.getLastNumber();

    return EXCEL_OK;
}

int QExcelTool::readCacheLine(QProduct &product, int flag)
{
    if(flag){
        int num = pXlsxCache->read(mRow,1).toInt();
        product.setp_id(num);

        QString imgPath = pXlsxCache->read(mRow,2).toString();
        product.setp_imagePath(imgPath);

        auto brand = pXlsxCache->read(mRow,3);
        if(!brand.isNull()){
            product.setp_brand(brand.toString());
            brand.clear();
        }

        auto name = pXlsxCache->read(mRow,4);
        if(!name.isNull()){
            product.setp_name(name.toString());
            name.clear();
        }

        auto hole = pXlsxCache->read(mRow,12);
        if(!hole.isNull() && hole.toString()!=""){
            product.setp_hole(hole.toString());
            hole.clear();
        }

        auto remark = pXlsxCache->read(mRow,14);
        if(!remark.isNull()){
            product.setp_remark(remark.toString());
            remark.clear();
        }

        auto isDelete = pXlsxCache->read(mRow,16);
        if(!isDelete.isNull()){
            product.setp_isDelete(isDelete.toBool());
            isDelete.clear();
        }

        auto fitting = pXlsxCache->read(mRow,13);
        if(!fitting.isNull()){
            product.setp_fitting(fitting.toString());
            fitting.clear();
        }
    }

    QProductItem productItem;

    auto type = pXlsxCache->read(mRow,5);
    if(!type.isNull()){
        productItem.setp_type(type.toString());
        type.clear();
    }

    auto price = pXlsxCache->read(mRow,6);
    if(!price.isNull()){
        productItem.setp_price(price.toFloat());
//        price.clear();
    }

    auto source = pXlsxCache->read(mRow,7);
    if(!source.isNull()){
        productItem.setp_source(source.toString());
        source.clear();
    }

    auto material = pXlsxCache->read(mRow,8);
    if(!material.isNull()){
        productItem.setp_material(material.toString());
        material.clear();
    }

    auto color = pXlsxCache->read(mRow,9);
    if(!color.isNull()){
        productItem.setp_color(color.toString());
        color.clear();
    }

    auto moq = pXlsxCache->read(mRow,10);
    if(!moq.isNull()){
        productItem.setp_MOQ(moq.toString());
        moq.clear();
    }

    auto mpq = pXlsxCache->read(mRow,11);
    if(!mpq.isNull()){
        productItem.setp_MPQ(mpq.toString());
        mpq.clear();
    }

    auto itemIsDelete = pXlsxCache->read(mRow,15);
    if(!itemIsDelete.isNull()){
        productItem.setp_itemIsDelete(itemIsDelete.toBool());
        itemIsDelete.clear();
    }

    product.insertItem(productItem);

    return EXCEL_OK;
}

int QExcelTool::readCacheProduct(QProduct &product)
{
    readCacheLine(product,true);
    mRow++;
    while(mRow < mNextRow){
        readCacheLine(product);
        mRow++;
    }
    mRow--;
    return EXCEL_OK;
}

int QExcelTool::readLine(QProduct &product, int flag)
{
    //产品第一行
    if(flag){
        mImageIndex = -1;
        //编号
        int num = pXlsx->read(mRow,1).toInt();
        product.setp_id(num);

        bool notFound = true;
        //查看是否有图片
//        printf("size:\t%d\n",lImages.size());
        for(EImage img:lImages){
            mImageIndex++;
            if(img.point.y() == mRow -1){
                product.setp_image(img.img);
                notFound = false;
                break;
            }
        }
        if(notFound){
            //缺少图片错误
            throw FormatException(mRow,"缺少图片");
        }
    }

    //品牌
    auto brand = pXlsx->read(mRow,3);
    if(flag && !brand.isNull()){
        product.setp_brand(brand.toString());
        brand.clear();
    }else if(!flag && !brand.isNull()){
        throw FormatException(mRow,"品牌重复");
    }


    //中文名
    auto name = pXlsx->read(mRow,4);
    if(flag && !name.isNull()){
        product.setp_name(name.toString());
        name.clear();
    }else if(!flag && !brand.isNull()){
        throw FormatException(mRow,"名称重复");
    }

    //孔位
    auto hole = pXlsx->read(mRow,12);
    if(flag && !hole.isNull() && hole.toString()!=""){
        product.setp_hole(hole.toString());
        hole.clear();
    }else if(!flag && !hole.isNull()){
        throw FormatException(mRow,"孔位重复");
    }

    //配套件型号
    auto fitting = pXlsx->read(mRow,13);
    if(flag && !fitting.isNull()){
        product.setp_fitting(fitting.toString());
        fitting.clear();
    }else if(!flag &&!fitting.isNull()){
        throw FormatException(mRow,"配套件重复");
    }

    //备注
    auto remark = pXlsx->read(mRow,14);
    if(flag && !remark.isNull()){
        product.setp_remark(remark.toString());
        remark.clear();
    }else if(!flag && !remark.isNull()){
        throw FormatException(mRow,"备注重复");
    }

    QProductItem productItem;
    bool itemNull = true;

    //型号
    auto type = pXlsx->read(mRow,5);
    if(!type.isNull() && type.toString() != " "){
        itemNull = false;
        QString typeS = type.toString();
        int i = 0;
        for(;i < typeS.size();i++){
            if(typeS[i] != ' '){
                break;
            }
        }
        int e = typeS.size() - 1;
        for(;e >= 0;e--){
            if(typeS[e] != ' '){
                break;
            }
        }
        QString typeT;
        if(i != 0 || e !=  typeS.size() - 1){
            for(int k = i;k <= e;k++){
                typeT+=typeS[k];
            }
        }else{
            typeT = typeS;
        }
        productItem.setp_type(typeT);
        type.clear();
    }else{
        throw FormatException(mRow,"缺少型号");
    }

    bool priceNull = true;
    //价格
    auto price = pXlsx->read(mRow,6);
    if(itemNull && !price.isNull()){
        throw FormatException(mRow,"缺少型号");
    }else if(!price.isNull()){
        priceNull = false;
        productItem.setp_price(price.toFloat());
        price.clear();
    }

    //来源
    auto source = pXlsx->read(mRow,7);
    if(itemNull && !source.isNull()){
        throw FormatException(mRow,"缺少型号");
    }else if(priceNull && !source.isNull()){
        throw FormatException(mRow,"缺少价格");
    }else if(!priceNull && source.isNull()){
        throw FormatException(mRow,"缺少来源");
    }else if(!source.isNull()){
        productItem.setp_source(source.toString());
        source.clear();
    }

    //材料
    auto material = pXlsx->read(mRow,8);
    if(itemNull && !material.isNull()){
        throw FormatException(mRow,"缺少型号");
    }else if(priceNull && !material.isNull()){
        throw FormatException(mRow,"缺少价格");
    }else if(!material.isNull()){
        productItem.setp_material(material.toString());
        material.clear();
    }

    //颜色
    auto color = pXlsx->read(mRow,9);
    if(itemNull && !color.isNull()){
        throw FormatException(mRow,"缺少型号");
    }else if(!color.isNull()){
        productItem.setp_color(color.toString());
        color.clear();
    }



    //MOQ
    auto moq = pXlsx->read(mRow,10);
    if(itemNull && !moq.isNull()){
        throw FormatException(mRow,"缺少型号");
    }else if(!moq.isNull()){
        productItem.setp_MOQ(moq.toString());
        moq.clear();
    }

    //mpq
    auto mpq = pXlsx->read(mRow,11);
    if(itemNull && !mpq.isNull()){
        throw FormatException(mRow,"缺少型号");
    }else if(!mpq.isNull()){
        productItem.setp_MPQ(mpq.toString());
        mpq.clear();
    }

//    auto isDelete = pXlsx->read(mRow,15);
//    if(itemNull && !isDelete.isNull()){
//        throw FormatException(mRow,"there is not type");
//    }else if(!isDelete.isNull()){
//        productItem.setp_itemIsDelete(isDelete.toBool());
//    }

    product.insertItem(productItem);

    return EXCEL_OK;
}

int QExcelTool::readProduct(QProduct &product)
{
    readLine(product,true);
    mRow++;
    while (mRow < mNextRow) {
        readLine(product);
        mRow++;
    }
    mRow--;

    return EXCEL_OK;
}

int QExcelTool::processRepeatProduct()
{
    QJsonObject obj;
    QJsonArray arrP;
    QList<QString> lTypes;

    QList<int> record;
    for(int index = 0;index < lProducts.size();index++){
        QProduct p = lProducts[index];
        QJsonArray arr;
        for(int i = 0;i < p.getItemsSize(); i++){
            if(!lTypes.contains(p.at(i).getp_type())){
                arr.append(p.at(i).getp_type());
                lTypes.append(p.at(i).getp_type());
            }else{
                exceptionProcess(p);
                QFile img(p.getp_imagePath());
                if(img.exists()){
                    img.remove();
                }
                lImagePaths.removeAll(p.getp_imagePath());
                record.append(index);
                break;
            }
        }
        arrP.append(arr);
    }
    for(int i = record.size() - 1;i >= 0;i--){
        lProducts.removeAt(record[i]);
//        lImagePaths.removeAt(record[i]);
//        cout<<"removeProduct:\t"<<record[i]<<"\tsize:\t"<<lProducts.size()<<endl;
    }
    lTypes.clear();
    QJsonDocument doc;
    doc.setArray(arrP);

    obj.insert("tempList",arrP);

    pQExcelTool = this;

    std::unique_lock<std::mutex> lk_http(pHttpTool->mSendingMutex);
    pHttpTool->clearData();
    pHttpTool->setPort("0");
    pHttpTool->setData(obj);
    pHttpTool->setRouting("/product/repeatModelProduct");
    pHttpTool->bindCallback(QExcelTool::interfaceCallback);
    pHttpTool->POST(JUDGETYPE_CODE);
    lk_http.unlock();

    if(record.size() != 0){
        return EXCEL_ERROR;
    }
    return EXCEL_OK;
}

void QExcelTool::downloadFileCallback(QJsonObject info, int requestCode, QByteArray data)
{
    if(info.value("statusCode").toInt() == 200){
        QString url = info.value("url").toString();
        auto sp = url.split("/");
        auto name = sp.last();
        string path = ImgFloder + "\\" + name.toStdString();
        QFile file(QString::fromStdString(path));
        if(file.exists()){
            file.remove();
        }
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.close();
    }
    return;
}

void QExcelTool::downloadEXCELCallback(QJsonObject info, int requestCode, QByteArray data)
{
    int blockNum = requestCode - 1000;
    if(info.value("statusCode").toInt() == 200){
        //删除旧文件
        string path = CachePath + std::to_string(blockNum);
        path += ".xlsx";

        cout<< path <<"\t";

        QFileInfo fInfo(QString::fromStdString(path));
        if(fInfo.exists()){
            QFile file(QString::fromStdString(path));
            file.remove();
        }
        //写新文件
        QFile newFile(QString::fromStdString(path));
        newFile.open(QIODevice::WriteOnly);
        newFile.write(data);
        newFile.close();

        cout<< "close" <<"\t";

        //更新config
        QXlsx::Document* xlsxCache;

        xlsxCache = new QXlsx::Document(QString::fromStdString(path));
        QString version = xlsxCache->read(1,21).toString();
        int row = xlsxCache->dimension().lastRow();

        std::unique_lock<std::mutex> lk(mConfigTool.configMutex);
        if(blockNum == mConfigTool.getBlockSize() - 1){
            mConfigTool.setBlockSize(blockNum+1);
            mConfigTool.setLastRow(row);

            while(row > 1){
                auto number = xlsxCache->read(row,1);
                if(!number.isNull()){
                    mConfigTool.setLastNumber(number.toInt());
                    mConfigTool.setLastBlockSize(number.toInt()%BlockSize);
                    break;
                }
                row--;
            }
        }
        mConfigTool.updateOneVersion(blockNum,version);

        lk.unlock();
        xlsxCache->deleteLater();

        cout<< "deletelater" <<"\n\n";

        std::unique_lock<std::mutex> lk1(mConfigTool.configMutex);
        mConfigTool.writeConfig();
        lk1.unlock();
    }else{
        if(info.value("fileType").toInt() == 1){
            emit signals_finished(DOWNLOAD_IMAGE_ERROR,blockNum);
        }else{
            emit signals_finished(DOWNLOAD_ERROR,blockNum);
        }
    }

    std::unique_lock<std::mutex> lk_modify(mModifyMutex);
    lModifyBlocks.removeAll(blockNum);
    if(requestDone && lModifyBlocks.empty()){
        isDownloading = false;
        requestDone = false;

        emit signals_finished(DOWNLOAD_OK,-1);
        cout<<"DOWNLOAD_OK\n";

//        if(finishOne){
//            finishOne = false;
//            emit signals_finished(DOWNLOAD_OK,-1);
//        }else{
//            finishOne = true;
//        }

    }
    lk_modify.unlock();

    return;
}

void QExcelTool::batchAdjustmentCallback(QJsonObject info, QJsonValue data)
{
    if(info.value("statusCode").toInt() == 200){
        std::unique_lock<std::mutex> lk_config(mConfigTool.configMutex);
        int size = info.value("size").toInt();
        float magnitude = info.value("changePrecent").toDouble();
        auto json = data.toObject();
        for(int i = 0;i < size;i++){
            if(json.value(QString::fromStdString(std::to_string(i))).isNull()){
                continue;
            }
            int ret = adjustBlock(i,magnitude);
            QString version = json.value(QString::fromStdString(std::to_string(i))).toString();
            if(ret == EXCEL_OK){
                mConfigTool.updateOneVersion(i,version);
            }
        }
        lk_config.unlock();
        emit signals_finished(BATCHADJUSTMENT_OK,-1);
    }else{
        emit signals_finished(BATCHADJUSTMENT_ERROR,-1);
    }
}

void QExcelTool::judgeTypeCallback(QJsonObject info, QJsonValue data)
{
    if(info.value("statusCode").toInt() == 200){
        //读取config
        std::unique_lock<std::mutex> lk_config(mConfigTool.configMutex);
        int blockNum = mConfigTool.getBlockSize();
        int lastBlockNum = mConfigTool.getLastBlockSize();
        lk_config.unlock();

        auto json = data.toArray();
        int size = json.size();
//        QList<QProduct> oneBlockProduct;
        int block = 0;

        QList<QProduct> pCopy = lProducts;
        QMap<int,QList<QProduct>> map;
//        cout<<"SrclProductsSize:"<<lProducts.size()<<endl;
        for(int i = size - 1;i >= 0; i--){
            int item = json.at(i).toObject().value("index").toString().toInt();
            int id = json.at(i).toObject().value("id").toInt();
            QProduct product = pCopy.at(item);
            lImagePaths.removeAt(item);
            product.setp_id(id);

            block = (id - 1) / BlockSize;
            if(!map.keys().contains(block)){
                QList<QProduct> temp;
                map.insert(block,temp);
            }
            map[block].append(product);

            pCopy.removeAt(item);
        }

        if(!lImagePaths.empty()){
            uploadImages();
        }

        QMap<int,QList<QProduct>>::Iterator it = map.begin();
        for(;it != map.end();it++){
            block = it.key();
            if(block != blockNum - 1){
                updateRepeatProducts(block,it.value());
            }else if(pCopy.size() == 0 || lastBlockNum % BlockSize == 0){
                updateRepeatProducts(block,it.value());
            }else{
                updateRepeatProducts(block,it.value(),false);
            }
        }

        if(pCopy.size() == 0){
            cout<<"全部重复"<<endl;
//            emit signals_finished(UPLOAD_ALL_REITERATED, -1);
            return;
        }
        lProducts = pCopy;
        cout<<"lProductsSize:"<<lProducts.size()<<endl;
        saveToCache();

    }else{
        emit signals_finished(UPLOAD_ERROR,-1);
    }

    if(pXlsx){
        pXlsx->save();
        pXlsx->deleteLater();
        pXlsx = nullptr;
    }

    if(pXlsxError){
        pXlsxError->save();
        pXlsxError->deleteLater();
        pXlsxError = nullptr;
    }

    if(pXlsxCache){
        pXlsxCache->save();
        pXlsxCache->deleteLater();
        pXlsxCache = nullptr;
    }
    lProducts.clear();
    lImages.clear();

    return;
}

void QExcelTool::replaceImageCallback(QJsonObject info, QJsonValue data)
{
    if(info.value("statusCode").toInt() == 200){
        auto json = data.toObject();
        QString version = json.value("time").toString();
        QString src = json.value("old").toString();
        QString dst = json.value("new").toString();
        int number = json.value("num").toInt();

        //删除旧图片
        QString path = QString::fromStdString(ImgFloder) + "\\" + src;
        QFile file(path);
        if(file.exists()){
            file.remove();
        }
//cout << __func__ << "\tnumber:\t" << number << endl;
//cout << __func__ << "\tsrc:\t" << src.toStdString() << endl;
//cout << __func__ << "\tdst:\t" << dst.toStdString() << endl;
        //修改excel内路径
        std::unique_lock<std::mutex> lk_modify(mModifyMutex);
        path = QString::fromStdString(ImgFloder) + "\\" + dst;
        int block = (number - 1)/BlockSize;
//cout << __func__ << "\tblock:\t" << block << endl;
        string excelPath = CachePath + std::to_string(block) + ".xlsx";
        QXlsx::Document* xlsx = new QXlsx::Document(QString::fromStdString(excelPath));
        int row = 2;
        int maxRow = xlsx->dimension().rowCount();
        while(row <= maxRow){
            auto cell = xlsx->read(row,1);
            if(!cell.isNull()){
                if(cell.toInt() == number){
                    xlsx->write(row,2,path);
                    break;
                }
            }
            row++;
        }
        xlsx->save();
        xlsx->deleteLater();
        lk_modify.unlock();

        //修改版本号
        std::unique_lock<std::mutex> lk_config(mConfigTool.configMutex);
        mConfigTool.updateOneVersion(block,version);
        mConfigTool.writeConfig();
        lk_config.unlock();

        emit signals_finished(REPLACE_IMAGE_OK,-1);
    }else{
//        cout << __func__ << "REPLACE_IMAGE_ERROR\n";
        emit signals_finished(REPLACE_IMAGE_ERROR,-1);
    }
    return;
}

void QExcelTool::getImageNameCallback(QJsonObject info, QJsonValue data)
{
    if(info.value("statusCode").toInt() == 200){
        auto nameList = data.toArray();
        for(auto item : nameList){
            QString url = QString::fromStdString(csIP) + item.toString();
            url.replace("img","images");
            url.replace("\\","/");
//            cout<<url.toStdString()<<endl;
            pHttpTool->DOWNLOAD(url);
        }
    }else{
        emit signals_finished(GET_IMAGE_NAME_ERROR,-1);
    }

    return;
}

void QExcelTool::uploadImagesCallback(QJsonObject info,QJsonValue data)
{
    std::unique_lock<std::mutex> lk_modify(mModifyMutex);
    if(info.value("statusCode").toInt() == 200){
        cout << __func__ << "\tuploadImages succ\n";
    }else{
        emit signals_finished(UPLOAD_IMAGE_ERROR,-1);
    }
    if(finishOne){
        emit signals_finished(UPLOAD_OK,-1);
        finishOne = false;
    }else{
        finishOne = true;
    }

    lk_modify.unlock();
    lImagePaths.clear();
    return;
}

void QExcelTool::getVersionCallback(QJsonObject info, QJsonValue data)
{
    if(info.value("statusCode").toInt() == 200){
        std::unique_lock<std::mutex> lk_download(mModifyMutex);
        isDownloading = true;
        lModifyBlocks.clear();
        lk_download.unlock();

        int realSize = info.value("size").toInt();

        std::unique_lock<std::mutex> lk(mConfigTool.configMutex);
        QList<QString> versions =  mConfigTool.getLocalVersions();
        int flag = mConfigTool.getFlag();
        mConfigTool.setBlockSize(realSize);
        mConfigTool.writeConfig();
        lk.unlock();

        bool needUpdate = false;
        QJsonObject obj = data.toObject();
        for(int i = 0;i < realSize;i++){
            QString version = obj.value(QString::fromStdString(std::to_string(i))).toString();
//            cout << "i:\t" << i << "\tversion:\t" << version.toStdString() << endl;
            if(version == ""){
                continue;
            }

            if(versions.size() <= i||version != versions.at(i)){
                std::unique_lock<std::mutex> lk_modify(mModifyMutex);
                lModifyBlocks.push_back(i);
                lk_modify.unlock();
                needUpdate = true;
                downloadBlockFile(i, flag);

                //获取
                QJsonObject obj;
                obj.insert("blockNum",QString::fromStdString(std::to_string(i)));
                std::unique_lock<std::mutex> lk_http(pHttpTool->mSendingMutex);
                pHttpTool->setPort("0");
                pHttpTool->setRouting("/product/OutBlockPic");
                pHttpTool->bindCallback(QExcelTool::interfaceCallback);
                pHttpTool->GET(GET_IMAGE_NAME_CODE);
                lk_http.unlock();
            }
            continue;
        }

        std::unique_lock<std::mutex> lk_modify(mModifyMutex);
        requestDone = true;
        lk_modify.unlock();

        if(realSize < versions.size()){

            for(int i = realSize; i < versions.size();i++){
                string path = CachePath + std::to_string(i) + ".xlsx";
                QFileInfo ff(QString::fromStdString(path));
                if(ff.exists()){
                    QFile file(QString::fromStdString(path));
                    file.remove();
                }
            }
        }

        if(!needUpdate){
            emit signals_finished(NOT_UPDATE,-1);
            cout<<"NOT_UPDATE\n";
        }

    }else{
        //发送获取版本号错误的信号
        emit signals_finished(GETVERSION_ERROR,-1);
    }
}

void QExcelTool::uploadFilesCallback(QJsonObject info, int requestCode, QJsonValue data)
{
    int block = requestCode - 20000;
    cout<<"block:"<<block<<"\trquestCode:"<<requestCode<<endl;
    //成功上传
    if(info.value("statusCode").toInt() == 200){

        //上传文件不合法
        if(info.value("requestCode").toInt() == -1){
            emit signals_finished(UPLOAD_ERROR,block);
            recover();
            cout<<"error in 1657\n";
            return ;
        }

        std::unique_lock<std::mutex> lk_modify(mModifyMutex);
        lModifyBlocks.clear();
        if(requestDone && lModifyBlocks.empty()){

            if(finishOne){
                finishOne = false;
                emit signals_finished(UPLOAD_OK,-1);

            }else{
                finishOne = true;
            }
            requestDone = false;

            printf("uplaoddone\n");

            //删除全部缓存
            QDir dir(QString::fromStdString(TempFolder));
            if(dir.exists()){
                dir.removeRecursively();
            }
            mkdir(TempFolder.c_str());
        }
        lk_modify.unlock();
    }else{
        emit signals_finished(UPLOAD_ERROR,block);
        //回滚
        //===================================================================================
        recover();

    }
    lOldVersion.clear();
    return;
}

int QExcelTool::recover()
{
    std::unique_lock<std::mutex> lk_modify(mModifyMutex);
    QDir dir(QString::fromStdString(TempFolder));
    dir.setFilter(QDir::Files);//除文件，其他的过滤掉
    auto infoList = dir.entryInfoList();

    //删除新建的文件
    QDir cacheDir(QString::fromStdString(CacheFolder));
    cacheDir.setFilter(QDir::Files);//除文件，其他的过滤掉
    auto cacheInfoList = cacheDir.entryInfoList();
    for(int i = lOldVersion.size(); i < cacheInfoList.size(); i++){
        string path = CachePath + std::to_string(i) + ".xlsx";

        QFileInfo fInfo(QString::fromStdString(path));
        if(fInfo.exists()){
            QFile file(fInfo.filePath());

            file.remove();
        }
    }
    //恢复修改文件
    for(auto item : infoList){
        QFile file(item.filePath());
        string path = CacheFolder + "\\" + item.fileName().toStdString();

        QFile errFile(QString::fromStdString(path));
        cout<<"remove:\t"<<path<<endl;
        errFile.remove();

        file.copy(QString::fromStdString(path));
        file.remove();
    }
    lModifyBlocks.clear();
    lk_modify.unlock();

    std::unique_lock<std::mutex> lk_config(mConfigTool.configMutex);
    mConfigTool.recover();
    mConfigTool.writeConfig();
    lk_config.unlock();

    return EXCEL_OK;
}

void QExcelTool::uploadCallback(QJsonObject info, int requestCode, QJsonValue data)
{
    int block = requestCode - 10000;
    //成功上传
    if(info.value("statusCode").toInt() == 200){
        std::unique_lock<std::mutex> lk_modify(mModifyMutex);
        lModifyBlocks.removeAll(block);
        if(requestDone && lModifyBlocks.empty()){
            requestDone = false;
            emit signals_finished(UPLOAD_OK,-1);
            printf("uplaoddone\n");

            //删除全部缓存
            QDir dir(QString::fromStdString(TempFolder));
            if(dir.exists()){
                dir.removeRecursively();
            }
            mkdir(TempFolder.c_str());
        }
        lk_modify.unlock();
    }else{
        emit signals_finished(UPLOAD_ERROR,block);
        //回滚
        //===================================================================================
//        QDir dir(QString::fromStdString(TempFolder));
    }
    return;
}

int QExcelTool::downloadBlockFile(int block, int flag)
{
//    cout << __func__ << block << "\n";
    QJsonObject data;
    data.insert("blockNum",QString::fromStdString(std::to_string(block)));
    data.insert("flag",QString::fromStdString(std::to_string(flag)));

    std::unique_lock<std::mutex> lk(pHttpTool->mSendingMutex);

    pHttpTool->setPort("0");
    pHttpTool->setRouting("/product/OutBlock");
    pHttpTool->setData(data);
    pHttpTool->bindCallback(interfaceFileCallback);
    pHttpTool->GET(1000+block);

    lk.unlock();

    return EXCEL_OK;
}

int QExcelTool::CacheInit(int flag)
{
    //config锁
    std::unique_lock<std::mutex> lk_config(mConfigTool.configMutex);
    mConfigTool.init();
    int f = mConfigTool.getFlag();
    lk_config.unlock();

    if(f == flag){
        init();
        return updateFiles();
    }
    QDir dir(QString::fromStdString(CacheFolder));
    if(dir.exists()){
        dir.removeRecursively();

    }

    string path = ".\\config.txt";
    QFileInfo fInfo(QString::fromStdString(path));
    if(fInfo.exists()){
        QFile file(QString::fromStdString(path));
        file.remove();
    }
    init();

    std::unique_lock<std::mutex> lk_config2(mConfigTool.configMutex);
    mConfigTool.init();
    mConfigTool.setFlag(flag);
    mConfigTool.writeConfig();
    lk_config2.unlock();

    updateFiles();

    return EXCEL_OK;
}

int QExcelTool::updateFiles()
{
    if(pHttpTool == nullptr){
        return EXCEL_NOHTTPTOOL;
    }

    pQExcelTool = this;

    //发请求获取文件版本号并比对
    std::unique_lock<std::mutex> lk(pHttpTool->mSendingMutex);
    pHttpTool->setPort("0");
    pHttpTool->setRouting("/product/findAllBlockEdition");
    pHttpTool->bindCallback(QExcelTool::interfaceCallback);
    pHttpTool->bindCallback(QExcelTool::interfaceFileCallback);
    pHttpTool->GET(GET_VERSION_CODE);
    lk.unlock();

    return EXCEL_OK;
}

int QExcelTool::uploadBlockFiles(int block)
{
    if(pHttpTool == nullptr){
        return EXCEL_NOHTTPTOOL;
    }

    std::lock_guard<std::mutex> lk_modify(mModifyMutex);

    if(lFilePaths.isEmpty()){
        emit signals_finished(UPLOAD_ERROR,-1);
        return EXCEL_ERROR;
    }
    QJsonObject obj;
    obj.insert("blockNum",QString::fromStdString(std::to_string(block)));

    pQExcelTool = this;

    std::unique_lock<std::mutex> lk_http(pHttpTool->mSendingMutex);
    pHttpTool->clearData();
    pHttpTool->setFileType(1);
    pHttpTool->setPort("0");
    pHttpTool->setData(obj);
    pHttpTool->setRouting("/product/InsertBlock");
    pHttpTool->bindCallback(QExcelTool::interfaceCallback);
    for(auto item : lFilePaths){
//        cout<<item.toStdString()<<endl;
        pHttpTool->insertFilePath(item);
    }
    pHttpTool->POST(20000+block);
    lk_http.unlock();

    return EXCEL_OK;
}

int QExcelTool::uploadBlockFile(int block)
{
    std::unique_lock<std::mutex> lk(mConfigTool.configMutex);
    int size = mConfigTool.getBlockSize();
    lk.unlock();
    if(size <= block){
        recover();
        return EXCEL_ERROR;
    }

    if(pHttpTool == nullptr){
        recover();
        return EXCEL_NOHTTPTOOL;
    }

    string path = CachePath + std::to_string(block) + ".xlsx";
    QFileInfo fInfo(QString::fromStdString(path));
    if(!fInfo.exists()){
        recover();
        return EXCEL_NOFILE;
    }

    QJsonObject obj;
    obj.insert("num",QString::fromStdString(std::to_string(block)));

    pQExcelTool = this;

    std::unique_lock<std::mutex> lk_http(pHttpTool->mSendingMutex);
    pHttpTool->clearData();
    pHttpTool->setPort("0");
    pHttpTool->setData(obj);
    pHttpTool->setRouting("/product/InsertBlock");
    pHttpTool->bindCallback(QExcelTool::interfaceCallback);
    pHttpTool->setFilePath(QString::fromStdString(path));
    pHttpTool->POST(10000+block);
    lk_http.unlock();
    std::cout<< "uploadBlockFile\t"<< block << std::endl;

    return EXCEL_OK;
}

int QExcelTool::exceptionProcess(int row,QString msg)
{
//    std::cout<<"error in\t"<<"preRow:\t"<<mPreRow<<"\tRow:\t"<<mRow<<"\tNextRow:\t"<<mNextRow<<std::endl;
//    std::cout<<msg.toStdString()<<std::endl<<endl;

    if(pXlsxError == nullptr){
        QFile file("Error.xlsx");
        file.copy(".\\ErrorTemp.xlsx");
        pXlsxError = new QXlsx::Document(".\\ErrorTemp.xlsx");
        pXlsxError->setRowHeight(1, 100);
    }
    QXlsx::Format format = getFormat();

    QVariant var = QVariant::fromValue(msg);
    pXlsxError->write(mErrorRow,15,var,format);

    if(msg != "缺少图片"){
//        printf("index:\t%d\trow:\t%d\n",mImageIndex,mPreRow+1);
        pXlsxError->insertImage(mErrorRow-1,1,lImages.at(mImageIndex).img);
    }

    for(int r = mPreRow + 1;r < mNextRow;r++){
        pXlsxError->setRowHeight(mErrorRow,100);
        for(int c = 1;c <= 14;c++){
            QVariant var = pXlsx->read(r,c);
            pXlsxError->write(mErrorRow,c,var,format);
        }

        mErrorRow++;
    }
    return EXCEL_OK;
}

int QExcelTool::exceptionProcess(QProduct product)
{
    if(pXlsxError == nullptr){
        QFile file("Error.xlsx");
        file.copy(".\\ErrorTemp.xlsx");
        pXlsxError = new QXlsx::Document(".\\ErrorTemp.xlsx");
        pXlsxError->setRowHeight(1, 100);
    }else{
        QFile file(".\\testtesttest.xlsx");
        if(file.exists()){
            file.remove();
        }
        pXlsxError->saveAs(".\\testtesttest.xlsx");
    }
    QXlsx::Format format = getFormat();

    if(!product.getp_image().isNull()){
        pXlsxError->insertImage(mErrorRow-1,1,product.getp_image());
    }
    pXlsxError->write(mErrorRow,15,"型号重复",format);

    return writeProduct(product.getp_id(),product,pXlsxError,mErrorRow,true,true);
}

int QExcelTool::nextProduct(int id)
{
    mPreRow = mRow;
    mRow += 1;
    mNextRow = mRow;

    while(true){
        mNextRow++;
        auto num = pXlsx->read(mNextRow,1);

        if(mNextRow > mMaxRow){
            mNextRow = mRow + 1;
            while(mNextRow <= mMaxRow){
                if(pXlsx->read(mNextRow,5).isNull()){
                    break;
                }
                mNextRow++;
            }
            return EXCEL_FINISHED;
        }
        if(!num.isNull()){
//            if(!id||num.toInt() == id){
//                break;
//            }
//            nextProduct(id);
            break;
        }

    }
    return EXCEL_OK;
}

int QExcelTool::nextCacheProduct(int id)
{
    mPreRow = mRow;
    mRow += 1;
    mNextRow = mRow;

    while(true){
        mNextRow++;
        auto num = pXlsxCache->read(mNextRow,1);

        if(mNextRow > mMaxRow){
            mNextRow = mRow + 1;
            while(mNextRow <= mMaxRow){
                if(pXlsxCache->read(mNextRow,5).isNull()){
                    break;
                }
                mNextRow++;
            }
            return EXCEL_FINISHED;
        }
        if(!num.isNull()){
            if(id <= 0||num.toInt() == id){
                break;
            }
            nextCacheProduct(id);
            break;
        }
    }
    return EXCEL_OK;
}

int QExcelTool::findRowById(int id)
{
    while(mRow <= mMaxRow){
        auto num = pXlsxCache->read(mRow,1);
        if(!num.isNull()){
            if(num.toInt() == id){
                break;
            }
        }
        mRow++;
    }
    mNextRow = mRow;
    while(true){
        mNextRow++;
        auto num = pXlsxCache->read(mNextRow,1);
        if(mNextRow > mMaxRow){
            mNextRow = mRow + 1;
            while(mNextRow <= mMaxRow){
                if(pXlsxCache->read(mNextRow,5).isNull()){
                    break;
                }
                mNextRow++;
            }
            return EXCEL_FINISHED;
        }
        if(!num.isNull()){
            break;
        }
    }
    return EXCEL_OK;
}

QXlsx::Format QExcelTool::getFormat(){
//    QXlsx::Format format =  pXlsx->rowFormat(1);
    QXlsx::Format format;

    format.setBorderColor(QColor("#000000"));
    format.setBorderStyle(QXlsx::Format::BorderThin);/*边框样式*/


    format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);/*横向居中*/

    return format;
}
