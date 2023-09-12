#pragma once

extern const uint8_t digitToSegment[16];
void tm1637_init(void);
void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos);
