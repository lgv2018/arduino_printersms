// include the libraries
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
//#include <AltSoftSerial.h>
#include <GSM.h>
#include <WString.h>

#define TX_PIN 6 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 5 // Arduino receive   GREEN WIRE   labeled TX on printer

//yellow: 9
// green: 8

#define PINNUMBER "1234" // PIN Number for the SIM

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
//AltSoftSerial mySerial;

Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

bool gsmdebug = false;
GSM gsmAccess(gsmdebug);
GSM_SMS sms;

// Array to hold the number a SMS is retreived from
char senderNumber[20];

bool usbdebug = false;
bool debugWithPrint = false;

//String allowedNumber = "+33645970094";

void setup() {
  if (usbdebug == true) {
    // initialize serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("SMS Messages Receiver");
  }

  // connection state
  boolean notConnected = true;

  // Start GSM connection
  while (notConnected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      notConnected = false;
    } else {
      if (usbdebug == true) {
        Serial.println("Not connected");
      }
      delay(1000);
    }
  }

  if (usbdebug == true) {
    Serial.println("GSM initialized");
    Serial.println("Waiting for messages");
  }

  
  // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  if (usbdebug == false) {
    mySerial.begin(19200);  // Initialize SoftwareSerial
    printer.begin();        // Init printer (same regardless of serial type)
  }
  initPrinter();
}

void loop() {
  // If there are any SMSs available()
  if (sms.available()) {
    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    String snumber = senderNumber;
    if (usbdebug == true) {
      Serial.println(senderNumber);
    }

    // Test si le numero est authorisé
    if (snumber == allowedNumber) {
      if (usbdebug == true) {
        Serial.println("GOOD NUMBER");
      }
      
      bool endPrinting = false;
      if (usbdebug == true) {
        Serial.println("Message received from:");
      }
  
      // An example of message disposal
      // Any messages starting with # should be discarded
      if (sms.peek() == '#') {
        if (usbdebug == true) {
          Serial.println("Discarded SMS");
        }
        sms.flush();
      }
      if (usbdebug == false) {
        endPrinting = printSms();
      } else {
        endPrinting = true;
      }
      if (usbdebug == true) {
        char c;
        while (c = sms.read()) {
          Serial.print(c);
        }
        Serial.println("\nEND OF MESSAGE");
        Serial.println("MESSAGE DELETED");
      }
      if (true == endPrinting) {
        sms.flush(); // Delete message from modem memory
      }
    } else {
      if (usbdebug == true) {
        Serial.println("BAD NUMBER");
        char c;
        while (c = sms.read()) {
          Serial.print(c);
        }
        Serial.println("\nEND OF MESSAGE");
        Serial.println("MESSAGE DELETED");
      }
      sms.flush(); // Delete message from modem memory
    }
  }
  delay(1000);
}

bool printSms () {
  char c;
  printer.wake();
  // The following calls are in setup(), but don't *need* to be.  Use them
  // anywhere!  They're just here so they run one time and are not printed
  // over and over (which would happen if they were in loop() instead).
  // Some functions will feed a line when called, this is normal.
  printer.setSize('S');        // Set type size, accepts 'S', 'M', 'L'
  printer.println(senderNumber);
  printer.boldOn();
  printer.setSize('M');
  printer.println(F("Envoi de Rod"));
  printer.boldOff();
  printer.feed(1);
  printer.setSize('S');
  printer.println(F("-------------"));
  printer.feed(1);
  //printer.underlineOn();
  //printer.println(F("Underlined text"));
  //printer.underlineOff();
  printer.setSize('S');
  while (c = sms.read()) {
    printer.print(c);
    //delay(10);
  }
  printer.feed(2);
  printer.setSize('S');
  printer.println(F("============="));
  printer.feed(2);
  printer.sleep();      // Tell printer to sleep
  
  delay(3000);         // Sleep for 3 seconds
  printer.wake();       // MUST wake() before printing again, even if reset
  printer.setDefault(); // Restore printer to defaults
  return true;
}

void initPrinter () {
  printer.wake();
  printer.setSize('S');
  printer.println(F("Printer Ready"));
  printer.feed(2);
  printer.setSize('S');
  printer.println(F("============="));
  printer.feed(2);
  printer.sleep();      // Tell printer to sleep
  delay(3000);         // Sleep for 3 seconds
}
