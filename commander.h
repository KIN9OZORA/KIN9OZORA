#ifndef COMMANDER
#define COMMANDER

#ifdef TWI_BUFFER_LENGTH
#undef TWI_BUFFER_LENGTH
#endif

#ifdef BUFFER_LENGTH
#undef BUFFER_LENGTH
#endif

#define BUFFER_LENGTH 128
#define TWI_BUFFER_LENGTH 128

#include <Arduino.h>
#include <Wire.h>

class commander {
  public :
    commander(int16_t *data, int16_t *info, uint8_t id = 0);
    void begin();
    void begin(void (*receive)(void), void (*request)(void));
    void stop();
    void resume();
    bool read(uint8_t id, uint8_t startAddr = 0, uint8_t endAddr = 0, bool endRequest = true);
    bool write(uint8_t id, uint8_t startAddr = 0, uint8_t endAddr = 0, bool endRequest = true);

    void requestEvent();
    void receiveEvent();

    int16_t *buffer;
    int16_t *status;

  private :
    uint8_t ID;
    uint8_t dataStartAddr;
    uint8_t dataEndAddr;
    uint8_t count;
    uint8_t length;
    uint32_t startTime;


    void (*requestAddr)(void);
    void (*receiveAddr)(void);
};

#endif
