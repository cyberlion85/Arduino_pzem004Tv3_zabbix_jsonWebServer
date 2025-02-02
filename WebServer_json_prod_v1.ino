//add watchdog test
#include <PZEM004Tv30.h>
#include <SPI.h>
#include <Ethernet.h>
#include <avr/wdt.h>

PZEM004Tv30 pzem(2, 3);

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x26, 0xF8, 0x55, 0x6F, 0x36, 0xEF
};
IPAddress ip(10, 20, 0, 35);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {

  wdt_enable(WDTO_8S); // разрешение работу сторожевого таймера с тайм-аутом 500 мс

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop()
{
  float voltage = pzem.voltage();
  if (voltage > 0 && voltage < 1000 ) {
    //Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  } else {
    voltage = 0;
    //Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  }

  float current = pzem.current();
  if (current > 0 && current < 1000 ) {
    //Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  } else {
    current = 0;
    //Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  }

  float power = pzem.power();
  if (power > 0 && power < 1000) {
    //Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  } else {
    power = 0;
    //Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  }

  float energy = pzem.energy();
  if (energy > 0 && energy < 1000) {
    //Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  } else {
    energy = 0;
    //Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  }


  //Serial.println();
  //delay(1000);



  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    //Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // отсылаем заголовок json


          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();

          client.print("{\"voltage\":\"");
          client.print(voltage);
          client.print("\",");

          client.print("\"current\":\"");
          client.print(current);
          client.print("\",");

          client.print("\"power\":\"");
          client.print(power);
          client.print("\",");

          client.print("\"energy\":\"");
          client.print(energy);
          client.print("\"}");

          wdt_reset(); //сброс сторожевого таймера


          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    //Serial.println("client disconnected");
  }
}
