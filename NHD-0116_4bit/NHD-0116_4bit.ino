/*****************************************************************************
 *
 * Program for writing to Newhaven Display's 1x16 Character LCD with the ST7066U Controller.
 * This code is written for the Arduino Uno R3 using 4-bit Parallel Interface.
 *
 * Program Loop:
 * 1. Initialize display
 * 2. Print "NEWHAVEN DISPLAY".
 * 
 * Newhaven Display invests time and resources providing this open source code,
 * Please support Newhaven Display by purchasing products from Newhaven Display!
 *
 * Copyright (c) 2022, Newhaven Display International
 *
 * This code is provided as an example only and is not guaranteed by Newhaven Display.
 * Newhaven Display accepts no responsibility for any issues resulting from its use.
 * The developer of the final application incorporating any parts of this
 * sample code is responsible for ensuring its safe and correct operation
 * and for any consequences resulting from its use.
 *
 *****************************************************************************/

/**
 * Wiring Reference:
 * - Pin 1 VSS - GND
 * - Pin 2 VDD - 5.0V
 * - Pin 3 V0 - Contrast potentiometer (refer to spec wiring diagram)
 * - Pin 4 RS - Arduino D8
 * - Pin 5 R/W - Arduino D9
 * - Pin 6 E - Arduino D10
 * - Pin 7 DB0 - NC
 * - Pin 8 DB1 - NC
 * - Pin 9 DB2 - NC
 * - Pin 10 DB3 - NC
 * - Pin 11 DB4 - Arduino D4
 * - Pin 12 DB5 - Arduino D5
 * - Pin 13 DB6 - Arduino D6
 * - Pin 14 DB7 - Arduino D7
 * - Pin 15 LED+ - 5.0V
 * - Pin 16 LED- - GND
 */

#include <stdint.h>

#define E_Pin 10
#define R_W 9
#define R_S 8

char const _text1[] = ("NEWHAVEN");
char const _text2[] = (" DISPLAY");

void setup()
{
  initLCD();
  disp();
}

void loop()
{
}

/**
 * @brief Clear the R/W pin to write data.
 * 
 * @return none
 */
void setWrite()
{
  digitalWrite(R_W, LOW); // Write
}

/**
 * @brief Clear the RS pin for commands.
 * 
 * @return none
 */
void setCommand()
{
  digitalWrite(R_S, LOW); // Command
}

/**
 * @brief Set the RS pin for data.
 * 
 * @return none
 */
void setData()
{
  digitalWrite(R_S, HIGH); // Data
}

/**
 * @brief Toggle enable signal to latch data.
 * @return none
 */
void latch()
{                            
  digitalWrite(E_Pin, HIGH);
  delay(1);
  digitalWrite(E_Pin, LOW);
  delay(20);
}

/**
 * @brief Send a command byte to the display.
 * 
 * @param command Desired command byte to send.
 * @return none
 */
void command(uint8_t command)
{
  setCommand();
  setWrite();

  putByte(command);
}

/**
 * @brief Send a data byte to the display.
 * 
 * @param data Desired data byte to send.
 * @return none
 */
void data(uint8_t data)
{
  setData();
  setWrite();

  putByte(data);
}

/**
 * @brief Clock a byte of data on the 4-bit parallel bus.
 * 
 * @param data Desired data byte to send.
 * @return none
 */
void putByte(uint8_t data)
{
  putNybble(data);
  latch();
  data <<= 4;
  putNybble(data);
  latch();
}

/**
 * @brief Put a nybble of data on the 4-bit parallel bus.
 * 
 * @param data Desired data to put on bus.
 * @return none
 */
void putNybble(uint8_t data)
{
  PORTD = data;
}

/**
 * @brief Initialize the connected LCD.
 * 
 * @return none
 */
void initLCD()
{
  DDRD = 0xFF; // First 8 pins of PORT D as output
  DDRB = 0x07; // Port B first 3 pins as output
  digitalWrite(E_Pin, LOW);

  delay(100); // Wait >15 msec after power is applied

  putNybble(0x30); // Put 30 on port D
  delay(30);     // must wait 5ms, busy flag not available
  latch();       // command 0x30 = Wake up
  delay(10);     // must wait 160us, busy flag not available
  latch();       // command 0x30 = Wake up #2
  delay(10);     // must wait 160us, busy flag not available
  latch();       // command 0x30 = Wake up #3
  delay(10);     // can check busy flag now instead of delay
  putNybble(0x20); // put 0x20 on the output port (NEEDED TO SET 4-bit INTERFACE)
  latch();
  command(0x28); // Function set: 8-bit/2-line  ; NECESSARY due to the DDRAM shift that takes place.
  command(0x0F); // Display ON; Blinking cursor
  command(0x06); // Entry Mode set
}

/**
 * @brief Clear the display.
 * 
 * @return none
 */
void clear_screen()
{
  command(0x01);
}

// Return to home position
/**
 * @brief Return the cursor to the home position.
 * 
 * @return none
 */
void ret_home()
{
  command(0x02);
}

/**
 * @brief Write "NEWHAVEN DISPLAY" on the display.
 * 
 * @return none
 */
void disp()
{
  clear_screen();
  ret_home(); // For 1st 8 characters
  for (unsigned int i = 0; i < 8; i++)
  {
    data(_text1[i]);
  }

  command(0xC0); // For 2nd 8 characters
  for (unsigned int i = 0; i < 8; i++)
  {
    data(_text2[i]);
  }
  ret_home();
}