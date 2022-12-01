#include <WiFi.h>
#include "time.h"
#include "sntp.h"
#include <LiquidCrystal_I2C.h>

//0x3F or 0x27
LiquidCrystal_I2C lcd(0x3F, 16, 2);   //LCD Object

const char* ssid       = "Your-SSID";
const char* password   = "Your-Password";

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("No time available (yet)");
    return;
  }

  //Display Date & Time
  lcd.clear();
  lcd.print(&timeinfo, "%B %d %Y");    //November 22 2022
  lcd.setCursor(0,1);
  lcd.print(&timeinfo, "%A,%H:%M:%S"); //Tuesday 10:30:05

}

// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t)
{
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void setup()
{
  Serial.begin(115200);

  // Setup LCD with backlight and initialize
  lcd.init();
  lcd.backlight();

  // set notification call-back function
  sntp_set_time_sync_notification_cb( timeavailable );

  /**
   * NTP server address could be aquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 aquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE aquired NTP server address
   */
  sntp_servermode_dhcp(1);    // (optional)

  /**
   * This will set configured ntp servers and constant TimeZone/daylightOffset
   * should be OK if your time zone does not need to adjust daylightOffset twice a year,
   * in such a case time adjustment won't be handled automagicaly.
   */
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  lcd.clear();
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  delay(1000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  lcd.clear();
  lcd.print("CONNECTED");
  delay(2000);
}

void loop()
{
  delay(1000);
  printLocalTime();     // it will take some time to sync time :)
}
