#include "main.h"

extern void Delay_us(uint32_t d);

//adapt pins

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
	dat_0;
	bitDelay();
}

void stop() {

	clk_0;
	hbitDelay();

	dat_0;
	hbitDelay();

	clk_1;
	bitDelay();
	dat_1;
	bitDelay();
}

void writeByte(uint8_t b) {
	uint8_t data = b;

	// 8 Data Bits
	for (uint8_t i = 0; i < 8; i++) {
		// CLK low
		clk_0;
		hbitDelay();

		// Set data bit
		if (data & 0x01)
			dat_1;
		else
			dat_0;

		hbitDelay();

		// CLK high
		clk_1;
		bitDelay();
		data = data >> 1;
	}

	// Wait for acknowledge
	// CLK to zero
	clk_0;
	dat_1;
	bitDelay();

	// CLK to high
	clk_1;
	bitDelay();
	// uint8_t ack = digitalRead(m_pinDIO);

}

#define TM1637_I2C_COMM1    0x40 // data command setting
#define TM1637_I2C_COMM2    0xC0 // display and control (brightness and on/off)
#define TM1637_I2C_COMM3    0x80 // address command setting

const uint8_t digitToSegment[] = {
// segment names starting from top clockwise, last is middle, dot is x
		// XGFEDCBA
		0b00111111,// 0
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

volatile int bright = 0x88;
volatile int set_bright=1;
void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos) {
	// Write COMM1
	start();
	writeByte(TM1637_I2C_COMM1); //write data to disp register, auto address adding, normal mode
	stop();

	// Write COMM2 + first digit address
	start();
	writeByte(TM1637_I2C_COMM2 + (pos & 0x03));

	// Write the data bytes
	for (uint8_t k = 0; k < length; k++)
		writeByte(segments[k]);

	stop();

	if(set_bright){
	// Write COMM3 + brightness + on/off
	start();
	writeByte(TM1637_I2C_COMM3 + (bright & 0x0f));
	stop();
	}

}

void tm1637_init(void) {

	clk_1;
	dat_1;

	setSegments(digitToSegment, 6, 0);
}
