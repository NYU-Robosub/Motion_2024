import rospy
from std_msgs.msg import Bool
from time import sleep

blocked = False
rospy.init_node('timeout', anonymous=True)

def responseCallback(data, timeoutPub):
    blocked = data
    if not blocked:
        sleep(0.1)
        timeoutPub.publish(Bool(True))


timeoutPub = rospy.Publisher("timeout", Bool)
responseSub = rospy.Subscriber("timeout_response", Bool, responseCallback, callback_args=timeoutPub)


while not blocked:
    timeoutPub.publish(Bool(True))