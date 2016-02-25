
import sys
import commands
import subprocess
import Gnuplot
import os
import random
import datetime
import locale

start_list = [17298]
goal_list = [3738]

start_hour_list = [18]
start_min_list = [0]

interval_sec_list = [-1]
lam_list = [1]
mode_list = [1, 2, 3]

edc = 3


# for start in start_list:
# 	for goal in goal_list:
while 1:
	d = datetime.datetime.today()
	print d.strftime("%Y-%m-%d %H:%M:%S")

	while 1:
		start = random.randint(1, 20000)
		goal = random.randint(1, 20000)
		# start = 17060
		# goal = 10433
		if(start != goal):
			break
	print "******************"
	print "%d -> %d" % (start, goal)
	for hour in start_hour_list:
		for min in start_min_list:
			for interval in interval_sec_list:
				for lam in lam_list:
					for mode in mode_list:
						cmd = "./go -start %s -goal %s -dptr_h %s -dptr_m %s -kph 30 -gx 50 -gy 50 -lam %s -mode 0 -tim_intrvl %s -alg %s -edc %s" % (start, goal, hour, min, lam, interval, mode, edc)
						subprocess.call( cmd.strip().split(" ")  )

						g = Gnuplot.Gnuplot()
						g('unset xtics')
						g('unset ytics')
						g('set term png  size 1200,900')
						g('set output "../outputfile/optimumPath/dptr%s_arrv%s/dptr_tim%s_%s/srchAlg%s/lam%s_intvl%s.png"' % (start, goal,hour, min, mode, lam, interval))
						g('pl "../outputfile/optimumPath/map_xy_coord.dat" u 1:2 w l lc rgb "grey70" notitle, "../outputfile/optimumPath/dptr%s_arrv%s/dptr_tim%s_%s/srchAlg%s/path_pos.dat" u 1:2 w l lw 2 lc rgb "red" title "MUDA", "../outputfile/optimumPath/dptr%s_arrv%s/dptr_tim%s_%s/srchAlg%s/wait_pos.dat" u 1:2 w p ps 3 pt 7 lc rgb "dark-green" title "occuredWaitTime"' % (start, goal, hour, min, mode, start, goal, hour, min, mode))
						g('set terminal wxt')
						g('exit')
