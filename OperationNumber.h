#ifndef OPERATIONNUMBER_H
#define OPERATIONNUMBER_H

//获取版本号出错
const int GETVERSION_ERROR = 0x100;

//上传文件错误
const int UPLOAD_ERROR = 0x200;
//上传文件全部完成
const int UPLOAD_OK = 0x201;
//上传图片错误
const int UPLOAD_IMAGE_ERROR = 0x202;
//上传的产品全部重复
const int UPLOAD_ALL_REITERATED = 0x203;

//下载文件错误
const int DOWNLOAD_ERROR = 0x300;
//下载文件全部完成
const int DOWNLOAD_OK = 0x301;
//无需更新
const int NOT_UPDATE = 0x302;
//获取图片名称错误
const int GET_IMAGE_NAME_ERROR = 0x303;
//替换图片错误
const int REPLACE_IMAGE_ERROR = 0x304;
//替换图片成功
const int REPLACE_IMAGE_OK = 0x305;
//下载图片失败
const int DOWNLOAD_IMAGE_ERROR = 0x306;
//修改价格成功
const int BATCHADJUSTMENT_OK = 0x400;
//修改价格失败
const int BATCHADJUSTMENT_ERROR = 0x401;

#endif // OPERATIONNUMBER_H
