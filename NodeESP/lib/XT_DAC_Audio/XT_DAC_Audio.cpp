// XTronical DAC Audio Library, currently supporting ESP32
// May work with other processors and/or DAC's with or without modifications
// (c) XTronical 2018, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty given
// See www.xtronical.com for documentation
// This software is currently under active development (Jan 2018) and may change and break your code with new versions
// No responsibility is taken for this. Stick with the version that works for you, if you need newer commands from later versions
// you will have to alter your original code



#include "esp32-hal-timer.h"
#include "XT_DAC_Audio.h"
#include "HardwareSerial.h"



// These globals are used because the interrupt routine was a little unhappy and unpredictable
// using objects, with kernal panics etc. I think this is a compiler / memory tracking issue
// For now any vars inside the interrupt are kept as simple globals

volatile uint32_t NextFillPos=0;								// position in buffer of next byte to fill
volatile int32_t NextPlayPos=0;									// position in buffer of next byte to play
volatile int32_t EndFillPos=BUFFER_SIZE;						// position in buffer of last byte+1 that can be filled
volatile uint8_t LastDacValue;									// Next Idx pos in buffer to send to DAC
volatile uint8_t Buffer[BUFFER_SIZE];							// The buffer to store the data that will be sent to the
volatile uint8_t DacPin;                              			// pin to send DAC data to, presumably one of the DAC pins!
volatile XT_PlayListItem_Class *FirstPlayListItem=0;          	// first play list item to play in linked list
volatile uint16_t BufferUsedCount=0;							// how much buffer used since last buffer fill

XT_Instrument_Class DEFAULT_INSTRUMENT;


// interrupt stuff
hw_timer_t * timerDac = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


// The FNOTE "defines" below contain actual frequencies for notes which range from a few Hz (around 30) to around 4000Hz
// But in essence there are only 89 different notes , we collect them into an array so that we can store a note as
// a number from 0 to 90, (0 being no 0Hz, or silence). In this way we can store musical notes in single bytes.
// To benefit from this the music data in your code should be greater than 90 bytes otherwise your technically using more
// memory than you would if you used the raw frequencies (which use 2 bytes per note). It is estimated in simple projects you
// would not gain but generally a simple project would have more spare memory anyway. For larger projects where memory could
// be more of an issue you will actually save memory, so hopefully a win!
uint16_t XT_Notes[90]={0,FNOTE_B0,FNOTE_C1,FNOTE_CS1,FNOTE_D1,FNOTE_DS1,FNOTE_E1,FNOTE_F1,FNOTE_FS1,FNOTE_G1,FNOTE_GS1,FNOTE_A1,FNOTE_AS1,FNOTE_B1,FNOTE_C2,FNOTE_CS2,FNOTE_D2,FNOTE_DS2,FNOTE_E2,FNOTE_F2,FNOTE_FS2,FNOTE_G2,FNOTE_GS2,FNOTE_A2,FNOTE_AS2,FNOTE_B2,FNOTE_C3,FNOTE_CS3,FNOTE_D3,FNOTE_DS3,FNOTE_E3,FNOTE_F3,FNOTE_FS3,FNOTE_G3,FNOTE_GS3,FNOTE_A3,FNOTE_AS3,FNOTE_B3,FNOTE_C4,FNOTE_CS4,FNOTE_D4,FNOTE_DS4,FNOTE_E4,FNOTE_F4,FNOTE_FS4,FNOTE_G4,FNOTE_GS4,FNOTE_A4,FNOTE_AS4,FNOTE_B4,FNOTE_C5,FNOTE_CS5,FNOTE_D5,FNOTE_DS5,FNOTE_E5,FNOTE_F5,FNOTE_FS5,FNOTE_G5,FNOTE_GS5,FNOTE_A5,FNOTE_AS5,FNOTE_B5,FNOTE_C6,FNOTE_CS6,FNOTE_D6,FNOTE_DS6,FNOTE_E6,FNOTE_F6,FNOTE_FS6,FNOTE_G6,FNOTE_GS6,FNOTE_A6,FNOTE_AS6,FNOTE_B6,FNOTE_C7,FNOTE_CS7,FNOTE_D7,FNOTE_DS7,FNOTE_E7,FNOTE_F7,FNOTE_FS7,FNOTE_G7,FNOTE_GS7,FNOTE_A7,FNOTE_AS7,FNOTE_B7,FNOTE_C8,FNOTE_CS8,FNOTE_D8,FNOTE_DS8
};


