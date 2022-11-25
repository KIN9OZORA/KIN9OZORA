#ifndef MULTI_TASKING
#define MULTI_TASKING

#include "Arduino.h"
#include "wiring_private.h"

#ifndef MAX_LOOP_INDEX
#define MAX_LOOP_INDEX  3
#endif

uint32_t loopCycle[MAX_LOOP_INDEX];
static volatile voidFuncPtr loopFunction[MAX_LOOP_INDEX];

static void nothing(void) {
}

uint32_t TimerCounter;
ISR(TIMER1_COMPA_vect) {
  TIMSK1 = 0;
  
  for (uint8_t index = 0; index < MAX_LOOP_INDEX; index++)
    if ((TimerCounter % loopCycle[index]) == 0)
      loopFunction[index]();
  TimerCounter++;
  
  TCNT1  = 0;
  TIMSK1 |= (1 << OCIE1A);
}

class multiTasking {
  public :
    void function(uint8_t index, void (*userFunction)(void), uint32_t cycle) {
      if(index >= MAX_LOOP_INDEX)
        return;
        
      loopFunction[index] = userFunction;
      loopCycle[index] = cycle ? cycle : 1;
    }

    void begin(uint32_t time) {
      noInterrupts();
      for (count = 0; count < MAX_LOOP_INDEX; count++) {
        loopFunction[count] = nothing;
        loopCycle[count] = 0xFFFFFFF;
      }

      time =  time < 10 ? 10 : time;

      TCCR1A = 0;
      TCCR1B = 0;
      TCNT1  = 0;

      float value = F_CPU / (2000000 / time);
      if (value < 0xFFF0) {
        OCR1A = value - 1;
        TCCR1B |= (1 << CS10);
      }
      else if ((value / 8) < 0xFFF0) {
        OCR1A = (value / 8) - 1;
        TCCR1B |= (1 << CS11);
      }
      else if ((value / 64) < 0xFFF0) {
        OCR1A = (value / 64) - 1;
        TCCR1B |= (1 << CS10) | (1 << CS11);
      }
      else if ((value / 256) < 0xFFF0) {
        OCR1A = (value / 256) - 1;
        TCCR1B |= (1 << CS12);
      }
      else if ((value / 1024) < 0xFFF0) {
        OCR1A = (value / 1024) - 1;
        TCCR1B |= (1 << CS10) | (1 << CS12);
      }

      TCCR1B |= (1 << WGM12);
      TIMSK1 |= (1 << OCIE1A);
      interrupts();
    }

    void end() {
      noInterrupts();
      for (count = 0; count < MAX_LOOP_INDEX; count++) {
        loopFunction[count] = nothing;
        loopCycle[count] = 0xFFFFFFF;
      }

      TCCR1A = 0;
      TCCR1B = 0;
      TCNT1  = 0;
      TIMSK1 = 0;
      interrupts();
    }

  private :
    uint8_t count;
};

#endif
