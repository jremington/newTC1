# newTC1
Replacement for nerdaqII seismomenter code for DIY TC1-like seismometers. For seismometer construction notes see 

https://www.boisestate.edu/eslab/building-a-tc1-system/construction-of-the-tc1/

The [nerdaqII code](https://github.com/brianxfury/Low-Cost-Arduino-based-Seismometer-Project/tree/master) is an odd combination of filters that interact to produce a bizarre oscillatory response to an impulse, which takes a couple of minutes to settle. This code replaces it with an advanced decimating, downsampling antialiasing filter, however uses the same oversampling and averaging approach to get 16 bits out of a 10 bit DAC.

I've added a slow DC offset removal filter, with a very long decay rate so that the final output is zero-centered. The low pass filter response is 0.1 Hz to 9 Hz, with quite a sharp drop at 10 Hz. Output sample rate is 18.8 SPS.
 
In JamaSeis, select the SEP detector, not the TC1 detector which expects a DC offset of about 32768.
