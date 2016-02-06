
import sys
import commands
import subprocess
import Gnuplot
import os
import random

start_list = [15000]
goal_list = [5000]
#start_list = [21000]
#goal_list = [3]


start_hour_list = [17, 18, 19, 20]
start_min_list = [0]
mode_list = [0, 1]

while 1:
	while 1:
		start = random.randint(1, 20000)
		goal = random.randint(1, 20000)
		if start != goal:
			break

	print "%d -> %d" % (start, goal)
	for hour in start_hour_list:
		for min in start_min_list:
			print "%d:%d" % (hour, min)
			for mode in mode_list:
				cmd = "./go -start %s -goal %s -dptr_h %s -dptr_m %s -kph 30 -gx 50 -gy 50 -lam 1 -mode 0 -tim_intrvl -1 -alg %s -edc 3" % (start, goal, hour, min, mode)
				subprocess.call( cmd.strip().split(" ")  )
