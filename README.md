# newTC1

This repository presents improved data acquistion code as a replacement for nerdaqII code for DIY TC-1-like vertical axis seismometers.

# Update 6/11/2025  
Added code for Hiltego ADS1256 24 bit ADC and Racotech geophone, used in Raspberry Shake
https://mindsetsonline.co.uk/shop/racotech-4-5hz-geophone/
Comparable or better performance to TC1.
 
<img align="right" height="400" src="https://github.com/user-attachments/assets/85fe7691-ba6a-405a-adf2-42fa054a5c9a">

The [nerdaqII code](https://github.com/brianxfury/Low-Cost-Arduino-based-Seismometer-Project/tree/master) employs an odd combination of filters that interact to produce a bizarre oscillatory response to an impulse, which takes a couple of minutes to settle. I don't think it was ever tested properly. The code posted here replaces it with an advanced decimating, downsampling antialiasing filter, which however uses the same oversampling and averaging approach to (theoretically) get 16 bits resolution from a 10 bit ADC. Four sets of 128 raw data samples are averaged and downsampled+antialiased to produce each data point.

I've added a DC offset removal filter, with a very slow decay (several minutes) so that the final output is zero-centered. The low pass filter response has been validated by testing to be reasonably flat from about 0.1 Hz to 9 Hz, with quite a sharp drop at 9.4 Hz. The impulse response shows none of the oscillatory behavior seen with the nerdaqII code, and no overshoot.

Output sample rate is 18.8 SPS on an Arduino Uno R3 or other ATmega board with 16 MHz clock.
In JamaSeis, select the SEP detector, not the TC1 detector which expects a DC offset of 32768.

For seismometer construction notes see https://www.boisestate.edu/eslab/building-a-tc1-system/construction-of-the-tc1/

# Update 4/25/2025:  
P-wave recording of magnitude 6.3 earthquake near Esmeraldas, Ecuador  

Event: 2025-04-25 11:44:55 (UTC), GPS coords 1.105°N 79.535°W.

Recorded using the DIY TC-1 detector pictured above with the posted code, located on west coast U.S. near GPS coordinates 44°N 123°W, **distance to source 6300 km.**
X-axis scale is seconds.

![TC1_20_90](https://github.com/user-attachments/assets/e0e724d2-ad06-4077-bbe6-5fcb71d6b96c)


**Comparison with RaspberryShake**

I've recently spent some time experimenting with a setup very similar to the RaspberryShake (same 4.5 Hz geophone, SeismicStream 24bit, 25 - 100 SPS ADC module), and have examined data sets available to the public from RaspberryShake stations. 

I am disappointed in their performance. I have concluded that without great care in filtering the data, the RaspberryShake is **not an improvement** over DIY TC-1-like seismometers, at least in very quiet locations like mine. The extra resolution, higher bandwidth and sensitivity of the geophone and 24 bit ADC appear to result mostly in additional noise, actually reducing the signal to noise ratio (SNR) for low frequency signals from distant earthquakes.

I'm not the first to comment on this, for example see this summary at: https://soylentnews.org/submit.pl?op=viewsub&subid=64699
The author's solution was a DIY swinging boom seismometer with very low frequency response: https://spectrum.ieee.org/build-a-better-diy-seismometer

My detectors sit on the concrete floor of an unoccupied building far from any roads in a very quiet location, and a comparison for the Ecuador event above shows that the geophone/SeismicStream response (SS18, top) has similar or lower SNR to that from the TC-1 (bottom), as shown below:

![TC1_SS18](https://github.com/user-attachments/assets/9a2ee325-5ddc-4cc7-890d-dfeb0ad4b2bb)

Below is a screenshot comparing three west coast U.S. station recordings at the time of the 6.3 magnitude event pictured above (4/25/2025 11:45 UTC). The top two seismograms are apparently DIY seismometers: one in Bend, Oregon, the other in Camerillo, California, bottom a RaspberryShake station in Portland, Oregon. The problem is pretty obvious!

![RPi_shake](https://github.com/user-attachments/assets/362a665e-43f9-4991-b02b-1ddb8310f0b5)

Only when the RaspberryShake data are **externally** low pass filtered (post-processed, in this case applying the 5s filter in JamaSeis), does a signal corresponding to that earthquake become readily apparent.

![RPiShake_5s](https://github.com/user-attachments/assets/79e27686-0a3c-437f-99a8-a4c933cefa1d)




