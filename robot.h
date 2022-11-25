#ifndef ROBOT
#define ROBOT

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "commander.h"
#include "sensor.h"
#include "variable.h"

LiquidCrystal_I2C             lcd(0x3F, 16, 2);
//LiquidCrystal_I2C             lcd(0x27, 16, 2);
commander                     leg(legData, legStatus);
sensor                        sensor;

#define legID                 2

class robot {
  public :
    void begin() {
      leg.begin();
      sensor.begin();
      lcd.init();
      
      
      lcd.backlight();
      lcd.setCursor(0, 0); lcd.print("-HEXA CAKRAWALA-");
      lcd.setCursor(0, 1); lcd.print("    - TEUM -    ");

      for (count = 0; count < 4; count++)
        digitalWrite(I2CPin[count], LOW);
      delay(10);
      for (count = 0; count < 4; count++)
        digitalWrite(I2CPin[count], HIGH);

      legShift(0);
      legData[0]  = 0;
      legData[1]  = 1000;
      legData[11] = 1024;
      legData[12] = 1024;

      sensorInverse = false;
      dxlOn; sensorOn; pumpOff;

      lastTime = millis();

      while (!leg.write(legID, 0, 12) && (millis() - lastTime) < (timeOut * 4));

      if (((float)(legStatus[0]) / 10) <= 11) {
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("!ERROR  VOLTAGE!");
        lcd.setCursor(0, 1); lcd.print(" Voltage:" + String((float)(legStatus[0]) / 10) + "V");

        for (count = 0; count < 4; count++)
          digitalWrite(I2CPin[count], LOW);
        delay(10);
        for (count = 0; count < 4; count++)
          digitalWrite(I2CPin[count], HIGH);
        delay(10);
        for (count = 0; count < 4; count++)
          digitalWrite(I2CPin[count], LOW);
        while (1);
      }

      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("-Ready!-");
      lcd.setCursor(0, 1);
      lcd.print("-Voltage:" + String((float)(legStatus[0]) / 10) + "V-");
      delay(1000);
      lcd.clear();
    }
    ///new
    uint16_t SetTangga; // 1

    void SwitchModeTangga() {
      if (SetTangga == 0) {
        SetTangga = 1;
      } else {
        SetTangga = 0;
      }
      ModeTangga = SetTangga;
    }
    void ModeTanggaOn() {
      SetTangga = 1;


      ModeTangga = SetTangga;
    }
    void ModeTanggaOff() {
      SetTangga = 0;

      ModeTangga = SetTangga;
    }

    void Diam(int16_t transitionTime) {
      for (int z = 1 ; z > 0 ; z++)
        move(0, 0, 0, 30);
    }
    void FireCounting(bool FireMode) {

      if (FireMode == true && FireCountTrig == false) {
        FireCountTrig = true;
      } else if (FireMode == false && FireCountTrig == true) {
        FireCountTrig = false;
        FireCount += 1;
      }
    }


    void shutdown() {
      lcd.clear();  lcd.backlight();  lcd.print("Shutting Down!");
      move(0, 0, 0, 30);
      position(0, 0, 0, 100);
      rotation(0, 0, 0, 100);
//      POWER_LED_OFF;  FIRE_LED_OFF;
      dxlOff; sensorOff;
      leg.write(legID, 0, 1);
      lcd.clear(); lcd.noBacklight();
    }

    uint8_t key() {
      uint16_t adc = analogRead(buttonPin);
      for (count = 0; count < 30; count++)
        adc = max(adc, analogRead(buttonPin));

      if (adc > 680 && adc < 880)  return 1; //790
      else if (adc > 480 && adc < 680)  return 2; //580
      else if (adc > 280 && adc < 480)  return 3; //380
      else if (adc > 100 && adc < 280)  return 4; //190
      else return 0;
    }

    void readJarak() {
      if (sensorInverse) {
        for (count = 0; count < 6; count++) {
          sensorData[count] = sensor.ultras(count + 6);
          sensorData[count + 6] = sensor.ultras(count);
        }
        for (count = 0; count < 4; count++)
          sensorData[count + 12] = sensor.ir((count + 2) % 4);
      }

      else {
        for (count = 0; count < 6; count++) {
          sensorData[count] = sensor.ultras(count);
          sensorData[count + 6] = sensor.ultras(count + 6);
        }
        for (count = 0; count < 4; count++)
          sensorData[count + 12] = sensor.ir(count);
      }
    }

    void showSensor() {
      for (count = 0; count < 16; count++)
        Serial.print(String(sensorData[count]) + "\t");
      Serial.println();
    }

