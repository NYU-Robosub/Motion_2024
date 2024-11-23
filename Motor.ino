#include <Servo.h>
#include <std_msgs/Int32MultiArray.h>

// Arduino pin for trusters. 
// F means front, B, means back, V means vertical, L means left, R means right. 
// F truster go backward, B truster go forward
byte trusterPinFL= 9;
byte trusterPinFR = 10;
byte trusterPinBL= 11;
byte trusterPinBR = 12;
byte trusterPinVL = 13;
byte trusterPinVR = 14;
Servo trusterFL;
Servo trusterFR;
Servo trusterBL;
Servo trusterBR;
Servo trusterVL;
Servo trusterVR;
// Signal value for truster to move forward or backward
int forward_max = 200; 
int backward_max = -200;
int noMove = 1500;

void turnLeft(const int value)
{
  // trusterFR.writeMicroseconds(backward);
  trusterBR.writeMicroseconds(noMove + value);
  trusterFL.writeMicroseconds(noMove + value);
  // trusterBL.writeMicroseconds(backward);  
}

void turnRight(const int value)
{
  trusterFR.writeMicroseconds(noMove + value);
  // trusterBR.writeMicroseconds(backward);
  // trusterFL.writeMicroseconds(backward);
  trusterBL.writeMicroseconds(noMove + value);  
}

void goForward(const int value)
{
  trusterFR.writeMicroseconds(noMove + value);
  trusterFL.writeMicroseconds(noMove + value); 
}

void goBackward(const int value)
{
  trusterBR.writeMicroseconds(noMove + value);
  trusterBL.writeMicroseconds(noMove + value);  
}

void goUpDown(const int value)
{
  trusterVL.writeMicroseconds(noMove + value);
  trusterVR.writeMicroseconds(noMove+ value);
}

void motorCallback(const std_msgs::Int32MultiArray& msg)
{
  msg.data[1] = msg.date[1] * 100
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
    if (msg.data[1] > 0)
    {
      goForward(msg.data[1]);
    }
    else if (msg.data[1] < 0)
    {
      goBackward(abs(msg.data[1]))
    }
    else
    {
      goBackward(0);
      goForward(0);
    }
  }
  else if (msg.data[0] == 2)
  {
    goUpDown(msg.data[1])
  }
}

ros::NodeHandle node_handle;

ros::Subscriber<std_msgs::Int32MultiArray> motor_subscriber("thruster", &motorCallback);

void setup() {
  trusterFL.attach(trusterPinFL);
  trusterFR.attach(trusterPinFR);
  trusterBL.attach(trusterPinBL);
  trusterBR.attach(trusterPinBR);
  trusterVL.attach(trusterPinVL);
  trusterVR.attach(trusterPinVR);

  // send "stop" signal to ESC to initialize it
  trusterFL.writeMicroseconds(1500);
  trusterFR.writeMicroseconds(1500);
  trusterBL.writeMicroseconds(1500);
  trusterBR.writeMicroseconds(1500);
  trusterVL.writeMicroseconds(1500);
  trusterVR.writeMicroseconds(1500);
  delay(7000); // delay to allow the ESC to recognize the stopped signal
  node_handle.initNode();
  node_handle.subscribe(motor_subscriber);
}
