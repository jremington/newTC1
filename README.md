# newTC1

This repository presents improved data acquistion code as a replacement for nerdaqII code for DIY TC-1-like vertical axis seismometers. 

<img align="right" height="400" src="https://github.com/user-attachments/assets/85fe7691-ba6a-405a-adf2-42fa054a5c9a">

The [nerdaqII code](https://github.com/brianxfury/Low-Cost-Arduino-based-Seismometer-Project/tree/master) employs an odd combination of filters that interact to produce a bizarre oscillatory response to an impulse, which takes a couple of minutes to settle. I don't think it was ever tested properly. This code replaces it with an advanced decimating, downsampling antialiasing filter, which however uses the same oversampling and averaging approach to get 16 bits out of a 10 bit ADC. Four sets of 128 raw data samples are averaged and downsampled+antialiased to produce each data point.

I've added a DC offset removal filter, with a very slow decay (several minutes) so that the final output is zero-centered. The low pass filter response has been validated by testing to be reasonably flat from about 0.1 Hz to 9 Hz, with quite a sharp drop at 9.4 Hz. The impulse response shows none of the oscillatory behavior seen with the nerdaqII code, and no overshoot.

Output sample rate is 18.8 SPS on an Arduino Uno R3 or other ATmega board with 16 MHz clock.
In JamaSeis, select the SEP detector, not the TC1 detector which expects a DC offset of 32768.

For seismometer construction notes see https://www.boisestate.edu/eslab/building-a-tc1-system/construction-of-the-tc1/

# Update 4/25/2025:  
S-wave recording of magnitude 6.3 earthquake near Esmeraldas, Ecuador  

Event: 2025-04-25 11:44:55 (UTC), GPS coords 1.105°N 79.535°W.

Recorded using a DIY TC-1 detector with the posted code, located on west coast U.S. near GPS coordinates 44°N 123°W, **distance to source 6300 km.**
X-axis scale is seconds.

![TC1_20_90](https://github.com/user-attachments/assets/e0e724d2-ad06-4077-bbe6-5fcb71d6b96c)


**Comparison with RaspberryShake**

I've recently spent some time experimenting with a setup very similar to the RaspberryShake (same geophone, SeismicStream 24bit 100 SPS ADC), and have examined data sets available to the public from RaspberryShake stations. 

I am very disappointed in their performance. I have concluded that without great care in filtering the data, the RaspberryShake is **not an improvement** over DIY TC-1-like seismometers. The extra resolution, higher bandwidth and sensitivity of the geophone and 24 bit ADC appear to result mostly in additional noise, actually reducing the signal to noise ratio (SNR). Perhaps the geophone mountings in typical installations are not satisfactory, as I've seen pictures of them just sitting in plastic boxes. Mine sits on the same concrete floor next to the TC-1, in a very quiet location, and gives signals with similar or lower SNR to those from the TC-1, as shown below:

![TC1_SS18](https://github.com/user-attachments/assets/9a2ee325-5ddc-4cc7-890d-dfeb0ad4b2bb)

Below is a screenshot comparing three west coast U.S. stations recording at the time of the 6.3 magnitude event pictured above (4/25/2025 11:45 UTC). The top two seismograms are apparently DIY seismometers: one in Bend, Oregon, the other in Camerillo, California, bottom a RaspberryShake station in Portland, Oregon. The problem is obvious!

![RPi_shake](https://github.com/user-attachments/assets/362a665e-43f9-4991-b02b-1ddb8310f0b5)

If the RaspberryShake data are **low pass filtered** (1s filter in JamaSeis), a signal likely corresponding to that earthquake becomes apparent (yellow highlighting).

![RPi_shake_LP1s](https://github.com/user-attachments/assets/bfc5a99c-2395-4c87-9ff9-fb9e6c51bd73)


