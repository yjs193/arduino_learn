/*!
 * MindPlus
 * arduino
 *
 */
#include <DFRobot_GPS.h>
#include <SoftwareSerial.h>
// 创建对象
DFRobot_GPS        gps;
SoftwareSerial softSerialGps(P0, P1);
;

// 主程序开始
void setup() {
  Serial.begin(9600);
  gps.begin(&softSerialGps);
}
void loop() {
  Serial.print("getYear: "); Serial.println(gps.getYear()); // 年
  Serial.print("getMonth:  "); Serial.println(gps.getMonth()); // 月
  Serial.print("getDay:  "); Serial.println(gps.getDay());  //日
  Serial.print("getHour:  "); Serial.println(gps.getHour());  //时
  Serial.print("getMinutes:  "); Serial.println(gps.getMinutes());  //分
  Serial.print("getSeconds:  "); Serial.println(gps.getSeconds());  //秒
  Serial.print("getDateTime:  "); Serial.println(gps.getDateTime());  //日期+时间
  Serial.print("getLatitude:  "); Serial.println(gps.getLatitude());  // 纬度
  Serial.print("getLongitude:  "); Serial.println(gps.getLongitude());  // 经度
  Serial.print("getState:  "); Serial.println(gps.getState());   // 状态
  Serial.print("getLatitudeHemisphere:  "); Serial.println(gps.getLatitudeHemisphere()); // 纬度半球
  Serial.print("getLongitudeHemisphere:  "); Serial.println(gps.getLongitudeHemisphere()); // 经度半球
  Serial.print("getGroundSpeed:  "); Serial.println(gps.getGroundSpeed());  // 地面速度

  Serial.print("getSatellitesCount:  "); Serial.println(gps.getSatellitesCount()); // 卫星数量
  Serial.print("getAltitude:  "); Serial.println(gps.getAltitude());  // 海拔

  Serial.print("getPdop:  "); Serial.println(gps.getPdop()); // 位置精度
  Serial.print("getHdop:  "); Serial.println(gps.getHdop()); // 水平精度
  Serial.print("getVdop:  "); Serial.println(gps.getVdop()); // 垂直精度
  Serial.print("getPositioningMode:  "); Serial.println(gps.getPositioningMode()); //定位模式

  Serial.println("-----------------Dividing line-----------------");
  delay(1000);
}
