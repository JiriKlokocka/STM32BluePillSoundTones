

/*
 * Simple sound and tone shapes generator based on ChrisMicro's BluePillSound library:
 * https://github.com/ChrisMicro/BluePillSound
 * especially on this sketch:
 * https://github.com/ChrisMicro/BluePillSound/tree/master/_082_dmaSigmaDeltaTimerOscillator
 *
 */


#include "BluePillSoundTones.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0])) //determine number of elements in a field

toneShape piano, flute, reverse;

int melody1[] = { NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_B6, NOTE_C7 };
int melody11[] = { NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6 };
int melody12[] = { NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5 };
int melody2[] = { NOTE_C7, NOTE_B6, NOTE_A6, NOTE_G6, NOTE_F6, NOTE_E6, NOTE_D6,NOTE_C6 };
int melody21[] = { NOTE_C6, NOTE_B5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_D5,NOTE_C5 };

int melodyXX[] = { NOTE_C6, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, 0, NOTE_B5, NOTE_C6 };

int melodyXXnoteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };


BPSound sound;

void setup()
{
	sound.BPSoundBegin();



	/* Tone shape definitions
	 *
	 *
	 * Attack      Sustain      Decay
	 *   \
	 *    \
	 *     \
	 *      _ _ _ _ _ _ _ _ _ _
	 *                          \
	 *                            \
	 *                              \
	 *
	 */

	piano.attackTime = 50; 		//Absolute in milliseconds
	piano.attackBeginVol = 100;	// % of tone volume
	piano.sustainVol = 60; 		// % of tone volume
	piano.decayTime = 90; 		// % of tone volume
	piano.decayFinalVol = 10; 	// % of tone time resting after Attack

	flute.attackTime = 60; //ms
	flute.attackBeginVol = 00; //%
	flute.sustainVol = 80; //%
	flute.decayTime = 10; //%
	flute.decayFinalVol = 10; //%

	reverse.attackTime = 100; //ms
	reverse.attackBeginVol = 100; //%
	reverse.sustainVol = 10; //%
	reverse.decayTime = 90; //%
	reverse.decayFinalVol = 100; //%
}



void loop()
{
	int i;
	sound.setSine(); //Sine wave
	//Just tone (Frequency in Hz, length in milliseconds, volume in %)
	sound.sineTone(600, 1000, 100);
	delay(300);

	sound.setSquare();  //Square wave
	sound.sineTone(600, 1000, 100);
	sound.setSine();

	delay(300);

	//Shaped tone (Tone shape, Frequency in Hz, length in milliseconds, volume in %)
	sound.playTone(piano, 1000, 1000, 100.0 );
	delay(300);
	sound.playTone(flute, 800, 1000, 100.0 );
	delay(300);
	sound.playTone(reverse, 600, 1000, 100.0 );
	delay(300);
	//Tone sweep (Frequency FROM in Hz, Frequency TO in Hz, step in microseconds, volume in %;
	sound.sweepTone(10,5000, 200, 100 );
	sound.sweepTone(5000,10, 80, 100 );
	sound.sweepTone(10,5000, 80, 100 );

	delay(300);

	for(int i = 0; i < NELEMS(melody1);i++)
	{
		//Shaped tone (Tone shape, Frequency in Hz, length in milliseconds, volume in %)
		sound.playTone(piano, melody1[i], 300, 100.0 );
	}
	delay(500);
	for(int i = 0; i < NELEMS(melody21);i++)
	{
		//Shaped tone (Tone shape, Frequency in Hz, length in milliseconds, volume in %)
		sound.playTone(flute, melody21[i], 300, 100.0 );
	}

	delay(500);
	for(int i = 0; i < NELEMS(melody1);i++)
	{
		//Shaped tone (Tone shape, Frequency in Hz, length in milliseconds, volume in %)
		sound.playTone(reverse, melody1[i], 500, 100.0 );
	}

	delay(300);


	sound.setSquare();
	for(int i = 0; i < NELEMS(melodyXX);i++)
	{
		//Shaped tone (Tone shape, Frequency in Hz, length in milliseconds, volume in %)
		int noteDuration = 1000 / melodyXXnoteDurations[i];
		sound.playTone(piano, melodyXX[i], noteDuration, 100.0 );
	}
	sound.setSine();

	//Slow sirene
	for(i = 0; i<2;i++) {
		//Tone sweep (Frequency FROM in Hz, Frequency TO in Hz, step in microseconds, volume in %;
		sound.sweepTone(400,1500, 600, 100 );
		sound.sweepTone(1500,400, 600, 100 );
	}

	//Fast sirene
	for(i = 0; i<12;i++) {
		//Tone sweep (Frequency FROM in Hz, Frequency TO in Hz, step in microseconds, volume in %;
		sound.sweepTone(500,2000, 45, 100 );
		sound.sweepTone(2000,500, 45, 100 );
	}

	delay(300);

	sound.setSquare();

	//Grrr Grrrr
	delay(100 );
	for(i = 0; i<60;i++) {
		sound.sineTone(700, 10, 100);
		if (i == 30)
			delay(100);
		else
			delay(5);
	}

	sound.setSine();
	delay(300);




}
