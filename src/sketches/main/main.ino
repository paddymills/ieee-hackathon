/* test bluetooth module HC-05
 *  Zach Martin
 *  4/5/23
 *  
 *  check out https://howtomechatronics.com/tutorials/arduino/arduino-and-hc-05-bluetooth-module-tutorial/
 */

/* SoftwareSerial is a UART in software. It allows us to create a virtual secondary UART to connect with the 
 *  HC-05, while freeing up the primary hardware UART for program/debug
 *  
 *  It cannot send and receive simultaneously
 */
#include <SoftwareSerial.h> 

/****************************************************************/
/* Define i/o pins */
// SoftwareSerial pins
#define rxd 2 
#define txd 3

#define ledPin 13 // built-in LED indicator

// HC05 Module pins
#define enable 4
#define power 5

#define pushbutton 6

/*
 * created by Rui Santos, https://randomnerdtutorials.com
 * 
 * Complete Guide for Ultrasonic Sensor HC-SR04
 *
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND
 */
int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, inches;
long lastCm, lastInches;

/****************************************************************/
/*relevant HC05 commands:
 * query name     "AT+NAME?\r\n"
 * set name       "AT+NAME=<newname>\r\n"
 * query pin      "AT+PSWD?\r\n"
 * set pin        "AT+PSWD=<newpin>\r\n"
 * 
 */

/****************************************************************/
/* function prototypes */
void bt_to_usb(void); // send all bt input to USB

SoftwareSerial bt_serial(rxd, txd); // intitialize softwareserial

/****************************************************************/
// functions


// setup, run once
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(power, OUTPUT);
  pinMode(pushbutton, INPUT);
  
  digitalWrite(ledPin, LOW);
  
  Serial.begin(9600);

  // ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lastCm = 0;
  lastInches = 0;
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }
  
  Serial.println("Program begin");

  // put HC05 into AT mode
  bt_serial.begin(38400);
  digitalWrite(enable, HIGH); // for AT mode
  digitalWrite(power, HIGH); // boot device

  delay(5000); // wait for bluetooth to boot
  digitalWrite(ledPin, HIGH);       // display AT cmd beginning


  /* Set name */
  Serial.println("\r\nSetting new name. Old name is");
  bt_serial.write("AT+NAME?\r\n");  // get current name
  bt_to_usb();
  bt_serial.write("AT+NAME=PatrickBT\r\n");  // Set new name
  bt_to_usb();
  Serial.println("New name is:");
  bt_serial.write("AT+NAME?\r\n");  // get current name
  bt_to_usb();


  /* Set password */
  Serial.println("\r\nSetting new pin. Old pin is");
  bt_serial.write("AT+PSWD?\r\n");  // get current pin
  bt_to_usb();
  bt_serial.write("AT+PSWD=0000\r\n");  // Set new password
  bt_to_usb();
  Serial.println("New pin is:");
  bt_serial.write("AT+PSWD?\r\n");  // get current password
  bt_to_usb();

  // Return to regular serial mode
  digitalWrite(power, LOW); // reboot device
  digitalWrite(enable, LOW); // get out of AT mode

  // go back to default baud rate
  bt_serial.end();
  bt_serial.begin(9600); 
  
  delay(10);
  
  digitalWrite(power, HIGH); // bring device back up
  delay(5000); // wait for bluetooth to boot
  
  Serial.println("\r\nNow in normal user mode");
}

// ye olde superloooop
void loop() {
  char input;
  char x = 'A';
  bool  button_state = read_button();


  if(button_state)
  {
      bt_serial.flush();
      bt_serial.println("pressed");
  }
  
  // -------- Ultrasonic Sensor -------------
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  
  if ( inches - lastInches < 2 && inches - lastInches > -2 ) {
    bt_serial.flush();
    bt_serial.println(inches);

    lastInches = inches;
  }
}

void bt_to_usb(void)
{
  int i = 100;

  // wait for input or else quit after 100ms
  while(i)
  {
    if(bt_serial.available()){
      Serial.write(bt_serial.read()); 
      digitalWrite(ledPin, !digitalRead(ledPin));
      i = 10;
    }
    else
    {
      i--;
      delay(1);
    }
  }
}

// read button, debounced
// the button logic is inverted (pull-up)
bool read_button(void)
{
  static bool buttonState = true;
  static int lastDebounceTime=millis();
  
  bool reading = digitalRead(pushbutton);
  
  if((millis() - lastDebounceTime) > 50 && reading != buttonState) { // 50ms
    lastDebounceTime = millis();
    buttonState = reading;
  }
  
  return !buttonState;
}
