/* Clock by Edwin Martin <edwin@bitstorm.org>    */
/* Using DCF77 receiver and SSD1306 128X64 OLED  */
/* Copyright 2017 Edwin Martin. MIT license      */

#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include "DCF77.h"
#include "TimeLib.h"

/* Configure u8g2 for SSD1306 128X64 OLED */
U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, 12, 11, 8, 9, 10);

#define DCF_PIN 2
#define DCF_INTERRUPT 0

/* Configure DCF77 reveiver */
DCF77 DCF = DCF77(DCF_PIN,DCF_INTERRUPT);

boolean show_init = true;
char time_string[6];
char date_string[20];
const char* weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* months[] = {"Jan.", "Feb.", "Mar.", "Apr.", "May", "Jun.", "Jul.", "Aug.", "Sep.", "Oct.", "Nov.", "Dec."};
int min_now, min_old = -1;

void setup(void) {
  Serial.begin(9600); 
  Serial.println("Initialising DCF77");
  DCF.Start();
  u8g2.begin();
}

void loop(void) {
  time_t DCFtime = DCF.getTime();
  if (DCFtime != 0) {
    Serial.println("Time is updated");
    setTime(DCFtime);
    show_init = false;
  }

  min_now = minute();
  if (min_now != min_old) {
    set_time_string(hour(), min_now, time_string);
    set_date_string(date_string);

    u8g2.firstPage();
    do {
      if (show_init) {
        u8g2.setFont(u8g2_font_mercutio_basic_nbp_tf);
        u8g2.setCursor(15, 40);
        u8g2.print("Waiting for signal...");
      } else {
        u8g2.setFont(u8g2_font_inb30_mn);
        u8g2.setCursor(64 - u8g2.getStrWidth(time_string) / 2, 33);
        u8g2.print(time_string);
        u8g2.setFont(u8g2_font_mercutio_basic_nbp_tf);
        u8g2.setCursor(64 - u8g2.getStrWidth(date_string) / 2, 60);
        u8g2.print(date_string);
      }
    } while (u8g2.nextPage());

    min_old = min_now;
  }

  delay(1000);
}

void set_time_string(int hour_num, int min_num, char time_string[]) {
  int position = 0;
  if (hour_num / 10 > 0) {
    time_string[position++] = '0' + hour_num / 10;
  }
  time_string[position++] = '0' + hour_num % 10;
  time_string[position++] = ':';
  time_string[position++] = '0' + min_num / 10;
  time_string[position++] = '0' + min_num % 10;
  time_string[position++] = '\0';
}

/* British notation like "Thursday, 24 Aug."
 * because because of European DCF77
 */
void set_date_string(char date_string[]) {
  int day_num = day();
  int month_num = month();
  int year_num = year();
  int weekday_num = weekday();
  char day_of_month[3];
  int position = 0;

  if (weekday_num == 0 || month_num == 0) {
    return;
  }
  
  strcpy(date_string, weekdays[weekday_num - 1]);
  strcat(date_string, ", ");
  if (day_num / 10 > 0) {
    day_of_month[position++] = '0' + day_num / 10;
  }
  day_of_month[position++] = '0' + day_num % 10;
  day_of_month[position++] = '\0';
  strcat(date_string, day_of_month);
  strcat(date_string, " ");
  strcat(date_string, months[month_num - 1]);
}