// Precalculate sine values in an 8 bit range (i.e. usually 0 to 360Deg, we want a new measure of a Degree that
// defines a circle as 256 parts. We'll call these 8Bit Degrees!
// As we're using an 8 bit value for the DAC 0-255 (256 parts) that means for us
// there are 256 'bits' to a complete circle not 360 (as in degrees)

int SineValues[256];       // an array to store our values for sine

void InitSineValues()
{
	float ConversionFactor=(2*3.142)/256;        		// convert my 0-255 bits in a circle to radians
														// there are 2 x PI radians in a circle hence the 2*PI
														// Then divide by 256 to get the value in radians
														// for one of my 0-255 bits.
	float RadAngle;                           			// Angle in Radians, have to have this as computers love radians!
	// calculate sine values
	for(int MyAngle=0;MyAngle<256;MyAngle++) {
		RadAngle=MyAngle*ConversionFactor;              // 8 bit angle converted to radians
		SineValues[MyAngle]=(sin(RadAngle)*127)+128;  	// get the sine of this angle and 'shift' up so
														// there are no negative values in the data
														// as the DAC does not understand them and would
														// convert to positive values.
	}
}





uint8_t SetVolume(uint8_t Value,uint8_t Volume)
{
	// returns the sound byte value adjusted for the volume passed, 0 min, 127 max
	// a value of 127 will not boost the sound higher than the original, it will
	// mean the value returned is the original, and any other value below 127 will
	// be a proportional fraction of the original
	// remember that the mid point ( no sound, speaker at rest) is 7f not 0

	uint8_t AdjustedValue;
	if(Volume>127)							// Max is 127
		Volume=127;
	if(Value==0x7f)
		return Value;  // a speaker at mid point, cannot change the volume of that, it is effectively 0
	if(Value>0x7f)
	{
		// +ve part of wave above 0, runs 1 to 128
		// value will actually be in range 128 to 255, adjust down to 1 to 128
		AdjustedValue=Value-127;
		// Now adjust the volume be the ratio passed in
		AdjustedValue=AdjustedValue*(float(Volume)/127);
		// finally put back in range 1 to 128
		return AdjustedValue+127;
	}
	// if we get this far then wave below 0, range 127 to 0 (127 being mid of wave, 0 nearest bottom(trough))
	// just flip the value so it's in range 1 to 127 with 1 being the low point
	AdjustedValue=0x7f-Value;
	// Then add in volume adjustment
	AdjustedValue=AdjustedValue*(float(Volume)/127);
	// adjust back to correct range before returning
	return 0x7f-AdjustedValue;

}





// The main interrupt routine called 50,000 times per second
void IRAM_ATTR onTimer()
{
	// Sound playing code, plays whatevers in the buffer.

	if(NextPlayPos!=NextFillPos)					// If not up against the next fill position then valid data to play
	{
		if(LastDacValue!=Buffer[NextPlayPos])		// Send value to DAC only of changed since last value else no need
		{
			// value to DAC has changed, send to actual hardware, else we just leave setting as is as it's not changed
			LastDacValue=Buffer[NextPlayPos];
			dacWrite(DacPin,LastDacValue);			// write out the data
		}
		NextPlayPos++;								// Move play pos to next byte in buffer
		if(NextPlayPos==BUFFER_SIZE)				// If gone past end of buffer,
			NextPlayPos=0;							// set back to beginning
		EndFillPos=NextPlayPos-1;					// Move area where we can fill up to to NextPlayPos
		if(EndFillPos<0)							// check if less than zero, if so then 1 less is
			EndFillPos=BUFFER_SIZE;					// BUFFER_SIZE (we can fill up to one less than this)
	}
	if(FirstPlayListItem!=0)  // sounds in Q
		BufferUsedCount++;
}

void XT_DAC_Audio_Class::AverageBufferUsage()
{
	// returns the average buffer usage over 50 iterations of your main loop.
	// Call this routine in your main loop and after 50 calls to this
	// routine it will display the avg buffer memory used via the serial link,
	// so ensure you have enabled serial comms.
	// This routine should only be used to check how much buffer is being used during your
	// code executing in order to optimise how much buffer you need to reserve.

	static bool ResultPrinted=false;
	static uint8_t LoopCount=0;

	if((LoopCount==50)&(ResultPrinted==false))
	{
		Serial.print("Avg Buffer Usage : ");
		Serial.print(BufferUsedCount/50);
		Serial.println(" bytes");
		ResultPrinted=true;
	}
	LoopCount++;
}

