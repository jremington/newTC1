// new dc offset removal 6/11/2025
// best yet.
// oversampling ADC for TC1 and related DIY seismometers for Arduino Uno R3 and the like.
// Started with nerdaqII and replaced the filtering with a
// fast downsampling (decimating with antialiasing) algorithm in a cascade
// https://www.musicdsp.org/en/latest/Filters/214-fast-downsampling-with-antialiasing.html
// Each source sample is convolved with { 0.25, 0.5, 0.25 } before downsampling.
// for a 16 MHz AVR clock, the ADC rate is 9615.4 sps
// output sample rate is 18.8 SPS, zero centered.
// S. James Remington 4/2025


#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned long runningsum;
volatile unsigned int runningcount;
volatile unsigned long current_sum;

volatile boolean next_sample_ready;
volatile unsigned int next_sample;

// low pass filter and parameters for DC offset removal
// smoothing factor for DC offset = FILTER_ALPHA
#define FILTER_ALPHA (0.003)  //time constant about 40 seconds at 18 SPS

// DC offset variable
float dc_offset; //value of DC offset

int result = 0;

void initADC() {

  runningsum = 0;
  runningcount = 0;
  current_sum = 0;

  next_sample_ready = false;
  next_sample = 0;

  // internal AVcc ref, no left adj, channel 0
  ADMUX = _BV(REFS0);
  DIDR0 = 0x01; // turn off the digital input for adc0

  // ACME off, free-running mode
  ADCSRB = 0;

  // enable ad, intr, auto
  // set divisor to 128
  // start first conversion
  ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADATE)
           | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2)
           | _BV(ADSC);
}


ISR(ADC_vect) {
  static int counter = 0;
  unsigned int low = ADCL;
  unsigned int high = ADCH;

  runningsum += low + (high << 8);
  if (++runningcount == 128) {

    current_sum = runningsum;
    next_sample =
      (unsigned int) (current_sum >> 1);  //oversample and average to 16 bit result
    runningsum = 0;
    runningcount = 0;
    next_sample_ready = true;
  }
}


void setup() {
  Serial.begin(115200);
  while (!Serial) delay(1);
  //  delay(1000); //wait for connection
  initADC();
  //  Serial.println("starting");
  //  estimate DC offset, average 500x128 ADC samples at 75Hz (~7 seconds)
  int32_t sum = 0;
  for (int i = 0; i < 500; i++) {
    while (!next_sample_ready); //wsit for a sample
    next_sample_ready = false;
    sum += next_sample;
  }
  dc_offset = (sum + 250L) / 500L;
  // Serial.print("DC offset: ");
  // Serial.println(dc_offset);
  //  dc_offset += 500; //test (removed as expected)
}

long int filter_state[2] = {0}; //states of downsample filters

void loop() {

  static int first = true; //skip printing first filtered value

  unsigned long output_sample[3] = {0}; //array for output (second) downsample step

  unsigned int filtered_signal;

  // cascade of decimating low pass filters
  // https://www.musicdsp.org/en/latest/Filters/214-fast-downsampling-with-antialiasing.html
  // Each source sample is convolved with { 0.25, 0.5, 0.25 } before downsampling.

  while (!next_sample_ready);  //wait for new sample
  next_sample_ready = false;
  output_sample[0] = filter_state[0] + (next_sample >> 1);

  while (!next_sample_ready);  //wait for new sample
  next_sample_ready = false;
  filter_state[0] = next_sample >> 2;
  output_sample[0] += filter_state[0];

  while (!next_sample_ready);  //wait for new sample
  next_sample_ready = false;
  output_sample[1] = filter_state[0] + (next_sample >> 1);

  while (!next_sample_ready);  //wait for new sample
  next_sample_ready = false;
  filter_state[0] = next_sample >> 2;
  output_sample[1] += filter_state[0];

  // decimate again and output

  output_sample[3] = filter_state[1] + (output_sample[0] >> 1);
  filter_state[1] = output_sample[1] >> 2;
  output_sample[3] += filter_state[1];

  filtered_signal = (unsigned int) output_sample[3];
  result = filtered_signal - (int)(dc_offset + 0.5);  //corrected value
 
  // update low pass DC offset
  dc_offset += (filtered_signal - dc_offset)*FILTER_ALPHA;

  //  Serial.print(filtered_signal);
  //  Serial.print(", ");
  if (!first) Serial.println(result);
  else first = false;

}
