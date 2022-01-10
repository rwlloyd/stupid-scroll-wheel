# stupid-scroll-wheel

Recent work with brushless motors in conjunction with the youtube video linked below has led to this little mini project, using a hacked hard drive motor as a fancy scroll wheel for general use on a PC.

https://www.youtube.com/watch?v=tjCJ3MlFt7g

## Required Hardware
---

- Old mechanical hard drive
- Arduino Pro Micro (ATmega32U4 based board)

It should be noted that other microcontrollers could be used as long at the have the capability to act as a HID device and are supported in the Arduino ecosystem.

## Wiring
---

After watching the video linked above, the wiring will be much clearer. 

After deciphering the pinout of your hard drive motor, connect the motor to the microcontroller in accordance with the table below.

| Motor Output | Arduino Pin |
| :----: | :----: |
| GND | GND |
| A | A0 |
| B | A1 |
| C | A2 |

That's it! Everything else is handled by the microcontroller.

## Flashing Firmware
---

Upload the finmware to the microcontroller in the usual way.

##  Modifying the code for Horizontal Scrolling
---

The code is currently set up for scrolling in the vertical direction. Change 
    
    Keyboard.press(KEY_UP_ARROW);

and

    Keyboard.press(KEY_DOWN_ARROW);

In the `loop()` to `KEY_LEFT_ARROW` and `KEY_RIGHT_ARROW`.

If you're having trouble with the scroll going too fast increase the delay at the end of the main loop.  