void XT_DAC_Audio_Class::FillBuffer()
{
	// Fill buffer with the sound to output

	if((NextFillPos==BUFFER_SIZE)&(EndFillPos!=0)&(NextPlayPos!=0))
		NextFillPos=0;

	// if there are items that need to be played & room for more in buffer
	while((FirstPlayListItem!=0)&(NextFillPos!=EndFillPos)&(NextFillPos!=BUFFER_SIZE))
	{
		Buffer[NextFillPos]=MixBytesToPlay();					// Get the byte to play and put into buffer
		NextFillPos++;											// move to next buffer position
		if(NextFillPos!=EndFillPos)
		{
			if(NextFillPos==BUFFER_SIZE)
				NextFillPos=0;
		}
	}
}


XT_DAC_Audio_Class::XT_DAC_Audio_Class(uint8_t TheDacPin, uint8_t TimerNo)
{
  // Using a prescaler of 80 gives a counting frequency of 1,000,000 (1MHz) and using
  // and calling the function every 20 counts of the frequency (the 20 below) means
  // that we will call our onTimer function 50,000 times a second
  FirstPlayListItem=0;
  timerDac = timerBegin(TimerNo, 80, true);                // use timer 0, pre-scaler is 80 (divide by 8000), count up
  timerAttachInterrupt(timerDac, &onTimer, true);          // P3= edge triggered
  timerAlarmWrite(timerDac, 20, true);                     // will trigger 50,000 times per second, so 50kHz is max freq.
  timerAlarmEnable(timerDac);                              // enable
  //XTDacAudioClassGlobalObject=this;						// set variable to use in ISR
  DacPin=TheDacPin;									// set dac pin to use
  LastDacValue=0;									// set to mid  point
  dacWrite(DacPin,LastDacValue);						// Set speaker to mid point, stops click at start of first sound
  InitSineValues();										// create our table of sine values for our special circular
														// angles of 0 - 255 "DAC Degrees". Speeds things up to if
														// you pre-calculate
}


uint8_t XT_DAC_Audio_Class::MixBytesToPlay()
{
	// goes through sounds, gets the bytes from each and mixes them together, returning a final byte

	// mix all sounds together in the list of sounds to play and then output that one mixed value
	// To mix waves you add the waves together.... That is presuming they are "correct" waves
	// that have both positive and negative peaks and troughs. However these wav files are saved
	// as 8bit unsigned, so therefore there is no negative, we need to adjust them back to having
	// correct positive and ngative peaks and troughs first before doing the "mixing", this is a
	// trivial task

	XT_PlayListItem_Class *PlayItem;
	int16_t ByteToPlay=0;

	PlayItem=FirstPlayListItem;
	int i=1;
	while(PlayItem!=0)
	{
		if(PlayItem->Playing)
		{
			 // This the actual mixing, simple addition of each "signed" part of the wave
			ByteToPlay+=PlayItem->NextByte()-127;
			PlayItem=PlayItem->NextItem;
		}
		else
		{
			RemoveFromPlayList(PlayItem);
			PlayItem=PlayItem->NextItem;
		}
		i++;
	}
	if(ByteToPlay>255)
		ByteToPlay=255;
	if(ByteToPlay<0)
		ByteToPlay=0;
	return ByteToPlay+127;
}


void XT_DAC_Audio_Class::RemoveFromPlayList(volatile XT_PlayListItem_Class *ItemToRemove)
{
	// removes a play item from the play list
	if(ItemToRemove->PreviousItem!=0)
		ItemToRemove->PreviousItem->NextItem=ItemToRemove->NextItem;
	else
		FirstPlayListItem=ItemToRemove->NextItem;
	if(ItemToRemove->NextItem!=0)
		ItemToRemove->NextItem->PreviousItem=ItemToRemove->PreviousItem;
	else
		LastPlayListItem=ItemToRemove->PreviousItem;
}


void XT_DAC_Audio_Class::Play(XT_PlayListItem_Class *Sound)
{

	Play(Sound,false);
}


