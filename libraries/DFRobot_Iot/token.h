#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "base64.h"
#include "md5.h"

#include <stdio.h>
#include <string.h>

typedef  void (*iapfun)(void); 			//定义一个函数类型的参数


typedef struct
{

	char authorization[128];			//检测是用到的签名
	
	char version[8];					//版本
	char token[32];						//Token
	unsigned int size;					//固件大小(字节)
	char md5[40];						//MD5校验
	
	unsigned int addr;					//跳转地址
	
	unsigned char ota_start : 1;		//OTA开始
	unsigned char ota_check : 1;		//OTA任务检测
	unsigned char ota_download_ok : 1;	//下载成功
	unsigned char ota_report_ok : 1;	//上报状态成功
	unsigned char reverse : 4;

} OTA_INFO;

extern OTA_INFO ota_info;

class DF_TOKEN
{
public:
unsigned char TOKEN_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *token_buf, unsigned short token_buf_len);
};
#endif
