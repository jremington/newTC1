# newTC1

This repository presents an improved replacement for nerdaqII seismomenter code for DIY TC-1-like seismometers. 

For seismometer construction notes see https://www.boisestate.edu/eslab/building-a-tc1-system/construction-of-the-tc1/

The [nerdaqII code](https://github.com/brianxfury/Low-Cost-Arduino-based-Seismometer-Project/tree/master) is an odd combination of filters that interact to produce a bizarre oscillatory response to an impulse, which takes a couple of minutes to settle. I don't think it was ever tested properly. This code replaces it with an advanced decimating, downsampling antialiasing filter, which however uses the same oversampling and averaging approach to get 16 bits out of a 10 bit ADC. Four sets of 128 raw data samples are averaged and downsampled+antialiased to produce each data point.

I've added a slow DC offset removal filter, with a very slow decay (several minutes) so that the final output is zero-centered. The low pass filter response has been validated by testing to be reasonably flat from about 0.1 Hz to 9 Hz, with quite a sharp drop at 9.4 Hz. 

Output sample rate is 18.8 SPS on an Arduino Uno R3 or other ATmega board with 16 MHz clock.
 
In JamaSeis, select the SEP detector, not the TC1 detector which expects a DC offset of 32768.

# UPDATE 4/25/2025:  
S-wave recording from distance of 6300 km of magnitude 6.3 earthquake
near Esmeraldas, Ecuador on 2025-04-25 11:44:55 (UTC), GPS coords 1.105°N 79.535°W.
Recorded using a DIY TC-1 detector with the posted code, located on west coast U.S. near GPS coordinates 44°N 123°W
X-axis scale is seconds.

![ksnip_20250425-120444ed](https://github.com/user-attachments/assets/884d0725-39b6-4b96-8ff3-9fd08030f7e6)

**Comparison with RaspberryShake**
Incidentally, I've spent some time playing around with a RaspberryShake setup, and was very disappointed. I have concluded that **it is not an improvement** over DIY TC-1 like seismometers. The extra resolution and sensitivity of the geophone and 24 bit ADC seems to result in additional noise, with little or no improvement in signal to noise ratio (SNR).

Below is a screenshot comparing three stations recording at the time of the 6.3 magnitiude event pictured above, (top two are DIY seismometers, bottom a RaspberryShake station. It is not hard to see the problem!


![RPi_shake](https://github.com/user-attachments/assets/362a665e-43f9-4991-b02b-1ddb8310f0b5)