void XT_DAC_Audio_Class::Play(XT_PlayListItem_Class *Sound,bool Mix)
{

	// Mixing not yet enabled until slight re-design
	//if(Mix==false)  // stop all currently playing sounds and just have this one
		StopAllSounds();
	// set up this sound to play, different types of sound may initialise differently
	Sound->Init();
	// add to list of currently playing items

	// create a new play list entry

	if(FirstPlayListItem==0) // no items to play in list yet
	{
		FirstPlayListItem=Sound;
		LastPlayListItem=Sound;
	}
	else{
		// add to end of list
		LastPlayListItem->NextItem=Sound;
		Sound->PreviousItem=LastPlayListItem;
		LastPlayListItem=Sound;

	}
	Sound->Playing=true;					// Will start it playing
}


void XT_DAC_Audio_Class::StopAllSounds()
{
	// stop all sounds and clear the play list

	volatile XT_PlayListItem_Class* PlayItem;
	PlayItem=FirstPlayListItem;
	while(PlayItem!=0)
	{
		PlayItem->Playing=false;
		RemoveFromPlayList(PlayItem);
		PlayItem=FirstPlayListItem;
	}
	FirstPlayListItem=0;
}







// Wav Class functions

XT_Wav_Class::XT_Wav_Class(unsigned char *WavData)
{
  // create a new wav class object

  SampleRate=(WavData[25]*256)+WavData[24];
  DataSize=(WavData[42]*65536)+(WavData[41]*256)+WavData[40]+44;
  IncreaseBy=float(SampleRate)/50000;
  Data=WavData;
  Count=0;
  LastIntCount=0;
  DataIdx=44;
}


void XT_Wav_Class::Init()
{
	LastIntCount=0;
	DataIdx=44;
	Count=0;
}


uint8_t XT_Wav_Class::NextByte()
{
	// Returns the next byte to be played, note that this routine will return values suitable to
	// be played back at 50,000Hz. Even if this sample is at a lesser rate than that it will be
	// padded out as required so that it will appear to have a 50Khz sample rate


	uint16_t IntPartOfCount;
	uint8_t ReturnValue;

	// increase the counter, if it goes to a new integer digit then write to DAC
	Count+=IncreaseBy;
	IntPartOfCount=floor(Count);
	ReturnValue=Data[DataIdx];				// by default we return previous value;

	if(IntPartOfCount>LastIntCount)
	{
		// gone to a new integer of count, we need to send a new value to the DAC
		LastIntCount=IntPartOfCount; // crashes on this line with panic
		ReturnValue=Data[DataIdx];
		DataIdx++;
		if(DataIdx>=DataSize)  				// end of data, flag end
		{
			Count=0;						// reset frequency counter
			DataIdx=44;						// reset data pointer back to beginning of WAV data
			// remove wav from play list
			Playing=false;  				// mark as completed
		}
	}

	return ReturnValue;

}


 /*
 XT_Wav_Class *XT_Wav_Class::Copy()
 {
	 // copies itself into a new item and returns that item, actually copies very little, only
	 // what is needed to getg job done, so beware. If you need more copying then add into this
	 // function. Note the WavData just copies the pointer, not the data itself

	 XT_Wav_Class *Item=new XT_Wav_Class(this->Data);
	 Item->Init();
 }

 */





// Instrument class routines

XT_Instrument_Class::XT_Instrument_Class():XT_Instrument_Class(INSTRUMENT_PIANO,127)
{
	  // See main constructor routine for description of passed parameters
	  // and defaults
}


XT_Instrument_Class::XT_Instrument_Class(int16_t InstrumentID):XT_Instrument_Class(InstrumentID,127)
{
	  // See main constructor routine for description of passed parameters
	  // and defaults
}


XT_Instrument_Class::XT_Instrument_Class(int16_t InstrumentID, uint8_t Volume)
{
	// Envelope to use for this instrument. pass 0 for no envelope
	// Volume : Volume of sound 0- 127 (max)

	this->Note=abs(NOTE_C4);					// default note
	this->SoundDuration=1000;					// default note length
	this->Duration=1000;						// default length of entire play action (i.e. after any decay)
	this->Volume=Volume;
	SetInstrument(InstrumentID);				// The default

}


void XT_Instrument_Class::SetInstrument(uint16_t Instrument)
{
	// To create your new "permanent" instrument to the code, add a line here that calls its
	// set up routine and add this as a private function to the header file for the
	// Instrument class. Also add a suitable # define to the Instruments.h header file
	// in the instruments section.
	// Any envelopes and waveforms will have their memory cleared here, no need to worry!

	ClearEnvelopes();

	switch (Instrument)
	{
		case(INSTRUMENT_NONE) 			: SetDefaultInstrument();break;
		case(INSTRUMENT_PIANO)			: SetPianoInstrument();break;
		case(INSTRUMENT_HARPSICHORD)	: SetHarpsichordInstrument();break;
		case(INSTRUMENT_ORGAN)			: SetOrganInstrument();break;
		case(INSTRUMENT_SAXOPHONE)		: SetSaxophoneInstrument();break;

		default: // compilation error, default to just square wave
			SetDefaultInstrument();break;
	}
}


