/**
 * From http://arduino.cc/forum/index.php/topic,64800.msg479422.html#msg479422
 */
#ifndef PIN_TRAITS_H
#define PIN_TRAITS_H

#include <stdint.h>

#define __digital_high(L, N) PORT ## L |= _BV(N);
#define __digital_low(L, N) PORT ## L &= ~_BV(N);

inline void digital_write(boolean state);

template <uint8_t Pin> class pin_traits;

#define PIN_TRAITS_GENERATOR(L, N, P)       \
template <> class pin_traits<P>             \
{                                           \
  public:                                   \
  static void digital_write(boolean state)  \
  {                                         \
    if (state == HIGH)                      \
      __digital_high(L, N)                  \
    else                                    \
      __digital_low(L, N)                   \
  }                                         \
};

// Digital pins 0 - 7 are part of the D group
PIN_TRAITS_GENERATOR(D, 0, 0)
PIN_TRAITS_GENERATOR(D, 1, 1)
PIN_TRAITS_GENERATOR(D, 2, 2)
PIN_TRAITS_GENERATOR(D, 3, 3)
PIN_TRAITS_GENERATOR(D, 4, 4)
PIN_TRAITS_GENERATOR(D, 5, 5)
PIN_TRAITS_GENERATOR(D, 6, 6)
PIN_TRAITS_GENERATOR(D, 7, 7)

// Digital pins 8 - 13 are part of the B group
PIN_TRAITS_GENERATOR(B, 0, 8)
PIN_TRAITS_GENERATOR(B, 1, 9)
PIN_TRAITS_GENERATOR(B, 2, 10)
PIN_TRAITS_GENERATOR(B, 3, 11)
PIN_TRAITS_GENERATOR(B, 4, 12)
PIN_TRAITS_GENERATOR(B, 5, 13)

#undef PIN_TRAITS_GENERATOR

#endif /* PIN_TRAITS_H */

/** Same as digitalWrite(), except the pins need to be known at compile
    time.  This function is roughly 10 times faster than digitalWrite().
*/
template <uint8_t N>
inline void digital_write(boolean state)
{
  pin_traits<N>::digital_write(state);
}
