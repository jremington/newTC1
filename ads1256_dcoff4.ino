// ADS1256 24bit ADC, Racotech 4.5Hz geophone used in Raspberry Shake
// 
// https://www.amazon.com/HiLetgo-ADS1256-Acquisition-Precision-Collecting/dp/B09KGXC44Q
// https://mindsetsonline.co.uk/shop/racotech-4-5hz-geophone/
//
// Code based on "Arduino Sample Code to use ADS1256 library"
// library https://github.com/ADS1xxx-Series-ADC-Libraries/ADS1255-ADS1256
//
// ADS1256 data sheet
// http://www.ti.com/lit/ds/symlink/ads1256.pdf
// ADC board from Hiletgo: https://www.amazon.com/HiLetgo-ADS1256-Acquisition-Precision-Collecting/dp/B09KGXC44Q
// single ended version with 80 mV bias on channel 0, 
// added downsampling/decimating code for TC-1 and other DIY seismometers
// best so far! 6/11/2025
// new DC offset removal.

/*
  ADS Board   Arduino UNO Board
  5V  5V
  GND   GND
  SCLK  pin 13 (SCK)
  DIN   pin 11 (MOSI)
  DOUT  pin 12 (MISO)
  DRDY  pin 9
  CS  pin 10
  PDOWN  pin 8
*/

#include <ADS1256.h>
#include <SPI.h>

float clockMHZ = 7.68; // crystal frequency used on ADS1256
float vRef = 2.5; // voltage reference

// Construct and init ADS1256 object
ADS1256 adc(clockMHZ, vRef, false); // RESETPIN is permanently tied to 3.3v

long sensor1;
long filter_state[2] = {0}; //states of downsample filters

// exponential low pass filter and parameters for DC offset removal
// smoothing factor = FILTER_ALPHA
#define FILTER_ALPHA (0.002)  //time constant about 40 seconds at 25 SPS

// DC offset variable
float dc_offset; //value of DC offset

void setup()
{
  Serial.begin(115200);
  while (!Serial) delay(1);

  pinMode(8, OUTPUT); //*PDWN on ADS1256
  digitalWrite(8, 1); //5V

  // start the ADS1256 with data rate and gain, INPUT BUFFER OFF!
  // input impedance = ~240K/gain with buffer off
  
  adc.begin(ADS1256_DRATE_100SPS, ADS1256_GAIN_1, false);

  // other data rates:
  // ADS1256_DRATE_30000SPS
  // ADS1256_DRATE_15000SPS
  // ADS1256_DRATE_7500SPS
  // ADS1256_DRATE_3750SPS
  // ADS1256_DRATE_2000SPS
  // ADS1256_DRATE_1000SPS
  // ADS1256_DRATE_500SPS
  // ADS1256_DRATE_100SPS
  // ADS1256_DRATE_60SPS
  // ADS1256_DRATE_50SPS
  // ADS1256_DRATE_30SPS
  // ADS1256_DRATE_25SPS
  // ADS1256_DRATE_15SPS
  // ADS1256_DRATE_10SPS
  // ADS1256_DRATE_5SPS
  // ADS1256_DRATE_2_5SPS
  //
  // NOTE : Data Rate vary depending on crystal frequency. Data rates listed below assumes the crystal frequency is 7.68Mhz
  //        for other frequency consult the datasheet.
  //Posible Gains
  //ADS1256_GAIN_1
  //ADS1256_GAIN_2
  //ADS1256_GAIN_4
  //ADS1256_GAIN_8
  //ADS1256_GAIN_16
  //ADS1256_GAIN_32
  //ADS1256_GAIN_64

  // Set MUX Register to AINO so it start doing the ADC conversion
  adc.setChannel(0);

  //  estimate DC offset, average 500 ADC samples at 100Hz (5 seconds)
  int32_t sum = 0;
  for (int i = 0; i < 500; i++) {
    adc.waitDRDY(); // wait for DRDY to go low before next register read
    sum += adc.readCurrentChannelRaw(); // read as voltage according to gain and vref
  }
  dc_offset = (sum + 250L) / 500L; //round

//  Serial.print("est DC offset: ");  //debugging
//  Serial.println(dc_offset);
//  dc_offset += 500; //test (removed as expected)

}

unsigned long last = 0;

void loop()
{
  static bool first = true;
  long output_sample[3] = {0}; //array for output (second) downsample step
 // if (first) filter_state[0] = dc_offset; //initialize downsample filter with typical reading
  long filtered_signal, result;

  // cascade of decimating low pass filters
  // https://www.musicdsp.org/en/latest/Filters/214-fast-downsampling-with-antialiasing.html
  // Each source sample is convolved with { 0.25, 0.5, 0.25 } before downsampling.
  last = micros();

  adc.waitDRDY(); // wait for DRDY to go low before next register read
  sensor1 = adc.readCurrentChannelRaw(); // read as voltage according to gain and vref
  output_sample[0] = filter_state[0] + (sensor1 >> 1);

  adc.waitDRDY(); // wait for DRDY to go low before next register read
  sensor1 = adc.readCurrentChannelRaw(); // read as voltage according to gain and vref
  filter_state[0] = sensor1 >> 2;
  output_sample[0] += filter_state[0];

  adc.waitDRDY(); // wait for DRDY to go low before next register read
  sensor1 = adc.readCurrentChannelRaw(); // read as voltage according to gain and vref
  output_sample[1] = filter_state[0] + (sensor1 >> 1);

  adc.waitDRDY(); // wait for DRDY to go low before next register read
  sensor1 = adc.readCurrentChannelRaw(); // read as voltage according to gain and vref
  filter_state[0] = sensor1 >> 2;
  output_sample[1] += filter_state[0];

  // decimate again and output

  output_sample[3] = filter_state[1] + (output_sample[0] >> 1);
  filter_state[1] = output_sample[1] >> 2;
  output_sample[3] += filter_state[1];

  filtered_signal = output_sample[3];
    result = filtered_signal - (int32_t)(dc_offset + 0.5);  //corrected value
 
  // update low pass DC offset
 // dc_offset += DIV_ROUND_CLOSEST((filtered_signal - dc_offset), FILTER_ALPHA);
  dc_offset += (filtered_signal - dc_offset)*FILTER_ALPHA;
  
  if (!first) {
//    Serial.print(filtered_signal); 
//    Serial.print(',');
//    Serial.println(dc_offset); //debugging
//    Serial.print(',');
    Serial.println(result);
  }
  else first = false;
//    first = false;
}
