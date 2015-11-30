
import sys
import commands
import subprocess
import Gnuplot

#start_list = [22000, 19499, 7407, 25, 865, 9820]
#goal_list = [1, 911, 9835, 22210, 19365, 9023]
#instanceA
#start_list = [21000, 19565, 21210, 9023, 19360]
#goal_list = [1, 25, 911, 865, 10, 40]

#instanceB
start_list = [1, 125, 1011, 965, 60, 140]
goal_list = [22000, 19365, 22210, 9023, 18360]


#instanceC
start_list = [7507, 7517, 7527, 7537, 7547, 7557]
goal_list = [9935, 9945, 9955, 9965, 9875]
#start_list = [6900]
#goal_list = [6570]

#instanceD
#start_list = [9935, 9945, 9955, 9965, 9875]
#goal_list = [7507, 7517, 7527, 7537, 7547, 7557]

#start_list = [11903, 11502, 11964, 15084, 15917, 18741]
#goal_list = [1, 753, 910, 4342, 4562, 6570]

#start_list = [13032, 14032, 15032, 16032, 17032, 18032, 19032, 20032]
#goal_list = [1, 1001, 1501, 2001, 2501, 3001]

#start_list = [15917]
#goal_list = [6570]

start_hour_list = [17]
start_min_list = [0]

for start in start_list:
	for goal in goal_list:
		for hour in start_hour_list:
			for min in start_min_list:
				#print "******************"
				#print "start %s goal %s dpt_h %s dpt_m %s" % (start, goal, hour, min)
				cmd = "./go -start %s -goal %s -dptr_h %s -dptr_m %s -kph 30 -gx 500 -gy 20 -lam 1 -mode 1" % (start, goal, hour, min)
				subprocess.call( cmd.strip().split(" ")  )
		
#				g = Gnuplot.Gnuplot()
#				g(' cd "C:\cygwin\home\sosmem\matsuda\kenkyuu\serchRouteInKokudoMap\dijMinGlareWithBuild_lambda\route" ')
#				g('set term png  size 1200,900')
#				g('set output "../route/PNG/route_%s_%s_%s_%s.png"' % (start, goal, hour, min))
#				g('pl "../route/DAT/map_xy_coord.dat" u 1:2 w l, "../route/DAT/uncom_%s_%s_%s_%s.dat" u 1:2 w l lw 2' % (start, goal, hour, min))
#				g('set terminal wxt')
#				g('exit')