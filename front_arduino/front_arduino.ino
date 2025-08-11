// Arduino code for the front arduino
#include <ros.h>
#include <std_msgs/Bool.h>
#include <Servo.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float32.h>
#include <DHT11.h>
#include <MPU6050.h>

byte leak_pin = 1;
byte temperature_pin = 2;
byte light1_pin = 6;
byte light2_pin = 9;
byte trusterPinBL= 10;
byte trusterPinBR = 11;
byte imu_SDA = 8;
byte imu_SCL = 9;
// Servo trusterBL;
// Servo trusterBR;
DHT11 dht11(temperature_pin);
MPU6050 mpu;

// Timer
int timer = 0;
float timeStep = 0.01;

// Pitch, Roll and Yaw values
float pitch = 0;
float roll = 0;
float yaw = 0;

// Displacement values
float x_disp = 0;
float y_disp = 0;
float z_disp = 0;

// Signal value for truster to move forward or backward
int forward_max = 200; 
int backward_max = -200;
int noMove = 1500;

// Light analog value
int brightness = 1600;


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

ros::Publisher leak_pub("leak_sensor", &leak_val);
ros::Publisher temperature_pub("temperature_sensor", &temp_val);
ros::Publisher gyro_pub("gyro_sensor", &gyro_val);
ros::Publisher displacement_pub("displacement_sensor", &displacement_val);
// ros::Subscriber<std_msgs::Int32MultiArray> motor_subscriber("thruster", &motorCallback);


void setup() {
  // put your setup code here, to run once:

  // Setup pins
  pinMode(leak_pin, INPUT);
  pinMode(light1_pin, OUTPUT);
  pinMode(light2_pin, OUTPUT);

  // Setup thrusters
  // trusterBL.attach(trusterPinBL);
  // trusterBR.attach(trusterPinBR);
  // trusterBL.writeMicroseconds(1500);
  // trusterBR.writeMicroseconds(1500);

  delay(7000); // delay to allow the ESC to recognize the stopped signal
  
  // Turn on light
  analogWrite(light1_pin, brightness);
  analogWrite(light2_pin, brightness);

  // Set up ROS node
  Serial.begin(9600);

  nh.initNode();
  nh.advertise(leak_pub);
  nh.advertise(temperature_pub);
  nh.advertise(gyro_pub);
  nh.advertise(displacement_pub);
  // nh.subscribe(motor_subscriber);

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  mpu.calibrateGyro();
  mpu.setThreshold(0);

  // Set accelerometer offsets
  // mpu.setAccelOffsetX();
  // mpu.setAccelOffsetY();
  // mpu.setAccelOffsetZ();
}

void loop() {
  timer = millis();

  //Measure from sensor
  bool leak;
  int temperature;
  leak = digitalRead(leak_pin);
  leak_val.data = leak;
  temperature = dht11.readTemperature();
  temp_val.data = temperature;
  Vector normGyro = mpu.readNormalizeGyro();
  Vector normAccel = mpu.readNormalizeAccel();

  // Calculate Pitch, Roll and Yaw
  pitch = pitch + normGyro.YAxis * timeStep;
  roll = roll + normGyro.XAxis * timeStep;
  yaw = yaw + normGyro.ZAxis * timeStep;
  float gyro_data[] = {pitch, roll, yaw};
  gyro_val.data = gyro_data;

  // Calculate displacement
  x_disp = x_disp + normAccel.XAxis * timeStep;
  y_disp = y_disp + normAccel.YAxis * timeStep;
  z_disp = z_disp + normAccel.ZAxis * timeStep;
  float displacement_data[] = {x_disp, y_disp, z_disp};
  displacement_val.data = displacement_data;
  
  leak_pub.publish(&leak_val);
  temperature_pub.publish(&temp_val);
  gyro_pub.publish(&gyro_val);
  displacement_pub.publish(&displacement_val);

  nh.spinOnce();
  delay((timeStep*1000) - (millis() - timer));
}
