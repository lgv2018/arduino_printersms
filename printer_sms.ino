// include the libraries
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include <GSM.h>

#define TX_PIN 6 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 5 // Arduino receive   GREEN WIRE   labeled TX on printer
#define PINNUMBER "1234" // PIN Number for the SIM

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

GSM gsmAccess;
GSM_SMS sms;

// Array to hold the number a SMS is retreived from
char senderNumber[20];

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Receiver");

  // connection state
  boolean notConnected = true;

  // Start GSM connection
  while (notConnected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      notConnected = false;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GSM initialized");
  Serial.println("Waiting for messages");

  
  // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  mySerial.begin(19200);  // Initialize SoftwareSerial
}

void loop() {
  char c;
  // If there are any SMSs available()
  if (sms.available()) {
    Serial.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#') {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    printer.begin();        // Init printer (same regardless of serial type)
    // The following calls are in setup(), but don't *need* to be.  Use them
    // anywhere!  They're just here so they run one time and are not printed
    // over and over (which would happen if they were in loop() instead).
    // Some functions will feed a line when called, this is normal.
    printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'
    printer.println(senderNumber);
    printer.boldOn();
    printer.println(F("Envoi de Rod"));
    printer.boldOff();
    printer.setSize('S');
    printer.feed(1);
    printer.println(F("-------------"));
    printer.feed(1);
    //printer.underlineOn();
    //printer.println(F("Underlined text"));
    //printer.underlineOff();
    printer.setSize('M');
    printer.println(F(""));
    while (c = sms.read()) {
      printer.print(c);
    }
    printer.setSize('S');
    printer.feed(1);
    printer.println(F("============="));
    printer.feed(2);
    printer.sleep();      // Tell printer to sleep
    
    sms.flush(); // Delete message from modem memory
    Serial.println("MESSAGE DELETED");
    delay(3000L);         // Sleep for 3 seconds
    printer.wake();       // MUST wake() before printing again, even if reset
    printer.setDefault(); // Restore printer to defaults
  }

  //delay(1000);
}
