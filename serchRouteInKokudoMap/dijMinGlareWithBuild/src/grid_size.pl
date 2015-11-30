use strict;
use warnings;


my $line;        #Contens of file
my $seed;        #seed value
my $i;           #show line num
my $ind=10;     #trial time
my $cross;       #crossover_rate value
my $result;      #output of GA program's data which is  supposed final update generation or elite fitness
my $sum_result;  #sum total of result(this variable is used to calculate average of result)
my $avg_result;  #average of result
my $estimateUncomParameter;
#my $start_hour;

my $sec;
my $min;
my $hour;
my $mday;
my $mon;
my $year;
my $wno;

my @dp_h = (17); #出発時刻[h]
my @dp_m = (0); #出発時刻[m]
my @grid_x=(100);  #x方向のサイズ
my @grid_y=(5, 10, 20, 50, 100, 200, 500, 1000); #y方向のサイズ


my @strlist;   #ファイルの中身（最終更新世代数or適応度）

print "start\n";

#perlプログラム実行開始日時出力
($sec,$min,$hour,$mday,$mon,$year,$wno) = localtime(time);
my @wdays = ('SUN','MON','TUE','WED','THU','FRI','SAT');
$year+=1900;
$mon+=1;
print "$year/$mon/$mday ($wdays[$wno]) $hour:$min:$sec\n";



foreach my $dp_h(@dp_h){
	foreach my $dp_m(@dp_m){
		
		foreach my $grid_x(@grid_x){
			foreach my $grid_y(@grid_y){
				($sec,$min,$hour,$mday,$mon,$year,$wno) = localtime(time);
				my @wdays = ('SUN','MON','TUE','WED','THU','FRI','SAT');
				$year+=1900;
				$mon+=1;
				print "****************\n";
				print "$year/$mon/$mday ($wdays[$wno]) $hour:$min:$sec\n";
				print "$grid_x $grid_y\n";
				$result = system("./go -start 22000 -goal 1 -dptr_h $dp_h -dptr_m $dp_m -kph 60 -gx $grid_x -gy $grid_y -lam 1 -mode 0 >> calc_time_[$dp_h]_[$dp_m]_621_unko.csv"); 	
				print "****************\n";
			}
		}
	}
}



($sec,$min,$hour,$mday,$mon,$year,$wno) = localtime(time);
$year+=1900;
$mon+=1;
print "$year/$mon/$mday ($wdays[$wno]) $hour:$min:$sec\n";
print "end\n";

=comment
