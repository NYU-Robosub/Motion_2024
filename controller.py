import rospy
from std_msgs.msg import Float64MultiArray, Int32MultiArray, Float32MultiArray

from util import move, gyroCallback, distanceCallback, depthCallback

thrusterPub = rospy.Publisher("thruster", Int32MultiArray)
sensor = {}
# Get angle from IMU
gyroSub = rospy.Subscriber('gyro_sensor', Float32MultiArray, gyroCallback, callback_args=(sensor, thrusterPub))
# Get distance travelled from IMU
distanceSub = rospy.Subscriber("displacement_sensor", Float32MultiArray, distanceCallback, callback_args=sensor)
# Get distance from bottom from bottom camera
depthSub = rospy.Subscriber('depth_sensor', Float64MultiArray, depthCallback, callback_args=sensor)

while True:
    direction = input("Type f for moving forward, b for moving backward, u for moving up, d for moving down, l for turning left, r for turning right, s for stoping all thrusters. Typing anything else will terminate the program.\n")
    distance = input("Enter the distance or angle: ")
    direction = direction.upper()
    if direction == "F":
        move("forward", sensor, thrusterPub, distance)
    elif direction == 'B':
        move("backward", sensor, thrusterPub, distance)
    elif direction == "U":
        move("up", sensor, thrusterPub, distance)
    elif direction == "D":
        move("down", sensor, thrusterPub, distance)
    elif direction == "L":
        move("left", sensor, thrusterPub, distance)
    elif direction == "R":
        move("right", sensor, thrusterPub, distance)
    elif direction == "S":
        pubMsg = Int32MultiArray()
        pubMsg.data=[0, 0]
        thrusterPub.publish(pubMsg)
        pubMsg.data=[2, 0]
        thrusterPub.publish(pubMsg)
    else:
        break