void XT_Instrument_Class::SetDefaultInstrument()
{
	delete(Envelope);			// No envelope for default instruments
	Envelope=0;
	SetWaveForm(WAVE_SQUARE);
}


void XT_Instrument_Class::SetPianoInstrument()
{
	SetWaveForm(WAVE_TRIANGLE);
	if(Envelope==0)
		Envelope=new XT_Envelope_Class();
	Envelope->AddPart(25,127);
	Envelope->AddPart(20,20);
	Envelope->AddPart(15,75);
	Envelope->AddPart(10,15);
	Envelope->AddPart(5,50);
	Envelope->AddPart(300,0);
}

void XT_Instrument_Class::SetHarpsichordInstrument()
{
	SetWaveForm(WAVE_SAWTOOTH);
	if(Envelope==0)
		Envelope=new XT_Envelope_Class();
	Envelope->AddPart(15,127);
	Envelope->AddPart(80,100);
	Envelope->AddPart(300,0);
}


void XT_Instrument_Class::SetOrganInstrument()
{
	SetWaveForm(WAVE_SINE);
	if(Envelope==0)
		Envelope=new XT_Envelope_Class();
	Envelope->AddPart(15,127);
	Envelope->AddPart(3000,0);	// An organ maintains until key released
}



void XT_Instrument_Class::SetSaxophoneInstrument()
{
	SetWaveForm(WAVE_SQUARE);
	if(Envelope==0)
		Envelope=new XT_Envelope_Class();
	Envelope->AddPart(15,127);
	Envelope->AddPart(3000,0);	// An organ maintains until key released
}


void XT_Instrument_Class::ClearEnvelopes()
{
	// Delete the envelope parts associated with this instrument
	XT_EnvelopePart_Class* Part,*NextPart;

	if(Envelope!=0)
	{
		// We have an envelope, delete any sub-parts first before deleting the parent envelope
		Part=Envelope->FirstPart;
		while(Part!=0)
		{
			NextPart=Part->NextPart;
			delete(Part);
			Part=NextPart;
		}
	}
}


void XT_Instrument_Class::SetWaveForm(uint8_t WaveFormType)
{
	// Sets the wave form for this instrument

	delete (WaveForm);										// free any previous memory for a previous waveform
	switch (WaveFormType)
	{
		case WAVE_SQUARE : WaveForm=new XT_SquareWave_Class();break;
		case WAVE_TRIANGLE : WaveForm=new XT_TriangleWave_Class();break;
		case WAVE_SAWTOOTH : WaveForm=new XT_SawToothWave_Class();break;
		case WAVE_SINE : WaveForm=new XT_SineWave_Class();break;

		default: // compilation error, default to square
			WaveForm=new XT_SquareWave_Class();break;
	}
}

void XT_Instrument_Class::Init()
{
	CurrentByte=0;
	WaveForm->Init(Note);
	SoundDurationCounter=SoundDuration*50;
	DurationCounter=Duration*50;          // converts to how many samples to return before stopping
	if(Envelope!=0)  {
		Envelope->EnvelopeCompleted=false;
		Envelope->CurrentEnvelopePart=0;		// Reset to no current part, i.e. will start at beginning
	}
}

