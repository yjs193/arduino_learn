#include "DFRobot_PlayerMini.h"

DFRobot_PlayerMini::DFRobot_PlayerMini()
{
}

DFRobot_PlayerMini::~DFRobot_PlayerMini()
{
}

void DFRobot_PlayerMini::setTimeOut(unsigned long timeOutDuration){
  _timeOutDuration = timeOutDuration;
}

void DFRobot_PlayerMini::uint16ToArray(uint16_t value, uint8_t *array){
  *array = (uint8_t)(value>>8);
  *(array+1) = (uint8_t)(value);
}

uint16_t DFRobot_PlayerMini::calculateCheckSum(uint8_t *buffer){
  uint16_t sum = 0;
  for (int i=Stack_Version; i<Stack_CheckSum; i++) {
    sum += buffer[i];
  }
  return -sum;
}

void DFRobot_PlayerMini::sendStack(){
  if (_sending[Stack_ACK]) {
    while (_isSending) {
      available();
    }
  }
  else{
    delay(10);
  }
#if !defined(ARDUINO_ARCH_RP2040)
  digitalWrite(tpin, HIGH);
#endif
  if(_serialH){
      _serialH->write(_sending, DFPLAYER_SEND_LENGTH);
  }else if(_serialS){
      _serialS->write(_sending, DFPLAYER_SEND_LENGTH);
  }
  _timeOutTimer = millis();
  _isSending = _sending[Stack_ACK];
#if !defined(ARDUINO_ARCH_RP2040)
  digitalWrite(tpin, LOW);
#endif
}

void DFRobot_PlayerMini::sendStack(uint8_t command){
  sendStack(command, 0);
}

void DFRobot_PlayerMini::sendStack(uint8_t command, uint16_t argument){
  _sending[Stack_Command] = command;
  uint16ToArray(argument, _sending+Stack_Parameter);
  uint16ToArray(calculateCheckSum(_sending), _sending+Stack_CheckSum);
  sendStack();
}

void DFRobot_PlayerMini::sendStack(uint8_t command, uint8_t argumentHigh, uint8_t argumentLow){
  uint16_t buffer = argumentHigh;
  buffer <<= 8;
  sendStack(command, buffer | argumentLow);
}

void DFRobot_PlayerMini::enableACK(){
  _sending[Stack_ACK] = 0x01;
}

void DFRobot_PlayerMini::disableACK(){
  _sending[Stack_ACK] = 0x00;
}

bool DFRobot_PlayerMini::waitAvailable(){
  _isSending = true;
  while (!available());
  return _handleType != TimeOut;
}

