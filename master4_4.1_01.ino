#include "menu.h"
#include "multiTasking.h"
#include "robot.h"

multiTasking  thread;

void setup() {
  Serial.begin(1000000);
  delay(100);
  thread.begin(20000);

  thread.function(0, UVtron, 1);
  //  thread.function(0, checkUV, 1);

  robot.begin();
  load();
//  while (1) {
//    robot.readJarak();
//    lcd.clear(); lcd.backlight();
//    lcd.setCursor(0, 0); lcd.print(String(sensorData[0]));
//    lcd.setCursor(0, 1); lcd.print(String(IR_FRONT));
//    robot.position(0, 0, 3, 45);
//    robot.position(0, 30, 3, 200);
//    delay(500);
//    robot.position(0, 0, 3, 200);
//    delay(1000);
//    robot.position(0, -30, 3, 200);
//    delay(500);
//    robot.position(0, 0, 3, 200);
//    delay(1000);
//   
//  }

}

void loop() {
  mainMenu();

}

void UVtron() {
  if (playing)
    FIRE |= sensor.uv(500);
  else
    FIRE = sensor.uv(500);
  //  digitalWrite(51, FIRE ? HIGH : LOW);
}

void checkUV() {
  digitalWrite(51, sensor.uv() ? HIGH : LOW);
}
