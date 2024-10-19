#include <ros.h>
// #include <std_msg/String.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float64.h>
#define F1 1

ros::NodeHandle nh;

//Assume msg is 6 characters, 1 char for force sensor, 1 comma, 4 char for depth reading
std_msgs::Bool touch_val;
std_msgs::Float64 depth_val;
int force1;
ros::Publisher touch("touch_sensor", &touch_val);
ros::Publisher depth("pressure_sensor", &depth_val);


void setup() {
  // put your setup code here, to run once:
  pinMode(F1, INPUT);
  Serial.begin(9600);

  nh.initNode();
  nh.advertise(touch);
  nh.advertise(depth);
}

void loop() {
  //Measure force from sensor
  force1 = analogRead(F1);
  //Threshold for bumping
  if (force1 > 700){
    touch_val.data = true;
  }
  else {
    touch_val.data = false;
  }
  depth_val.data = 1024;//Do conversion here
  //format msg string as "1,1234", first char is whether or not 
  //the arduino bumped into something, last 4 char is for depth sensor reading;
  
  touch.publish(&touch_val);
  depth.publish(&depth_val);
  nh.spinOnce();
  delay(10);
}
