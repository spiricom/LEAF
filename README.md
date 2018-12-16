# LEAF
Previously named OOPS, LEAF is a C library for Audio synthesis and processing created by Mike Mulshine and Jeff Snyder.

The library consists of a set of high-level audio synthesis components (Oscillators, Filters, Envelopes, Delays, Reverbs, and other Utilities).

Our primary use case is embedded audio computing on 32-bit ARM microcontrollers that can run "bare-metal" (without an OS), such as the STM32f4 and STM32f7. The code, however, is general enough to be used in many other situations as well. We have included a JUCE VST/AU generating template to test the library (2), and the python script we use to generate wavetables. 

Most of these algorithms are sourced from other projects, especially the STK (Sound Toolkit) library and various discussions on the music-DSP mailing list. We also owe a lot to open source computer programming languages, such as C-sound, ChucK, PureData, and Supercollider. 

Other interesting projects to check out that similarly target embedded applicatons are: TeensyAudio (C++), Hoxton Owl (C++), Axoloti (C), and Mutable Instruments (C++). 

(1) Dynamic allocation is avoided to meet the needs of a larger number of embedded applications. Embedded developers often utilize architectures that don't fully support stdlib memory allocation functions like calloc, malloc, and free.

(2) The template features an easily reconfigurable UI and simple block and tick setup to test the library components. Of course, if you intend to use the provided JUCE plugin project, you need to get JUCE and the Projucer ( https://www.juce.com/get-juce ). Check out the first tutorial to get started - it's fun an easy! If you intend to include the LEAF framework in your own C++ project using JUCE or other platforms, you will probably need to rename each source file from .c to .cpp. This should be straightforward. Contact the developer if you have any troubles (mrmulshine@gmail.com).
