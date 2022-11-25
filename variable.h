#ifndef VARIABLE
#define VARIABLE

#include <Arduino.h>

//menu variable
#define maxAddr         30

bool playing = false;

uint8_t menu;
uint8_t data[maxAddr];
int32_t temp[4];

int16_t FireCount;
bool FireCountTrig;

bool off = false;
bool FIRE = false;

#define greyReff       24000 //(data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 32))
#define blackReff      12000 //(data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 32))
#define apiReff         (data[8] | (data[9] << 8) | (data[10] << 16) | (data[11] << 32))
#define apiRepeat       (data[12])
#define soundFreq       (data[13] | (data[14] << 8) | (data[15] << 16) | (data[16] << 32))
#define soundTime       (data[17])
#define soundRpt        (data[18])
#define roomSpeed       (data[19])
#define navigasiSpeed   (data[20])
#define roomMove        (data[21]) // PERPINDAHAN LANGKAH
#define navigasiMove    (data[22]) // PERPINDAHAN LANGKAH
#define lantaiOff       (data[23])


//senssor variable
typedef struct {
  int16_t sudut;
  int16_t resultan;
} vector;



//navigasi variable
typedef struct {
  uint8_t door, room;
} ruang;

#define unknown     0
#define doorA       1
#define doorB       2
#define doorC       3

#define bonekaA     1
#define bonekaB     2
#define bonekaC     3

//robot variable
int16_t sensorData[20];
int16_t legData[20];
int16_t legStatus[3];

//int16_t BGReff   = 1750; //(blackReff + greyReff) / 2;
//int16_t BWReff   = 4000;
//int16_t hitamReff   = 3000;
//int16_t abuAbuReff  = 500;

#define batasApi              apiReff
#define transTime             legData[1]
#define Xspeed                legData[2]
#define Yspeed                legData[3]
#define Rspeed                legData[4]
#define bodyRotX              legData[5]
#define bodyRotY              legData[6]
#define bodyRotZ              legData[7]
#define bodyPosX              legData[8]
#define bodyPosY              legData[9]
#define bodyPosZ              legData[10]
#define ModeTangga            legData[13]


#define dxlOn                 legData[0] |= ((1 << 14) | (3 << 8))
#define pumpOn                legData[0] |= ((1 << 12) | (3 << 8))
#define sensorOn              legData[0] |= ((1 << 13) | (3 << 8))
#define dxlOff                legData[0] &= (~(1 << 14) & ~(12 << 8))
#define pumpOff               legData[0] &= (~(1 << 12) & ~(12 << 8))
#define sensorOff             legData[0] &= (~(1 << 13) & ~(12 << 8))

#define sensor1               (legStatus[1] / 10)
#define sensor2               (legStatus[2] / 10)
#define sensorLantai          (sensor1 + sensor2)

#define black                 0
#define grey                  1
#define white                 2
bool black_ck = false;
bool grey_ck  = false;

#define FRONT                 sensorData[0]
#define FRONT_LEFT            sensorData[1]
#define LEFT_MIDLE_FRONT      sensorData[2]
#define LEFT_MIDLE            sensorData[3]
#define LEFT_MIDLE_REAR       sensorData[4]
#define REAR_LEFT             sensorData[5]
#define REAR                  sensorData[6]
#define REAR_RIGHT            sensorData[7]
#define RIGHT_MIDLE_REAR      sensorData[8]
#define RIGHT_MIDLE           sensorData[9]
#define RIGHT_MIDLE_FRONT     sensorData[10]
#define FRONT_RIGHT           sensorData[11]

#define IR(a)                 sensorData[12 + (a > 3 ? 3 : a)]
#define IR_FRONT              IR(0)
#define IR_LEFT               IR(1)
#define IR_REAR               IR(2)
#define IR_RIGHT              IR(3)

#define timeOut               1000

#define putarKiri(a,b)        putar(-abs(a) * 1.1, b)
#define putarKanan(a,b)       putar(abs(a) * 1.1, b)
#define majuBelokKanan(a,b)   majuBelok(a, b)
#define majuBelokKiri(a,b)    majuBelok(-a, b)

#define DINDING_KANAN         (RIGHT_MIDLE_FRONT < 40 && RIGHT_MIDLE < 40 && RIGHT_MIDLE_REAR < 40)
#define DINDING_KIRI          (LEFT_MIDLE_FRONT < 40 && LEFT_MIDLE < 40 && LEFT_MIDLE_REAR < 40)
#define DINDING_KANAN_RUANG   (RIGHT_MIDLE_FRONT < 25 && RIGHT_MIDLE < 25 && RIGHT_MIDLE_REAR < 25)
#define DINDING_KIRI_RUANG    (LEFT_MIDLE_FRONT < 25 && LEFT_MIDLE < 25 && LEFT_MIDLE_REAR < 25)

#endif
