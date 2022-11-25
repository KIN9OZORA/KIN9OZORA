#include <LiquidCrystal_I2C.h>

#include <LiquidCrystal_I2C.h>

#ifndef NAVIGASI
#define NAVIGASI

#include <Arduino.h>
#include "robot.h"
#include "variable.h"



robot robot;

class navigasi {
  public :
    // === === === MAIN NAVIGASI === === === ///
    void room(uint8_t start, uint8_t goal, int16_t Xmove, int16_t speed, bool exit = true, uint8_t jump = 0) {
      nowRoom = start; nextRoom = goal;

      if (nowRoom == 1 && door1 == doorA)  robot.move(40, 40, 0, 20);
      else                                robot.move(40, -40, 0, 20);

      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);

      lcd.print("Dari " + String(start));
      if (start == 1)
        if (door1 == doorA)       lcd.print("A");
        else if (door1 == doorB)  lcd.print("B");
        else if (door1 == doorC)  lcd.print("C");
      if (start == 4)
        if (door4 == doorA)       lcd.print("A");
        else if (door4 == doorB)  lcd.print("B");

      lcd.print(" ke " + String(goal));
      if (goal == 1)
        if (door1 == doorA)       lcd.print("A");
        else if (door1 == doorB)  lcd.print("B");
        else if (door1 == doorC)  lcd.print("C");
      if (goal == 4)
        if (door4 == doorA)       lcd.print("A");
        else if (door4 == doorB)  lcd.print("B");

      uint32_t timer = millis();

      switch (start) {
        case 1 :
          switch (goal) {
            case 2 :  room1_2(Xmove, speed);  break;
            case 3 :  room1_3(Xmove, speed);  break;
            case 4 :  room1_4(Xmove, speed);  break;
          }
          break;
        case 2 :
          switch (goal) {
            case 1 :  room2_1(Xmove, speed);  break;
            case 3 :  room2_3(Xmove, speed);  break;
            case 4 :  room2_4(Xmove, speed);  break;
          }
          break;
        case 3 :
          switch (goal) {
              //            case 1 :  room3_1(Xmove, speed);  break;
              //            case 2 :  room3_2(Xmove, speed);  break;
              //            case 4 :  room3_4(Xmove, speed);  break;
          }
          break;
        case 4 :
          switch (goal) {
              //            case 1 :  room4_1(Xmove, speed, jump);  break;
              //            case 2 :  room4_2(Xmove, speed, jump);  break;
              //            case 3 :  room4_3(Xmove, speed, jump);  break;
          }
          break;
      }

      timer = millis() - timer;

