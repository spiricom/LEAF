# LEAF
LEAF (Lightweight Embedded Audio Framework) is a C library for Audio synthesis and processing created by Jeff Snyder, Mike Mulshine, and Matt Wang. It was originally called OOPS when we started writing it in 2017, so you may see references to it under that name as well. 

The library consists of a set of high-level audio synthesis components (Oscillators, Filters, Envelopes, Delays, Reverbs, and other Utilities).

Our primary use case is embedded audio computing on 32-bit ARM microcontrollers that can run "bare-metal" (without an OS), such as the STM32f4, STM32f7, and STM32H7. The code, however, is general enough to be used in many other situations as well. We have included a JUCE VST/AU generating template to test the library (2), and the python script we use to generate wavetables. 

Many of these algorithms are sourced from other projects, especially the STK (Sound Toolkit) library and various discussions on the music-DSP mailing list. We also owe a lot to open source computer programming languages, such as C-sound, ChucK, PureData, and Supercollider. 

Other interesting projects to check out that similarly target embedded applicatons are: TeensyAudio (C++), Hoxton Owl (C++), Axoloti (C), and Mutable Instruments (C++). 

(1) Use of standard malloc and calloc are avoided, and a custom memory pool implementation is included instead, allowing dynamic memory allocation/deallocation within a fixed block size.

(2) The included JUCE template is intended to simplify using LEAF for audio software development aimed at computers or mobile devices. features an easily reconfigurable UI and simple block and tick setup to test the library components. Of course, if you intend to use the provided JUCE plugin project, you need to get JUCE and the Projucer ( https://www.juce.com/get-juce ). Check out the first tutorial to get started - it's fun an easy! 

(3) if you are looking to add LEAF to a System Workbench (SW4STM32) project (the free IDE for developing STM32 embedded firmware) then follow this guide: https://docs.google.com/document/d/1LtMFigQvnIOkRCSL-UVge4GM91woTmVkidlzzgtCjdE/edit?usp=sharing



///
LEAF conventions:

Objects types start with a lowercase t: like tCycle, tSawtooth, tRamp, tEnvelopeFollower

All function names start with the object type name, followed by an underscore, then the function name in camel-case: like tCycle_setFreq(), tRamp_setDest()

LEAF assumes a global sample rate (passed into LEAF when the library itself is initialized). 
////



Example of using LEAF:

//in your user code, create an instance of a leaf object

tCycle mySine;


//also, create a mempool object where you can store the data for the LEAF objects. It should be an array of chars

#define MEM_SIZE 500000
char myMemory[MEM_SIZE];


//we'll assume your code has some kind of audio buffer that is transmitting samples to an audio codec or an operating system's audio driver. In this example, let's define this here.

#define AUDIO_BUFFER_SIZE 128
float audioBuffer[AUDIO_BUFFER_SIZE];


//then initialize the whole LEAF library (this only needs to be done once, it sets global parameters like the default mempool and the sample rate)
//the parameters are: sample rate, audio buffer size in samples, name of mempool array, size of mempool array, and address of a function to generate a random number. In this case, there is a function called randomNumber that exists elsewhere in the user code that generates a random floating point number from 0.0 to 1.0. We ask the user to pass in a random number function because LEAF has no dependencies, and users developing on embedded systems may want to use a hardware RNG, for instance.

LEAF_init(48000, AUDIO_BUFFER_SIZE, myMemory, MEM_SIZE, &randomNumber);


//now initialize the object you want to use, in this case the sine wave oscillator you created above.

tCycle_init(&mySine);


//set the frequency of the oscillator (defaults to zero). In a real use case, you'd probably want to be updating this to new values in the audio frame based on knob positions or midi data or other inputs, but here we'll assume it stays fixed.

tCycle_setFreq(&mySine, 440.0);


//now, in your audio callback (a function that will be called every audio frame, to compute the samples needed to fill the audio buffer) tick the LEAF audio object to generate or process audio samples. 

void audioFrame()
{
  for (int i = 0; i < AUDIO_BUFFER_SIZE; i++)
  {
    audioBuffer[i] = tCycle_tick(&mySine);
  }
}


