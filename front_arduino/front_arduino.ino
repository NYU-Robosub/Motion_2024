// Arduino code for the front arduino

#include <ros.h>
#include <std_msgs/Bool.h>
#include <Servo.h>
#include <std_msgs/Int32MultiArray.h>
byte leak_pin = 1;
byte light1_pin = 6;
byte light2_pin = 9;
byte trusterPinBL= 10;
byte trusterPinBR = 11;
Servo trusterBL;
Servo trusterBR;

// Signal value for truster to move forward or backward
int forward_max = 200; 
int backward_max = -200;
int noMove = 1500;

// Light analog value
int brightness = 1600;


void turnLeft(const int value)
{
  // trusterFR.writeMicroseconds(backward);
  trusterBR.writeMicroseconds(noMove + value);
  // trusterFL.writeMicroseconds(noMove + value);
  // trusterBL.writeMicroseconds(backward);  
}

void turnRight(const int value)
{
  // trusterFR.writeMicroseconds(noMove + value);
  // trusterBR.writeMicroseconds(backward);
  // trusterFL.writeMicroseconds(backward);
  trusterBL.writeMicroseconds(noMove + value);  
}

void goBackward(const int value)
{
  trusterBR.writeMicroseconds(noMove + value);
  trusterBL.writeMicroseconds(noMove + value);  
}

void motorCallback(const std_msgs::Int32MultiArray& msg)
{
  if (msg.data[1] > forward_max)
  {
    msg.data[1] = forward_max
  }
  if (msg.data[1] < backward_max)
  {
    msg.data[1] = backward_max
  }
  if (msg.data[0] == 1)
  {
    if (msg.data[1] > 0)
    {
      turnRight(msg.data[1]);
    }
    else if (msg.data[1] < 0)
    {
      turnLeft(abs(msg.data[1]))
    }
    else
    {
      turnLeft(0);
      turnRight(0);
    }
  }
  else if (msg.data[0] == 0)
  {
    if (msg.data[1] =< 0)
    {
      goBackward(abs(msg.data[1]))
    }
  }
}

ros::NodeHandle nh;
std_msgs::Bool leak_val;
bool leak;

ros::Publisher leak("leak_sensor", &leak_val);
ros::Subscriber<std_msgs::Int32MultiArray> motor_subscriber("thruster", &motorCallback);


void setup() {
  // put your setup code here, to run once:

  // Setup pins
  pinMode(leak_pin, INPUT);
  pinMode(light1_pin, OUTPUT);
  pinMode(light2_pin, OUTPUT);

  // Setup thrusters
  trusterBL.attach(trusterPinBL);
  trusterBR.attach(trusterPinBR);
  trusterBL.writeMicroseconds(1500);
  trusterBR.writeMicroseconds(1500);

  delay(7000); // delay to allow the ESC to recognize the stopped signal
  
  // Turn on light
  analogWrite(light1_pin, brightness)
  analogWrite(light2_pin, brightness)

  // Set up ROS node
  Serial.begin(9600);

  nh.initNode();
  nh.advertise(leak);
  nh.subscribe(motor_subscriber);
}

void loop() {
  //Measure force from sensor
  leak = digitalRead(leak_pin)
  leak_val.data = leak
  
  
  leak.publish(&leak_val);
  // depth.publish(&depth_val);
  nh.spinOnce();
  delay(10);
}
