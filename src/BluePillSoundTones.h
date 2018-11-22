/*
 * BluePillSoundTones.h
 *
 *  Created on: 15. 11. 2018
 *      Author: KlokockaJir
 */

#ifndef BLUEPILLSOUNDTONES_H_
#define BLUEPILLSOUNDTONES_H_


#include "Arduino.h"
#include "stm32f103.h"
#include "Tones.h"



#define DACSAMPLINGRATE_HZ    20000
#define TIMER_INTTERUPT_US    1000000UL / DACSAMPLINGRATE_HZ
#define DMABUFFERLENGTH 100
#define SPEAKERPIN PB0            // speaker pin, be sure that it corresponds to the DMA pin setup in the next line
#define PIN_PB 0                  // speaker pin, you can choose 0..7, 0 means PB0, 1 means PB1 ...
#define PORTB_PINMASK (1<<PIN_PB)


typedef struct
{
  uint16_t attackTime; //in milliseconds
  uint8_t attackBeginVol; //in % of Volume
  uint8_t sustainVol; //in % of Volume
  uint8_t decayFinalVol; //in % of Volume
  uint8_t decayTime; //in % tone length

} toneShape;

class BPSound {
public:
	static void DACsampleRateHandler();
	void startDMA();
	void BPSoundBegin();
	void startTimer();
	void writeDac( uint32_t sollwert );
	void sweepTone(uint16_t freqv1,uint16_t freqv2, uint16_t lengthStep, uint8_t volume );
	void playTone(toneShape shape, uint16_t freqv, uint16_t length, uint8_t volume );
	void setVolume(float vol);
	void toneStart(uint16_t freqv);
	void toneStop();
	void sineTone(uint16_t freqv, uint16_t dur, uint8_t volume);
	void checkSampleRate();
	void setSine();
	void setSquare();

};

#endif /* BLUEPILLSOUNDTONES_H_ */