      lcd.setCursor(0, 1);  lcd.print("Waktu:" + String(float(timer) / 1000));
      masukRuang(roomMove, roomSpeed, exit);
      if (exit)  keluarRuang(Xmove, speed);
    }

    // === === ===KELUAR RUANG START === === === //
    ruang keluarRuangStart(int16_t Xmove, int16_t speed) {
      playing = false;
      FIRE = false;
      sensor.clearTime = 10;

      robot.readJarak();
      int16_t maxData;
      while (1) {
        for (count = 0; count < 12; count++)
          maxData = max(maxData, sensorData[count]);
        if (maxData > 20)
          break;
        robot.move(0, 0, 10, speed);
        robot.readJarak();
      }

      int16_t diff;
      if (!DINDING_KANAN && !DINDING_KIRI) {
        if (max(FRONT, REAR) < 10)
          for (count = 0; count < (200 / Xmove) && RIGHT_MIDLE > 15; count++) {
            robot.move(0, Xmove, 0, speed);
            robot.readJarak();
          }
        robot.putarKiri(90, speed);
        robot.readJarak();
      }
      else if (DINDING_KANAN && max(RIGHT_MIDLE_FRONT, RIGHT_MIDLE_REAR) < RIGHT_MIDLE) {
        if (max(FRONT, REAR) < 10)
          for (count = 0; count < (200 / Xmove) && RIGHT_MIDLE > 15; count++) {
            robot.move(0, Xmove, 0, speed);
            robot.readJarak();
          }
        robot.putarKanan(30, speed);
        robot.readJarak();
      }
      else if (DINDING_KIRI && max(LEFT_MIDLE_FRONT, LEFT_MIDLE_REAR) < LEFT_MIDLE) {
        if (max(FRONT, REAR) < 10)
          for (count = 0; count < (200 / Xmove) && LEFT_MIDLE > 15; count++) {
            robot.move(0, -Xmove, 0, speed);
            robot.readJarak();
          }
        robot.putarKiri(30, speed);
        robot.readJarak();
      }
      else if (DINDING_KIRI && max(FRONT, REAR) < 10) {
        for (count = 0; count < (200 / Xmove) && RIGHT_MIDLE > 15; count++) {
          robot.move(0, Xmove, 0, speed);
          robot.readJarak();
        }
      }
      else if (DINDING_KANAN && max(FRONT, REAR) < 10) {
        for (count = 0; count < (200 / Xmove) && LEFT_MIDLE > 15; count++) {
          robot.move(0, -Xmove, 0, speed);
          robot.readJarak();
        }
      }

      if (DINDING_KANAN || DINDING_KIRI) {
        for (count = 0; count < 10; count++) {
          if (DINDING_KIRI) {
            robot.pidKiri(0, speed, 10);
            if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 20)
              robot.move(0, -15, 0, speed);

            robot.readJarak();
            diff = LEFT_MIDLE_FRONT - LEFT_MIDLE_REAR;
          }
          else if (DINDING_KANAN) {
            robot.pidKanan(0, speed, 10);
            if (min(RIGHT_MIDLE_FRONT, min(RIGHT_MIDLE, RIGHT_MIDLE_REAR)) > 20)
              robot.move(0, 15, 0, speed);

            robot.readJarak();
            diff = RIGHT_MIDLE_FRONT - RIGHT_MIDLE_REAR;
          }
          if (abs(diff) < 2)
            break;
        }
      }

      robot.move(0, 0, 0, speed);
      robot.readJarak();
      bool dindingKiri;
      if (max(LEFT_MIDLE, max(LEFT_MIDLE_REAR, LEFT_MIDLE_FRONT)) < 20 && max(LEFT_MIDLE, max(LEFT_MIDLE_REAR, LEFT_MIDLE_FRONT)) < 20) {
        if (FRONT < 20 && min(FRONT_LEFT, FRONT_RIGHT) < 30)
          dindingKiri = FRONT_LEFT < FRONT_RIGHT ? true : false;
        else if (REAR < 20 && min(REAR_LEFT, REAR_RIGHT) < 30)
          dindingKiri = REAR_LEFT < REAR_RIGHT ? true : false;
      }
      else if (LEFT_MIDLE < 20 && LEFT_MIDLE_REAR < 20 && LEFT_MIDLE_FRONT < 20 && ((FRONT < 20 && FRONT_LEFT < 30) || (REAR < 20 && REAR_LEFT < 30)))
        dindingKiri = true;
      else if (RIGHT_MIDLE < 20 && RIGHT_MIDLE_REAR < 20 && RIGHT_MIDLE_FRONT < 20 && ((FRONT < 20 && FRONT_RIGHT < 30) || (REAR < 20 && REAR_RIGHT < 30)))
        dindingKiri = false;
      else if (LEFT_MIDLE < 20 && LEFT_MIDLE_REAR < 20 && LEFT_MIDLE_FRONT < 20)
        dindingKiri = true;
      else if (RIGHT_MIDLE < 20 && RIGHT_MIDLE_REAR < 20 && RIGHT_MIDLE_FRONT < 20)
        dindingKiri = false;

      robot.move(0, 0, 0, speed);
      if (max(LEFT_MIDLE, max(LEFT_MIDLE_FRONT, LEFT_MIDLE_REAR)) < 20) {
        if (max(min(FRONT, FRONT_LEFT), min(REAR, REAR_LEFT)) < 10)
          while (LEFT_MIDLE < 25 && RIGHT_MIDLE > 12) {
            robot.move(0, Xmove, 0, speed);
            robot.readJarak();
          }
      }
      else if (max(RIGHT_MIDLE, max(RIGHT_MIDLE_FRONT, RIGHT_MIDLE_REAR)) < 20) {
        if (max(min(FRONT, FRONT_RIGHT), min(REAR, REAR_RIGHT)) < 10)
          while (RIGHT_MIDLE < 25 && LEFT_MIDLE > 12) {
            robot.move(0, -Xmove, 0, speed);
            robot.readJarak();
          }
      }

      if (dindingKiri) {
        if (min(REAR, REAR_LEFT) < 30 && min(RIGHT_MIDLE, min(RIGHT_MIDLE_FRONT, RIGHT_MIDLE_REAR)) > 20) {
          robot.putarKanan(90, speed);
          dindingKiri = false;
          robot.readJarak();
        }
        else if (REAR < 30 && FRONT < 30) {
          robot.putarKanan(90, speed);
          robot.readJarak();
          if (REAR_LEFT < 30 && LEFT_MIDLE_FRONT < 30)
            dindingKiri = true;
          else if (REAR_RIGHT < 30 && RIGHT_MIDLE_FRONT < 30)
            dindingKiri = false;
        }
        else if (min(FRONT, FRONT_LEFT) < 30 && min(RIGHT_MIDLE, min(RIGHT_MIDLE_FRONT, RIGHT_MIDLE_REAR)) > 20) {
          robot.putarKanan(90, speed);
          dindingKiri = true;
          robot.readJarak();
        }
      }
      else {
        if (min(REAR, REAR_RIGHT) < 30 && min(LEFT_MIDLE, min(LEFT_MIDLE_FRONT, LEFT_MIDLE_REAR)) > 20) {
          robot.putarKiri(90, speed);
          dindingKiri = true;
        }
        else if (REAR < 30 && FRONT < 30) {
          robot.putarKiri(90, speed);
          robot.readJarak();
          if (REAR_LEFT < 30 && LEFT_MIDLE_FRONT < 30)
            dindingKiri = true;
          else if (REAR_RIGHT < 30 && RIGHT_MIDLE_FRONT < 30)
            dindingKiri = false;
        }
        else if (min(FRONT, FRONT_RIGHT) < 30 && min(LEFT_MIDLE, min(LEFT_MIDLE_FRONT, LEFT_MIDLE_REAR)) > 20) {
          robot.putarKiri(90, speed);
          dindingKiri = false;
          robot.readJarak();
        }
      }

      robot.readJarak();
      if (FRONT < 25) {
        robot.inverse();
        dindingKiri = !dindingKiri;
        robot.readJarak();
      }

      while (REAR < 20 && FRONT > 15) {
        robot.move(Xmove, 0, 0, speed);
        robot.readJarak();
      }
      sensor.clearTime = 10;

      diff = ((float)((LEFT_MIDLE_FRONT + LEFT_MIDLE_REAR) - (RIGHT_MIDLE_FRONT + RIGHT_MIDLE_REAR))) \
             * 100 / ((float)(LEFT_MIDLE_FRONT + LEFT_MIDLE_REAR) + (RIGHT_MIDLE_FRONT + RIGHT_MIDLE_REAR));

      if (dindingKiri && diff > 0)
        dindingKiri = false;
      else if (!dindingKiri && diff < 0)
        dindingKiri = true;

      uint8_t nSample = 0;
      bool takeDataAgain = true;
      uint8_t action, nAction = 0;

      for (count = 0; count < 30 * 4; count++)
        logger[count / 30][count % 30] = 0;

      if (robot.lantai() == grey) {
        logger[0][nSample++] = robot.lantai();
        takeDataAgain = false;
      }
      else {
        while (FRONT > 13 && robot.lantai() != 2) {
          if (dindingKiri && LEFT_MIDLE_FRONT < 30)
            robot.pidKiri(Xmove, speed, 12);
          else if (RIGHT_MIDLE_FRONT < 30)
            robot.pidKanan(Xmove, speed, 12);
          robot.move(Xmove, 0, 0, speed);
          robot.readJarak();

          if (nSample < 30) {
            logger[0][nSample] = FRONT + 12;
            logger[1][nSample] = LEFT_MIDLE + 8;
            logger[2][nSample] = REAR + 12;
            logger[3][nSample] = RIGHT_MIDLE + 8;
            nSample++;
          }
        }

        if (takeDataAgain && robot.lantai() != 2) {
          if (dindingKiri) {
            if (LEFT_MIDLE > RIGHT_MIDLE && LEFT_MIDLE > 20) {
              if (robot.sensorInverse) {
                robot.putarKanan(90, speed);
                robot.inverse();
              }
              else
                robot.putarKiri(90, speed);
              dindingKiri = false;
              loggerShift(1);
            }
            else if (LEFT_MIDLE < RIGHT_MIDLE && RIGHT_MIDLE > 20) {
              if (robot.sensorInverse) {
                robot.putarKiri(90, speed);
                robot.inverse();
              }
              else
                robot.putarKanan(90, speed);
              dindingKiri = true;
              loggerShift(3);
            }
            else {
              robot.inverse();
              dindingKiri = false;
              loggerShift(2);
            }
          }
          else {
            if (LEFT_MIDLE < RIGHT_MIDLE && RIGHT_MIDLE > 20) {
              if (robot.sensorInverse) {
                robot.putarKiri(90, speed);
                robot.inverse();
              }
              else
                robot.putarKanan(90, speed);
              dindingKiri = true;
              loggerShift(3);
            }
            else if (LEFT_MIDLE > RIGHT_MIDLE && LEFT_MIDLE > 20) {
              if (robot.sensorInverse) {
                robot.putarKanan(90, speed);
                robot.inverse();
              }
              else
                robot.putarKiri(90, speed);
              dindingKiri = false;
              loggerShift(1);
            }
            else {
              robot.inverse();
              dindingKiri = false;
              loggerShift(2);
            }
          }
        }
      }

      if (robot.lantai() != 2) {
        robot.readJarak();
        while (robot.lantai() != 2) {
          if (dindingKiri) {
            action = robot.scanKiriRuang(Xmove, speed);
            nAction = (action & 6) > 0 ? nAction + 1 : nAction;
            dindingKiri = (action & 3) > 0 ? !dindingKiri : dindingKiri;
          }
          else {
            action = robot.scanKananRuang(Xmove, speed);
            nAction = (action & 6) > 0 ? nAction + 1 : nAction;
            dindingKiri = (action & 5) > 0 ? !dindingKiri : dindingKiri;
          }

          if (FRONT < 15)
            takeDataAgain = false;

          if (takeDataAgain && nSample < 30) {
            logger[0][nSample] = FRONT + 12;
            logger[1][nSample] = LEFT_MIDLE + 8;
            logger[2][nSample] = REAR + 12;
            logger[3][nSample] = RIGHT_MIDLE + 8;
            nSample++;
          }
        }
      }

      robot.move(0, 0, 0, speed);
      sensor.clearTime = 5;

      ruang value;
      value.room = roomIdentification(nAction, nSample);
      if (value.room == 1) {
        robot.readJarak();
        if (max(LEFT_MIDLE_FRONT, LEFT_MIDLE) < 30)
          value.door = doorA;
        else if (max(RIGHT_MIDLE_FRONT, RIGHT_MIDLE) < 30)
          value.door = doorC;
        else
          value.door = doorB;
      }

      robot.showSensor();
      robot.move(40, 0, 0, speed);
      lcd.clear();
      lcd.print("nNow Room : " + String(value.room));
      if (value.room == 1)
        if (value.door == doorA) lcd.println(" door A");
        else if (value.door == doorB) lcd.println(" door B");
        else if (value.door == doorC) lcd.println(" door C");
        else lcd.println("-");
      else lcd.println();
      return value;
    }

    void loggerShift(uint8_t shift) {
      int16_t temp;
      uint8_t index;

      if (shift == 1)
        for (index = 0; index < 30; index++) {
          temp = logger[0][index];
          logger[0][index] = logger[1][index];
          logger[1][index] = logger[2][index];
          logger[2][index] = logger[3][index];
          logger[3][index] = temp;
        }
      else if (shift == 2)
        for (index = 0; index < 30; index++) {
          temp = logger[0][index];
          logger[0][index] = logger[2][index];
          logger[2][index] = temp;
          temp = logger[1][index];
          logger[1][index] = logger[3][index];
          logger[3][index] = temp;
        }
      else if (shift == 3)
        for (index = 0; index < 30; index++) {
          temp = logger[0][index];
          logger[0][index] = logger[3][index];
          logger[3][index] = logger[2][index];
          logger[2][index] = logger[1][index];
          logger[1][index] = temp;
        }
    }
    void p_menyebar(int16_t speed) {
      robot.move(0, 0, 0, 45);
      pumpOn;
      robot.putarKiri(9, 100);
      robot.putarKanan(9, 100);
      robot.putarKiri(9, 100);
      pumpOff;
      robot.putarKiri(0, 100);
    }

    // === === === ROOM IDENTIFICATION === === === //
    uint8_t roomIdentification(uint8_t nAction, uint8_t nSample) {
      int32_t x, y;
      uint8_t hasil = unknown;
      uint8_t room;
      int16_t value;
      int16_t error = 0x7FFF;
      uint8_t index;

      robot.readJarak();
      if (nSample == 1) {
        //        Serial.println("Lantai Abu-abu -> Ruang 2");
        hasil = 2;
      }
      else if (nAction == 0 && min(FRONT, min(FRONT_LEFT, FRONT_RIGHT)) > 50) {
        //        Serial.println("depan tidak ada dinding -> ruang 1");
        hasil = 1;
      }
      else {
        for (count = 0; count < nSample; count++) {
          //          Serial.print("FRONT : " + String(logger[0][count]) + "\t");
          //          Serial.print("LEFT  : " + String(logger[1][count]) + "\t");
          //          Serial.print("REAR  : " + String(logger[2][count]) + "\t");
          //          Serial.print("RIGHT : " + String(logger[3][count]) + "\t");
          x = logger[(nAction % 2)][count] + logger[(nAction % 2) + 2][count];
          y = logger[((nAction + 1) % 2)][count] + logger[((nAction + 1) % 2) + 2][count];
          //          Serial.println("X : " + String(x) + "\tY : " + String(y));
        }
        //        Serial.println("jumlah aksi " + String(nAction));

        if (nAction == 0) {
          x = REAR + 12;
          for (count = 0; count < nSample; count++)
            x = max(x, logger[2][count]);
        }
        //        else if (nAction == 1) {
        //          x = REAR + 12;
        //        }

        for (count = 0; count < nSample; count++) {
          if (nAction > 0)
            x = logger[(nAction % 2)][count] + logger[(nAction % 2) + 2][count];
          y = logger[((nAction + 1) % 2)][count] + logger[((nAction + 1) % 2) + 2][count];

          //          Serial.println("\n\nX : " + String(x) + "\tY : " + String(y));
          //          Serial.println("mencari error minimum");
          for (room = 0; room < 4; room++) {
            value = abs(((float)(x - dataRuang[0][room]) * 100) / ((float)(x + dataRuang[0][room]))) ;
            value += abs(((float)(y - dataRuang[1][room]) * 100) / ((float)(x + dataRuang[1][room]))) ;
            if (error > value) {
              index = room;
              error = value;
            }
            //            Serial.print("Index Ruang : " + String(room + 1));
            //            Serial.println("\terror : " + String(value));
          }
        }
        //        Serial.print("index minimum : " + String(index + 1));
        //        Serial.println(" dengan error : " + String(error));
        hasil = (index + 1) == 2 ? 1 : (index + 1);
      }
      return hasil;
    }

    // === === === KELUAR RUANG BERAPI === === === //
    void keluarRuangApi(int16_t Xmove, int16_t speed) {
      playing = false;
      FIRE = false;
      //      FIRE_LED_OFF;

      lcd.clear(); lcd.backlight();
      lcd.setCursor(0, 0); lcd.print("Fire room :" + String(nowRoom));
      lcd.setCursor(0, 1); lcd.print("Start room:" + String(startRoom));

      robot.inverse();
      int16_t diff;
      if (!DINDING_KANAN && !DINDING_KIRI) {
        if (max(FRONT, REAR) < 10)
          for (count = 0; count < (200 / Xmove) && RIGHT_MIDLE > 15; count++) {
            robot.move(0, Xmove, 0, speed);
            robot.readJarak();
          }
        //robot.scanKanan();
        robot.putarKiri(90, speed);
        robot.readJarak();
      }
      else if (DINDING_KANAN && max(RIGHT_MIDLE_FRONT, RIGHT_MIDLE_REAR) < RIGHT_MIDLE) {
        if (max(FRONT, REAR) < 10)
          for (count = 0; count < (200 / Xmove) && RIGHT_MIDLE > 15; count++) {
            robot.move(0, Xmove, 0, speed);
            robot.readJarak();
          }
        robot.putarKanan(30, speed);
        robot.readJarak();
      }
      else if (DINDING_KIRI && max(LEFT_MIDLE_FRONT, LEFT_MIDLE_REAR) < LEFT_MIDLE) {
        if (max(FRONT, REAR) < 10)
          for (count = 0; count < (200 / Xmove) && LEFT_MIDLE > 15; count++) {
            robot.move(0, -Xmove, 0, speed);
            robot.readJarak();
          }
        robot.putarKiri(30, speed);
        robot.readJarak();
      }
      else if (DINDING_KIRI && max(FRONT, REAR) < 10) {
        for (count = 0; count < (200 / Xmove) && RIGHT_MIDLE > 15; count++) {
          robot.move(0, Xmove, 0, speed);
          robot.readJarak();
        }
      }
      else if (DINDING_KANAN && max(FRONT, REAR) < 10) {
        for (count = 0; count < (200 / Xmove) && LEFT_MIDLE > 15; count++) {
          robot.move(0, -Xmove, 0, speed);
          robot.readJarak();
        }
      }

      if (DINDING_KANAN || DINDING_KIRI) {
        for (count = 0; count < 10; count++) {
          if (DINDING_KIRI) {
            robot.pidKiri(0, speed, 10);
            if (min(LEFT_MIDLE, min(LEFT_MIDLE_FRONT, LEFT_MIDLE_REAR)) > 20)
              robot.move(0, -15, 0, speed);

            robot.readJarak();
            diff = LEFT_MIDLE_FRONT - LEFT_MIDLE_REAR;
          }
          else if (DINDING_KANAN) {
            robot.pidKanan(0, speed, 10);
            if (min(RIGHT_MIDLE_FRONT, min(RIGHT_MIDLE, RIGHT_MIDLE_REAR)) > 20)
              robot.move(0, 15, 0, speed);

            robot.readJarak();
            diff = RIGHT_MIDLE_FRONT - RIGHT_MIDLE_REAR;
          }
          if (abs(diff) < 2)
            break;
        }
      }

      robot.move(0, 0, 0, speed);
      robot.readJarak();
      bool dindingKiri;
      if (max(LEFT_MIDLE, max(LEFT_MIDLE_REAR, LEFT_MIDLE_FRONT)) < 20) {
        if (FRONT < 20 && min(FRONT_LEFT, FRONT_RIGHT) < 30)
          dindingKiri = FRONT_LEFT < FRONT_RIGHT ? true : false;
        else if (REAR < 20 && min(REAR_LEFT, REAR_RIGHT) < 30)
          dindingKiri = REAR_LEFT < REAR_RIGHT ? true : false;
      }
      else if (LEFT_MIDLE < 20 && LEFT_MIDLE_REAR < 20 && LEFT_MIDLE_FRONT < 20 && ((FRONT < 20 && FRONT_LEFT < 30) || (REAR < 20 && REAR_LEFT < 30)))
        dindingKiri = true;
      else if (RIGHT_MIDLE < 20 && RIGHT_MIDLE_REAR < 20 && RIGHT_MIDLE_FRONT < 20 && ((FRONT < 20 && FRONT_RIGHT < 30) || (REAR < 20 && REAR_RIGHT < 30)))
        dindingKiri = false;
      else if (LEFT_MIDLE < 20 && LEFT_MIDLE_REAR < 20 && LEFT_MIDLE_FRONT < 20)
        dindingKiri = true;
      else if (RIGHT_MIDLE < 20 && RIGHT_MIDLE_REAR < 20 && RIGHT_MIDLE_FRONT < 20)
        dindingKiri = false;

      robot.move(0, 0, 0, speed);
      if (max(LEFT_MIDLE, max(LEFT_MIDLE_FRONT, LEFT_MIDLE_REAR)) < 20) {
        if (max(min(FRONT, FRONT_LEFT), min(REAR, REAR_LEFT)) < 10)
          while (LEFT_MIDLE < 25 && RIGHT_MIDLE > 12) {
            robot.move(0, Xmove, 0, speed);
            robot.readJarak();
          }
      }
      else if (max(RIGHT_MIDLE, max(RIGHT_MIDLE_FRONT, RIGHT_MIDLE_REAR)) < 20) {
        if (max(min(FRONT, FRONT_RIGHT), min(REAR, REAR_RIGHT)) < 10)
          while (RIGHT_MIDLE < 25 && LEFT_MIDLE > 12) {
            robot.move(0, -Xmove, 0, speed);
            robot.readJarak();
          }
      }

      if (dindingKiri) {
        if (min(REAR, REAR_LEFT) < 30 && min(RIGHT_MIDLE, min(RIGHT_MIDLE_FRONT, RIGHT_MIDLE_REAR)) > 20) {
          robot.putarKanan(90, speed);
          dindingKiri = false;
          robot.readJarak();
        }
        else if (REAR < 30 && FRONT < 30) {
          robot.putarKanan(90, speed);
          robot.readJarak();
          if (REAR_LEFT < 30 && LEFT_MIDLE_FRONT < 30)
            dindingKiri = true;
          else if (REAR_RIGHT < 30 && RIGHT_MIDLE_FRONT < 30)
            dindingKiri = false;
        }
        else if (min(FRONT, FRONT_LEFT) < 30 && min(RIGHT_MIDLE, min(RIGHT_MIDLE_FRONT, RIGHT_MIDLE_REAR)) > 20) {
          robot.putarKanan(90, speed);
          dindingKiri = true;
          robot.readJarak();
        }
      }
      else {
        if (min(REAR, REAR_RIGHT) < 30 && min(LEFT_MIDLE, min(LEFT_MIDLE_FRONT, LEFT_MIDLE_REAR)) > 20) {
          robot.putarKiri(90, speed);
          dindingKiri = true;
        }
        else if (REAR < 30 && FRONT < 30) {
          robot.putarKiri(90, speed);
          robot.readJarak();
          if (REAR_LEFT < 30 && LEFT_MIDLE_FRONT < 30)
            dindingKiri = true;
          else if (REAR_RIGHT < 30 && RIGHT_MIDLE_FRONT < 30)
            dindingKiri = false;
        }
        else if (min(FRONT, FRONT_RIGHT) < 30 && min(LEFT_MIDLE, min(LEFT_MIDLE_FRONT, LEFT_MIDLE_REAR)) > 20) {
          robot.putarKiri(90, speed);
          dindingKiri = false;
          robot.readJarak();
        }
      }

      robot.readJarak();
      if (FRONT < 25) {
        robot.inverse();
        dindingKiri = !dindingKiri;
        robot.readJarak();
      }

      while (REAR < 20 && FRONT > 15) {
        robot.move(Xmove, 0, 0, speed);
        robot.readJarak();
      }
      sensor.clearTime = 10;

      diff = ((float)((LEFT_MIDLE_FRONT + LEFT_MIDLE_REAR) - (RIGHT_MIDLE_FRONT + RIGHT_MIDLE_REAR))) \
             * 100 / ((float)(LEFT_MIDLE_FRONT + LEFT_MIDLE_REAR) + (RIGHT_MIDLE_FRONT + RIGHT_MIDLE_REAR));

      if (dindingKiri && diff > 0)
        dindingKiri = false;
      else if (!dindingKiri && diff < 0)
        dindingKiri = true;

      uint8_t action;
      while (robot.lantai() != 2) {
        if (dindingKiri) {
          action = robot.scanKiriRuang(Xmove, speed);
          dindingKiri = (action & 3) > 0 ? !dindingKiri : dindingKiri;
        }
        else {
          action = robot.scanKananRuang(Xmove, speed);
          dindingKiri = (action & 5) > 0 ? !dindingKiri : dindingKiri;
        }
      }

      if (nowRoom == 1 && door1 == doorA) {
        robot.move(30, 0, 0, speed);
        robot.move(0, 40, 0, speed);
      }
      else {
        robot.move(30, 0, 0, speed);
        robot.move(0, -40, 0, speed);
      }

      robot.move(0, 0, 0, speed);
      lcd.clear(); lcd.noBacklight();

      room(nowRoom, startRoom, 40, 8, false);
      robot.shutdown();
      while (1);
    }

    // === === === MASUK RUANG API === === === //
    void masukRuangApi(int16_t Xmove, int16_t speed, int16_t batasApi_Code = 915, bool leftWall = true) {
      //      FIRE_LED_ON;
      robot.move(0, 0, 0, speed);
      robot.position(0, 0, 45, speed);
      vector temp = robot.trackingApi(60, 80, true);
      temp.resultan = temp.resultan > 1023 ? 0 : temp.resultan;
      lcd.clear();

      while (temp.resultan < batasApi_Code || abs(temp.sudut - 90) < 5) { //      while (temp.resultan < batasApi_Code || abs(temp.sudut - 90) < 5) {

        temp = robot.trackingApi(30, 60, leftWall);
        temp.resultan = temp.resultan > 1023 ? 0 : temp.resultan;
        lcd.setCursor(0, 0);
        lcd.print(temp.resultan);
        lcd.print("  ");
      }

      bool apiPadam = false;
      do {
        apiPadam = padamkanApi(Xmove, speed,  batasApi_Code, apiPadam);
        delay(1000);
      } while (FIRE);

      robot.move(0, 0, 0, 10);
      FIRE = false;
      playing = false;
      //      FIRE_LED_OFF;
      lcd.clear(); lcd.print("Api Padam!");
    }


    // === === === KELUAR RUANG === === ===//
    void keluarRuang(int16_t Xmove, int16_t speed) {
      playing = false;
      FIRE = false;
      robot.readJarak();
      if (nowRoom == 1 && door1 == doorA) {
        while (robot.lantai() != 2)
          if (LEFT_MIDLE_FRONT < 40)
            robot.scanKiriRuang(Xmove, speed);
          else
            robot.move(Xmove, 0, 0, speed);
        robot.move(30, 0, 0, speed);
        robot.move(0, 40, 0, speed);
      }
      else {
        while (robot.lantai() != 2)
          if (RIGHT_MIDLE_FRONT < 40)
            robot.scanKananRuang(Xmove, speed);
          else
            robot.move(Xmove, 0, 0, speed);
        robot.move(30, 0, 0, speed);
        robot.move(0, -40, 0, speed);
      }

      lcd.clear(); lcd.noBacklight();
    }

    // === === === MASUK RUANG === === === //
    bool masukRuang(int16_t Xmove, int16_t speed, bool end = true) {
      nowRoom = nextRoom;
      robot.move(0, 0, 0, speed);
      playing = true;

      FIRE = false;
      bool leftWall;

      if (!end) {
        robot.readJarak();
        if ((nowRoom == 1 && door1 == doorB) || nowRoom == 4) {
          while (!DINDING_KIRI && FRONT < 20) {
            robot.move(Xmove, 0, 0, speed);
            robot.readJarak();
          }
        }

        robot.readJarak();
        for (count = 0; count < 4 && FRONT > 8; count++)
          if (nowRoom == 1  && door1 == doorA)
            robot.scanKananRuang(40, speed);
          else
            robot.scanKiriRuang(40, speed);
        return false;
      }

      switch (nowRoom) {
        case 1 :
          switch (door1) {
            case doorA :
              robot.readJarak();
              while (min(RIGHT_MIDLE_REAR, RIGHT_MIDLE_FRONT) > 15) {
                robot.move(0, -Xmove, 0, speed);
                robot.readJarak();
              }

              for (count = 0; count < 4; count++) {
                robot.pidKanan(0, speed, 10);
                robot.readJarak();
              }

              robot.readJarak();
              for (count = 0; count < 6 && FRONT > 10; count++) {
                robot.move(30, 0, 0, speed);
                robot.readJarak();
              }

              if (FIRE)
                goto padamkan;

              robot.move(0, -40, 0, speed);
              if (robot.sensorInverse) {
                robot.inverse();
                for (count = 0; count < 180; count += 15) {
                  robot.move(0, 0, 15, speed);
                  if (FIRE && abs(sensor.radarApi().sudut - 90) < 40)
                    goto padamkan;
                }
              }
              else {
                for (count = 0; count < 130; count += 16) {
                  robot.move(0, 0, -16, speed);
                  if (FIRE  && abs(sensor.radarApi().sudut - 90) < 60)
                    goto padamkan;

                }
                robot.move(0, 0, 0, speed);
                for (count = 0; count < 130; count += 16) {
                  robot.move(0, 0, 16, speed);
                  if (FIRE && abs(sensor.radarApi().sudut - 90) < 60)
                    goto padamkan;
                }
              }

              if (!FIRE) {
                robot.readJarak();
                for (count = 0; count < 3; count++) {
                  robot.pidKanan(0, speed, 10);
                  robot.readJarak();
                }
                robot.inverse();
                return false;
              }
              leftWall = false;
              break;

            case doorB :
              robot.readJarak();
              while (LEFT_MIDLE_REAR > 40 && FRONT < 8) {
                robot.move(Xmove, 0, 0, speed);
                robot.readJarak();
              }
              robot.readJarak();
              while (min(LEFT_MIDLE_REAR, LEFT_MIDLE_FRONT) > 15) {
                robot.move(0, -Xmove, 0, speed);
                robot.readJarak();
              }

              robot.readJarak();
              if (abs(LEFT_MIDLE_FRONT - LEFT_MIDLE_REAR) > 3)
                if (LEFT_MIDLE_FRONT > LEFT_MIDLE_REAR)
                  robot.putarKiri(20, speed);
                else
                  robot.putarKanan(20, speed);

              robot.readJarak();
              for (count = 0; count < 4; count++) {
                robot.pidKiri(0, speed, 10);
                robot.readJarak();
              }

              robot.readJarak();
              for (count = 0; count < 6 && FRONT > 10; count++) {
                robot.move(30, 0, 0, speed);
                robot.readJarak();
              }

              robot.move(0, 40, 0, speed);
              if (robot.sensorInverse) {
                robot.inverse();
                for (count = 0; count < 180; count += 15) {
                  robot.move(0, 0, -15, speed);
                  if (FIRE && abs(sensor.radarApi().sudut - 90) < 40) {
                    goto padamkan;
                  }
                }
              }
              else {
                for (count = 0; count < 130; count += 16) {
                  if (FIRE && abs(sensor.radarApi().sudut - 90) < 60)
                    goto padamkan;
                  robot.move(0, 0, 16, speed);
                }
                for (count = 0; count < 130; count += 16) {
                  if (FIRE && abs(sensor.radarApi().sudut - 90) < 60)
                    goto padamkan;
                  robot.move(0, 0, -16, speed);
                }
              }

              if (FIRE)
                goto padamkan;

              if (!FIRE) {
                for (count = 0; count < 3; count++) {
                  robot.pidKiri(0, speed, 10);
                  robot.readJarak();
                }
                robot.inverse();
                return false;
              }
              leftWall = true;
              break;

            case doorC :
              leftWall = true;
              break;
          }
          break;

        case 2:
          robot.readJarak();
          while (max(LEFT_MIDLE_REAR, LEFT_MIDLE_FRONT) > 20) {
            robot.move(0, -Xmove, 0, speed);
            robot.readJarak();
          }

          robot.readJarak();
          for (count = 0; count < 4; count++) {
            robot.pidKiri(0, speed, 10);
            robot.readJarak();
          }

          for (count = 0; count < 6; count++)
            robot.move(30, 0, 0, speed);

          robot.move(0, 40, 0, speed);
          if (robot.sensorInverse) {
            robot.inverse();
            for (count = 0; count < 180; count += 15) {
              if (FIRE && abs(sensor.radarApi().sudut - 90) < 40)
                goto padamkan;
              robot.move(0, 0, -15, speed);
            }
            robot.move(-40, 0, 0, speed);
            robot.inverse();
          }
          else {
            for (count = 0; count < 120; count += 15) {
              if (FIRE && abs(sensor.radarApi().sudut - 90) < 60)
                goto padamkan;
              robot.move(0, 0, 15, speed);
            }
            robot.move(0, 0, 0, speed);
            for (count = 0; count < 120; count += 15) {
              if (FIRE && abs(sensor.radarApi().sudut - 90) < 60)
                goto padamkan;
              robot.move(0, 0, -15, speed);
            }
            robot.inverse();
          }


          if (!FIRE) {
            robot.readJarak();
            for (count = 0; count < 3; count++) {
              robot.pidKanan(0, speed);
              robot.readJarak();
            }
            return false;
          }
          leftWall = true;
          break;

        case 3 :
          robot.readJarak();
          while (min(LEFT_MIDLE_REAR, LEFT_MIDLE_FRONT) > 15) {
            robot.move(0, -Xmove, 0, speed);
            robot.readJarak();
          }

          robot.readJarak();
          for (count = 0; count < 3; count++) {
            robot.pidKiri(0, speed, 10);
            robot.readJarak();
          }

          count = 6;
          robot.readJarak();
          while (min(min(FRONT_LEFT, FRONT_RIGHT), FRONT) > 10 && count--) {
            robot.move(30, 0, 0, speed);
            robot.readJarak();
          }
          count = 0;

          //          for (count = 0; count < 5; count++)
          //            robot.move(30, 0, 0, speed);

          if (robot.sensorInverse) {
            robot.inverse();
            robot.move(0, -40, 0, speed);
            for (count = 0; count < 180; count += 15) {
              if (FIRE && abs(sensor.radarApi().sudut - 90) < 40)
                goto padamkan;
              robot.move(0, 0, -15, speed);
            }
          }

          if (FIRE)
            goto padamkan;

          for (count = 0; count < 120; count += 15) {
            if (FIRE && abs(sensor.radarApi().sudut - 90) < 60)
              goto padamkan;
            robot.move(0, 0, 15, speed);
          }
          robot.move(0, 0, 0, speed);

          for (count = 0; count < 120; count += 15) {
            if (FIRE && abs(sensor.radarApi().sudut - 90) < 60)
              goto padamkan;
            robot.move(0, 0, -15, speed);
          }

          if (!FIRE) {
            robot.inverse();
            robot.readJarak();
            for (count = 0; count < 3; count++) {
              robot.pidKanan(0, speed);
              robot.readJarak();
            }
            return false;
            leftWall = true;
          }
          break;

        case 4 :
          robot.readJarak();
          while (LEFT_MIDLE_REAR > 40 && FRONT < 8) {
            robot.move(Xmove, 0, 0, speed);
            robot.readJarak();
          }

          robot.readJarak();
          while (min(LEFT_MIDLE_REAR, LEFT_MIDLE_FRONT) > 15) {
            robot.move(0, -Xmove, 0, speed);
            robot.readJarak();
          }

          robot.readJarak();
          for (count = 0; count < 4; count++) {
            robot.pidKiri(0, speed);
            robot.readJarak();
          }

          robot.readJarak();
          for (count = 0; count < 4 && FRONT > 10; count++) {
            robot.move(30, 0, 0, speed);
            robot.readJarak();
          }

          if (robot.sensorInverse) {
            robot.inverse();
            robot.move(0, -40, 0, speed);
            for (count = 0; count < 180; count += 15) {
              if (FIRE && abs(sensor.radarApi().sudut - 90) < 40)
                goto padamkan;
              robot.move(0, 0, -15, speed);
            }
          }
          else {
            robot.move(0, Xmove, 0, speed);
            for (count = 0; count < 120; count += 15) {
              if (FIRE)
                goto padamkan;
              robot.move(0, 0, 15, speed);
            }
            robot.putarKiri(120, speed);
          }

          if (!FIRE) {
            robot.inverse();
            robot.readJarak();
            for (count = 0; count < 3; count++) {
              robot.pidKanan(0, speed);
              robot.readJarak();
            }
            return false;
          }
          leftWall = true;
          break;
      }

padamkan:
      robot.move(0, 0, 0, speed);
      masukRuangApi(30, 20, leftWall);
      keluarRuangApi(Xmove, speed);
      return true;
    }

    void room1_2(int16_t Xmove, int16_t speed) {
      robot.readJarak();

      if (door1 == doorA) {
        for (count = 0; count < 4; count++)
          robot.scanKiri(Xmove, speed);
        while (robot.lantai() != 2)
          robot.scanKiri(Xmove, speed);
      }

      if (door1 == doorB) {
        for (count = 0; count < 3 && FRONT > 20; count++) {
          robot.move(40, 0, 0, speed);
          robot.readJarak();
        }

        if (robot.sensorInverse) {
          robot.putarKanan(90, speed);
          robot.inverse();
        }
        else
          robot.putarKiri(90, speed);


        while (robot.lantai() != 2)
          robot.scanKanan(Xmove, speed);
      }

      if (door1 == doorC) {
        /*========================================================*/
        /*========================================================*/
        /*========================================================*/
        /*========================================================*/
        /*========================================================*/
      }

      robot.move(0, 0, 0, 20);
    }


    void room2_3(int16_t Xmove, int16_t speed) {
      robot.readJarak();
      for (count = 0; count < 2; count++)
        robot.scanKanan(0, speed);
      robot.putarKiri(30, speed);
      robot.majuBelokKiri(60, speed);

      robot.readJarak();
      while (LEFT_MIDLE_REAR > 40) {
        robot.move(Xmove, 0, 0, speed);
        robot.readJarak();
      }

      while (robot.lantai() != 2)
        robot.scanKiri(Xmove, speed);
    }

    void room2_4(int16_t Xmove, int16_t speed) {
      robot.putarKiri(30, speed);
      robot.majuBelokKiri(60, speed);

      robot.readJarak();
      while (LEFT_MIDLE_REAR > 40) {
        robot.move(Xmove, 0, 0, speed);
        robot.readJarak();
      }

      while (LEFT_MIDLE_FRONT < 40)
        robot.scanKiri(Xmove, speed);

      robot.majuBelokKiri(90, speed);
      robot.readJarak();
      while (FRONT > 50)
        robot.scanKiri(Xmove, speed);

      robot.inverse();
      gotoRoom4(Xmove, speed);


      robot.move(0, 0, 0, 20);
    }

    void room3_1(int16_t Xmove, int16_t speed, bool jump = false) {
      if (!jump) {
        robot.readJarak();
        while (min(FRONT, IR_FRONT) > 15)
          robot.scanKanan(40, speed);

        if (robot.sensorInverse) {
          robot.putarKanan(90, speed);
          robot.inverse();
        }
        else
          robot.putarKiri(90, speed);

        robot.readJarak();
        while (RIGHT_MIDLE_FRONT < 40)
          robot.scanKanan(Xmove, speed);

        robot.majuBelokKanan(90, speed);
      }
      robot.readJarak();
      while (FRONT > 20) {
        if (FRONT > 50)
          robot.pidKanan(Xmove, speed);
        else
          robot.move(Xmove, 0, 0, speed);
        robot.readJarak();
      }

      if (door1 == unknown) {
        if (LEFT_MIDLE > 60) {
r1A :
          door1 = doorA;
          if (robot.sensorInverse) {
            robot.putarKanan(90, speed);
            robot.inverse();
          }
          else
            robot.putarKiri(90, speed);

          robot.readJarak();

          while (robot.lantai() != 2)
            robot.scanKanan(Xmove, speed);
        }
        else {
r1B :
          robot.putarKiri(10, speed);
          robot.inverse();
          door1 = doorB;

          robot.readJarak();
          while (robot.lantai() != 2)
            robot.scanKanan(Xmove, speed);
        }
      }
      else if (door1 == doorA) goto r1A;
      else if (door1 == doorB) goto r1B;

      robot.move(0, 0, 0, 20);
    }



    //    void room3_4(int16_t Xmove, int16_t speed) {
    //      robot.readJarak();
    //      while (min(FRONT, IR_FRONT) > 30)
    //        robot.scanKanan(40, speed);
    //
    //      if (robot.sensorInverse) {
    //        robot.putarKanan(90, speed);
    //        robot.inverse();
    //      }
    //      else
    //        robot.putarKiri(90, speed);
    //
    //      for (count = 0; count < 4; count++)
    //        robot.move(30, 0, 0, speed);
    //      gotoRoom4(Xmove, speed);
    //      robot.move(0, 0, 0, 20);
    //    }

    void gotoRoom4(int16_t Xmove, int16_t speed) {
      robot.readJarak();
      int16_t range;

      while (RIGHT_MIDLE_FRONT < 60)
        robot.scanKanan(Xmove, speed, 25);
      robot.majuBelokKiri(90, speed);

      robot.readJarak();
      if (FRONT_RIGHT < 15)
        robot.move(0, -24, -8, speed);

      for (count = 0; count < 3; count++)
        robot.move(40, -10, 0, speed);
      for (count = 0; count < 3; count++)
        robot.scanKiri(Xmove, speed);

      if (door4 == unknown)
        if (boneka != bonekaA) {
          while (FRONT > 20)
            robot.scanKiri(Xmove, speed);

          range = 400;
          while (FRONT > 13) {
            robot.move(45, 0, 0, speed);
            robot.readJarak();
            range = min(IR_RIGHT, range);
          }
          robot.move(0, 0, 0, speed);

          if (range < 40) {
            boneka = bonekaA;
r4bA: //RUANG 4 BONEKA A
            robot.inverse();
            robot.readJarak();
            while (REAR < 50)
              robot.scanKanan(Xmove, speed);
            while (LEFT_MIDLE_FRONT < 40)
              robot.scanKiri(Xmove, speed);
            robot.majuBelokKiri(90, speed);

            for (count = 0; count < 2; count++)
              robot.move(40, 0, 0, speed);

            robot.readJarak();
            if (LEFT_MIDLE_FRONT > 40) {
              door4 = doorA;

              while (robot.lantai() != 2)
                robot.majuBelokKiri(7, speed);
            }
            else {
              door4 = doorB;
r4BbA:
              while (LEFT_MIDLE_FRONT < 60)
                robot.scanKiri(Xmove, speed);
              robot.majuBelokKiri(90, speed);

              robot.readJarak();
              while (FRONT > 20)
                robot.scanKanan(Xmove, speed);

              if (robot.sensorInverse) {
                robot.putarKanan(90, speed);
                robot.inverse();
              }
              else
                robot.putarKiri(90, speed);

              robot.readJarak();
              while (REAR < 50)
                robot.scanKanan(Xmove, speed);

              robot.putarKiri(90, speed);

              while (robot.lantai() != 2)
                robot.move(Xmove, 0, 0, speed);
            }
          }
          else {
            robot.move(-45, 0, 0, speed);
            robot.move(-45, 0, 0, speed);
r4B:
            if (robot.sensorInverse) {
              robot.putarKiri(90, speed);
              robot.inverse();
            }
            else
              robot.putarKanan(90, speed);

            robot.readJarak();
            while (REAR < 60)
              robot.scanKiri(Xmove, speed);
            count = 4;
            while (count--)
              robot.move(45, 0, 0, speed);

            robot.readJarak();
            if (!DINDING_KANAN || door4 == doorB) {
              door4 = doorB;
              robot.putarKanan(80, speed);

              while (robot.lantai() != 2)
                robot.move(Xmove, 0, 0, speed);
              boneka = bonekaB;
            }
            else {
              door4 = doorA;
              robot.inverse();
              robot.readJarak();
              while (FRONT > 20)
                robot.scanKanan(Xmove, speed);

              if (robot.sensorInverse) {
                robot.putarKanan(90, speed);
                robot.inverse();
              }
              else
                robot.putarKiri(90, speed);
r4A:
              robot.readJarak();
              while (REAR < 50)
                robot.scanKanan(Xmove, speed);
              while (LEFT_MIDLE_FRONT < 40)
                robot.scanKiri(Xmove, speed);
              robot.majuBelokKiri(90, speed);

              for (count = 0; count < 2; count++)
                robot.move(40, 0, 0, speed);

              while (robot.lantai() != 2)
                robot.majuBelokKiri(7, speed);
            }
          }
        }
        else goto r4bA;
      else if (door4 == doorA) {
        robot.inverse();
        goto r4A;
      }
      else if (door4 == doorB)
        if (boneka == bonekaA) {
          robot.inverse();
          robot.readJarak();
          while (REAR < 50)
            robot.scanKanan(Xmove, speed);
          while (LEFT_MIDLE_FRONT < 40)
            robot.scanKiri(Xmove, speed);
          robot.majuBelokKiri(90, speed);

          count = 3;
          while (count--)
            robot.move(45, 0, 0, speed);
          robot.readJarak();
          goto r4BbA;
        }
        else {
          while (FRONT > 20)
            robot.scanKiri(Xmove, speed);
          door4 = doorB;
          goto r4B;
        }
      robot.move(0, 0, 0, 20);
    }

    void fromRoom4(int16_t Xmove, int16_t speed) {
      for (count = 0; count < 3; count++)
        robot.move(40, 0, 0, speed);
      if (robot.sensorInverse) {
        robot.putarKiri(90, speed);
        robot.inverse();
      }
      else
        robot.putarKanan(90, speed);

      robot.readJarak();
      while (min(LEFT_MIDLE_FRONT, LEFT_MIDLE_REAR) > 15) {
        robot.move(0, -Xmove, 0, speed);
        robot.readJarak();
      }

      if (abs(LEFT_MIDLE_FRONT - LEFT_MIDLE_REAR) > 3)
        if (LEFT_MIDLE_FRONT > LEFT_MIDLE_REAR)
          robot.putarKiri(20, speed);
        else
          robot.putarKanan(20, speed);

      robot.readJarak();
      int16_t range = 200;

      if (door4 == doorA) {
        while (LEFT_MIDLE_FRONT < 40)
          robot.scanKiri(Xmove, speed);
        goto fromRoom4DoorA;
      }
      else if (door4 == doorB && boneka == bonekaA) {
        while (FRONT > 20)
          robot.scanKiri(Xmove, speed);
        goto fromRoom4DoorBbonekaA;
      }
      else if (door4 == doorB) {
        robot.scanKiri(0, speed);
        robot.scanKiri(0, speed);
        goto fromRoom4DoorB;
      }

      for (count = 0; count < 6; count++) {
        robot.scanKiri(0, speed, 15);
        if (count >= 4)
          range = min(range, IR_REAR);
      }
      robot.move(0, 0, 0, speed);

      if (range < 40)
        boneka = bonekaA;

      robot.readJarak();
      while (FRONT > 20 && LEFT_MIDLE_FRONT < 40)
        robot.scanKiri(Xmove, speed, 15);

      if (LEFT_MIDLE_FRONT > 35) {
        door4 = doorA;
        if (boneka == bonekaA)  boneka = bonekaB;

fromRoom4DoorA :
        robot.majuBelokKiri(45, speed);
        for (count = 0; count < 3; count++)
          robot.move(45, 0, 0, speed);
        robot.readJarak();
        while (RIGHT_MIDLE_FRONT < 40)
          robot.scanKanan(Xmove, speed);
      }
      else {
        door4 = doorB;
        if (boneka == bonekaA) {
fromRoom4DoorBbonekaA :
          if (robot.sensorInverse) {
            robot.putarKiri(90, speed);
            robot.inverse();
          }
          else
            robot.putarKanan(90, speed);

          robot.readJarak();
          while (REAR < 30)
            robot.scanKiri(Xmove, speed);
          while (RIGHT_MIDLE_FRONT < 40)
            robot.scanKanan(Xmove, speed);

          robot.majuBelokKanan(80, speed);
          robot.readJarak();
          for (count = 0; count < 5; count++)
            robot.scanKanan(45, speed);
          while (REAR < 30)
            robot.scanKanan(Xmove, speed);
          while (RIGHT_MIDLE_REAR > 40)
            robot.scanKanan(Xmove, speed);
          robot.readJarak();
          while (LEFT_MIDLE_FRONT < 60)
            robot.scanKiri(Xmove, speed);

          robot.majuBelokKiri(90, speed);
          for (count = 0; count < 3; count++)
            robot.move(45, 0, 0, speed);
          robot.readJarak();
          while (RIGHT_MIDLE_FRONT < 40)
            robot.scanKanan(Xmove, speed);
        }
        else {
fromRoom4DoorB :
          robot.inverse();
          robot.readJarak();
          while (FRONT > 25)
            robot.scanKanan(Xmove, speed, 15);

          if (robot.sensorInverse) {
            robot.putarKanan(90, speed);
            robot.inverse();
          }
          else {
            robot.putarKiri(90, speed);
          }

          robot.readJarak();
          while (RIGHT_MIDLE_FRONT < 60)
            robot.scanKanan(Xmove, speed, 15);

          robot.majuBelokKanan(80, speed);
          robot.readJarak();
          if (FRONT_RIGHT < 15)
            robot.move(0, -24, -8, speed);

          for (count = 0; count < 2; count++)
            robot.move(45, 0, 0, speed);
          robot.readJarak();
          while (FRONT > 40)
            robot.scanKiri(Xmove, speed);
        }
      }
    }

    //    void room4_1(int16_t Xmove, int16_t speed, uint8_t jump = 0) {
    //      if (jump == 0 || jump == 1)
    //        fromRoom4(Xmove, speed);
    //
    //      if (jump == 0 || jump == 2) {
    //        robot.readJarak();
    //        if (door4 == doorA) {
    //          room3_1(Xmove, speed, true);
    //        }
    //        else if (boneka == bonekaA) {
    //
    //          room3_1(Xmove, speed, true);
    //        }
    //        else {
    //          robot.inverse();
    //          robot.readJarak();
    //          while (RIGHT_MIDLE_FRONT < 40)
    //            robot.scanKanan(Xmove, speed);
    //          robot.majuBelokKanan(90, speed);
    //
    //          for (count = 0; count < 4; count++)
    //            robot.scanKanan(Xmove, speed);
    //
    //          room3_1(Xmove, speed, true);
    //        }
    //      }
    //    }

    //    void room4_2(int16_t Xmove, int16_t speed, uint8_t jump = 0) {
    //      if (jump == 0 || jump == 1)
    //        fromRoom4(Xmove, speed);
    //
    //      if (jump == 0 || jump == 2) {
    //        if (door4 == doorB && boneka != bonekaA)
    //          robot.inverse();
    //
    //        robot.readJarak();
    //
    //        while (robot.lantai() != 2)
    //          robot.scanKanan(Xmove, speed);
    //      }
    //    }
    //
    //    void room4_3(int16_t Xmove, int16_t speed, uint8_t jump = 0) {
    //      if (jump == 0 || jump == 1)
    //        fromRoom4(Xmove, speed);
    //
    //      if (jump == 0 || jump == 2) {
    //        if (door4 == doorA || (door4 == doorB && boneka == bonekaA)) {
    //          robot.inverse();
    //          for (count = 0; count < 4; count++)
    //            robot.move(Xmove, 0, 0, speed);
    //        }
    //
    //        robot.readJarak();
    //
    //        while (robot.lantai() != 2)
    //          robot.scanKiri(Xmove, speed);
    //      }
    //    }
    void home_r1(int16_t Xmove, int16_t speed, uint8_t jump = 0) {
      int gerak;
      // Mulai dari start

      //      // move by sensor
      while (1) {
        robot.readJarak();


        if (FRONT < 10) {

          lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
          lcd.print("FRONT " + String(sensorData[0]));
          for (gerak = 0 ; gerak <= 1 ; gerak++)
            robot.move(-25, 0, 0, 45);

        }
        if (LEFT_MIDLE < 10) {

          lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
          lcd.print("LEFT_MIDLE " + String(sensorData[3]));
          for (gerak = 0 ; gerak <= 1 ; gerak++)
            robot.move(0, 25, 0, 45);
        }
        if (REAR < 10) {

          lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
          lcd.print("REAR " + String(sensorData[6]));
          for (gerak = 0 ; gerak <= 1 ; gerak++)
            robot.move(25, 0, 0, 45);
        }
        if (RIGHT_MIDLE < 10) {

          lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
          lcd.print("RIGHT_MIDLE " + String(sensorData[9]));
          for (gerak = 0 ; gerak <= 1 ; gerak++)
            robot.move(0, -25, 0, 45);

        }
        if (FRONT_LEFT < 9 && LEFT_MIDLE_FRONT < 9) {

          lcd.clear(); lcd.backlight();
          lcd.setCursor(0, 0); lcd.print("FRONT_LEFT " + String(sensorData[2]));
          lcd.setCursor(0, 1); lcd.print("LM_FRONT " + String(sensorData[3]));
          for (gerak = 0 ; gerak <= 1 ; gerak++)
            robot.putarKanan(25, 45);
        }
        if (REAR_LEFT < 9 && LEFT_MIDLE_REAR < 9) {

          lcd.clear(); lcd.backlight();
          lcd.setCursor(0, 0); lcd.print("FRONT_LEFT " + String(sensorData[5]));
          lcd.setCursor(0, 1); lcd.print("LM_FRONT " + String(sensorData[4]));
          for (gerak = 0 ; gerak <= 1 ; gerak++)
            robot.putarKiri(25, 45);
        }
        if (REAR_RIGHT < 9 && RIGHT_MIDLE_REAR < 9) {

          lcd.clear(); lcd.backlight();
          lcd.setCursor(0, 0); lcd.print("REAR_RIGHT " + String(sensorData[7]));
          lcd.setCursor(0, 1); lcd.print("RM_REAR " + String(sensorData[8]));
          for (gerak = 0 ; gerak <= 1 ; gerak++)
            robot.putarKanan(25, 45);

        }
        if (FRONT_RIGHT < 9 && RIGHT_MIDLE_FRONT < 9) {

          lcd.clear(); lcd.backlight();
          lcd.setCursor(0, 0); lcd.print("FRONT_RIGHT " + String(sensorData[10]));
          lcd.setCursor(0, 1); lcd.print("RM_FRONT " + String(sensorData[11]));
          for (gerak = 0 ; gerak <= 1 ; gerak++)
            robot.putarKiri(25, 45);
        }

      }
      // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari Home ke R1");

      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 13);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
          robot.move(0, 20, 0, 45);
      }

      robot.position(0, 0, 30, 45);
      while (DINDING_KANAN)
        robot.scanKanan(30, 45);
      for (gerak = 0 ; gerak < 35 ; gerak++)
        robot.scanKiri(45, 45);
      delay(5000);
      p_menyebar(speed);
      robot.putarKanan(90, speed);
      robot.putarKanan(90, speed);
      for (gerak = 0 ; gerak < 35 ; gerak++)
        robot.scanKiri(45, 45);
      robot.Diam(speed);
      //      for(gerak = 0 ; gerak < 19 ; gerak++)
      //      for (gerak = 0 ; gerak < 10 ; gerak++)
      //        robot.scanKanan(45, 30);
      //      while (FRONT >= 25)
      //        robot.scanKanan(30, 45);
      //      for (gerak = 0 ; gerak < 3 ; gerak++) {
      //        robot.readJarak();
      //        robot.pidKanan(0, 30, 20);
      //        if (min(FRONT, min(FRONT_LEFT, FRONT_RIGHT)) < 21)
      //          robot.move(-20, 0, 0, 45);
      //        else if (min(RIGHT_MIDLE, min(RIGHT_MIDLE_FRONT, RIGHT_MIDLE_REAR)) < 22)
      //          robot.move(0, -20, 0, 45);
      //      }
      //    robot.position(0, 0, 15, 45);
      //    for (int gerak = 0 ; gerak < 10 ; gerak++)
      //      robot.move(0, -75, 0, 120);
      //    robot.putarKiri(20, 90);
      //    for (int gerak = 0 ; gerak < 10 ; gerak++)
      //      robot.scanKiri(20, 75);
      //
      //    // ruang api 1
      //
      //    for (int miring2 = 0 ; miring2 < 10 ; miring2++)
      //      robot.move(0, 40, 0, 60);
      //    robot.putarKanan(90, speed);
      //    for (int api1 = 0 ; api1 < 8 ; api1++)
      //      robot.move(30, 0, 0, 45);
      //    while (DINDING_KIRI) {
      //      robot.scanKiri(30, 45);
      //      if (FRONT < 25)
      //        break;
      //    }
      //    robot.FireCounting(FIRE);
      //    masukRuangApi(20, 80, 890);
      //    robot.FireCounting(FIRE);
      //    robot.move(0, 0, 0, 0);
      //    while (DINDING_KIRI)
      //      robot.scanKiri(-45, 75);
      //    for (int gerak = 0 ; gerak < 5 ; gerak++)
      //      robot.move(-45, 0, 0, 40);
      //    for (int gerak = 2 ; gerak > 0 ; gerak++)
      //      robot.move(0, 0, 0, 0);


      // ruang api 2

      //      for(int gerak = 0 ; gerak < 5 ; gerak++)
      //        robot.move(-45, 0, 0, 40);
      //      for(int gerak = 0 ; gerak < 25 ; gerak++)
      //        robot.move(0, -55, 0, 45);
      //    for (int gerak = 0 ; gerak < 3 ; gerak++)
      //      robot.scanKanan(45, 60);
      //    robot.position(0, 0, 22, 30);
      //    for (int gerak = 0 ; gerak < 10 ; gerak++)
      //      robot.scanKanan(40, 80);
      //    robot.position(0, 0, 30, 30);
      //    for (int gerak = 0 ; gerak < 55 ; gerak++)
      //      robot.scanKanan(40, 80);


      //    ==== ==== ==== ==== ====   //



      robot.position(0, 0, 40, 30);
      //    for (int balik = 0 ; balik < 60 ; balik++)
      //      robot.scanKiri(-40, 45);
      //    robot.move(0, 0, 0, 30);
      //    for (int agak = 0 ; agak < 2 ; agak++)
      //      robot.move(30, 0, 0, 30);
      //    for (int house = 0 ; house < 40 ; house++)
      //      robot.move(0, -60, 0, 45);
      //    for (int back = 0 ; back < 5 ; back++)
      //      robot.scanKiri(-Xmove, speed);
      //    for (int diam = 1 ; diam > 0 ; diam++)
      //      robot.move(0, 0, 0, 0);

    }

    void safe1(int16_t Xmove, int16_t speed) { //mode penyelamatan & menuju ke safety zone r1
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("R1 ke Safe1");
      robot.move(0, 0, 0, 0);
      robot.readJarak();
      for (int z = 0; z < 5; z++) { //meluruskan dinding kiri
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
          robot.move(0, 15, 0, 30);
      }
      robot.position(0, 0, 30, 30);
      robot.putarKiri(90, 30);
      robot.putarKiri(90, 30);
      while (DINDING_KIRI)
        robot.scanKiri(30, 45);
      robot.move(0, 0, 0, 30);
      for (int house = 0 ; house < 40 ; house++)
        robot.move(0, -60, 0, 45);
      robot.inverse();
      for (int back = 0 ; back < 5 ; back++)
        robot.scanKanan(Xmove, speed);
      robot.move(0, 0, 0, 0);
    }

















    void r1_r2(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R1 ke R2");
      robot.readJarak();
      while (robot.lantai() != 2)
        robot.scanKanan(Xmove, speed);
      for (count = 0; count < 7 ; count++)
        robot.move(40, 0, 0, speed);
      robot.putarKanan(70, speed);
      for (count = 0; count < 10 ; count++)
        robot.move(40, 0, 0, speed);
      for (count = 0; count < 4 ; count++)
        robot.scanKiri(Xmove, speed);

      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
          robot.move(0, 15, 0, 30);
      }
      while (DINDING_KIRI)
        robot.scanKiri(Xmove, speed);
      robot.putarKiri(98, speed);
      for (int z = 0; z < 8; z++) {
        robot.readJarak();
        robot.pidKanan(0, 30, 10);
        if (min(RIGHT_MIDLE_FRONT, min(RIGHT_MIDLE, RIGHT_MIDLE_REAR)) > 45)
          robot.move(0, 15, 0, 30);
      }
      robot.position(0, 0, 35, 45);
      while (robot.lantai() != 2)
        robot.scanKanan(Xmove, 45);
      while (robot.lantai() != 0)
        robot.scanKanan(Xmove, 45);
      robot.position(0, 0, 0, 120);
      robot.putarKiri(90, speed);

      for (count = 0; count < 4 ; count++)
        robot.scanKanan(Xmove, speed);
      //      robot.FireCounting(FIRE);
      //      masukRuangApi(60, 70, 960);
      //      robot.FireCounting(FIRE);
      //      robot.move(0, 0, 0, 0);
    }

    void safe2(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R2 ke Safe2");
      robot.readJarak();
      for (count = 0; count < 2 ; count++)
        robot.move(0, -40, 0, 45);
      for (count = 0; count < 3 ; count++)
        robot.move(0, 40, 0, 45);
      //        mode penyelamatan
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKanan(0, 30, 10);
        if (min(RIGHT_MIDLE_FRONT, min(RIGHT_MIDLE, RIGHT_MIDLE_REAR)) > 45)
          robot.move(0, 15, 0, 30);
      }
      robot.putarKiri(90, speed);
      for (count = 0; count < 10 ; count++)
        robot.move(40, 0, 0, 45);
      for (count = 0; count < 10 ; count++)
        robot.move(-40, 0, 0, 45);
      robot.putarKiri(90, speed);
      robot.move(0, 0, 0, 0);
    }

    void r2_home(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R2 ke Home");
      robot.readJarak();
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
          robot.move(0, 15, 0, 30);
      }
      for (count = 0; count < 10 ; count++)
        robot.scanKiri(Xmove, speed);
      robot.position(0, 0, 35, 45);
      while (robot.lantai() != 2)
        robot.scanKiri(Xmove, 45);
      for (count = 0; count < 7 ; count++)
        robot.scanKiri(Xmove, 45);
      while (robot.lantai() != 0)
        robot.scanKiri(Xmove, 45);
      while (!DINDING_KANAN)
        robot.scanKiri(Xmove, 45);
      for (count = 0; count < 6 ; count++)
        robot.scanKiri(Xmove, 45);
      while (DINDING_KANAN)
        robot.scanKiri(Xmove, 45);

      robot.position(0, 0, 0, 125);
      while (robot.lantai() != 1)
        robot.scanKiri(Xmove, speed);
      for (count = 0; count < 3 ; count++)
        robot.move(40, 0, 0, speed);
      robot.move(0, 0, 0, 0);
      //      robot.shutdown();
      while (1);
    }

    //============================================================//


    void room2_1(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R2 ke R1");
      robot.move(0, 0, 0, speed);
      robot.readJarak();
      robot.putarKiri(180, speed);
      while (robot.lantai() != 2)
        robot.scanKanan(Xmove, speed);
      for (count = 0; count < 10; count++)
        robot.scanKanan(Xmove, speed);
      while (robot.lantai() != 2)
        robot.scanKanan(Xmove, speed);
      for (count = 0; count < 10; count++)
        robot.scanKiri(Xmove, speed);
      p_menyebar(speed);
    }
    void room1_4(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R1 ke R4");
      robot.putarKiri(180, speed);
      while (robot.lantai() != 2)
        robot.scanKanan(Xmove, speed);
      for (count = 0; count < 5; count++)
        robot.scanKanan(70, 5);
      while (robot.lantai() != 1)
        robot.scanKanan(Xmove, speed);
      for (count = 0; count < 5; count++)
        robot.scanKanan(70, 5);
      while (robot.lantai() != 0)
        robot.scanKanan(Xmove, speed);
      robot.move(Xmove, 0, 0, speed);
      //      robot.putarKiri(90, speed);
      //      for (count = 0; count < 3; count++)
      //        robot.move(40, 0, 0, 40);
      for (count = 0; count < 2; count++)//3
        robot.move(0, -70, 0, 40);
      for (count = 0; count < 2; count++)//3
        robot.move(-40, 0, 0, 40);
      while (robot.lantai() != 2)
        robot.scanKiri(Xmove, speed);
      while (FRONT > 20)
        robot.scanKiri(Xmove, speed);
      p_menyebar(speed);
    }
    void room4_home(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R4 ke Home");
      robot.putarKiri(180, speed);
      //      for (int count = 0; count <= 40; count++)
      //        robot.scanKanan(Xmove, speed);
      while (robot.lantai() != 2)
        robot.scanKanan(Xmove, speed);
      for (int count = 0; count <= 25; count++)//30
        robot.scanKanan(Xmove, speed);
      while (FRONT > 20)
        robot.scanKiri(Xmove, speed);
    }
    void home_1(int16_t Xmove, int16_t speed) {

      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari Home ke R1");
      lcd.setCursor(0, 1); lcd.print("Sesi 2");
      robot.readJarak();
      while (DINDING_KIRI)
        robot.scanKiri(Xmove, speed);
      while (robot.lantai() != 0)
        robot.scanKanan(Xmove, speed);
      for (int count = 0; count < 5; count++)
        robot.scanKanan(Xmove, speed);

      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKanan(0, 30, 10);
        if (min(RIGHT_MIDLE_FRONT, min(RIGHT_MIDLE, RIGHT_MIDLE_REAR)) > 20)
          robot.move(0, 15, 0, 30);
      }

      //      for (int z = 0; z < 5; z++) {
      //        robot.readJarak();
      //        robot.pidKiri(0, 30, 10);
      //        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
      //          robot.move(0, 15, 0, 30);
      //      }
      robot.position(0, 0, 35, speed);

      //      while (1) {
      //        robot.move(0, -40, 0, 45);
      //        robot.readJarak();
      //        if (FRONT >= 25)
      //          break;
      //      }
      //      robot.position(0, 0, 0, 120);
      //      for (int count = 0; count < 3; count++)
      //        robot.move(0, -30, 0, 30);
      //      for (int count = 0; count < 7; count++)
      //        robot.move(30, 0, 0, 30);
      //      for (int count = 0; count < 10; count++)
      //        robot.scanKanan(Xmove, speed);
      //      robot.putarKiri(90, speed);
      //      robot.putarKiri(90, speed);
      //      for (int count = 0; count < 15; count++)
      //        robot.scanKanan(Xmove, speed);
      //      while (robot.lantai() != 0)
      //        robot.scanKanan(Xmove, speed);
      //      robot.move(0, 0, 0, 0);
      //      for (int count = 0; count < 3; count++)
      //        robot.move(0, -30, 0, 30);
      //      while (robot.lantai() != 2)
      //        robot.scanKiri(Xmove, speed);
      //      for (count = 0; count < 10; count++)
      //        robot.scanKiri(Xmove, speed);
      //      p_menyebar(speed);
    }
    void room1_3(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R1 ke R3");
      lcd.setCursor(0, 1); lcd.print("Sesi 2");
      robot.readJarak();
      robot.putarKiri(180, speed);
      while (robot.lantai() != 2)
        robot.scanKanan(Xmove, speed);
      while (1) {
        robot.move(70, 0, 0, 5);
        robot.readJarak();
        if (FRONT <= 25)
          break;
      }
      robot.putarKanan(90, speed);
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
          robot.move(0, 15, 0, 30);
      }
      while (robot.lantai() != 2)
        robot.scanKiri(Xmove, speed);
      for (count = 0; count < 10; count++)
        robot.scanKiri(Xmove, speed);
      p_menyebar(speed);
    }
    void room3_4(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R3 ke R4");
      lcd.setCursor(0, 1); lcd.print("Sesi 2");
      robot.readJarak();
      robot.putarKiri(180, speed);
      //      while (FRONT < 100){
      //        robot.scanKanan(Xmove, speed);
      //        robot.readJarak();
      //        }
      while (robot.lantai() != 2)//lantai()!=1
        robot.scanKanan(Xmove, speed);
      for (int z = 0; z < 5; z++)
        robot.scanKanan(Xmove, speed);
      while (robot.lantai() != 1)//lantai()!=1
        robot.scanKanan(Xmove, speed);
      for (int z = 0; z < 10; z++) {
        robot.readJarak();
        robot.pidKanan(0, 30, 10);
        if (min(RIGHT_MIDLE_FRONT, min(RIGHT_MIDLE, RIGHT_MIDLE_REAR)) > 25)
          robot.move(0, 15, 0, 30);
      }
      for (int z = 0; z < 10; z++)
        robot.move(40, 0, 0, 5);
      while (robot.lantai() != 0)
        robot.move(40, 0, 0, 5);
      for (int z = 0; z < 2; z++)
        robot.move(0, -40, 0, 5);
      for (int z = 0; z < 2; z++)
        robot.move(-40, 0, 0, 5);
      while (robot.lantai() != 2)
        robot.scanKiri(Xmove, speed);
      while (FRONT > 20)
        robot.scanKiri(Xmove, speed);
      p_menyebar(speed);
    }
    void room4_2(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R4 ke R2");
      lcd.setCursor(0, 1); lcd.print("Sesi 2");
      robot.readJarak();
      robot.putarKiri(180, speed);
      while (robot.lantai() != 2)
        robot.scanKanan(Xmove, speed);
      while (1) {
        robot.move(70, 0, 0, 5);
        robot.readJarak();
        if (FRONT <= 20)
          break;
      }
      robot.putarKanan(90, speed);
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
          robot.move(0, 15, 0, 30);
      }

      //      while (1) {
      //        robot.move(70, 0, 0, 5);
      //        robot.readJarak();
      //        if (LEFT_MIDLE >= 50)
      //          break;
      //      }
      while (robot.lantai() != 1)//0
        robot.scanKiri(Xmove, speed);
      while (!DINDING_KANAN)//0
        robot.scanKiri(Xmove, speed);
      //      for (count = 0; count < 1; count++)
      //        robot.scanKiri(Xmove, speed);
      //      while (robot.lantai() != 0)//0
      //        robot.scanKiri(Xmove, speed);
      for (count = 0; count < 10; count++)
        robot.scanKanan(Xmove, speed);
      while (robot.lantai() != 2)

        robot.scanKanan(Xmove, speed);
      for (count = 0; count < 15; count++)
        robot.scanKiri(Xmove, speed);
      p_menyebar(speed);
    }
    void room2_home(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R2 ke Home");
      lcd.setCursor(0, 1); lcd.print("Sesi 2");
      robot.readJarak();
      robot.putarKiri(180, speed);
      while (robot.lantai() != 2)
        robot.scanKanan(Xmove, speed);
      for (int count = 0; count < 5; count++)
        robot.move(0, -30, 0, 30);
      while (1) {
        robot.scanKiri(Xmove, speed);
        robot.readJarak();
        if (RIGHT_MIDLE >= 70)
          break;
      }
      for (int count = 0; count < 10; count++)
        robot.scanKiri(Xmove, speed);
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) < 25)
          robot.move(0, 15, 0, 30);
      }
      while (1) {
        robot.move(40, 0, 0, 5);
        robot.readJarak();
        if (LEFT_MIDLE >= 50)
          break;
      }
      //      for (int z = 0; z < 3; z++)
      //      robot.move(0, -40, 0, 45);
      robot.putarKanan(80, speed);
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) < 45)
          robot.move(0, 15, 0, 30);
      }
      robot.position(0, 0, 35, speed);
      for (int z = 0; z < 10; z++)
        robot.move(0, -40, 0, 45);
      while (1) {
        robot.move(0, -40, 0, 45);
        robot.readJarak();
        if (LEFT_MIDLE  <= 25 && FRONT > 50 )
          break;
      }
      robot.position(0, 0, 0, 120);
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) < 25)
          robot.move(0, 15, 0, 30);
      }
      while (1) {
        robot.move(-40, 0, 0, 45);
        robot.readJarak();
        if (REAR < 20 )
          break;
      }
    }
    void home_1a(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari home ke R1 api");
      lcd.setCursor(0, 1); lcd.print("Sesi 3__" + String(FireCount));
      robot.readJarak();
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKanan(0, 30, 10);
        if (min(RIGHT_MIDLE_FRONT, min(RIGHT_MIDLE, RIGHT_MIDLE_REAR)) > 20)
          robot.move(0, 15, 0, 30);
      }
      robot.putarKanan(90, speed);
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
          robot.move(0, 15, 0, 30);
      }
      robot.position(0, 0, 35, speed);

      while (1) {
        robot.move(0, -40, 0, 45);
        robot.readJarak();
        if (FRONT >= 25)
          break;
      }
      robot.position(0, 0, 0, 120);
      for (int count = 0; count < 3; count++)
        robot.move(0, -30, 0, 30);
      for (int count = 0; count < 7; count++)
        robot.move(30, 0, 0, 30);
      for (int count = 0; count < 10; count++)
        robot.scanKanan(Xmove, speed);
      robot.putarKiri(90, speed);
      robot.putarKiri(90, speed);
      for (int count = 0; count < 15; count++)
        robot.scanKanan(Xmove, speed);
      while (robot.lantai() != 0)
        robot.scanKanan(Xmove, speed);
      robot.move(0, 0, 0, 0);
      for (int count = 0; count < 3; count++)
        robot.move(0, -30, 0, 30);
      while (robot.lantai() != 2)
        robot.scanKiri(Xmove, speed);
      for (int count = 0; count < 10; count++)
        robot.scanKiri(Xmove, speed);
      robot.FireCounting(FIRE);
      masukRuangApi(60, 70, 915);
      robot.FireCounting(FIRE);
    }
    void room1_2a(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R1 ke R2 api");
      lcd.setCursor(0, 1); lcd.print("Sesi 3__" + String(FireCount));
      robot.readJarak();
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
          robot.move(0, 15, 0, 30);
      }
      robot.putarKanan(180, speed);
      while (1) {
        robot.move(0, 40, 0, 45);
        robot.readJarak();
        if (LEFT_MIDLE  <= 25)
          break;
      }
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKanan(0, 30, 10);
        if (min(RIGHT_MIDLE_FRONT, min(RIGHT_MIDLE, RIGHT_MIDLE_REAR)) < 25)
          robot.move(0, -15, 0, 30);
      }
      while (robot.lantai() != 2)
        robot.scanKanan(Xmove, speed);
      for (int count = 0; count < 3; count++)
        robot.scanKiri(Xmove, speed);
      while (robot.lantai() != 2)
        robot.scanKiri(Xmove, speed);
      robot.FireCounting(FIRE);
      masukRuangApi(60, 60, 910);
      robot.FireCounting(FIRE);
    }
    void room2_3a(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R2 ke R3 api");
      lcd.setCursor(0, 1); lcd.print("Sesi 3__" + String(FireCount));
      robot.readJarak();
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
          robot.move(0, 15, 0, 30);
      }
      while (1) {
        robot.move(0, -40, 0, 45);
        robot.readJarak();
        if (LEFT_MIDLE  <= 10)
          break;
      }
      robot.putarKiri(180, speed);
      for (int count = 0; count < 3; count++)
        robot.scanKiri(Xmove, speed);
      while (robot.lantai() != 2)
        robot.scanKiri(Xmove, speed);
      for (int count = 0; count < 10; count++)
        robot.scanKiri(Xmove, speed);
      while (robot.lantai() != 2)
        robot.scanKiri(Xmove, speed);
      robot.FireCounting(FIRE);
      masukRuangApi(60, 60, 910);
      robot.FireCounting(FIRE);
    }
    void room3_home(int16_t Xmove, int16_t speed) {
      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
      lcd.print("Dari R2 ke R3 api");
      lcd.setCursor(0, 1); lcd.print("Sesi 3__" + String(FireCount));
      robot.readJarak();
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
          robot.move(0, 15, 0, 30);
      }
      while (1) {
        robot.move(0, -40, 0, 45);
        robot.readJarak();
        if (LEFT_MIDLE  <= 10)
          break;
      }
      robot.putarKiri(180, speed);
      while (robot.lantai() != 2)
        robot.scanKanan(Xmove, speed);
      for (int count = 0; count < 3; count++)
        robot.scanKanan(Xmove, speed);
      while (robot.lantai() != 1)
        robot.scanKanan(Xmove, speed);
      for (int z = 0; z < 10; z++) {
        robot.readJarak();
        robot.pidKanan(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) < 45)
          robot.move(0, 15, 0, 30);
      }
      while (1) {
        robot.move(40, 0, 0, 5);
        robot.readJarak();
        if (RIGHT_MIDLE >= 25)
          break;
      }
      for (int z = 0; z < 5; z++) {
        robot.move(40, 0, 0, 5);
      }
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) < 45)
          robot.move(0, 15, 0, 30);
      }
      robot.position(0, 0, 35, speed);
      for (int z = 0; z < 10; z++)
        robot.move(0, -40, 0, 45);
      while (1) {
        robot.move(0, -40, 0, 45);
        robot.readJarak();
        if (LEFT_MIDLE  <= 25 && FRONT > 50 )
          break;
      }
      robot.position(0, 0, 0, 120);
      for (int z = 0; z < 5; z++) {
        robot.readJarak();
        robot.pidKiri(0, 30, 10);
        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) < 25)
          robot.move(0, 15, 0, 30);
      }
      while (1) {
        robot.move(-40, 0, 0, 45);
        robot.readJarak();
        if (REAR < 20 )
          break;
      }
    }
    //    void home_3a(int16_t Xmove, int16_t speed) {
    //      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
    //      lcd.print("Dari Home ke R3 api");
    //      lcd.setCursor(0, 1); lcd.print("Sesi 3__" + String(FireCount));
    //      robot.readJarak();
    //      for (int z = 0; z < 5; z++) {
    //        robot.readJarak();
    //        robot.pidKanan(0, 30, 10);
    //        if (min(RIGHT_MIDLE_FRONT, min(RIGHT_MIDLE, RIGHT_MIDLE_REAR)) > 20)
    //          robot.move(0, 15, 0, 30);
    //      }
    //      robot.putarKanan(90, speed);
    //      for (int z = 0; z < 5; z++) {
    //        robot.readJarak();
    //        robot.pidKiri(0, 30, 10);
    //        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
    //          robot.move(0, 15, 0, 30);
    //      }
    //      robot.position(0, 0, 35, speed);
    //
    //      while (1) {
    //        robot.move(0, -40, 0, 45);
    //        robot.readJarak();
    //        if (FRONT >= 25)
    //          break;
    //      }
    //      robot.position(0, 0, 0, 120);
    //      for (int count = 0; count < 3; count++)
    //        robot.move(0, -30, 0, 30);
    //      for (int count = 0; count < 7; count++)
    //        robot.move(30, 0, 0, 30);
    //
    //      while (robot.lantai() != 2)
    //        robot.scanKanan(Xmove, speed);
    //
    //      robot.FireCounting(FIRE);
    //      masukRuangApi(40, 40);
    //      robot.FireCounting(FIRE);
    //
    //    }
    //    void room3_2a(int16_t Xmove, int16_t speed) {
    //      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
    //      lcd.print("Dari R3 ke R2 api");
    //      lcd.setCursor(0, 1); lcd.print("Sesi 3__" + String(FireCount));
    //      robot.readJarak();
    //      robot.putarKanan(180, speed);
    //      while (FRONT < 100) {
    //        robot.readJarak();
    //        robot.scanKanan(Xmove, speed);
    //      }
    //      robot.putarKiri(90, speed);
    //
    //      //      while (robot.lantai() != 2)
    //      //        robot.scanKanan(Xmove, speed);
    //      while (FRONT < 100) {
    //        robot.readJarak();
    //        robot.scanKanan(Xmove, speed);
    //      }
    //      robot.putarKanan(90, speed);
    //      for (int c = 0; c < 2; c++)
    //        robot.move(30, 0, 0, 30);
    //      robot.FireCounting(FIRE);
    //      masukRuangApi(40, 40);
    //      robot.FireCounting(FIRE);
    //    }

    //    void room2_1a(int16_t Xmove, int16_t speed) {
    //      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
    //      lcd.print("Dari R2 ke R1 api");
    //      lcd.setCursor(0, 1); lcd.print("Sesi 3__" + String(FireCount));
    //      robot.readJarak();
    //      robot.putarKanan(180, speed);
    //      while (FRONT < 100)
    //        robot.scanKanan(Xmove, speed);
    //      while (robot.lantai() != 2)
    //        robot.scanKanan(Xmove, speed);
    //
    //      robot.FireCounting(FIRE);
    //      masukRuangApi(40, 40);
    //      robot.FireCounting(FIRE);
    //    }
    //    void room1_homea(int16_t Xmove, int16_t speed) {
    //      lcd.clear(); lcd.backlight(); lcd.setCursor(0, 0);
    //      lcd.print("Dari R1 ke home api");
    //      lcd.setCursor(0, 1); lcd.print("Sesi 3__" + String(FireCount));
    //      robot.readJarak();
    //      robot.putarKanan(180, speed);
    //      while (robot.lantai() != 2)
    //        robot.scanKanan(Xmove, speed);
    //      for (int count = 0; count < 10; count++)
    //        robot.move(30, 0, 0, 30);
    //      robot.putarKanan(90, speed);
    //      for (int z = 0; z < 10; z++) {
    //        robot.readJarak();
    //        robot.pidKiri(0, 30, 10);
    //        if (min(LEFT_MIDLE_FRONT, min(LEFT_MIDLE, LEFT_MIDLE_REAR)) > 45)
    //          robot.move(0, 15, 0, 30);
    //      }
    //      for (int z = 0; z < 20; z++) {
    //        robot.scanKiri(Xmove, speed);
    //      }
    //      robot.putarKanan(90, speed);
    //      robot.position(0, 0, 35, speed);
    //      while (1) {
    //        robot.move(0, -40, 0, 40);
    //        robot.readJarak();
    //        if (FRONT >= 25 && LEFT_MIDLE < 10)
    //          break;
    //      }
    //
    //    }






    // === === === === PADAMKAN API === === === ===//
    bool padamkanApi(int16_t Xmove, int16_t speed, int16_t batasApi_Code = 912, bool apiPadam = true) {
      bool r;
      int16_t sudutApi;
      uint8_t n;
      float lastR;

      robot.move(0, 0, 0, speed);
      robot.position(0, 0, 25, 100);
      while (FIRE || !apiPadam) {
        robot.Kp = 30; robot.Kd = 10;
        vector api = sensor.radarApi(apiRepeat);
        sudutApi += robot.PID(api.sudut, 90, &lastR) / 100;
        lcd.clear();
        lcd.setCursor(0, 0);  lcd.print(String(api.sudut) + "  ");
        lcd.setCursor(0, 1);  lcd.print(String(api.resultan) + "  ");
        sudutApi = constrain(sudutApi, -20, 20);

        if (n > 8)  {
          pumpOn;
          delay(500);
          apiPadam = true;
          FIRE = false;
          playing = false;
          delay(100);
        }
        else {
          pumpOff;
        }

        if (abs(sudutApi) > 10) {
          if (r == true) {
            robot.rotation(0, 0, 0, speed * 8);
            r = false;
          }
          robot.putar(sudutApi, speed);
          sudutApi = 0; n = 0;
        }
        else {
          robot.rotation(0, 0, sudutApi, speed * 10);
          r = true; n++;
        }

        if (!FIRE && !apiPadam) {
          robot.rotation(0, 0, 0, speed * 8);
          //          robot.position(0, 0, 0, speed * 8);
          r = false;
          sudutApi = 0; n = 0;
          vector temp = robot.trackingApi(Xmove, speed);
          while (abs(temp.sudut - 90) > 5 && temp.resultan < batasApi_Code)
            temp = robot.trackingApi(Xmove, speed);
          robot.move(0, 0, 0, speed);
          //          robot.position(0, 0, 40, 100);
        }
      }
      pumpOff; r = false;
      robot.rotation(0, 0, 0, speed * 8);
      robot.position(0, 0, 0, speed * 8);
      return apiPadam;
    }

    uint8_t door1     = unknown;
    uint8_t door4     = unknown;
    uint8_t boneka    = unknown;
    uint8_t nowRoom   = unknown;
    uint8_t nextRoom  = unknown;
    uint8_t startRoom = unknown;

  private :
    uint8_t count;
    int16_t logger[4][30];
    const int16_t dataRuang[2][5] = {
      {95, 122, 87, 51}, \
      {122, 95, 72, 72}
    };
};
//TAHAP

#endif
