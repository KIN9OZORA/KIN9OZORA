#include "commander.h"

commander :: commander(int16_t *data, int16_t *info, uint8_t id) {
  ID = id;
  buffer = data;
  status = info;
}

void commander :: begin() {
  Wire.begin();
}

void commander :: begin(void (*receive)(void), void (*request)(void)) {
  requestAddr = request;
  receiveAddr = receive;

  if (ID) {
    Wire.begin(ID);
    Wire.onRequest(request);
    Wire.onReceive(receive);
  }
  else Wire.begin();
}

void commander :: stop() {
  Wire.end();
}

void commander :: resume() {
  if (ID) {
    Wire.begin(ID);
    Wire.onRequest(requestAddr);
    Wire.onReceive(receiveAddr);
  }
  else Wire.begin();
}

bool commander :: read(uint8_t id, uint8_t startAddr, uint8_t endAddr, bool endRequest) {
  startTime = millis();
  do {
    Wire.beginTransmission(id);
    if ((millis() - startTime) > 500) return false;
  } while (Wire.endTransmission() != 0);

  Wire.beginTransmission(id);
  Wire.write(startAddr);
  Wire.write(endAddr);
  Wire.endTransmission();

  delayMicroseconds(10);
  Wire.requestFrom(id, ((endAddr - startAddr) * 2) + 6);
  delayMicroseconds(10);

  status[0] = Wire.read() | (Wire.read() << 8);
  status[1] = Wire.read() | (Wire.read() << 8);
  status[2] = Wire.read() | (Wire.read() << 8);
  for (count = startAddr; Wire.available() &&  count <= endAddr; count++)
    buffer[count] = Wire.read() | (Wire.read() << 8);

  if (endRequest)
    do {
      Wire.beginTransmission(id);
      if ((millis() - startTime) > 500);
    } while (Wire.endTransmission() == 0);
  return true;
}

bool commander :: write(uint8_t id, uint8_t startAddr, uint8_t endAddr, bool endRequest) {
  startTime = millis();
  do {
    Wire.beginTransmission(id);
    if ((millis() - startTime) > 500) return false;
  } while (Wire.endTransmission() != 0);

  Wire.beginTransmission(id);
  Wire.write(startAddr);
  Wire.write(0);
  for (count = startAddr; count <= endAddr; count++) {
    Wire.write(lowByte(buffer[count]));
    Wire.write(highByte(buffer[count]));
  }
  Wire.endTransmission();

  delayMicroseconds(10);
  Wire.requestFrom(id, 6);
  delayMicroseconds(10);

  status[0] = Wire.read() | (Wire.read() << 8);
  status[1] = Wire.read() | (Wire.read() << 8);
  status[2] = Wire.read() | (Wire.read() << 8);

  if (endRequest)
    do {
      Wire.beginTransmission(id);
      if ((millis() - startTime) > 500);
    } while (Wire.endTransmission() == 0);
  return true;
}

void commander :: requestEvent() {
  Wire.write(lowByte(status[0]));
  Wire.write(highByte(status[0]));
  Wire.write(lowByte(status[1]));
  Wire.write(highByte(status[1]));
  Wire.write(lowByte(status[2]));
  Wire.write(highByte(status[2]));
  for (count = dataStartAddr; count <= dataEndAddr; count++) {
    Wire.write(lowByte(buffer[count]));
    Wire.write(highByte(buffer[count]));
  }
}

void commander :: receiveEvent() {
  dataStartAddr = Wire.read();
  dataEndAddr = Wire.read();
  for (count = 0; Wire.available(); count++)
    buffer[count + dataStartAddr] = Wire.read() | (Wire.read() << 8);
}
