set tics font "Times New Roman,28"   # �ڐ���̃t�H���g�̕ύX
set xlabel font "Times New Roman,28" # xlabel�̃t�H���g�̕ύX
set ylabel font "Times New Roman,28" # ylabel�̃t�H���g�̕ύX
set zlabel font "Times New Roman,28" # zlabel�̃t�H���g�̕ύX
set key font "Times New Roman,28"    # �}��̃t�H���g�̕ύX

######################
#�����̕s������\���p�����[�^�]���������s���ꍇ 
######################
#unset xtics #x���W�̖ڐ��폜
#unset ytics#y���W�̖ڐ��폜
#set term postscript enhanced eps color
#set output 'figure\lam_route_15917_6570_1800.eps'
#pl "../route/map_xy_coord.dat" u 1:2 w l lc rgb "grey70" notitle, "lam0_15917_6570_18_0.dat" u 1:2 w l lw 10 lc rgb "blue" title "{/Symbol l} = 0", "lam1_15917_6570_18_0.dat" u 1:2 w l lw 10 lc rgb "red" title "{/Symbol l} = 1", "lamInf_15917_6570_18_0.dat" u 1:2 w l lw 10 lc rgb "dark-green" title "{/Symbol l} = {/Symbol \245}" 

######################
#�e�����ɂ�����s���x�̖_�O���t���쐬����ꍇ
######################
#set term postscript enhanced eps color
#set output "MinDistUncomComp.eps"
#set boxwidth 1.5
#set xtics rotate by -90
#set style fill solid border lc rgb "black"
#pl "minDistUncomComp.dat" using ($0*4+0):2:xtic(1) with boxes lw 2 lc rgb "blue" title "MTDA", "minDistUncomComp.dat" using ($0*4+1.5):3 with boxes lw 2 lc rgb "red" title "MUDA"

######################
#��̌o�H��\������ꍇ 
######################
unset xtics #x���W�̖ڐ��폜
unset ytics#y���W�̖ڐ��폜
set term postscript enhanced eps color
set output 'figure\route_11903_9875_1700.eps'
pl "DAT/map_xy_coord.dat" u 1:2 w l lc rgb "grey70" notitle, "DAT/dist_11903_9875_17_0.dat" u 1:2 w l lw 10 lc rgb "blue" title "MTDA", "DAT/uncom_11903_9875_17_0.dat" u 1:2 w l lw 10 lc rgb "red" title "MUDA" 
set output '../glr_route/figure\glr_route_11903_9875_1700.eps'
pl "../glr_route/DAT/map_xy_coord.dat" u 1:2 w l lc rgb "grey70" notitle, "../glr_route/DAT/glr_dist_11903_9875_17_0.dat" u 1:2 w l lw 10 lc rgb "blue" title "MTDA", "../glr_route/DAT/glr_uncom_11903_9875_17_0.dat" u 1:2 w l lw 10 lc rgb "red" title "MUDA" 