#if defined(NRF5)
bool DFRobot_PlayerMini::begin(int rx, int tx, bool isACK){
  if (isACK) {
    enableACK();
  }
  else{
    disableACK();
  }
  rpin = rx;
  tpin = tx;
  rx = g_PinID[rx];
  tx = g_PinID[tx];
  MicroBitPin *rxpin = getMicroBitPin(rx);
  MicroBitPin *txpin = getMicroBitPin(tx);
  uBit.serial.redirect(txpin->name, rxpin->name);
  _serialH = &Serial;
  _serialS = NULL;
  _serialH->begin(9600);
  _timeOutDuration += 3000;
  reset();
  waitAvailable();
  _timeOutDuration -= 3000;
  delay(200);
  setTimeOut(500);
  return (readType() == DFPlayerCardOnline) || !isACK;
}
#elif defined(NRF52833)
bool DFRobot_PlayerMini::begin(int rx, int tx, bool isACK){
  if (isACK) {
    enableACK();
  }
  else{
    disableACK();
  }
  rpin = rx;
  tpin = tx;
  rx = g_PinID[rx];
  tx = g_PinID[tx];
  MicroBitPin *rxpin = getMicroBitPin(rx);
  MicroBitPin *txpin = getMicroBitPin(tx);
  uBit.serial.redirect(*txpin, *rxpin);
  _serialH = &Serial;
  _serialS = NULL;
  _serialH->begin(9600);
  _timeOutDuration += 3000;
  reset();
  waitAvailable();
  _timeOutDuration -= 3000;
  delay(200);
  setTimeOut(500);
  return (readType() == DFPlayerCardOnline) || !isACK;
}
#else
bool DFRobot_PlayerMini::begin(HardwareSerial *serial, int rx, int tx, bool isACK){
  if (isACK) {
    enableACK();
  }
  else{
    disableACK();
  }
  rpin = rx;
  tpin = tx;
  _serialH = serial;
  _serialS = NULL;
#if defined(ESP_PLATFORM)
  _serialH->begin(9600,rx,tx);
#else
  _serialH->begin(9600);
#endif
  _timeOutDuration += 3000;
  reset();
  waitAvailable();
  _timeOutDuration -= 3000;
  delay(200);
  setTimeOut(500);
  return (readType() == DFPlayerCardOnline) || !isACK;
}
#endif
bool DFRobot_PlayerMini::begin(SoftwareSerial *serial, bool isACK){
  if (isACK) {
    enableACK();
  }
  else{
    disableACK();
  }
  _serialH = NULL;
  _serialS = serial;
  _serialS->begin(9600);
  rpin = _serialS->getReceivePin();
  tpin = _serialS->getTransmitPin();
  Serial.println(rpin);
  Serial.println(tpin);
  _timeOutDuration += 3000;
  reset();
  waitAvailable();
  _timeOutDuration -= 3000;
  delay(200);
  setTimeOut(500);
  return (readType() == DFPlayerCardOnline) || !isACK;
}

uint8_t DFRobot_PlayerMini::readType(){
  _isAvailable = false;
  return _handleType;
}

uint16_t DFRobot_PlayerMini::read(){
  _isAvailable = false;
  return _handleParameter;
}

bool DFRobot_PlayerMini::handleMessage(uint8_t type, uint16_t parameter){
  _receivedIndex = 0;
  _handleType = type;
  _handleParameter = parameter;
  _isAvailable = true;
  return _isAvailable;
}

bool DFRobot_PlayerMini::handleError(uint8_t type, uint16_t parameter){
  handleMessage(type, parameter);
  _isSending = false;
  return true;
}

uint8_t DFRobot_PlayerMini::readCommand(){
  _isAvailable = false;
  return _handleCommand;
}

void DFRobot_PlayerMini::parseStack(){
  _handleCommand = *(_received + Stack_Command);
  _handleParameter =  arrayToUint16(_received + Stack_Parameter);

  switch (_handleCommand) {
    case 0x3D:
      handleMessage(DFPlayerPlayFinished, _handleParameter);
      break;
    case 0x3F:
      if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardOnline, _handleParameter);
      }
      break;
    case 0x3A:
      if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardInserted, _handleParameter);
      }
      break;
    case 0x3B:
      if (_handleParameter & 0x02) {
        handleMessage(DFPlayerCardRemoved, _handleParameter);
      }
      break;
    case 0x40:
      handleMessage(DFPlayerError, _handleParameter);
      break;
    case 0x41:
      _isSending = false;
      break;
    case 0x3C:
    case 0x3E:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4B:
    case 0x4C:
    case 0x4D:
    case 0x4E:
    case 0x4F:
      _isAvailable = true;
      break;
    default:
      handleError(WrongStack);
      break;
  }
}

uint16_t DFRobot_PlayerMini::arrayToUint16(uint8_t *array){
  uint16_t value = *array;
  value <<=8;
  value += *(array+1);
  return value;
}

bool DFRobot_PlayerMini::validateStack(){
  return calculateCheckSum(_received) == arrayToUint16(_received+Stack_CheckSum);
}

