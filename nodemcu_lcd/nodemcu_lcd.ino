
#include <LiquidCrystal_I2C.h>
#include <LcdBarGraph.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
LcdBarGraph lbg(&lcd, 16, 0, 1);

void setup()
{
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  //lcd.noBacklight();
  lbg.drawValue(10, 100);
}

void loop()
{
  // Do nothing here...
}
