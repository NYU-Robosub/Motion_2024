// Arduino code for the back arduino
#include <ros.h>
#include <Servo.h>
#include <std_msgs/Int32MultiArray.h>
#include <MPU6050_light.h>
#include <std_msgs/Float32MultiArray.h>

// Arduino pin for trusters. 
// F means front, B, means back, V means vertical, L means left, R means right. 
// F truster go backward, B truster go forward
byte trusterPinFL= 6;
byte trusterPinFR = 11;
byte trusterPinVFL = 5;
byte trusterPinVFR = 10;
byte trusterPinVBL = 3;
byte trusterPinVBR = 9;
Servo trusterFL;
Servo trusterFR;
Servo trusterVFL;
Servo trusterVFR;
Servo trusterVBL;
Servo trusterVBR;
// Signal value for truster to move forward or backward
byte forward_max = 200; 
byte backward_max = -200;
byte noMove = 1500;

// Timer
unsigned long timer = 0;

// Displacement values
short x_disp = 0;
short y_disp = 0;
short z_disp = 0;

MPU6050 mpu(Wire);


void turnLeft(const int value)
{
  // trusterFR.writeMicroseconds(backward);
  // trusterBR.writeMicroseconds(noMove + value);
  trusterFL.writeMicroseconds(noMove + value);
  // trusterBL.writeMicroseconds(backward);  
}

void turnRight(const int value)
{
  trusterFR.writeMicroseconds(noMove + value);
  // trusterBR.writeMicroseconds(backward);
  // trusterFL.writeMicroseconds(backward);
  // trusterBL.writeMicroseconds(noMove + value);  
}

void goForward(const int value)
{
  trusterFR.writeMicroseconds(noMove + value);
  trusterFL.writeMicroseconds(noMove + value); 
}

void goUpDown(const int value)
{
  trusterVFL.writeMicroseconds(noMove - value);
  trusterVBR.writeMicroseconds(noMove - value);
  trusterVBL.writeMicroseconds(noMove + value);
  trusterVFR.writeMicroseconds(noMove + value);
}

void pitch(const int value)
{
  trusterVFL.writeMicroseconds(noMove - value);
  trusterVBR.writeMicroseconds(noMove + value);
  trusterVBL.writeMicroseconds(noMove + value);
  trusterVFR.writeMicroseconds(noMove - value);
}

void roll(const int value)
{
  trusterVFL.writeMicroseconds(noMove - value);
  trusterVBR.writeMicroseconds(noMove + value);
  trusterVBL.writeMicroseconds(noMove - value);
  trusterVFR.writeMicroseconds(noMove + value);
}

void motorCallback(const std_msgs::Int32MultiArray& msg)
{
  if (msg.data[1] > forward_max)
  {
    msg.data[1] = forward_max;
  }
  if (msg.data[1] < backward_max)
  {
    msg.data[1] = backward_max;
  }
  if (msg.data[0] == 0)
  {
    if (msg.data[1] >= 0)
    {
      goForward(msg.data[1]);
    }
  }
  else if (msg.data[0] == 1)
  {
    if (msg.data[1] > 0)
    {
      turnRight(msg.data[1]);
    }
    else if (msg.data[1] < 0)
    {
      turnLeft(abs(msg.data[1]));
    }
    else
    {
      turnLeft(0);
      turnRight(0);
    }
  }
  else if (msg.data[0] == 2)
  {
    goUpDown(msg.data[1]);
  }
  else if (msg.data[0] == 3)
  {
    pitch(msg.data[1]);
  }
  else if (msg.data[0] == 4)
  {
    roll(msg.data[1]);
  }
  
}

ros::NodeHandle node_handle;

std_msgs::Float32MultiArray gyro_val;
std_msgs::Float32MultiArray displacement_val;

ros::Subscriber<std_msgs::Int32MultiArray> motor_subscriber("t", &motorCallback);
ros::Publisher gyro_pub("g", &gyro_val);
ros::Publisher displacement_pub("d", &displacement_val);

void setup() {
  trusterFL.attach(trusterPinFL);
  trusterFR.attach(trusterPinFR);
  trusterVFL.attach(trusterPinVFL);
  trusterVFR.attach(trusterPinVFR);
  trusterVBL.attach(trusterPinVBL);
  trusterVBR.attach(trusterPinVBR);

  // send "stop" signal to ESC to initialize it
  trusterFL.writeMicroseconds(1500);
  trusterFR.writeMicroseconds(1500);
  trusterVFL.writeMicroseconds(1500);
  trusterVFR.writeMicroseconds(1500);
  trusterVBL.writeMicroseconds(1500);
  trusterVBR.writeMicroseconds(1500);
  delay(7000); // delay to allow the ESC to recognize the stopped signal

  // Setup IMU
  Wire.begin();
  byte status = mpu.begin();
  delay(1000);
  mpu.calcOffsets(true,true);

  // Setup ROS
  Serial.begin(57600);
  node_handle.initNode();
  node_handle.subscribe(motor_subscriber);
  node_handle.advertise(gyro_pub);
  node_handle.advertise(displacement_pub);
}

void loop() {
  unsigned long new_time = millis();
  mpu.update();

  // Calculate Pitch, Roll and Yaw
  float gyro_data[] = {mpu.getAngleX(), mpu.getAngleY(), mpu.getAngleZ()};
  gyro_val.data = gyro_data;

  // Calculate displacement
  x_disp = x_disp + mpu.getAccX() * (new_time-timer);
  y_disp = y_disp + mpu.getAccY() * (new_time-timer);
  z_disp = z_disp + mpu.getAccZ() * (new_time-timer);
  float displacement_data[] = {x_disp, y_disp, z_disp};
  displacement_val.data = displacement_data;
  
  
  gyro_pub.publish(&gyro_val);
  displacement_pub.publish(&displacement_val);
  timer=new_time;
  node_handle.spinOnce();
}