uint8_t XT_Instrument_Class::NextByte()
{
	uint8_t ByteToPlay;

	if(DurationCounter==0)									// If completed mark as so and return no sound
	{
		Playing=false;
		return 0;
	}
	DurationCounter--;

	if(SoundDurationCounter==0)								// If sound completed return no sound
		return 0;
	SoundDurationCounter--;

	if(WaveForm->Frequency==0)										// If no frequency then no sound
		return 0;

	// This next bit handles basic wave form if a sound is being produced

	ByteToPlay=WaveForm->NextByte();

	// Next add in envelope control
	if(Envelope!=0)
	{
		// are their any parts, if not do nothing
		if(Envelope->FirstPart!=0)
		{
			// OK we have some envelope parts, manipulate the wave form volume according to the
			// current envelope part in use for this instrument
			if(Envelope->EnvelopeCompleted==false)
			{
				if(Envelope->CurrentEnvelopePart==0)  				// start of envelope
				{
					Envelope->CurrentVolume=0;					  	// At very start of any note there is no volume!
					Envelope->CurrentEnvelopePart=Envelope->FirstPart;
					Envelope->InitEnvelopePart(Envelope->CurrentEnvelopePart,Envelope->CurrentVolume);
				}
				Envelope->DurationCounter--;
				if(Envelope->DurationCounter==0)
				{
					// move to next envelope part
					Envelope->CurrentEnvelopePart=Envelope->CurrentEnvelopePart->NextPart;
					if(Envelope->CurrentEnvelopePart==0)  			// End of envelope parts, volume remains at last setting
						Envelope->EnvelopeCompleted=true;
					else
						Envelope->InitEnvelopePart(Envelope->CurrentEnvelopePart,Envelope->CurrentVolume);
				}
				Envelope->CurrentVolume+=Envelope->VolumeIncrement;
			}
			ByteToPlay=SetVolume(ByteToPlay,int(Envelope->CurrentVolume));
		}
	}


	// adjust for current volume of this sound
	return SetVolume(ByteToPlay,Volume);
}





//Music score class

XT_MusicScore_Class::XT_MusicScore_Class(int8_t* Score):XT_MusicScore_Class(Score,TEMPO_ALLEGRO,&DEFAULT_INSTRUMENT,0)
{
	// set tempo and instrument to default, number of plays to 1 (once)
}


XT_MusicScore_Class::XT_MusicScore_Class(int8_t* Score,uint16_t Tempo):XT_MusicScore_Class(Score,Tempo,&DEFAULT_INSTRUMENT,0)
{
	// set instrument to default and plays to 1
}

XT_MusicScore_Class::XT_MusicScore_Class(int8_t* Score,uint16_t Tempo,XT_Instrument_Class* Instrument):XT_MusicScore_Class(Score,Tempo,Instrument,0)
{
}



XT_MusicScore_Class::XT_MusicScore_Class(int8_t* Score,uint16_t Tempo,XT_Instrument_Class* Instrument,uint16_t Repeat)
{
	// Create music score
	this->Score=Score;
	this->Tempo=Tempo;
	this->Instrument=Instrument;
	this->Repeat=Repeat;
}


XT_MusicScore_Class::XT_MusicScore_Class(int8_t* Score,uint16_t Tempo,uint16_t InstrumentID,uint16_t Repeat)
{
	// Create music score
	this->Score=Score;
	this->Tempo=Tempo;
	this->Instrument=&DEFAULT_INSTRUMENT;
	Instrument->SetInstrument(InstrumentID);
	this->Repeat=Repeat;
}


void XT_MusicScore_Class::Init()
{
	// Called before as score starts playing
	Instrument->Init(); // Initialise instrument

	// convert the tempo in BPM into a value that counts down in 50,000's of a second as this is the sample
	// rate sent to the DAC
	ChangeNoteEvery=(60/float(Tempo))*50000;
	ChangeNoteCounter=0;					// ensures starts by playing first note with no delay
	ScoreIdx=0;								// set position to first note
	if(Repeat>0)							// If not infinite repeats set the repeat counter
		RepeatIdx=Repeat;
}


uint8_t XT_MusicScore_Class::NextByte()
{
	// returns next byte for the DAC

	// are we ready to play another note?
	if(ChangeNoteCounter==0)  {
		// Yes, new note
		if(Score[ScoreIdx]==SCORE_END)  // end of data
		{
			// check if we need to repeat
			if(Repeat>0)  // a fixed number of plays
			{
				RepeatIdx--;
				if(RepeatIdx==0)  		// no more plays
				{
					Playing=false;
					return 0;    	// return silence
				}
			}
			ScoreIdx=0;                 // reset to start of data
		}
		Instrument->Note=abs(Score[ScoreIdx]);			// convert the negative value to positive index.
		ScoreIdx++;										// move to next note

		// set length of play for instrument
		// Check next data value to see if it is a beat value
		if(Score[ScoreIdx]>0) 							// positive value, therefore not default beat length
		{
			// Set the duration, beat value of 1=0.25 beat, 2 0.5 beat etc. So just divide by 4
			// to get the real beat length,
			Instrument->Duration=(ChangeNoteEvery*(float(Score[ScoreIdx])/4));
			// Then times by 0.8 to allow for natural movement
			// of players finger on the instrument.
			Instrument->SoundDuration=Instrument->Duration*0.8;
			ChangeNoteCounter=Instrument->Duration;      	// set back to start of count ready for next note
			ScoreIdx++;										// point to next note, ready for next time
		}
		else
		{
			// default single beat values
			Instrument->Duration=ChangeNoteEvery;
			Instrument->SoundDuration=Instrument->Duration*0.8;  	// By default a note plays for 80% of tempo
																// this allows for the natural movement of the
																// player performing the next note
			ChangeNoteCounter=ChangeNoteEvery;              	// set back to start of count ready for next note
		}
		// Note that we do not call DacAudio.Play() here as it's already been done for this music score
		// and it's this music score that effectively controls the note playing, however we still need
		// to initialise the instrument for each new note played
		Instrument->Init();
	}
	ChangeNoteCounter--;
	// return the next byte for this instrument
	return Instrument->NextByte();
}




