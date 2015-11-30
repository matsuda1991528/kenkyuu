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

my @st_hour=(16, 17, 18, 19);  #出発時刻を格納した配列
my @st_min=(0, 30); #出発時刻を格納した配列
my @strlist;   #ファイルの中身（最終更新世代数or適応度）

print "start\n";

#perlプログラム実行開始日時出力
($sec,$min,$hour,$mday,$mon,$year,$wno) = localtime(time);
my @wdays = ('SUN','MON','TUE','WED','THU','FRI','SAT');
$year+=1900;
$mon+=1;
print "$year/$mon/$mday ($wdays[$wno]) $hour:$min:$sec\n";

foreach my $start_hour(@st_hour){	
	foreach my $start_min(@st_min){
		print "****************\n";
		print "$start_hour $start_min\n";
		$result = system("./go -start 22000 -goal 1 -dptr_h $start_hour -dptr_m $start_min -kph 60"); 	
		print "****************\n";
	}
}



($sec,$min,$hour,$mday,$mon,$year,$wno) = localtime(time);
$year+=1900;
$mon+=1;
print "$year/$mon/$mday ($wdays[$wno]) $hour:$min:$sec\n";
print "end\n";

=comment
