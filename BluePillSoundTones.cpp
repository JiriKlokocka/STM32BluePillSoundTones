/*
 * BluePillSoundTones.cpp
 *
 *  Created on: 15. 11. 2018
 *      Author: KlokockaJir
 */

#include "BluePillSoundTones.h"

HardwareTimer timer(2);

volatile float volume = 1.0;
volatile uint32_t Oscillator1_Frequency_Hz;
uint16_t SineTable[256];
uint32_t BPS_DmaBuffer[DMABUFFERLENGTH];
uint32_t CoarseSigmaDeltaTable[] = {0x00000000 * PORTB_PINMASK, 0x01000000 * PORTB_PINMASK, 0x01000100 * PORTB_PINMASK, 0x01010100 * PORTB_PINMASK, 0x01010101 * PORTB_PINMASK};


void BPSound::BPSoundBegin()
{
	pinMode(SPEAKERPIN, OUTPUT);
	startDMA();
	uint32_t n;
	for (n = 0; n < 256; n++) SineTable[n] = ((sin(2 * PI * n / 255) + 1) * 500);
	startTimer();
	Serial.println("Begin BPSound");
}


void BPSound::setSine()
{
	for (int n = 0; n < 256; n++) SineTable[n] = ((sin(2 * PI * n / 255) + 1) * 500);
}

void BPSound::setSquare()
{
	for (int n = 0; n < 128; n++) SineTable[n] = 1000;
	for (int n = 129; n < 256; n++) SineTable[n] = 0;
}

/*void BPSound::setTriangle()
{
}
}*/


void BPSound::checkSampleRate()
{
	// ** just check the write speed **
	uint16_t n;
	uint32_t startTime = micros();
	for (n = 0; n < 1000; n++) writeDac(0);
	uint32_t stopTime = micros();

	uint32_t DacSamplingFrequency;
	Serial.print("DAC write time for 1000 samples in us: "); Serial.println(stopTime - startTime);
	DacSamplingFrequency = 1000000UL * 1000 / (stopTime - startTime);
	Serial.print("maximum possible DAC sampling frequency [Hz]: "); Serial.println(DacSamplingFrequency);

}


// good DMA article:
// https://vjordan.info/log/fpga/stm32-bare-metal-start-up-and-real-bit-banging-speed.html
void BPSound::startDMA()
{
  RCC->AHBENR |= RCC_AHBENR_DMA1EN; // enable DMA

  // channel 1: mem:8bit -> peri:8bit
  DMA1_Channel1->CNDTR = DMABUFFERLENGTH;
  DMA1_Channel1->CMAR = (uint32_t)BPS_DmaBuffer;
  DMA1_Channel1->CPAR = (uint32_t) & (GPIOB->ODR);
  DMA1_Channel1->CCR = 0;
  DMA1_Channel1->CCR = DMA_CCR1_MEM2MEM | DMA_CCR1_PL | DMA_CCR1_MINC | DMA_CCR1_CIRC | DMA_CCR1_DIR | DMA_CCR1_EN;
}

// maple timer
// http://docs.leaflabs.com/docs.leaflabs.com/index.html
void BPSound::startTimer()
{
  // Pause the timer while we're configuring it
  timer.pause();
  // Set up period
  timer.setPeriod(1000000UL / DACSAMPLINGRATE_HZ); // in microseconds
  // Set up an interrupt on channel 1
  timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  timer.attachCompare1Interrupt(DACsampleRateHandler);
  // Refresh the timer's count, prescale, and overflow
  timer.refresh();
  // Start the timer counting
  timer.resume();
}

void BPSound::DACsampleRateHandler()
{
	BPSound snd;
	static uint16_t Phase;
	Phase += 256 * 256 * Oscillator1_Frequency_Hz / DACSAMPLINGRATE_HZ;
	snd.writeDac((SineTable[Phase >> 8])*volume);
}

// Sigma Delta DAC
// input value 10 bit 0..1023
void BPSound::writeDac( uint32_t sollwert )
{
  uint32_t  integrator = 0;
  static uint32_t  oldValue = 0;
  uint32_t n;
  // sigma delta DAC
  for (n = 0; n < DMABUFFERLENGTH / 4; n++)
  {
    integrator += sollwert - oldValue;
    oldValue = integrator & 0b11100000000;
    BPS_DmaBuffer[n] = CoarseSigmaDeltaTable[ oldValue >> 8 ];
  }
}

void BPSound::sweepTone(uint16_t freqv1,uint16_t freqv2, uint16_t lengthStep, uint8_t volume )
{
	setVolume((float)volume);
	toneStart(freqv1);
	int16 i = 0;
	if(freqv2 > freqv1)
	{
		for(i = freqv1; i < freqv2; i++)
		{
			Oscillator1_Frequency_Hz = i;
			delay_us(lengthStep);
		}
		Serial.println(i);
	} else {
		for(i = freqv1; i > freqv2; i--)
		{
			Oscillator1_Frequency_Hz = i;
			delay_us(lengthStep);
		}
	}
	toneStop();
}

void BPSound::playTone(toneShape shape, uint16_t freqv, uint16_t length, uint8_t vol )
{
	float pctVol, pctLn;
	int16_t n, restLn;
	pctVol = (float)vol / 100.0;
	restLn = length - shape.attackTime;
	pctLn = (float)restLn / 100.0;
	setVolume(shape.attackBeginVol * pctVol);
	toneStart(freqv);
	for (n=0;n<((shape.attackTime < length) ? shape.attackTime : length)  ;n++)
	{
		delay(1);
		setVolume( (uint8_t)(((float)shape.attackBeginVol * pctVol)  -  (n * ((((float)shape.attackBeginVol * pctVol) - ((float)shape.sustainVol * pctVol)) /  shape.attackTime  ))));
	}

	if(restLn > 0)
	{
		//setVolume(shape.sustainVol  * pctVol);
		for (n=0;n<(restLn - (shape.decayTime * pctLn));n++)
		{
			delay(1);
		}
		for (n=0;n<(shape.decayTime * pctLn);n++)
		{
			delay(1);
			setVolume( ((float)shape.sustainVol * pctVol)  -  (n * ((((float)shape.sustainVol * pctVol) - ((float)shape.decayFinalVol * pctVol)) /  (shape.decayTime * pctLn ))));
		}
	}
    toneStop();
}

void BPSound::setVolume(float vol)
	{
	volume = (float)vol / 100;
	}
//void setVolume(uint8_t vol) {volume = vol;}

void BPSound::toneStart(uint16_t freqv)
	{
	Oscillator1_Frequency_Hz = freqv;
	}

void BPSound::toneStop()
	{
	Oscillator1_Frequency_Hz = 0;
	}

void BPSound::sineTone(uint16_t freqv, uint16_t dur, uint8_t volume)
{
	setVolume(volume);
	Oscillator1_Frequency_Hz = freqv;
	delay(dur);
	Oscillator1_Frequency_Hz = 0;
}

