# LEAF
LEAF (Lightweight Embedded Audio Framework) is a C library for Audio synthesis and processing created by Jeff Snyder, Mike Mulshine, and Matt Wang. It was originally called OOPS when we started writing it in 2017, so you may see references to it under that name as well. 

The library consists of a set of high-level audio synthesis components (Oscillators, Filters, Envelopes, Delays, Reverbs, and other Utilities).

Our primary use case is embedded audio computing on 32-bit ARM microcontrollers that can run "bare-metal" (without an OS), such as the STM32f4, STM32f7, and STM32H7. The code, however, is general enough to be used in many other situations as well. We have included a JUCE VST/AU generating template to test the library (2), and the python script we use to generate wavetables. 

Many of these algorithms are sourced from other projects, especially the STK (Sound Toolkit) library and various discussions on the music-DSP mailing list. We also owe a lot to open source computer programming languages, such as C-sound, ChucK, PureData, and Supercollider. 

Other interesting projects to check out that similarly target embedded applicatons are: TeensyAudio (C++), Hoxton Owl (C++), Axoloti (C), and Mutable Instruments (C++). 

(1) Use of standard malloc and calloc are avoided, and a custom memory pool implementation is included instead, allowing dynamic memory allocation/deallocation within a fixed block size.

(2) The included JUCE template is intended to simplify using LEAF for audio software development aimed at computers or mobile devices. features an easily reconfigurable UI and simple block and tick setup to test the library components. Of course, if you intend to use the provided JUCE plugin project, you need to get JUCE and the Projucer ( https://www.juce.com/get-juce ). Check out the first tutorial to get started - it's fun an easy! 

(3) if you are looking to add LEAF to a System Workbench (SW4STM32) project (the free IDE for developing STM32 embedded firmware) then follow this guide: https://docs.google.com/document/d/1LtMFigQvnIOkRCSL-UVge4GM91woTmVkidlzzgtCjdE/edit?usp=sharing


