
#include <SparkFunBME280.h>
#include <SparkFunCCS811.h>
#include <SPI.h>
#include <Ethernet.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#define CCS811_ADDR 0x5B
CCS811 myCCS811(CCS811_ADDR);
BME280 myBME280;


unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
byte mac[] = SECRET_MAC;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(10, 6, 128, 79);
IPAddress myDns(10, 6, 128 ,12);


EthernetClient client;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  Ethernet.begin(mac, ip, myDns);
  delay(1000);

  Wire.begin();//initialize I2C bus
  
  //This begins the CCS811 sensor and prints error status of .begin()
  myCCS811.begin();


  
  //Initialize BME280
  //For I2C, enable the following and disable the SPI section
  myBME280.settings.commInterface = I2C_MODE;
  myBME280.settings.I2CAddress = 0x77;
  myBME280.settings.runMode = 3; //Normal mode
  myBME280.settings.tStandby = 0;
  myBME280.settings.filter = 4;
  myBME280.settings.tempOverSample = 5;
  myBME280.settings.pressOverSample = 5;
  myBME280.settings.humidOverSample = 5;

  //Calling .begin() causes the settings to be loaded
  delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
  byte id = myBME280.begin(); //Returns ID of 0x60 if successful






  ThingSpeak.begin(client);  // Initialize ThingSpeak



}
void loop() {
  float BMEtempC = myBME280.readTempC()-4;
  float BMEpressure = myBME280.readFloatPressure()/100;
  float BMEhumid = myBME280.readFloatHumidity();
  float CCSCO2 = myCCS811.getCO2();
  float CCSTVOC = myCCS811.getTVOC();



  if (myCCS811.dataAvailable())
  {
    //Calling this function updates the global tVOC and eCO2 variables
    myCCS811.readAlgorithmResults();
    //printData fetches the values of tVOC and eCO2

    //This sends the temperature data to the CCS811
    myCCS811.setEnvironmentalData(BMEhumid, BMEtempC);
  }
  else if (myCCS811.checkForStatusError())
  {
    Serial.println(myCCS811.getErrorRegister()); //Prints whatever CSS811 error flags are detected
  }

  delay(2000); //Wait for next reading


  // set the fields with the values
  ThingSpeak.setField(1, BMEtempC);
  ThingSpeak.setField(2, BMEhumid);
  ThingSpeak.setField(3, BMEpressure);
  ThingSpeak.setField(4, CCSCO2);
  ThingSpeak.setField(5, CCSTVOC);



  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  
  // change the values

  
  delay(60000); // Wait 20 seconds to update the channel again
}
