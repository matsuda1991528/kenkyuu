import sys
import commands
import subprocess
import Gnuplot
import os
import datetime
import locale
import random

mode_list = [-1, 60, 3600]
vertex_list = [1000, 2000, 3000, 4000, 5000]

for vertex in vertex_list:
    iterate = 0
    while iterate < 30:
        d = datetime.datetime.today()
        print d.strftime("%Y-%m-%d %H:%M:%S"), '\n'
        while 1:
            start = random.randint(1, vertex)
            goal = random.randint(1, vertex)
            if start != goal:
                break
        print"%d -> %d" % (start, goal)
        for mode in mode_list:
            cmd = "./go -start %s -goal %s -dptr_h 18 -dptr_m 0 -kph 30 -gx 50 -gy 50 -lam 1 -mode %s -tim_intrvl %s -alg 1 -edc 3" % (start, goal, vertex, mode)
            subprocess.call( cmd.strip().split(" ")  )
        iterate = iterate+1