void XT_MusicScore_Class::SetInstrument(uint16_t InstrumentID)
{
	Instrument->SetInstrument(InstrumentID);
}





// Envelope class, see www.xtronical.com for description of how to use envelopes with
// the instrument class

XT_Envelope_Class::XT_Envelope_Class()
{
	// nothing yet
}


void XT_Envelope_Class::Init()
{
	// Reset envelope ready for next note to play
	CurrentEnvelopePart=0;
	EnvelopeCompleted=false;
}

void XT_Envelope_Class::InitEnvelopePart(XT_EnvelopePart_Class* EPart,uint8_t LastVolume)
{
	// initialises the properties in preparation to starting to use this envelope object
	// in note production
	DurationCounter=EPart->RawDuration;
	// calculate how much the volume should increment per sample, this depends on what
	// the last volume reached was for the last envelope part, initially for the first
	// envelope part this would be 0. Volume is in the range 0-127
	VolumeIncrement=float((EPart->TargetVolume-LastVolume))/float(DurationCounter);
}

XT_EnvelopePart_Class* XT_Envelope_Class::AddPart(uint16_t Duration,uint16_t TargetVolume)
{
	// creates and adds an envelope part to this current envelope
	XT_EnvelopePart_Class* EPart=new XT_EnvelopePart_Class();

	EPart->SetDuration(Duration);
	EPart->TargetVolume=TargetVolume;
	if(FirstPart==0)											// First in list of envelope parts
	{
		FirstPart=EPart;
		LastPart=EPart;
	}
	else														// Add to end of list of envelope parts
	{
		LastPart->NextPart=EPart;
		LastPart=EPart;
	}
	return EPart;
}





// Envelope part class
void XT_EnvelopePart_Class::SetDuration(uint16_t Duration)
{
	this->Duration=Duration;
	this->RawDuration=50*Duration;
}

uint16_t XT_EnvelopePart_Class::GetDuration()
{
	return Duration;
}





// Wave type classes

// Square wave

uint8_t XT_SquareWave_Class::NextByte()
{
	// returns the next byte for this frequency of a square wave
	Counter--;
	if(Counter<0)
	{
		Counter+=CounterStartValue;				// as this can be a decimal, any amount extra below zero
												// is taken away from next starting value, in this way
												// for higher frequencies we do not lose as much
												// accuracy over a few waves, they average out to be
												// about right "on average"
		CurrentByte^=255;
		return CurrentByte;
	}
	else
		return CurrentByte;
}

void XT_SquareWave_Class::Init(int8_t Note)
{
	if(Note!=-1)								// use the note not a raw frequency
		Frequency=XT_Notes[Note];  				// if -1 then use the raw frequency
	if(Frequency>25000)
		Frequency=25000;
	if(Frequency!=0)							// avoid divide by 0, a freq of 0 means no sound
	{
		CounterStartValue=(25000/float(Frequency));
		Counter=CounterStartValue;
	}
	else
		Counter=0;
	ChangeAmplitudeBy=255;   					// amount to add on to change wave every time waveform needs changing
												// this value effectivly swaps the value from 0 to 255 and then 255 to 0
}





// Triangle wave functions
// Note that their

