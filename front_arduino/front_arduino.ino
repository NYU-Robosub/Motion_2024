// Arduino code for the front arduino
#include <ros.h>
#include <std_msgs/Bool.h>
// #include <Servo.h>
// #include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float32.h>
#include <DHT11.h>
#include <MPU6050_light.h>

byte leak_pin = 1;
byte temperature_pin = 2;
byte light1_pin = 3;
byte light2_pin = 5;
// byte trusterPinBL= 10;
// byte trusterPinBR = 11;
// Servo trusterBL;
// Servo trusterBR;
DHT11 dht11(temperature_pin);
// MPU6050 mpu(Wire);

// Timer
int timer = 0;
byte timeStep = 10; // in ms

// Displacement values
short x_disp = 0;
short y_disp = 0;
short z_disp = 0;

// Signal value for truster to move forward or backward
// int forward_max = 200; 
// int backward_max = -200;
// int noMove = 1500;


// void turnLeft(const int value)
// {
//   // trusterFR.writeMicroseconds(backward);
//   trusterBR.writeMicroseconds(noMove + value);
//   // trusterFL.writeMicroseconds(noMove + value);
//   // trusterBL.writeMicroseconds(backward);  
// }

// void turnRight(const int value)
// {
//   // trusterFR.writeMicroseconds(noMove + value);
//   // trusterBR.writeMicroseconds(backward);
//   // trusterFL.writeMicroseconds(backward);
//   trusterBL.writeMicroseconds(noMove + value);  
// }

// void goBackward(const int value)
// {
//   trusterBR.writeMicroseconds(noMove + value);
//   trusterBL.writeMicroseconds(noMove + value);  
// }

// void motorCallback(const std_msgs::Int32MultiArray& msg)
// {
//   if (msg.data[1] > forward_max)
//   {
//     msg.data[1] = forward_max;
//   }
//   if (msg.data[1] < backward_max)
//   {
//     msg.data[1] = backward_max;
//   }
//   if (msg.data[0] == 1)
//   {
//     if (msg.data[1] > 0)
//     {
//       turnRight(msg.data[1]);
//     }
//     else if (msg.data[1] < 0)
//     {
//       turnLeft(abs(msg.data[1]));
//     }
//     else
//     {
//       turnLeft(0);
//       turnRight(0);
//     }
//   }
//   else if (msg.data[0] == 0)
//   {
//     if (msg.data[1] <= 0)
//     {
//       goBackward(abs(msg.data[1]));
//     }
//   }
// }

ros::NodeHandle nh;
std_msgs::Bool leak_val;
std_msgs::Float32 temp_val;
std_msgs::Float32MultiArray gyro_val;
std_msgs::Float32MultiArray displacement_val;

ros::Publisher leak_pub('l', &leak_val);
ros::Publisher temperature_pub('t', &temp_val);
ros::Publisher gyro_pub('g', &gyro_val);
ros::Publisher displacement_pub('d', &displacement_val);
// ros::Subscriber<std_msgs::Int32MultiArray> motor_subscriber("t", &motorCallback);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  delay(1000);
  Serial.print(F("Starting setup\n"));
  // Setup pins
  pinMode(leak_pin, INPUT);
  pinMode(light1_pin, OUTPUT);
  pinMode(light2_pin, OUTPUT);

  // Setup thrusters
  // trusterBL.attach(trusterPinBL);
  // trusterBR.attach(trusterPinBR);
  // trusterBL.writeMicroseconds(1500);
  // trusterBR.writeMicroseconds(1500);

  // delay(7000); // delay to allow the ESC to recognize the stopped signal
  
  // Turn on light
  Serial.print(F("Turning on light\n"));
  // byte brightness = 1600;
  digitalWrite(light1_pin, HIGH);
  digitalWrite(light2_pin, HIGH);
  Serial.print(F("Setting up ROS\n"));
  delay(1000);

  // Set up ROS node
  nh.initNode();
  nh.advertise(leak_pub);
  nh.advertise(temperature_pub);
  nh.advertise(gyro_pub);
  nh.advertise(displacement_pub);
  // nh.subscribe(motor_subscriber);

  // Setup IMU
  // Wire.begin();
  // byte status = mpu.begin();
  // if (!status){
  //   analogWrite(light1_pin, 0);
  //   analogWrite(light2_pin, 0);
  // }
  // delay(1000);
  // mpu.calcOffsets(true,true);
  Serial.print(F("Setup completes"));
}

void loop() {
  timer = millis();

  //Measure from sensor
  bool leak;
  short temperature;
  leak = digitalRead(leak_pin);
  leak_val.data = leak;
  temperature = dht11.readTemperature();
  temp_val.data = temperature;
  leak_pub.publish(&leak_val);
  temperature_pub.publish(&temp_val);

  // mpu.update();

  // // Calculate Pitch, Roll and Yaw
  // float gyro_data[] = {mpu.getAngleX(), mpu.getAngleY(), mpu.getAngleZ()};
  // gyro_val.data = gyro_data;

  // // Calculate displacement
  // x_disp = x_disp + mpu.getAccX() * timeStep;
  // y_disp = y_disp + mpu.getAccY() * timeStep;
  // z_disp = z_disp + mpu.getAccZ() * timeStep;
  // float displacement_data[] = {x_disp, y_disp, z_disp};
  // displacement_val.data = displacement_data;
  
  
  // gyro_pub.publish(&gyro_val);
  // displacement_pub.publish(&displacement_val);

  nh.spinOnce();
  delay((timeStep) - (millis() - timer));
}
