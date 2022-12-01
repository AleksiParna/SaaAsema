#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include "ThingSpeak.h"
#define DHTPIN 8
#define DHTTYPE DHT22

#define SECRET_CH_ID 1964742
#define SECRET_WRITE_APIKEY "1VAJZEIAQVCUHEM1"
EthernetClient client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
DHT sensor(DHTPIN, DHTTYPE);
byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x82, 0x22};

IPAddress ip(10, 6, 128, 88);
IPAddress myDns(10, 6, 128, 12);
EthernetServer server(80);
 
void setup() 
{
  Serial.begin (9600);

  sensor.begin( );
  Ethernet.begin(mac, ip, myDns);
  Serial.print("Your IP Adress is ");
  Serial.println(Ethernet.localIP( ) );
  ThingSpeak.begin(client);
}

void loop( ) 
{
  float humidity = sensor.readHumidity( );
  float temperature_C = sensor.readTemperature(false);
  float temperature = temperature_C - 4;
  if (isnan(humidity) || isnan(temperature_C)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

   
  

  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  


  
  delay(60000); // Wait 60 seconds to update the channel again
}
