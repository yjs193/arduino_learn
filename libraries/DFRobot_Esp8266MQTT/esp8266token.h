#ifndef _ESP8266TOKEN_H_
#define _ESP8266TOKEN_H_

#include "esp8266base64.h"
#include "esp8266md5.h"

#include <stdio.h>
#include <string.h>

typedef  void (*iapfun)(void); 			//����һ���������͵Ĳ���


typedef struct
{

	char authorization[128];			//������õ���ǩ��
	
	char version[8];					//�汾
	char token[32];						//Token
	unsigned int size;					//�̼���С(�ֽ�)
	char md5[40];						//MD5У��
	
	unsigned int addr;					//��ת��ַ
	
	unsigned char ota_start : 1;		//OTA��ʼ
	unsigned char ota_check : 1;		//OTA������
	unsigned char ota_download_ok : 1;	//���سɹ�
	unsigned char ota_report_ok : 1;	//�ϱ�״̬�ɹ�
	unsigned char reverse : 4;

} OTA_INFO;

extern OTA_INFO ota_info;

class DF_TOKEN
{
public:
unsigned char TOKEN_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *token_buf, unsigned short token_buf_len);
};
#endif
