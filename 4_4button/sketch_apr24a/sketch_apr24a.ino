#include <IRremote.h>

int RECV_PIN = 3;//定义红外接收器的引脚为11

IRrecv irrecv(RECV_PIN);

decode_results results;//注意这个数据类型

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // 初始化红外接收器
}

void loop() {
  if (irrecv.decode(&results)) {  //注意这里取解码结果的方法，传递的是变量指针。
    Serial.println(results.value,HEX); //以16进制换行输出接收代码。注意取值方法【results.value】。
    irrecv.resume(); // 给红外传送指令，让其继续接收下一个值。同315M无线接收一个道理。
  }
}
