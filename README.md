# LEAF
LEAF (Lightweight Embedded Audio Framework) is a C library for audio synthesis and processing created by Jeff Snyder, Mike Mulshine, and Matt Wang at Princeton University's New Instrument Research Lab. It was originally called OOPS when we started writing it in 2017, so you may see references to it under that name as well. 

The library consists of a set of high-level audio synthesis components (Oscillators, Filters, Envelopes, Delays, Reverbs, and other Utilities).

Our primary use case is embedded audio computing on 32-bit ARM microcontrollers that can run "bare-metal" (without an OS), such as the STM32f4, STM32f7, and STM32H7. The code, however, is general enough to be used in many other situations as well. We have included a JUCE VST/AU generating template to test the library (2), and the python script we use to generate wavetables. 

Many of these algorithms are sourced from other projects, especially the STK (Sound Toolkit) library and various discussions on the music-DSP mailing list. We also owe a lot to open source computer programming languages, such as C-sound, ChucK, PureData, and Supercollider. 

Other interesting projects to check out that similarly target embedded applicatons are: TeensyAudio (C++), Hoxton Owl (C++), Axoloti (C), and Mutable Instruments (C++). 

Some notes about LEAF:

(1) LEAF has no dependencies on any other code, except for the standard math.h functions like sinf().

(2) Use of standard malloc and calloc are avoided, and a custom memory pool implementation is included instead. This allows dynamic memory allocation/deallocation within a fixed block size, with more control over where the memory is located (very useful for embedded systems). 

(3) The included JUCE template is intended to simplify using LEAF for audio software development aimed at computers or mobile devices. features an easily reconfigurable UI and simple block and tick setup to test the library components. Of course, if you intend to use the provided JUCE plugin project, you need to get JUCE and the Projucer ( https://www.juce.com/get-juce ). Check out the first tutorial to get started - it's fun and easy! 

(4) if you are looking to add LEAF to a System Workbench (SW4STM32) project (the free IDE for developing STM32 embedded firmware) then follow this guide to include LEAF: https://docs.google.com/document/d/1LtMFigQvnIOkRCSL-UVge4GM91woTmVkidlzzgtCjdE/edit?usp=sharing   If you don't want to deal with using leaf as a git submodule, you can also just drop the .c and .h files from LEAF's Src and Inc folders into your own Src and Inc folders, that will work as well - it'll just be a little harder to update things to newer versions of LEAF later on.



///
<h2>
LEAF conventions:
</h2>
We call the psuedo-objects in LEAF "objects" because it's simpler to say, even though technically they are just structs with associated functions. 

Objects types start with a lowercase t: like tCycle, tSawtooth, tRamp, tEnvelopeFollower

All function names start with the object type name, followed by an underscore, then the function name in camel-case: like tCycle_setFreq(), tRamp_setDest().

The first input parameter of all LEAF functions associated with an object type is the object instance it is operating on. 

LEAF assumes a global sample rate (passed into LEAF when the library itself is initialized). You can change this sample rate whenever you want, and all objects that use the global sample rate will see the change.

All LEAF objects operate on single-precision float input and output data. There are sometimes double-precision operations inside LEAF objects, but only when the precision is deemed to actually be necessary for the operation. Float literal notation (like 0.12f instead of 0.12) is used to force computation using single precision when it would otherwise be ambigious, to make LEAF more efficient for processors that have a single-precision FPU, or where the FPU computes single-precision faster than double-precision.

Audio inputs and outputs are assumed to be between -1.0 and 1.0

Internally, LEAF objects that the user defines globally (by writing something like "tCycle mySine") are actually just pointers. The structs for the data of the LEAF object is created in the mempool when the init function is called on that pointer (as in tCycle_init(&mySine)). For instance, when you create a tCycle, you are actually just creating a pointer, and when you call tCycle_init() on that tCycle, you are then creating a struct that has real data inside the mempool. This is done to make the footprint of the LEAF objects very small outside of their designated mempools, so that a large number of LEAF object pointers can exist globally even if only a few complete objects can exist in memory at a single time. 

All LEAF objects must have an init() function and a free() function -- these will create the objects inside the default mempool, as well as an initToPool() and freeFromPool() function -- these will create the objects inside a user-defined specific mempool that is not the default. 

LEAF objects assume that they will be "ticked" once per sample, and generally take single sample input and produce single sample output. The alternative would be to have the user pass in an array and have the objects operate on the full array, which could have performance advantages if SIMD instructions are available on the processor, but would have disadvantages in flexibility of use. If an audio object requires some kind of buffer to operate on (such as a pitch detector) it will collect samples in its sample-by-sample tick function and store them in its own internal buffer. 


////



<h2>Example of using LEAF:</h2>


```
//in your user code, create an instance of a leaf object

tCycle mySine;


//also, create a memory pool array where you can store the data for the LEAF objects. It should be an array of chars. Note that you can also define multiple mempool objects in different memory locations in you want to, and initialize different LEAF objects in different places. However, LEAF needs at least one mempool, so the one you pass into the LEAF_init function is considered the default location for any LEAF-related memory, unless you specifically put something elsewhere by using an initToPool() function instead of init(). LEAF object store a pointer to which mempool they are in, so if you initToPool it will remember where you put it and the free() function will free it from the correct location.

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
```

