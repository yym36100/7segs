#include "main.h"
#include "stm32f4xx_ll_gpio.h"

extern void Delay_us(uint32_t d);

// 444opt

//adapt pins

//todo: instead of doing this, use open drain port instead
// not low level enough
#if 1
static void pinmode_clock(int mode) {
	LL_GPIO_SetPinMode(seg_clock_GPIO_Port, seg_clock_Pin, 1-mode);
}
static void pinset_clock(int val) {
	HAL_GPIO_WritePin(seg_clock_GPIO_Port, seg_clock_Pin, val);
}

static void pinmode_data(int mode) {
	LL_GPIO_SetPinMode(seg_data_GPIO_Port, seg_data_Pin, 1-mode);
}
static void pinset_data(int val) {
	HAL_GPIO_WritePin(seg_data_GPIO_Port, seg_data_Pin, val);
}
#endif
#define clk_1	seg_clock_GPIO_Port->BSRR = seg_clock_Pin
#define clk_0	seg_clock_GPIO_Port->BSRR = seg_clock_Pin<<16

#define dat_1	seg_data_GPIO_Port->BSRR = seg_data_Pin
#define dat_0	seg_data_GPIO_Port->BSRR = seg_data_Pin<<16


void bitDelay() {
	Delay_us(200);
}

void hbitDelay() {
	Delay_us(100);
}

void start() {
	//pinmode_data(0);
	dat_0;
	bitDelay();
}

void stop() {
	//pinmode_clock(0);
	clk_0;
	hbitDelay();
	//pinmode_data(0);
	dat_0;
	hbitDelay();

	//pinmode_clock(1);
	clk_1;
	bitDelay();
	dat_1;
	//pinmode_data(1);
	bitDelay();
}

void writeByte(uint8_t b) {
  uint8_t data = b;

  // 8 Data Bits
  for(uint8_t i = 0; i < 8; i++) {
    // CLK low
	  //pinmode_clock(0);
	  clk_0;
	  hbitDelay();

	// Set data bit
    if (data & 0x01) dat_1;//pinmode_data(1);
    else dat_0;//pinmode_data(0);

    hbitDelay();

	// CLK high
    //pinmode_clock(1);
    clk_1;
    bitDelay();
    data = data >> 1;
  }


  // Wait for acknowledge
  // CLK to zero
  //pinmode_clock(0);
  clk_0;
  //pinmode_data(1);
  dat_1;
  bitDelay();

  // CLK to high
  //pinmode_clock(1);
  clk_1;
  bitDelay();
 // uint8_t ack = digitalRead(m_pinDIO);
 // if (ack == 0)
  //pinmode_data(0);

/*
  bitDelay();
  pinmode_clock(0);
  bitDelay();
  */

}

#define TM1637_I2C_COMM1    0x40
#define TM1637_I2C_COMM2    0xC0
#define TM1637_I2C_COMM3    0x80


const uint8_t digitToSegment[] = {
 // XGFEDCBA
  0b00111111,    // 0
  0b00000110,    // 1
  0b01011011,    // 2
  0b01001111,    // 3
  0b01100110,    // 4
  0b01101101,    // 5
  0b01111101,    // 6
  0b00000111,    // 7
  0b01111111,    // 8
  0b01101111,    // 9
  0b01110111,    // A
  0b01111100,    // b
  0b00111001,    // C
  0b01011110,    // d
  0b01111001,    // E
  0b01110001     // F
  };

void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos) {
    // Write COMM1
	start();
	writeByte(TM1637_I2C_COMM1);
	stop();

	// Write COMM2 + first digit address
	start();
	writeByte(TM1637_I2C_COMM2 + (pos & 0x03));

	// Write the data bytes
	for (uint8_t k=0; k < length; k++)
	  writeByte(segments[k]);

	stop();

	// Write COMM3 + brightness
	start();
	writeByte(TM1637_I2C_COMM3 + (5 & 0x0f));
	stop();

	start();
		writeByte(0x88);
		stop();
}

void tm1637_init(void) {
	//pinmode_clock(1);
	//pinmode_data(1);
	//pinset_clock(0);
	//pinset_data(0);
	clk_1;
	dat_1;

	setSegments(digitToSegment,6,0);
}
