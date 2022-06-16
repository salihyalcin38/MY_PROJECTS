#!/usr/bin/env python3


#######################################
## IMPORT
from numpy import *
from math import *
import math

import rospy
import sys
import time
import random
import numpy as np
import scipy.linalg as la

import threading
import os
import subprocess
import yaml
import PyKDL

from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
from sensor_msgs.msg import LaserScan
from sensor_msgs.msg import Imu
from tf import transformations as trans

from gazebo_msgs.msg import *
from std_msgs.msg import *




### TOPICS  ###
TOPIC_VELOCITY = "/cmd_vel"
TOPIC_MODELSTATE = '/gazebo/model_states'
TOPIC_SCAN  = "/scan"
TOPIC_ODOM  = "/odom"
TOPIC_IMU = "/imu"


FREQ = 10.0   # Frequency
TS = 1.0/FREQ # Timestamp


def quat_to_angle(quat):
  rot = PyKDL.Rotation.Quaternion(quat.x, quat.y, quat.z, quat.w)
  return rot.GetRPY()[2]
def normalize_angle(angle):
  res = angle
  while res > pi:
    res -= 2.0*pi
  while res < -pi:
    res += 2.0*pi
  return res


class Turtlebot :

	def __init__(self,name,rate):


		''' Variables initializations '''


		self.__x = None
		self.__y = None
		self.__z = None
		self.__angle = None
		self.current_laser_msg =  None
		self.current_imu_msg = None
		self.model_state  = None
		self.have_Scanner = False
		self.have_Imu = False
		self.rate = rate
		self.name = name


		self.roll = None
		self.pitch=None
		self.yaw=None
	

		## Subscribers
		self.__scan_sub = rospy.Subscriber( TOPIC_SCAN, LaserScan, self.__scan_handler)
		self.__odom_sub = rospy.Subscriber(TOPIC_ODOM, Odometry, self.__odom_handler)
		self.__imu_sub = rospy.Subscriber(TOPIC_IMU,Imu,self.__imu_handler, queue_size=10)
		self.model_state = rospy.Subscriber( TOPIC_MODELSTATE,ModelStates,self.cb_model_state)
		## Publishers
		self.__cmd_vel_pub = rospy.Publisher(TOPIC_VELOCITY, Twist, queue_size=10)

		self.vel = Twist()





	######### Publishers ####################################
	########################################################

	def move(self, linearx=0.0,lineary=0.0, angular=0.0):
	    """Moves the robot at a given linear speed and angular velocity
	    The speed is in meters per second and the angular velocity is in radians per second
	    """
	    # Message generation
	    msg = Twist()
	    msg.linear.x = linearx
	    msg.linear.y = lineary
	    msg.angular.z = angular 	    
	    self.__cmd_vel_pub.publish(msg)



	######### Callbacks ####################################
	########################################################

	# CallBack Gazebo Model States
	def cb_model_state(self,state):
		self.model_state = state

	def __odom_handler(self, msg): # ODOMETRY
	    self.__x = msg.pose.pose.position.x
	    self.__y = msg.pose.pose.position.y
	    self.__z = msg.pose.pose.position.z
	    q = msg.pose.pose.orientation
	    a = trans.euler_from_quaternion([q.x, q.y, q.z, q.w])[2]
	    #print(	"ODOM  ",self.__x,self.__y,self.__z)


	def __scan_handler(self, msg): # LIDAR

		"""CallBack from Lidar sensor """
		

		fleft = list(msg.ranges[0:9]) 
		fright = list(msg.ranges[350:359])
		front = fright + fleft
		left = list(msg.ranges[80:100])
		right = list(msg.ranges[260:280])
		back = list(msg.ranges[170:190])

		min_front = min(front)
		min_left = min(left)
		min_right = min(right)
		min_back = min(back)

		print(min_front,min_left,min_right,min_back)

		if min_front < 1.0:  # IF ENCOUNTER AN OBSTACLE STOP THERE AND CHANGE YOUR WAY. (in 1m)
			self.vel.linear.x = 0.0
			self.vel.angular.z = 0.6
			self.rate.sleep()
			self.__cmd_vel_pub.publish(self.vel)
		else: # IF NOT GO AND DRAW THE CIRCLE
			self.vel.linear.x = 0.4
			self.vel.angular.z = 0.2
			self.rate.sleep()
			self.__cmd_vel_pub.publish(self.vel)	


	
	def __imu_handler(self,imu):
		self.have_Imu = True
		
		try:
			quaternion = (imu.orientation.x,imu.orientation.y,imu.orientation.z,imu.orientation.w)
			euler = trans.euler_from_quaternion(quaternion)
			self.roll = euler[0]
			self.pitch = euler[1]
			self.yaw = euler[2]
		#	print("ROLL/PITCH/YAW :",self.roll,self.pitch,self.yaw)
		except:
			print("ERROR")
	


		
		

# Main function
def main():

    	# Initializing node
	rospy.init_node('turtlebot_node', anonymous=True)

	# Generating the turtlebots
	names = [] 
	turtlebots = []
	rate = rospy.Rate(FREQ)
	turtlebot = Turtlebot("robot_0",rate )


 	

	while not rospy.is_shutdown():
	        rate.sleep()



if __name__ == '__main__':
	try:
		print(' Randomwalk ......')
		main()
	except rospy.ROSInterruptException:
		pass
