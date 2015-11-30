import sys
import commands
import subprocess
import Gnuplot

if __name__ == '__main__':
	argv = sys.argv
	argc = len(argv)
	if(argc != 5):
		print 'Usage : python createRtCmpFig.py dp_pos ar_pos dp_hour dp_min'
		quit()
			
	start = int(argv[1])
	goal = int(argv[2])
	hour = int(argv[3])
	min = int(argv[4])

	cmd = "./go -start %s -goal %s -dptr_h %s -dptr_m %s -kph 30 -gx 100 -gy 50 -lam 1 -mode 0" % (start, goal, hour, min)
	subprocess.call( cmd.strip().split(" ")  )
	cmd = "./go -start %s -goal %s -dptr_h %s -dptr_m %s -kph 30 -gx 100 -gy 50 -lam 1 -mode 1" % (start, goal, hour, min)
	subprocess.call( cmd.strip().split(" ")  )
	
	g = Gnuplot.Gnuplot()
	g(' cd "C:\cygwin\home\sosmem\matsuda\kenkyuu\serchRouteInKokudoMap\dijMinGlareWithBuild_lambda\route" ')
	g('unset xtics')
	g('unset ytics')
	g('set term png set size 1200 900')
	g('set output "../route/PNG/route_%s_%s_%s_%s.png"' % (start, goal, hour, min))
	g('pl "../route/DAT/map_xy_coord.dat" u 1:2 w l lc rgb "grey70" notitle, "../route/DAT/dist_%s_%s_%s_%s.dat" u 1:2 w l lw 6 lc rgb "blue" title "MTDA", "../route/DAT/uncom_%s_%s_%s_%s.dat" u 1:2 w l lw 6 lc rgb "red" title "MUDA"' % (start, goal, hour, min, start, goal, hour, min))
	g('set output "../glr_route/PNG/glr_route_%s_%s_%s_%s.png"' % (start, goal, hour, min))
	g('pl "../glr_route/DAT/map_xy_coord.dat" u 1:2 w l lc rgb "grey70" notitle, "../glr_route/DAT/glr_dist_%s_%s_%s_%s.dat" u 1:2 w l lw 6 lc rgb "blue" title "MTDA", "../glr_route/DAT/glr_uncom_%s_%s_%s_%s.dat" u 1:2 w l lw 6 lc rgb "red" title "MUDA"' % (start, goal, hour, min, start, goal, hour, min))
	g('set terminal wxt')
	g('exit')