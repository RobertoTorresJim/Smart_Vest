#include <VirtualWire.h>

//value from digital pin of RF transmitter
const int data_pin_transmitter = 10;

/*****************************************************
  Digital and analog pin values for Motorcycle buttons
 *****************************************************/
const int value_brake_sensor = 2;
const int value_daytime = 4;
const int value_left = 5;
const int value_right = 6;
const int value_blinker = 7;

/**************************************************************
  Motorcycle switch buttons values read from digital and analog
 **************************************************************/
int button_brake_sensor = 0;
int button_left = 0;
int button_right = 0;
int button_blinker = 0;
int button_daytime = 0;

//Array of buttons values: {brake,left,right,blink,daytime}
int directions[5] = {0, 0, 0, 0, 0};

void setup() {
  //Put your setup code here, to run once:
  Serial.begin(9600);

  //pin for RF transmitter data
  vw_set_tx_pin(data_pin_transmitter);
  vw_setup(2000); // Bits per second

  //Buttons for Motocycle behaviour
  pinMode(value_left, INPUT);
  pinMode(value_right, INPUT);
  pinMode(value_blinker, INPUT);
  pinMode(value_brake_sensor, INPUT_PULLUP);
  pinMode(value_daytime, INPUT);
}

void loop() {

  //data send values for motorcycle switch
  button_left = digitalRead(value_left);
  button_right = digitalRead(value_right);
  button_blinker = digitalRead(value_blinker);
  button_brake_sensor = digitalRead(value_brake_sensor);
  button_daytime = digitalRead(value_daytime);

  //values of buttons allocated in array
  directions[0] = button_brake_sensor;
  directions[1] = button_left;
  directions[2] = button_right;
  directions[3] = button_blinker;
  directions[4] = button_daytime;

  //method for send data
  SendDirection(directions);
}

void SendDirection(int directions[]) {
  //array char to store all directions and send to receiver
  char msg[5];
  //concat values of int array in string variable
  String values = String(directions[0]) + " " + String(directions[1]) + " " + String(directions[2]) + " " + String(directions[3]) + " " + String(directions[4]);
  //Cast from string to char array
  values.toCharArray(msg, 10);
  //Send data to RF433 Receiver
  vw_send((uint8_t *)msg, strlen(msg));
  if (vw_tx_active()) {
    Serial.println(msg);
  }
  vw_wait_tx();//Wait until the whole message is gone

  delay(100);
}
