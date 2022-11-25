#ifndef SENSOR
#define SENSOR

#include <Arduino.h>
#include "variable.h"

#define echo            0
#define trig            1

#define uvPin1          A6
#define uvPin2          A5

//#define micPin          53 //
#define buttonPin       A7

#define POWER_LED_ON    digitalWrite(49, HIGH)
#define POWER_LED_OFF   digitalWrite(49, LOW)
//#define FIRE_LED_ON     digitalWrite(51, HIGH)
//#define FIRE_LED_OFF    digitalWrite(51, LOW)

#define KILL_MK         (digitalRead(45) == 1)
#define KILL_MTR        (digitalRead(46) == 1)

const uint8_t ultrasPin[2][12] = {
  {27, 25, 23, 19, 7, 9, 11, 13, 15, 17, 31, 29}, 
  {26, 24, 22, 18, 6, 8, 10, 12, 14, 16, 30, 28}
};

const uint8_t I2CPin[4] = {4, 5, 6, 3};
const uint8_t IRrangePin[5] = {A0, A1, A2, A3, A4};
const uint8_t IRflamePin[5] = {51, 50, 48, 47, A8};

class sensor {
  public :
    void begin() {
      for (count = 0; count < 12; count++) {
        pinMode(ultrasPin[trig][count], OUTPUT);
        pinMode(ultrasPin[echo][count], INPUT_PULLUP);
        pinMode(IRflamePin[count % 4], OUTPUT);
        pinMode(I2CPin[count % 4], OUTPUT);
      }
//      pinMode(micPin, INPUT);
      pinMode(buttonPin, INPUT);
      
      pinMode(51, OUTPUT);
      pinMode(49, OUTPUT);
      pinMode(45, INPUT_PULLUP);
      pinMode(46, INPUT_PULLUP);
    }

    int16_t ultras(uint8_t num) {
      return srf04(ultrasPin[trig][num], ultrasPin[echo][num]);
    }

    int16_t ir(uint8_t num) {
      int16_t val;
      for (count = 0; count < 3; count++)
        val = filter(12343.85 * pow(analogRead(IRrangePin[num]), -1.13));
      return val;
    }

//    bool sound(uint32_t time = 1000) {
//      if (digitalRead(micPin)) lastSound = millis();
//      if ((millis() - lastSound) < time)  return true;
//      return false;
//    }
      

    bool uv(uint32_t time = 1000) {
      if (digitalRead(uvPin1) || digitalRead(uvPin2)) lastFire = millis();
      if ((millis() - lastFire) < time) return true;
      return false;
    }

    int16_t flame(uint8_t x) {
      for (count = 0; count < 4; count++)
        digitalWrite(IRflamePin[count], ((x >> count) & 0x01 ? HIGH : LOW));
      delay(1);
      return analogRead(IRflamePin[4]);
    }

    vector radarApi(uint8_t replay = 5) {
      vector api = scanApi();
      vector avg = api;
      if (avg.sudut == 0 || avg.sudut == 180)
        return avg;

      while (replay--) {
        api = scanApi();
        avg.sudut = (avg.sudut + api.sudut) / 2;
        avg.resultan = (avg.resultan + api.resultan) / 2;
      }
      if (abs(avg.sudut - 90) > 80)
        avg.resultan = 0;
      return avg;
    }

    uint8_t clearTime = 5;

  private :
    int16_t srf04(uint8_t  trigPin, uint8_t echoPin) {
      while ((millis() - lastFrame) < clearTime);

      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(5);
      digitalWrite(trigPin, LOW);
      lastFrame = millis();

      return (pulseIn(echoPin, HIGH, 35000) / 29 / 2);
    }

    vector vectorGenerate(int16_t *data, uint8_t length) {
      vector value;
      uint8_t count;

      float vector1 = 0;
      uint8_t index1;
      for (count = 0; count < length; count++)
        if (vector1 < data[count]) {
          vector1 = data[count];
          index1 = count;
        }

      float vector2 = 0;
      uint8_t index2;
      for (count = 0; count < length; count++)
        if (vector2 < data[count] && count != index1) {
          vector2 = data[count];
          index2 = count;
        }

      value.resultan = sqrt(pow(vector1, 2) + pow(vector2, 2) + (2 * vector1 * vector2 * cos(radians(30))));
      value.sudut = degrees(asin(sin(radians(30)) * vector2 / value.resultan));

      vector1 = 1023 - vector1;
      vector2 = 1023 - vector2;
      value.resultan = sqrt(pow(vector1, 2) + pow(vector2, 2) + (2 * vector1 * vector2 * cos(radians(30))));

      value.sudut = index1 < index2 ? value.sudut : -value.sudut;
      value.sudut += (index1 * 30) + 30;

      return value;
    }

    vector scanApi() {
      uint8_t count;
      int16_t data[7];
      int16_t maxData = 0;

      for (count = 0; count < 7; count++)
        data[count] = flame(count);
      vector apiA = vectorGenerate(data, 7);

      for (count = 0; count < 7; count++)
        maxData = max(maxData, data[count]);

      for (count = 15; count > 8; count--)
        data[15 - count] = flame(count);
      vector apiB = vectorGenerate(data, 7);

      for (count = 0; count < 7; count++)
        maxData = max(maxData, data[count]);

      vector value;
      value.resultan = 2 * apiA.resultan * apiB.resultan * cos(radians(180 - (apiA.sudut + apiB.sudut)));
      value.resultan = sqrt(pow(apiA.resultan, 2) + pow(apiB.resultan, 2) - value.resultan);

      float scale = value.resultan / 64;
      value.resultan = (pow(apiA.resultan, 2) / 2) + (pow(apiB.resultan, 2) / 2) - (pow(value.resultan, 2) / 2);
      value.resultan = sqrt(value.resultan) / scale;

      float a = 180 - (apiA.sudut + apiB.sudut);
      scale = a - ((a * apiB.sudut) / (apiA.sudut + apiB.sudut));
      value.sudut = (a * apiA.sudut) / (apiA.sudut + apiB.sudut);
      value.sudut = (a + scale) / 2;
      value.sudut = 180 - (value.sudut + apiA.sudut);

      value.resultan = maxData;
      if (apiA.sudut > 150 || apiB.sudut > 150) {
        apiA.sudut = apiA.sudut > apiB.sudut ? 0 : 180;
        return apiA;
      }

      return value;
    }


    int16_t data1, data2;
    int16_t filter(int16_t data) {
      int16_t diff1 = abs(abs(data) - abs(data1));
      int16_t diff2 = abs(abs(data) - abs(data2));
      int16_t diff3 = abs(abs(data1) - abs(data2));

      if (diff1 == max(diff1, max(diff2, diff3)))
        data2 = (data2 + (diff2 < diff3 ? data : data1)) / 2;
      if (diff2 == max(diff1, max(diff2, diff3)))
        data2 = (data1 + (diff1 < diff3 ? data : data2)) / 2;
      if (diff3 == max(diff1, max(diff2, diff3)))
        data2 = (data + (diff1 < diff2 ? data1 : data2)) / 2;

      data1 = data;
      return data2;
    }


    uint8_t count;
    uint32_t lastFrame;
    uint32_t lastFire, lastSound;
};

#endif
