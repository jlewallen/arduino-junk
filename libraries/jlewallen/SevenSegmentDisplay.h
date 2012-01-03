#ifndef SevenSegmentDisplay_h
#define SevenSegmentDisplay_h

const static boolean SEVEN_SEGMENT_DISPLAY_ENCODING[11][8] = {
  { 0, 0, 0, 0, 0, 0, 1, 1 }, // 0
  { 1, 0, 0, 1, 1, 1, 1, 1 }, // 1
  { 0, 0, 1, 0, 0, 1, 0, 1 }, // 2
  { 0, 0, 0, 0, 1, 1, 0, 1 }, // 3
  { 1, 0, 0, 1, 1, 0, 0, 1 }, // 4
  { 0, 1, 0, 0, 1, 0, 0, 1 }, // 5
  { 0, 1, 0, 0, 0, 0, 0, 1 }, // 6
  { 0, 0, 0, 1, 1, 1, 1, 1 }, // 7
  { 0, 0, 0, 0, 0, 0, 0, 1 }, // 8
  { 0, 0, 0, 0, 1, 0, 0, 1 }, // 9

  { 0, 1, 1, 0, 0, 0, 0, 1 }, // E
};

class SevenSegmentDisplay {
  public:
    static const boolean *forDigit(char c) {
      int i = c - '0';
      if (i >= 0 && i < 10) {
        return SEVEN_SEGMENT_DISPLAY_ENCODING[i];
      }
      return SEVEN_SEGMENT_DISPLAY_ENCODING[10];
    }
};

#endif
