use strict;
use warnings;


my $line;        #Contens of file
my $seed;        #seed value
my $i;           #show line num
my $ind=30;     #trial time
my $cross;       #crossover_rate value
my $result;      #output of GA program's data which is  supposed final update generation or elite fitness
my $sum_result;  #sum total of result(this variable is used to calculate average of result)
my $avg_result;  #average of result

my $sec;
my $min;
my $hour;
my $mday;
my $mon;
my $year;
my $wno;

my @mutation=(0.01,0.02,0.05,0.1,0.2,0.5);  #突然変異率を格納した配列
my @strlist;   #ファイルの中身（最終更新世代数or適応度）

print "start\n";

#perlプログラム実行開始日時出力
($sec,$min,$hour,$mday,$mon,$year,$wno) = localtime(time);
my @wdays = ('SUN','MON','TUE','WED','THU','FRI','SAT');
$year+=1900;
$mon+=1;
print "$year/$mon/$mday ($wdays[$wno]) $hour:$min:$sec\n";

#プログラムを実行し，実行結果をファイルに順次保存
foreach my $mutate(@mutation){                                 #mutation配列の要素数の回数ループを回す
  for($cross=0.1;$cross<=1;$cross+=0.1){                       #crossover_rateを0.1から0.1ずつ増やす
    open(OUT,"> data/mut_[$mutate]_crs_$cross.csv");            #100試行分のデータを格納するファイルの生成
    print OUT "\"mutation_rate\", $mutate\n";                   #mutation_rateをファイル出力
    print OUT "\"crossover_rate\", $cross\n";	
    close(OUT);
		
    for($seed=0;$seed<$ind;$seed++){			
      $result=system("./go -seed $seed -cross $cross -mutate $mutate -estimate 1 >> data/mut_[$mutate]_crs_$cross.csv");  
    }
  }
}


open(OUT,"> data/fitness_kekka.csv");		
foreach(@mutation){		
	print OUT ",$_";
}
print OUT "\n";

for($cross=0.1;$cross<=1;$cross+=0.1){
  print OUT "$cross,";		
  foreach my $mutate(@mutation){
    $sum_result=0;
    $i=0;
    open(DAT,"data/mut_[$mutate]_crs_$cross.csv");		
    while($line = <DAT>){	
      @strlist=split(/, /, $line);		
      if($i>=2){       #if line num over 2 start calculation		
        $sum_result+=$line;		
      }
      $i++;
    }
    $avg_result = $sum_result/$ind;		#average of result		
    close(DAT);
    print OUT "$avg_result,";		
  }
  print OUT "\n";
}
close(OUT);


($sec,$min,$hour,$mday,$mon,$year,$wno) = localtime(time);
$year+=1900;
$mon+=1;
print "$year/$mon/$mday ($wdays[$wno]) $hour:$min:$sec\n";
print "end\n";

=comment