bool DFRobot_PlayerMini::available(){
  if(_serialH){
      while (_serialH->available()) {
        if (_receivedIndex == 0) {
          _received[Stack_Header] = _serialH->read();
          if (_received[Stack_Header] == 0x7E) {
            _isAvailable = false;
            _receivedIndex ++;
          }
        }
        else{
          _received[_receivedIndex] = _serialH->read();
          switch (_receivedIndex) {
            case Stack_Version:
              if (_received[_receivedIndex] != 0xFF) {
                return handleError(WrongStack);
              }
              break;
            case Stack_Length:
              if (_received[_receivedIndex] != 0x06) {
                return handleError(WrongStack);
              }
              break;
            case Stack_End:
              if (_received[_receivedIndex] != 0xEF) {
                return handleError(WrongStack);
              }
              else{
                if (validateStack()) {
                  _receivedIndex = 0;
                  parseStack();
                  if (_isAvailable && !_sending[Stack_ACK]) {
                    _isSending = false;
                  }
                  return _isAvailable;
                }
                else{
                  return handleError(WrongStack);
                }
              }
              break;
            default:
              break;
          }
          _receivedIndex++;
        }
      }
  }else if(_serialS){
      while (_serialS->available()) {
        if (_receivedIndex == 0) {
          _received[Stack_Header] = _serialS->read();
          if (_received[Stack_Header] == 0x7E) {
            _isAvailable = false;
            _receivedIndex ++;
          }
        }
        else{
          _received[_receivedIndex] = _serialS->read();
          switch (_receivedIndex) {
            case Stack_Version:
              if (_received[_receivedIndex] != 0xFF) {
                return handleError(WrongStack);
              }
              break;
            case Stack_Length:
              if (_received[_receivedIndex] != 0x06) {
                return handleError(WrongStack);
              }
              break;
            case Stack_End:
              if (_received[_receivedIndex] != 0xEF) {
                return handleError(WrongStack);
              }
              else{
                if (validateStack()) {
                  _receivedIndex = 0;
                  parseStack();
                  if (_isAvailable && !_sending[Stack_ACK]) {
                    _isSending = false;
                  }
                  return _isAvailable;
                }
                else{
                  return handleError(WrongStack);
                }
              }
              break;
            default:
              break;
          }
          _receivedIndex++;
        }
      }
  }
  if (_isSending && (millis()-_timeOutTimer>=_timeOutDuration)) {
    return handleError(TimeOut);
  }
  
  return _isAvailable;
}

void DFRobot_PlayerMini::next(){
  sendStack(0x01);
  delay(70);
}

void DFRobot_PlayerMini::previous(){
  sendStack(0x02);
  delay(70);
}

void DFRobot_PlayerMini::play(int fileNumber){
  sendStack(0x03, fileNumber);
  delay(70);
}

void DFRobot_PlayerMini::volumeUp(){
  sendStack(0x04);
  delay(70);
}

void DFRobot_PlayerMini::volumeDown(){
  sendStack(0x05);
  delay(70);
}

void DFRobot_PlayerMini::volume(uint8_t volume){
  if(volume < 0) volume = 0;
  if(volume > 100) volume = 100;
  sendStack(0x06, (uint8_t)((volume/100.0) * 30.0));
  delay(70);
}

void DFRobot_PlayerMini::EQ(uint8_t eq) {
  sendStack(0x07, eq);
  delay(70);
}

void DFRobot_PlayerMini::loop(int fileNumber) {
  sendStack(0x08, fileNumber);
  delay(70);
}

void DFRobot_PlayerMini::outputDevice(uint8_t device) {
  sendStack(0x09, device);
  delay(200);
}

void DFRobot_PlayerMini::sleep(){
  sendStack(0x0A);
  delay(70);
}

void DFRobot_PlayerMini::reset(){
  sendStack(0x0C);
  delay(70);
}

void DFRobot_PlayerMini::start(){
  sendStack(0x0D);
  delay(70);
}

void DFRobot_PlayerMini::pause(){
  sendStack(0x0E);
  delay(70);
}