uint8_t XT_TriangleWave_Class::NextByte()
{
	// returns the next byte for this frequency of a square wave
	// called at the raw sample rate
	NextAmplitude+=ChangeAmplitudeBy;
	if(NextAmplitude>255)
	{	// top of a peak, reverse direction
		ChangeAmplitudeBy=-ChangeAmplitudeBy;    	// reverse direction
		NextAmplitude=255;
	}
	else
	{
		if(NextAmplitude<0)
		{	// bottom of a trough, reverse direction
			ChangeAmplitudeBy=-ChangeAmplitudeBy;    	// reverse direction
			NextAmplitude=0;
		}
	}
	return int(NextAmplitude);
}

void XT_TriangleWave_Class::Init(int8_t Note)
{
	NextAmplitude=127;
	if(Note!=-1)								// use the note not a raw frequency
		Frequency=XT_Notes[Note];  				// if -1 then use the raw frequency

	if(Frequency>25000)
		Frequency=25000;
	if(Frequency!=0)							// avoid divide by 0, a freq of 0 means no sound
	{
		ChangeAmplitudeBy=256/(25000/float(Frequency));// has to be 25K as has to ramp up and down to return to peak

	}
}




uint8_t XT_SawToothWave_Class::NextByte()
{
	// returns the next byte for this frequency of a square wave

	NextAmplitude+=ChangeAmplitudeBy;
	if(NextAmplitude>255)  // top of a peak, right down to bottom again
		NextAmplitude=0;
	return int(NextAmplitude);
}

void XT_SawToothWave_Class::Init(int8_t Note)
{
	NextAmplitude=0;
	if(Note!=-1)								// use the note not a raw frequency
		Frequency=XT_Notes[Note];  				// if -1 then use the raw frequency

	if(Frequency>25000)
		Frequency=25000;
	if(Frequency!=0)							// avoid divide by 0, a freq of 0 means no sound
		ChangeAmplitudeBy=256/(50000/float(Frequency)); // determines amplitude change per sample
}






///////////////////////////////////////////////
uint8_t XT_SineWave_Class::NextByte()
{
	// returns the next byte for this frequency of a sine wave
	CurrentAngle+=AngleIncrement;
	if(CurrentAngle>255)
		CurrentAngle=0;							// Set to start
	return SineValues[int(CurrentAngle)];
}

void XT_SineWave_Class::Init(int8_t Note)
{
	CurrentAngle=0;
	if(Note!=-1)								// use the note not a raw frequency
		Frequency=XT_Notes[Note];  				// if -1 then use the raw frequency

	if(Frequency>25000)
		Frequency=25000;
	if(Frequency!=0)							// avoid divide by 0, a freq of 0 means no sound
		AngleIncrement=256/(50000/float(Frequency));   // determines frequency
}





//////////////////////////////////////////////
void XT_MultiPlay_Class::Init()
{
	if(FirstItem!=0)
		CurrentItem=FirstItem;
	else
	{
		CurrentItem=0;
		return;
	}
	while(CurrentItem!=0)
	{
		CurrentItem->Init();
		CurrentItem=CurrentItem->NextItem;
	}
	CurrentItem=FirstItem; 					// set back to start
	CurrentItem->Playing=true;				// mark as playing, init is called just prior to
											// first play

}


void XT_MultiPlay_Class::AddPlayItem(XT_PlayListItem_Class *PlayItem)
{
	// we copy the item so there are no conflicts with the item being played by the main Play routine
	// which also uses the linked lists and other properties of the play object etc. This will only
	// be a problem when we implement mixing of sounds.

	//XT_Wav_Class *CopiedItem=PlayItem->Copy();    // not copying yet, for next code release
	XT_PlayListItem_Class *CopiedItem=PlayItem;		// delete this line when copy implemented

	if(FirstItem==0) // no items to play in list yet
	{
		FirstItem=CopiedItem;
		LastItem=CopiedItem;
	}
	else
	{
		// add to end of list
		LastItem->NextItem=CopiedItem;
		CopiedItem->PreviousItem=LastItem;
		LastItem=CopiedItem;

	}
}


uint8_t XT_MultiPlay_Class::NextByte()
{
	if(CurrentItem==0)
		return 0;
	if(CurrentItem->Playing)
		return CurrentItem->NextByte();
	// If we get this far then the current item has stopped playing, time to play next item
	CurrentItem=CurrentItem->NextItem;
	// free up memory used by last playing item
//	delete(CurrentItem->PreviousItem);          // implement when we copy items
	if(CurrentItem!=0)
	{
		CurrentItem->Playing=true;
		return CurrentItem->NextByte();
	}
	else
		Playing=false;					// completed

}
