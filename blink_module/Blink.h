#ifndef BLINK_H
#define BLINK_H

#define RED_PIN            0
#define GREEN_PIN          9
#define YELLOW_PIN         10   
#define WHITE_PIN          8
#define PURPLE_PIN         1
#define RGB_G_PIN          2
#define RGB_B_PIN          3
#define RGB_R_PIN          5

#define RED_MASK          (1 << 0)
#define YELLOW_MASK       (1 << 1)
#define GREEN_MASK        (1 << 2)
#define PURPLE_MASK       (1 << 3)
#define WHITE_MASK        (1 << 4)

#define SET_LED_MASK      (1 << 5)
#define SET_RGB_MASK      (1 << 6)
#define SET_BLINK_MASK    (1 << 7)
#define IS_SET(v, m)      (((v) & (m)) == m)

#define I2C_SLAVE_ADDR     0x21

#define BLINK_SET_LED(p)   ((p) | SET_LED_MASK)
#define BLINK_ALL_ON       0xff
#define BLINK_ALL_OFF      0x00

#endif