    void run(int16_t transitionTime, bool waitResponse = false) {
      transTime = transitionTime;  dxlOn;
      lastTime = millis();
      while ((millis() - lastTime) < (timeOut + (5000 * waitResponse)))
        if (leg.write(legID, 0, 10)) break;
    }

    void move(int16_t x, int16_t y, int16_t r, int16_t transitionTime) {
      Xspeed = x / 2; Yspeed = y / 2; Rspeed = -r / 2.4;
      run(transitionTime, true);
    }

    // === === === ROBOT PUTAR === === === //
    void putar(int16_t r, int16_t transitionTime) {
      lastErrorR = 0; lastErrorY = 0;
      move(-2, 0, 0, transitionTime);
      while (abs(r) > 20) {
        move(0, 0, r > 0 ? 20 : -20, transitionTime);
        r = r > 0 ? r - 20 : r + 20;
      }
      move(0, 0, 0, transitionTime);
      lastErrorR = 0; lastErrorY = 0;
    }

    void rotation(int16_t x, int16_t y, int16_t z, int16_t transitionTime) {
      bodyRotX = x; bodyRotY = y; bodyRotZ = z;
      run(transitionTime, true);
      lastErrorR = 0; lastErrorY = 0;
    }


    void position(int16_t x, int16_t y, int16_t z, int16_t transitionTime) {
      bodyPosX = x; bodyPosY = y; bodyPosZ = z;
      run(transitionTime, true);
      lastErrorR = 0; lastErrorY = 0;
    }

    void legShift(uint8_t a) {
      a %= 6;
      legData[0] &= 0xFF00;
      legData[0] |= a;
    }

    float PID(int16_t data1, int16_t data2, float *lastError) {
      error = data1 - data2;
      if (error == 0)  i = 0;
      else i += (*lastError + error) / 100;
      value = ((error * Kp) + (Ki * i) + (Kd * (error - *lastError)));
      *lastError = error;
      return value;
    }

    uint8_t pidKiri(int16_t Xmove = 0, int16_t speed = 60, uint8_t jarakDinding = 15) {
      //      if (FRONT < jarakDinding && Xmove > 0) {
      //        move(0, 0, 0, speed);
      //        return 0;
      //      }

      Kp = 80; Ki = 0; Kd = 20;
      if (LEFT_MIDLE_FRONT < 60 && LEFT_MIDLE_REAR < 60)
        r = PID(LEFT_MIDLE_REAR, LEFT_MIDLE_FRONT + 1, &lastErrorR) / 50;
      else  r = 0;
      r = constrain(r, -20, 20);

      Kp = 160; Ki = 0; Kd = 50;
      uint16_t m = min(min(LEFT_MIDLE_FRONT, LEFT_MIDLE_REAR), LEFT_MIDLE);
      if (abs(r) < 5 && m < 60)
        y = PID(m, jarakDinding, &lastErrorY) / 50;
      else
        y = 0;
      y = constrain(y, -40, 40);

      x = constrain(Xmove, -40, 40);
      if (x != 0 && abs(x) > abs(r))  x = r > 0 ? x - r * 2 : x + r * 2;
      move(x, y, r, speed);

      return (legStatus[0] & 0x03);
    }

    uint8_t pidKanan(int16_t Xmove = 0, int16_t speed = 60, uint8_t jarakDinding = 15) {
      //      if (FRONT < jarakDinding && Xmove > 0) {
      //        move(0, 0, 0, speed);
      //        return 0;
      //      }

      Kp = 80; Ki = 0; Kd = 20;
      if (RIGHT_MIDLE_FRONT < 60 && RIGHT_MIDLE_REAR < 60)
        r = PID(RIGHT_MIDLE_FRONT, RIGHT_MIDLE_REAR + 1, &lastErrorR) / 50;
      else r = 0;
      r = constrain(r, -20, 20);

      Kp = 160; Ki = 0; Kd = 50;
      uint16_t m = min(min(RIGHT_MIDLE_FRONT, RIGHT_MIDLE_REAR), RIGHT_MIDLE);
      if (abs(r) < 5 && m < 60)
        y = PID(jarakDinding, m, &lastErrorY) / 50;
      else
        y = 0;
      y = constrain(y, -40, 40);

      x = constrain(Xmove, -40, 40);
      if (x != 0 && abs(x) > abs(r))  x = r > 0 ? x - r * 2 : x + r * 2;
      move(x, y, r, speed);

      return (legStatus[0] & 0x03);
    }