void DFRobot_PlayerMini::playFolder(uint8_t folderNumber, uint8_t fileNumber){
  sendStack(0x0F, folderNumber, fileNumber);
  delay(70);
}

void DFRobot_PlayerMini::outputSetting(bool enable, uint8_t gain){
  sendStack(0x10, enable, gain);
  delay(70);
}

void DFRobot_PlayerMini::enableLoopAll(){
  sendStack(0x11, 0x01);
  delay(70);
}

void DFRobot_PlayerMini::disableLoopAll(){
  sendStack(0x11, 0x00);
  delay(70);
}

void DFRobot_PlayerMini::playMp3Folder(int fileNumber){
  sendStack(0x12, fileNumber);
  delay(70);
}

void DFRobot_PlayerMini::advertise(int fileNumber){
  sendStack(0x13, fileNumber);
  delay(70);
}

void DFRobot_PlayerMini::playLargeFolder(uint8_t folderNumber, uint16_t fileNumber){
  sendStack(0x14, (((uint16_t)folderNumber) << 12) | fileNumber);
  delay(70);
}

void DFRobot_PlayerMini::stopAdvertise(){
  sendStack(0x15);
  delay(70);
}

void DFRobot_PlayerMini::stop(){
  sendStack(0x16);
  delay(70);
}

void DFRobot_PlayerMini::loopFolder(int folderNumber){
  sendStack(0x17, folderNumber);
  delay(70);
}

void DFRobot_PlayerMini::randomAll(){
  sendStack(0x18);
  delay(70);
}

void DFRobot_PlayerMini::enableLoop(){
  sendStack(0x19, 0x00);
}

void DFRobot_PlayerMini::disableLoop(){
  sendStack(0x19, 0x01);
}

void DFRobot_PlayerMini::enableDAC(){
  sendStack(0x1A, 0x00);
}

void DFRobot_PlayerMini::disableDAC(){
  sendStack(0x1A, 0x01);
}

int DFRobot_PlayerMini::readState(){
  sendStack(0x42);
  if (waitAvailable()) {
    return read();
  }
  else{
    return -1;
  }
}

int DFRobot_PlayerMini::readVolume(){
  sendStack(0x43);
  if (waitAvailable()) {
    return read();
  }
  else{
    return -1;
  }
}

uint8_t DFRobot_PlayerMini::readEQ(){
  sendStack(0x44);
  while (!available());
  if (waitAvailable()) {
    return read();
  }
  else{
    return -1;
  }
}

int DFRobot_PlayerMini::readFileCounts(uint8_t device){
  switch (device) {
    case DFPLAYER_DEVICE_U_DISK:
      sendStack(0x47);
      break;
    case DFPLAYER_DEVICE_SD:
      sendStack(0x48);
      break;
    case DFPLAYER_DEVICE_FLASH:
      sendStack(0x49);
      break;
    default:
      break;
  }
  
  if (waitAvailable()) {
    return read();
  }
  else{
    return -1;
  }
}

int DFRobot_PlayerMini::readCurrentFileNumber(uint8_t device){
  switch (device) {
    case DFPLAYER_DEVICE_U_DISK:
      sendStack(0x4B);
      break;
    case DFPLAYER_DEVICE_SD:
      sendStack(0x4C);
      break;
    case DFPLAYER_DEVICE_FLASH:
      sendStack(0x4D);
      break;
    default:
      break;
  }
  if (waitAvailable()) {
    return read();
  }
  else{
    return -1;
  }
}

int DFRobot_PlayerMini::readFileCountsInFolder(int folderNumber){
  sendStack(0x4E, folderNumber);
  if (waitAvailable()) {
    return read();
  }
  else{
    return -1;
  }
}

int DFRobot_PlayerMini::readFileCounts(){
  readFileCounts(DFPLAYER_DEVICE_SD);
  return 0;
}

int DFRobot_PlayerMini::readCurrentFileNumber(){
  readCurrentFileNumber(DFPLAYER_DEVICE_SD);
  return 0;
}


