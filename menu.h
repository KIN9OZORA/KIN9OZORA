#include <EEPROM.h>

#ifndef MENU
#define MENU

#include <Arduino.h>
#include <EEPROM.h>
#include <stdio.h>
#include "robot.h"
#include "navigasi.h"
#include "variable.h"

navigasi navigasi;

void play(int16_t Xmove = 40, int16_t speed = 5, uint8_t start = 0) {
  playing = true;
  POWER_LED_ON;
  lcd.clear();
  ruang value = navigasi.keluarRuangStart(25, 15);

  if (value.room == 1)
    navigasi.door1 = value.door;
  if (start != 0)
    value.room = start;
  navigasi.startRoom = value.room;

  lcd.clear();
  lcd.print("Ruang " + String(navigasi.startRoom));
  if (navigasi.startRoom == 1)
    if (navigasi.door1 == doorA)        lcd.print("A");
    else if (navigasi.door1 == doorB)   lcd.print("B");
    else if (navigasi.door1 == doorC)   lcd.print("C");
    else                                lcd.print("-");
  else if (navigasi.startRoom == 4)
    if (navigasi.door4 == doorA)        lcd.print("A");
    else if (navigasi.door4 == doorB)   lcd.print("B");
    else                                lcd.print("-");

  switch (value.room) {
    case 1 :
      if (value.door == doorC)
        navigasi.room(1, 4, Xmove, speed);
      else {
        navigasi.room(1, 2, Xmove, speed);
        navigasi.room(2, 3, Xmove, speed);
        navigasi.room(3, 4, Xmove, speed);
      }

      if (navigasi.door4 == doorA || navigasi.boneka == bonekaA)
        while (1) {
          navigasi.room(4, 2, Xmove, speed);
          navigasi.room(2, 3, Xmove, speed);
          navigasi.room(3, 4, Xmove, speed);
        }
      else
        while (1) {
          navigasi.room(4, 3, Xmove, speed);
          navigasi.room(3, 2, Xmove, speed);
          navigasi.room(2, 3, Xmove, speed);
          navigasi.room(3, 4, Xmove, speed);
        }
      break;

    case 2 :
      navigasi.room(2, 1, Xmove, speed);
      navigasi.room(1, 3, Xmove, speed);
      navigasi.room(3, 4, Xmove, speed);

      if (navigasi.door4 == doorA || navigasi.boneka == bonekaA)
        while (1) {
          navigasi.room(4, 1, Xmove, speed);
          navigasi.room(1, 3, Xmove, speed);
          navigasi.room(3, 4, Xmove, speed);
        }
      else
        while (1) {
          navigasi.room(4, 3, Xmove, speed);
          navigasi.room(3, 1, Xmove, speed);
          navigasi.room(1, 3, Xmove, speed);
          navigasi.room(3, 4, Xmove, speed);
        }
      break;

    case 3 :
      navigasi.room(3, 2, Xmove, speed);
      navigasi.room(2, 1, Xmove, speed);
      navigasi.room(1, 4, Xmove, speed);

      while (1) {
        navigasi.room(4, 2, Xmove, speed);
        navigasi.room(2, 1, Xmove, speed);
        navigasi.room(1, 4, Xmove, speed);
      }
      break;

    case 4 :
      navigasi.fromRoom4(Xmove, speed);
      if (navigasi.door4 == doorA || navigasi.boneka == bonekaA) {
        navigasi.room(4, 2, Xmove, speed, true, 2);
        while (1) {
          navigasi.room(2, 1, Xmove, speed);
          navigasi.room(1, 3, Xmove, speed);
          navigasi.room(3, 2, Xmove, speed);
        }
      }
      else {
        navigasi.room(4, 3, Xmove, speed, true, 2);
        while (1) {
          navigasi.room(3, 2, Xmove, speed);
          navigasi.room(2, 1, Xmove, speed);
          navigasi.room(1, 3, Xmove, speed);
        }
      }
      break;

    default :
      POWER_LED_OFF;
      break;
  }
}

void save() {
  lcd.clear(); lcd.print("Saving...");
  for (int addr = 0; addr < maxAddr; addr++)
    EEPROM.update(addr, data[addr]);
  delay(200);
}

void load() {
  for (int addr = 0; addr < maxAddr; addr++)
    data[addr] = EEPROM.read(addr);
}

