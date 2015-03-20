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

my @st_hour=(13, 15, 17, 19, 21);  #突然変異率を格納した配列
my @strlist;   #ファイルの中身（最終更新世代数or適応度）

print "start\n";

#perlプログラム実行開始日時出力
($sec,$min,$hour,$mday,$mon,$year,$wno) = localtime(time);
my @wdays = ('SUN','MON','TUE','WED','THU','FRI','SAT');
$year+=1900;
$mon+=1;
print "$year/$mon/$mday ($wdays[$wno]) $hour:$min:$sec\n";

#プログラムを実行し，実行結果をファイルに順次保存
#estimateParameter_ver
#for($estimateUncomParameter=1;$estimateUncomParameter<=1;$estimateUncomParameter++){
#	open(OUT,">convergenceData/estUncomParameter_$estimateUncomParameter.csv");
#   print OUT "\"estimateUncomParameter\", $estimateUncomParameter\n";                   #mutation_rateをファイル出力	
#    close(OUT);
	
#	for($seed=0;$seed<$ind;$seed++){
#		$result=system("./go -seed $seed -cross 0.4 -mutate 0.2 -estimate $estimateUncomParameter >> convergenceData/estUncomParameter_$estimateUncomParameter.csv");  	
#	}
#}


foreach my $start_hour(@st_hour){	
	for($seed=0;$seed<$ind;$seed++){
		$result=system("./go -seed $seed -inverse -2 -start_hour $start_hour >> start_hour/start_hour_$start_hour.csv");  	
	}
}

#open(OUT,"> estimateUncomData/generation_kekka.csv");		
#foreach(@mutation){		
#	print OUT ",$_";
#}
#print OUT "\n";

#for($estimateUncomParameter=0;$estimateUncomParameter<=0.7;$estimateUncomParameter+=0.1){
#	print OUT "$estimateUncomParameter,";		
 #   $sum_result=0;
  #  $i=0;
  #  open(DAT,"estimateUncomData/estUncomParameter_$estimateUncomParameter.csv");		
  #  while($line = <DAT>){	
  #    @strlist=split(/, /, $line);		
  #    if($i>=1){       #if line num over 2 start calculation		
  #      $sum_result+=$line;		
  #    }
    #  $i++;
    #}
    #$avg_result = $sum_result/$ind;		#average of result		
    #close(DAT);
   # print OUT "$avg_result,";		
  #print OUT "\n";
#}
#close(OUT);


($sec,$min,$hour,$mday,$mon,$year,$wno) = localtime(time);
$year+=1900;
$mon+=1;
print "$year/$mon/$mday ($wdays[$wno]) $hour:$min:$sec\n";
print "end\n";

=comment
