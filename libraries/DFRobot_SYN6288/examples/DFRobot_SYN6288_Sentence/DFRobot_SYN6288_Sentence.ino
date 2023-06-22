/*
语音播放短文字示例
*/

/*
这是中文语音合成示例
*/
const uint16_t test1[] =
{0x8FD9, 0x662F, 0x4E2D, 0x6587, 0x8BED, 0x97F3, 0x5408, 0x6210, 0x793A, 0x4F8B};

/*
在语音合成技术日益成熟走向市场的今天，作为语音合成的基础的语音合成语料库扮演着越来越重要的角色。
作为语音合成和语音分析的物质基础，建立设计合理、高质量录音的语音语料库有着极为重要的研究价值和实用价值。
*/
const uint16_t test2[] =
{0x5728, 0x8BED, 0x97F3, 0x5408, 0x6210, 0x6280, 0x672F, 0x65E5, 0x76CA, 0x6210,
 0x719F, 0x8D70, 0x5411, 0x5E02, 0x573A, 0x7684, 0x4ECA, 0x5929, 0xFF0C, 0x4F5C,
 0x4E3A, 0x8BED, 0x97F3, 0x5408, 0x6210, 0x7684, 0x57FA, 0x7840, 0x7684, 0x8BED,
 0x97F3, 0x5408, 0x6210, 0x8BED, 0x6599, 0x5E93, 0x626E, 0x6F14, 0x7740, 0x8D8A,
 0x6765, 0x8D8A, 0x91CD, 0x8981, 0x7684, 0x89D2, 0x8272, 0x3002, 0x4F5C, 0x4E3A,
 0x8BED, 0x97F3, 0x5408, 0x6210, 0x548C, 0x8BED, 0x97F3, 0x5206, 0x6790, 0x7684,
 0x7269, 0x8D28, 0x57FA, 0x7840, 0xFF0C, 0x5EFA, 0x7ACB, 0x8BBE, 0x8BA1, 0x5408,
 0x7406, 0x3001, 0x9AD8, 0x8D28, 0x91CF, 0x5F55, 0x97F3, 0x7684, 0x8BED, 0x97F3,
 0x8BED, 0x6599, 0x5E93, 0x6709, 0x7740, 0x6781, 0x4E3A, 0x91CD, 0x8981, 0x7684,
 0x7814, 0x7A76, 0x4EF7, 0x503C, 0x548C, 0x5B9E, 0x7528, 0x4EF7, 0x503C, 0x3002};

/*
当前工作的重中之重是要在重重困难中保证重庆市的重点工程的顺利进行，坚决拒绝重复建设。。。。
*/
const uint16_t test3[] =
{0x5F53, 0x524D, 0x5DE5, 0x4F5C, 0x7684, 0x91CD, 0x4E2D, 0x4E4B, 0x91CD, 0x662F,
 0x8981, 0x5728, 0x91CD, 0x91CD, 0x56F0, 0x96BE, 0x4E2D, 0x4FDD, 0x8BC1, 0x91CD,
 0x5E86, 0x5E02, 0x7684, 0x91CD, 0x70B9, 0x5DE5, 0x7A0B, 0x7684, 0x987A, 0x5229,
 0x8FDB, 0x884C, 0xFF0C, 0x575A, 0x51B3, 0x62D2, 0x7EDD, 0x91CD, 0x590D, 0x5EFA,
 0x8BBE, 0x3002, 0x3002, 0x3002, 0x3002};

#include "DFRobot_SYN6288.h"

SoftwareSerial softSerialSyn6288(2, 3);

DFRobot_SYN6288 syn6288;

void setup()
{
    syn6288.begin(&Serial, 0, 1, 13);
    // syn6288.begin(&softSerialSyn6288, 13);
    syn6288.setCode(syn6288.eUNICODE);
}

void loop()
{
    syn6288.setVolume(true, 2);
    syn6288.setSpeed(0);
    syn6288.playText(test1, sizeof(test1), 1);

    syn6288.setVolume(true, 9);
    syn6288.setSpeed(3);
    syn6288.playText(test2, sizeof(test2), 2);

    syn6288.setVolume(true, 16);
    syn6288.setSpeed(5);
    syn6288.playText(test3, sizeof(test3), 3);
}