void change(uint8_t value) {
  menu = value;
  delay(200);
  lcd.clear();
}

bool run = false;
void mainMenu() {
  vector api;
  switch (menu) {
    case 0 :
//      if (sensor.sound(1000)) {
//        run = false;
//        robot.move(0, 0, 0, 1);
//      }
//      else
//        run = true;
/*  switch (robot.key()) {
        case 2 : navigasi.home_r1(70,5);navigasi.safe1(70,5);
                 navigasi.r1_r2  (70,5);navigasi.safe2(70,5);
                 navigasi.r2_home(70,5);
                 break;
          }*/

      while (run)
        play(navigasiMove, navigasiSpeed);
        
//      temp[0] = sensor.sound(500);
      api = sensor.radarApi(apiRepeat);
      api.resultan = api.resultan > 999 ? 0 : api.resultan;

      lcd.setCursor(0, 0);  lcd.print(temp[0] ? "Ready..! " : "OK...    ");                 lcd.print("   ");
      lcd.setCursor(10, 0); lcd.print("F" + String((int)sensorLantai));                     lcd.print("   ");

      lcd.setCursor(0, 1);  lcd.print((FIRE ? "A" : "a") + String(api.sudut - 90));  lcd.print("   ");
      lcd.setCursor(4, 1);  lcd.print("-" + String(api.resultan));                          lcd.print("   ");
      lcd.setCursor(10, 1); lcd.print(String((float)(legStatus[0]) / 10) + "V");

      if (KILL_MTR && off == false) {
        off = true;
        robot.shutdown();
        while (KILL_MTR);
      }
      else if (!KILL_MTR && off == true) {
        off = false;
        robot.begin();

      }
      
      if (robot.key() > 1) change(1);
      if (robot.key() == 1) {
        pumpOn; robot.move(0, 0, 0, 5);
        Serial.println(String(legData[0]));
      }
      else {
        pumpOff; robot.move(0, 0, 0, 5);
      }
      break;

    case 1 :
//      lcd.setCursor(0, 0); lcd.print(">Cal.Lantai     ");
//      lcd.setCursor(0, 1); lcd.print(" Cal.Batas api  ");
      lcd.setCursor(0, 0); lcd.print(" ");
      navigasi.home_r1(70,0);
      navigasi.safe1(30,0);
      navigasi.r1_r2  (30,0);navigasi.safe2(30,0);
      navigasi.r2_home(30,0); 
      
      switch (1) {
        case 1 : change(0); break;
        case 2 : change(0); break;
        case 3 : change(1); break;
        case 4 : change(3); break;
      }
//      switch (robot.key()) {
//        case 1 : temp[0] = lantaiOff * 10; temp[2] = 0; change(7); break;
//        case 2 : change(0); break;
//        case 3 : change(6); break;
//        case 4 : change(2); break;
//      }
        temp[0] = constrain(temp[0], 1, 4);    
        break;

    case 2 :
      lcd.setCursor(0, 0); lcd.print(" Cal.Lantai     ");
      lcd.setCursor(0, 1); lcd.print(">Cal.Batas api  ");
      switch (robot.key()) {
        case 1 : temp[3] = apiRepeat; change(8); break;
        case 2 : change(0); break;
        case 3 : change(1); break;
        case 4 : change(3); break;
      }
      break;

    case 3 :
      lcd.setCursor(0, 0); lcd.print(">Cal.Sound freq ");
      lcd.setCursor(0, 1); lcd.print(" Set.Speed      ");
      switch (robot.key()) {
        case 2 : change(0); break;
        case 3 : change(2); break;
        case 4 : change(4); break;
      }
      break;

    case 4 :
      lcd.setCursor(0, 0); lcd.print(" Cal.Sound freq ");
      lcd.setCursor(0, 1); lcd.print(">Set.Speed      ");
      switch (robot.key()) {
        case 1 : temp[0] = roomSpeed;
          temp[1] = navigasiSpeed; change(10); break;
        case 2 : change(0); break;
        case 3 : change(3); break;
        case 4 : change(5); break;
      }
      break;

    case 5 :
      lcd.setCursor(0, 0); lcd.print(">Set.Step       ");
      lcd.setCursor(0, 1); lcd.print(" Running test   ");
      switch (robot.key()) {
        case 1 : temp[0] = roomMove;
          temp[1] = navigasiMove; change(14); break;
        case 2 : change(0); break;
        case 3 : change(4); break;
        case 4 : change(6); break;
      }
      break;

    case 6 :
      lcd.setCursor(0, 0); lcd.print(" Set.Step  ");
      lcd.setCursor(0, 1); lcd.print(">Running test   ");
      switch (robot.key()) {
        case 1 : temp[0] = 1; change(18); break;
        case 2 : change(0); break;
        case 3 : change(5); break;
        case 4 : change(1); break;
      }
      break;

    case 7 :
      lcd.setCursor(0, 0);  lcd.print("A" + String(sensor1) + "  ");
      lcd.setCursor(5, 0);  lcd.print("B" + String(sensor2) + "  ");
      lcd.setCursor(10, 0); lcd.print("Off" + String(temp[0]) + "  ");
      lcd.setCursor(0, 1);  lcd.print(String(temp[1]) + "  ");
      lcd.setCursor(8, 1);  lcd.print("Max" + String(temp[2]) + "  ");

      robot.move(0, 0, 0, 0);
      temp[1] = pow(sensorLantai, 2);
      temp[2] = max(temp[2], temp[1]);

      // === === === SETTING SENSOR GARIS === === === //
      switch (robot.key()) {
        case 1 :
          delay(200); lcd.clear();
          lcd.setCursor(0, 0); lcd.print("3.Save to grey   ");
          lcd.setCursor(0, 1); lcd.print("4.Save to black  ");
          data[23] = temp[0] / 10;
          while (robot.key() < 3);
          if (robot.key() == 3) {
            data[0] = (temp[2] >> 0)  & 0xFF;
            data[1] = (temp[2] >> 8)  & 0xFF;
            data[2] = (temp[2] >> 16) & 0xFF;
            data[3] = (temp[2] >> 32) & 0xFF;
          }
          else {
            data[4] = (temp[2] >> 0)  & 0xFF;
            data[5] = (temp[2] >> 8)  & 0xFF;
            data[6] = (temp[2] >> 16) & 0xFF;
            data[7] = (temp[2] >> 32) & 0xFF;
          }
          change(1);  save(); break;
        case 2: change(1); break;
        case 3 :
          if (temp[0] >= 990)  temp[0] = 0;
          else                  temp[0] += 5;
          delay(200);
          break;
        case 4:
          if (temp[0] <= 0)     temp[0] = 990;
          else                  temp[0] -= 5;
          delay(200);
          break;
      }
      break;

    // === === === SETTING SENSOR API === === === //
    case 8 :
      lcd.setCursor(0, 0);  lcd.print("A" + String(temp[0]) + "  ");
      lcd.setCursor(9, 0);  lcd.print("R" + String(temp[1]) + "  ");
      lcd.setCursor(0, 1);  lcd.print("Ref" + String(temp[2]) + "  ");
      lcd.setCursor(9, 1);  lcd.print("Rpt" + String(temp[3]) + "  ");

      api = sensor.radarApi(temp[3]);
      temp[0] = api.sudut - 90;
      temp[1] = api.resultan;
      temp[2] = (temp[2] + api.resultan) / 2;

      switch (robot.key()) {
        case 1 :
          data[8]  = (temp[2] >> 0)  & 0xFF;
          data[9]  = (temp[2] >> 8)  & 0xFF;
          data[10] = (temp[2] >> 16) & 0xFF;
          data[11] = (temp[2] >> 32) & 0xFF;
          data[12] = (temp[3] >> 0)  & 0xFF;
          change(2);  save(); break;
        case 2: change(2); break;
        case 3 :
          if (temp[3] >= 50)    temp[3] = 0;
          else                  temp[3] ++;
          delay(200);
          break;
        case 4:
          if (temp[3] <= 0)     temp[3] = 50;
          else                  temp[3] --;
          delay(200);
          break;
      }
      break;

    case 10 :
      lcd.setCursor(0, 0);  lcd.print(">Ruang  " + String(roomSpeed)     + "     ");
      lcd.setCursor(0, 1);  lcd.print(" Lorong " + String(navigasiSpeed) + "     ");

      switch (robot.key()) {
        case 1 : temp[0] = roomSpeed; change(12); break;
        case 2 : change(4); break;
        case 3 : change(11); break;
        case 4 : change(11); break;
      }
      break;

    case 11 :
      lcd.setCursor(0, 0);  lcd.print(" Ruang  " + String(roomSpeed)     + "     ");
      lcd.setCursor(0, 1);  lcd.print(">Lorong " + String(navigasiSpeed) + "     ");

      switch (robot.key()) {
        case 1 : temp[1] = navigasiSpeed; change(13); break;
        case 2 : change(4); break;
        case 3 : change(10); break;
        case 4 : change(10); break;
      }
      break;

    // === === === SETTING ROOM SPEED === === === //
    case 12 :
      lcd.setCursor(0, 0);  lcd.print(">Ruang  " + String(temp[0]) + "~   ");
      lcd.setCursor(0, 1);  lcd.print(" Lorong " + String(temp[1]) + "    ");

      switch (robot.key()) {
        case 1 : change(10); roomSpeed = temp[0]; save(); break;
        case 2 : change(10); break;
        case 3 :
          if (temp[0] >= 200)   temp[0] = 0;
          else                  temp[0] ++;
          delay(200);
          break;
        case 4:
          if (temp[0] <= 0)     temp[0] = 200;
          else                  temp[0] --;
          delay(200);
          break;
      }
      break;

    // === === === SETTING NAVIGASI SPEED === === === //
    case 13 :
      lcd.setCursor(0, 0);  lcd.print(" Ruang  " + String(temp[0]) + "    ");
      lcd.setCursor(0, 1);  lcd.print(">Lorong " + String(temp[1]) + "~   ");

      switch (robot.key()) {
        case 1 : change(11); navigasiSpeed = temp[1]; save(); break;
        case 2 : change(11); break;
        case 3 :
          if (temp[1] >= 200)   temp[1] = 0;
          else                  temp[1] ++;
          delay(200);
          break;
        case 4:
          if (temp[1] <= 0)     temp[1] = 200;
          else                  temp[1] --;
          delay(200);
          break;
      }
      break;

    case 14 :
      lcd.setCursor(0, 0);  lcd.print(">Ruang  " + String(roomMove)     + "      ");
      lcd.setCursor(0, 1);  lcd.print(" Lorong " + String(navigasiMove) + "      ");

      switch (robot.key()) {
        case 1 : temp[0] = roomMove; change(16); break;
        case 2 : change(5); break;
        case 3 : change(15); break;
        case 4 : change(15); break;
      }
      break;

    case 15 :
      lcd.setCursor(0, 0);  lcd.print(" Ruang  " + String(data[21]) + "  ");
      lcd.setCursor(0, 1);  lcd.print(">Lorong " + String(data[22]) + "  ");

      switch (robot.key()) {
        case 1 : temp[1] = navigasiMove; change(17); break;
        case 2 : change(5); break;
        case 3 : change(14); break;
        case 4 : change(14); break;
      }
      break;

    // === === === SETTING ROOM MOVE === === === //
    case 16 :
      lcd.setCursor(0, 0);  lcd.print(">Ruang  " + String(temp[0]) + "~    ");
      lcd.setCursor(0, 1);  lcd.print(" Lorong " + String(temp[1]) + "     ");

      switch (robot.key()) {
        case 1 : change(14); roomMove = temp[0]; save(); break;
        case 2 : change(14); break;
        case 3 :
          if (temp[0] >= 200)   temp[0] = 0;
          else                  temp[0] ++;
          delay(200);
          break;
        case 4:
          if (temp[0] <= 0)     temp[0] = 200;
          else                  temp[0] --;
          delay(200);
          break;
      }
      break;

    // === === === SETTING NAVIGASI MOVE === === === //
    case 17 :
      lcd.setCursor(0, 0);  lcd.print(" Ruang  " + String(temp[0]) + "     ");
      lcd.setCursor(0, 1);  lcd.print(">Lorong " + String(temp[1]) + "~    ");

      switch (robot.key()) {
        case 1 : change(15); navigasiMove = temp[1]; save(); break;
        case 2 : change(15); break;
        case 3 :
          if (temp[1] >= 200)   temp[1] = 0;
          else                  temp[1] ++;
          delay(200);
          break;
        case 4:
          if (temp[1] <= 0)     temp[1] = 200;
          else                  temp[1] --;
          delay(200);
          break;
      }
      break;

      break;
      case 18 :
      lcd.setCursor(0, 0);  lcd.print(">Keluar start    ");
      lcd.setCursor(0, 1);  lcd.print(" Navigasi start " + String(temp[0]) + "   ");

      switch (robot.key()) {
       // case 1 : navigasi.keluarRuangStart(roomMove, roomSpeed);
        case 1 : change(24); break;
        case 2 : change(6); break;
        case 3 : change(23); break;
        case 4 : change(19); break;
      }
      break;

    case 19 :
      lcd.setCursor(0, 0);  lcd.print(" Keluar start    ");
      lcd.setCursor(0, 1);  lcd.print(">Navigasi " + String(temp[0]) + "    ");

      switch (robot.key()) {
        case 1 : change(20); break;
        case 2 : change(6); break;
        case 3 : change(18); break;
        case 4 : change(21); break;
      }
      temp[0] = constrain(temp[0], 1, 4);
      break;

    case 20 :
      lcd.setCursor(0, 0);  lcd.print(" Keluar start    ");
      lcd.setCursor(0, 1);  lcd.print(">Navigasi " + String(temp[0]) + "    ");

      switch (robot.key()) {
        case 1 : play(navigasiMove, navigasiSpeed, temp[0]); change(19); break;
        case 2 : change(19); break;
        case 3 : temp[0]++; delay(200); break;
        case 4 : temp[0]--; delay(200); break;
      }
      temp[0] = constrain(temp[0], 1, 4);
      break;

     case 21 :
      lcd.setCursor(0, 0);  lcd.print(" Navigasi " + String(temp[0]) + "    ");
      lcd.setCursor(0, 1);  lcd.print(">Sesi 1 KRI2021");

      switch (robot.key()) {
        case 1 : navigasi.home_r1(70,5);navigasi.safe1(70,5);
                 navigasi.r1_r2(70,5);navigasi.safe2(70,5);
                 navigasi.r2_home(70,5);
                 break;
        case 2 : change(6); break;
        case 3 : change(19); break;
        case 4 : change(22); break;
      }
      temp[0] = constrain(temp[0], 1, 4);
      break;

   case 22 :
      lcd.setCursor(0, 0);  lcd.print(" Sesi 1 KRI2020");
      lcd.setCursor(0, 1);  lcd.print(">Sesi 2 KRI2020");

      switch (robot.key()) {
        case 1 : navigasi.home_1(70,5);//navigasi.room1_3(70,5);navigasi.room3_4(70,5);navigasi.room4_2(70,5);navigasi.room2_home(70,5);
                break;
        case 2 : change(6); break;
        case 3 : change(21); break;
        case 4 : change(23); break;
      }
      temp[0] = constrain(temp[0], 1, 4);
      break;

   case 23 :
      lcd.setCursor(0, 0);  lcd.print(" Sesi 2 KRI2020");
      lcd.setCursor(0, 1);  lcd.print(">Sesi 3 KRI2020");

      switch (robot.key()) {
        //case 1 : 
        case 1 : navigasi.home_1a(70,5);navigasi.room1_2a(70,5);navigasi.room2_3a(70,5);navigasi.room3_home(70,5);
                break;
//        case 1 : break;         
        case 2 : change(6); break;
        case 3 : change(22); break;
        case 4 : change(18); break;
      }
      temp[0] = constrain(temp[0], 1, 4);
      break;
   case 24 :
      lcd.setCursor(0, 0);  lcd.print(">Ruang 1 Api cek");
      lcd.setCursor(0, 1);  lcd.print("Ruang 2 Api cek");

      switch (robot.key()) {
        //case 1 : 
        case 1 : navigasi.masukRuangApi(60, 70,915);
                break;
//        case 1 : break;         
        case 2 : change(6); break;
        case 3 : change(25); break;
        case 4 : change(25); break;
      }
      temp[0] = constrain(temp[0], 1, 4);
      break;
    case 25 :
      lcd.setCursor(0, 0);  lcd.print("Ruang 1 Api cek");
      lcd.setCursor(0, 1);  lcd.print(">Ruang 2 dan 3 Api cek");

      switch (robot.key()) {
        //case 1 : 
        case 1 : navigasi.masukRuangApi(60, 60,910);
                break;
//        case 1 : break;         
        case 2 : change(6); break;
        case 3 : change(24); break;
        case 4 : change(24); break;
      }
      temp[0] = constrain(temp[0], 1, 4);
      break;
  }
}

#endif