    void scanKanan(int16_t Xmove, int16_t speed, uint8_t jarakDinding = 15) {
      if (FRONT < 12) {
        move(-40, 0, 0, speed);
        putarKiri(45, speed);
      }

      else if (FRONT_LEFT < 12)
        move(0, 24, 8, speed);

      else if (FRONT_RIGHT < 12)
        move(0, -24, -8, speed);

      else if (RIGHT_MIDLE_FRONT > 30)
        majuBelokKanan(21, speed);
      //        move(24, 0, 8, speed);

      else if (FRONT > 16)
        pidKanan(Xmove, speed, jarakDinding - 2);

      else if (RIGHT_MIDLE > 30)
        if (sensorInverse) {
          putarKiri(90, speed);
          inverse();
        }
        else
          putarKanan(90, speed);

      else if (LEFT_MIDLE > 30)
        if (sensorInverse) {
          putarKanan(90, speed);
          inverse();
        }
        else
          putarKiri(90, speed);
      //ganti cuy
      //      else
      //        inverse();

      readJarak();
    }

    void scanKiri(int16_t Xmove, int16_t speed, uint8_t jarakDinding = 15) {
      if (FRONT < 12) {
        move(-40, 0, 0, speed);
        putarKanan(45, speed);
      }

      else if (FRONT_RIGHT < 12)
        move(0, -24, -8, speed);

      else if (FRONT_LEFT < 12)
        move(0, 24, 8, speed);

      else if (LEFT_MIDLE_FRONT > 30)
        majuBelokKiri(21, speed);
      //        move(24, 0, -8, speed);

      else if (FRONT > 16)
        pidKiri(Xmove, speed, jarakDinding - 2);

      else if (LEFT_MIDLE > 30)
        if (sensorInverse) {
          putarKanan(90, speed);
          inverse();
        }
        else
          putarKiri(90, speed);

      else if (RIGHT_MIDLE > 30)
        if (sensorInverse) {
          putarKiri(90, speed);
          inverse();
        }
        else
          putarKanan(90, speed);

      //      else
      // inverse();

      readJarak();
    }

    uint8_t scanKananRuang(int16_t Xmove, int16_t speed, bool canExit = true, int16_t jarakDinding = 11) {
      uint8_t action = 0;
      if (FRONT > 15)
        if (RIGHT_MIDLE_FRONT < 25) {
          pidKanan(Xmove, speed, jarakDinding);
          move(Xmove, 0, 0, speed);
        }
        else
          move(Xmove, 0, 0, speed);

      else if (RIGHT_MIDLE > 20 && canExit) {
        if (sensorInverse) {
          putarKiri(90, speed);
          inverse();
        }
        else
          putarKanan(90, speed);
        readJarak();
        action |= 4;
      }

      else if (LEFT_MIDLE > 20) {
        if (sensorInverse) {
          putarKanan(90, speed);
          inverse();
        }
        else
          putarKiri(90, speed);
        action |= 2;
      }

      else {
        inverse();
        action |= 1;
      }

      if (lantai() == 2 && !canExit) {
        inverse();
        action |= 1;
        while (lantai() == 2) {
          readJarak();
          pidKiri(Xmove, speed);
        }
      }

      readJarak();
      return action;
    }

    uint8_t scanKiriRuang(int16_t Xmove, int16_t speed, bool canExit = true, int16_t jarakDinding = 11) {
      uint8_t action = 0;
      if (FRONT > 15)
        if (LEFT_MIDLE_FRONT < 25) {
          pidKiri(Xmove, speed, jarakDinding);
          move(Xmove, 0, 0, speed);
        }
        else
          move(Xmove, 0, 0, speed);

      else if (LEFT_MIDLE > 20 && canExit) {
        if (sensorInverse) {
          putarKanan(90, speed);
          inverse();
        }
        else
          putarKiri(90, speed);
        readJarak();
        action |= 2;
      }

      else if (RIGHT_MIDLE > 20) {
        if (sensorInverse) {
          putarKiri(90, speed);
          inverse();
        }
        else
          putarKanan(90, speed);
        action |= 4;
      }

      else {
        inverse();
        action |= 1;
      }

      if (lantai() == 2 && !canExit) {
        inverse();
        action |= 1;
        while (lantai() == 2) {
          readJarak();
          pidKiri(Xmove, speed);
        }
      }

      readJarak();
      return action;
    }

