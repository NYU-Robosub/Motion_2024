import rospy
import ast
from time import sleep
from std_msgs.msg import Float64MultiArray, Float64, Int32MultiArray, Bool
from util import cvCallback, depthCallback, gyroCallback, cv, turn, changeDepth, searchGate, move, moveTillGone, touchCallback, distanceCallback


if __name__ == '__main__':
    rospy.init_node('publish')

    cvPub = rospy.Publisher('CV', Float64MultiArray)
    depthPub = rospy.Publisher('Depth', Float64)
    gyroPub = rospy.Publisher('Gyro', Float64)
    touchPub = rospy.Publisher('Touch', Bool)
    distancePub = rospy.Publisher('Distance', Float64)

    #prevyear
    cvBottomPub = rospy.Publisher('CV_bottom', Float64MultiArray)
    pressurePub = rospy.Publisher("pressure", Float64)
    
    


    while not rospy.is_shutdown():
        topic = input("Topic: ").upper()
        msg = input("Message: ")
        if topic == "CV":
            msg = ast.literal_eval(msg)
            cvMsg = Float64MultiArray()
            cvMsg.data = msg
            cvPub.publish(cvMsg)
        elif topic == "DEPTH":
            depthF = float(msg)
            depthMsg = Float64(depthF)
            depthPub.publish(depthMsg)
        elif topic == "GYRO":
            gyroF = float(msg)
            gyroMsg = Float64(gyroF)
            gyroPub.publish(gyroMsg)
        elif topic == "TOUCH":
            touchB = False
            if msg.upper() == "T":
                touchB = True
            touchMsg = Bool(touchB)
            touchPub.publish(touchMsg)
        elif topic == "DISTANCE":
            distanceF = float(msg)
            distanceMsg = Float64(distanceF)
            distancePub.publish(distanceMsg)
        elif topic == "CV_BOTTOM":
            msg = ast.literal_eval(msg)
            cvBottomMsg = Float64MultiArray()
            cvBottomMsg.data = msg
            cvBottomPub.publish(cvBottomMsg)
        elif topic == "PRESSURE":
            pressureF = float(msg)
            pressureMsg = Float64(pressureF)
            pressurePub.publish(pressureMsg)
        else:
            print("Invalid topic")


