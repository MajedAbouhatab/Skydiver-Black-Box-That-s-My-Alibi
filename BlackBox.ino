#include <GNSS.h>
#define STRING_BUFFER_SIZE  128
static SpGnss Gnss;
#include <SDHCI.h>

SDClass SD;
File ThisFile;

void setup() {
  Serial.begin(2000000);
  sleep(3);
  // GPS Fix LED
  pinMode(LED2, OUTPUT);
  // Write to file LED
  pinMode(LED3, OUTPUT);
  // Initialize sensor
  Gnss.begin();
  // Aim for 10 data points per second
  Gnss.setInterval(0.1);
  // Only interested in GPS
  Gnss.select(GPS);
  // Start sensor
  Gnss.start();
  // Write String to file and Serial
  PrintText("DateTime,Latitude,Longitude,Altitude,Speed,Heading,numSatellitesCalcPos");
}

void loop()
{
  static int LoopCount = 0;
  if (Gnss.waitUpdate(-1))
  {
    // Ready with update
    digitalWrite(LED3, HIGH);
    SpNavData NavData;
    // Get data
    Gnss.getNavData(&NavData);
    // Process new data
    ProcessUpdate(&NavData);
    digitalWrite(LED3, LOW);
  }
}

void PrintText(String ThisText)
{
  // Write to Serial
  Serial.println(ThisText);
  // Open file
  ThisFile = SD.open("GPSData.txt", FILE_WRITE);
  // Write to file
  ThisFile.println(ThisText);
  // Close file
  ThisFile.close();
}

static void ProcessUpdate(SpNavData *pNavData)
{
  // Empty String
  String AllText = "";
  char StringBuffer[STRING_BUFFER_SIZE];
  if (pNavData->posFixMode != 1)
  {
    // GPS Fix
    digitalWrite(LED2, HIGH);
    snprintf(StringBuffer, STRING_BUFFER_SIZE, "%04d/%02d/%02d ", pNavData->time.year, pNavData->time.month, pNavData->time.day);
    // Add date to String
    AllText += StringBuffer;
    snprintf(StringBuffer, STRING_BUFFER_SIZE, "%02d:%02d:%02d.%06d, ", pNavData->time.hour, pNavData->time.minute, pNavData->time.sec, pNavData->time.usec);
    // Add time to String
    AllText += StringBuffer;
    snprintf(StringBuffer, STRING_BUFFER_SIZE, "%3.6f, %3.6f, %4f, %3f, %3f, ", pNavData->latitude, pNavData->longitude, pNavData->altitude, pNavData->velocity, pNavData->direction);
    // Add coordinates to String
    AllText += StringBuffer;
    snprintf(StringBuffer, STRING_BUFFER_SIZE, "%2d", pNavData->numSatellitesCalcPos);
    // Add number of Satellites to String
    AllText += StringBuffer;
    // Write String to file and Serial
    PrintText(AllText);
    digitalWrite(LED2, LOW);
  }
}