    vector trackingApi(int16_t Xmove, int16_t speed, bool leftWall = true, uint8_t jarakDinding = 10) {
      readJarak();
      y = 0;
      if (min(FRONT, IR_FRONT) < 5) {
        readJarak();
        //        if (FRONT_LEFT > FRONT_RIGHT)
        //          putarKiri(30, speed);
        //        else if (FRONT_LEFT < FRONT_RIGHT)
        //          putarKanan(30, speed);
        move(-40, 0, 0, speed);
      }
      else if (FRONT < 15) {
        readJarak();
        if (leftWall) {
          if (LEFT_MIDLE > 20) {
            y = -Xmove;
            Xmove = 0;
          }
          else if (RIGHT_MIDLE > 20) {
            y = Xmove;
            Xmove = 0;
          }
        }
        else {
          readJarak();
          if (LEFT_MIDLE > 20) {
            y = Xmove;
            Xmove = 0;
          }
          else if (RIGHT_MIDLE > 20) {
            y = -Xmove;
            Xmove = 0;
          }
        }
      }
      else
        move(Xmove, 0, 0, speed);

      readJarak();
      if (min(LEFT_MIDLE, min(LEFT_MIDLE_FRONT, FRONT_LEFT)) < jarakDinding)
        y = Xmove;
      else if (min(RIGHT_MIDLE, min(RIGHT_MIDLE_FRONT, FRONT_RIGHT)) < jarakDinding)
        y = -Xmove;

      vector temp = sensor.radarApi(apiRepeat);
      Kp = 20; Ki = 0; Kd = 10;
      r = PID(temp.sudut, 90, &lastErrorR) / 50;
      r = constrain(r, -20, 20);

      if (!FIRE || temp.resultan < 600)
        r = 0;

      x = constrain(Xmove, -40, 40);
      if (x != 0 && abs(x) > abs(r))
        x = r > 0 ? x - r * 2 : x + r * 2;
      move(x, y, r, speed);
      return temp;
    }


    bool maju(int16_t Xmove, int16_t speed) {
      readJarak();
      lastErrorR = 0; lastErrorY = 0;
      while (abs(Xmove) > 40) {
        if (FRONT < 20 || lantai() == 2) return false;
        else if (DINDING_KIRI) pidKiri(40, speed);
        else if (DINDING_KIRI) pidKiri(40, speed);
        else {
          move(Xmove > 0 ? 40 : -40, 0, 0, speed);
          readJarak();
        }
        Xmove = Xmove > 0 ? Xmove - 40 : Xmove + 40;
      }
      if (abs(Xmove) > 0) {
        if (FRONT < 20 || lantai() == 2) return false;
        else if (DINDING_KIRI) pidKiri(Xmove, speed);
        else if (DINDING_KIRI) pidKiri(Xmove, speed);
        else {
          move(Xmove, 0, 0, speed);
          readJarak();
        }
      }
      lastErrorR = 0; lastErrorY = 0;
      return true;
    }

    void majuBelok(int16_t sudut, int16_t speed = 40) {
      int16_t temp[2] = {0, 0};
      while (abs(sudut) >= 7) {
        move(24, 0, sudut > 0 ? 8 : -8, speed);
        sudut = sudut > 0 ? sudut - 7 : sudut + 7;
        temp[0] = max(temp[0], legStatus[1]);
        temp[1] = max(temp[1], legStatus[2]);
      }
      if (sudut != 0)
        move(3.5 * abs(sudut), 0, 1.2 * sudut, speed);
      temp[0] = max(temp[0], legStatus[1]);
      temp[1] = max(temp[1], legStatus[2]);

      legStatus[1] = temp[0];
      legStatus[2] = temp[1];
      lastErrorR = 0; lastErrorY = 0;
    }

    // === === === NGUWALIK ENDASE === === === //
    void inverse() {
      move(0, 0, 0, 15);
      shift += 3;
      shift %= 6;
      legShift(shift);
      sensorInverse = shift == 0 ? false : true;
      lastErrorR = 0; lastErrorY = 0;
      move(0, 0, 0, 10);
    }

    // === === === LANTAI === === === //
    uint8_t lantai() {
      uint32_t value = pow(sensorLantai, 2);
      if (value >= 0 && value < 8000)//15000
        return 0;
      else if (8000 <= value  && value < 33000)//15000
        return 1;
      else
        return 2;

      //      if (value >= 0 && value < blackReff)
      //        return 0;
      //      else if ((value >= (blackReff)) && (value < (greyReff)))
      //        return 1;
      //      else
      //        return 2;


    }

    bool sensorInverse = false;
    uint8_t shift = 0;
    int8_t Kp, Ki, Kd;

  private :
    uint32_t lastTime;
    uint8_t count;

    float error;
    float lastErrorY;
    float lastErrorR;
    int16_t x, y, z, r;
    float i, value;
};

#endif
