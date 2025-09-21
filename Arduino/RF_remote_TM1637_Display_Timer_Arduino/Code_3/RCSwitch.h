#ifndef _RCSwitch_h
#define _RCSwitch_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#elif defined(ENERGIA)
#include "Energia.h"
#elif defined(RPI)
#define RaspberryPi
#include <string.h>
#include <stdlib.h>
#include <wiringPi.h>
#elif defined(SPARK)
#include "application.h"
#else
#include "WProgram.h"
#endif

#include <stdint.h>

#if defined(__AVR_ATtinyX5__) or defined(__AVR_ATtinyX4__)
#define RCSwitchDisableReceiving
#endif
#define RCSWITCH_MAX_CHANGES 67

class RCSwitch {

public:
  RCSwitch();

  void switchOn(int nGroupNumber, int nSwitchNumber);
  void switchOff(int nGroupNumber, int nSwitchNumber);
  void switchOn(const char* sGroup, int nSwitchNumber);
  void switchOff(const char* sGroup, int nSwitchNumber);
  void switchOn(char sFamily, int nGroup, int nDevice);
  void switchOff(char sFamily, int nGroup, int nDevice);
  void switchOn(const char* sGroup, const char* sDevice);
  void switchOff(const char* sGroup, const char* sDevice);
  void switchOn(char sGroup, int nDevice);
  void switchOff(char sGroup, int nDevice);

  void sendTriState(const char* sCodeWord);
  void send(unsigned long code, unsigned int length);
  void send(const char* sCodeWord);

#if not defined(RCSwitchDisableReceiving)
  void enableReceive(int interrupt);
  void enableReceive();
  void disableReceive();
  bool available();
  void resetAvailable();

  unsigned long getReceivedValue();
  unsigned int getReceivedBitlength();
  unsigned int getReceivedDelay();
  unsigned int getReceivedProtocol();
  unsigned int* getReceivedRawdata();
#endif

  void enableTransmit(int nTransmitterPin);
  void disableTransmit();
  void setPulseLength(int nPulseLength);
  void setRepeatTransmit(int nRepeatTransmit);
#if not defined(RCSwitchDisableReceiving)
  void setReceiveTolerance(int nPercent);
#endif

  /**
     * Description of a single pule, which consists of a high signal
     * whose duration is "high" times the base pulse length, followed
     * by a low signal lasting "low" times the base pulse length.
     * Thus, the pulse overall lasts (high+low)*pulseLength
     */
  struct HighLow {
    uint8_t high;
    uint8_t low;
  };

  /**
     * A "protocol" describes how zero and one bits are encoded into high/low
     * pulses.
     */
  struct Protocol {
    /** base pulse length in microseconds, e.g. 350 */
    uint16_t pulseLength;

    HighLow syncFactor;
    HighLow zero;
    HighLow one;

    bool invertedSignal;
  };

  void setProtocol(Protocol protocol);
  void setProtocol(int nProtocol);
  void setProtocol(int nProtocol, int nPulseLength);

private:
  char* getCodeWordA(const char* sGroup, const char* sDevice, bool bStatus);
  char* getCodeWordB(int nGroupNumber, int nSwitchNumber, bool bStatus);
  char* getCodeWordC(char sFamily, int nGroup, int nDevice, bool bStatus);
  char* getCodeWordD(char group, int nDevice, bool bStatus);
  void transmit(HighLow pulses);

#if not defined(RCSwitchDisableReceiving)
  static void handleInterrupt();
  static bool receiveProtocol(const int p, unsigned int changeCount);
  int nReceiverInterrupt;
#endif
  int nTransmitterPin;
  int nRepeatTransmit;

  Protocol protocol;

#if not defined(RCSwitchDisableReceiving)
  static int nReceiveTolerance;
  volatile static unsigned long nReceivedValue;
  volatile static unsigned int nReceivedBitlength;
  volatile static unsigned int nReceivedDelay;
  volatile static unsigned int nReceivedProtocol;
  const static unsigned int nSeparationLimit;
  static unsigned int timings[RCSWITCH_MAX_CHANGES];
#endif
};

#endif
