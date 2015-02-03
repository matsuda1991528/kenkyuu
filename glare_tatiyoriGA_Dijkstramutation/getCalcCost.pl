use strict;
use warnings;


my $line;        #Contens of file
my $seed;        #seed value
my $i;           #show line num
my $ind=100;     #trial time
my $cross;       #crossover_rate value
my $result;      #output of GA program's data which is  supposed final update generation or elite fitness
my $sum_result;  #sum total of result(this variable is used to calculate average of result)
my $avg_result;  #average of result
my $estimateUncomParameter;

my $sec;
my $min;
my $hour;
my $mday;
my $mon;
my $year;
my $wno;

my @mutation=(0.001, 0.005, 0.01,0.02,0.05,0.1,0.2,0.5);  #�ˑR�ψٗ����i�[�����z��
my @strlist;   #�t�@�C���̒��g�i�ŏI�X�V���㐔or�K���x�j

print "start\n";

#perl�v���O�������s�J�n�����o��
($sec,$min,$hour,$mday,$mon,$year,$wno) = localtime(time);
my @wdays = ('SUN','MON','TUE','WED','THU','FRI','SAT');
$year+=1900;
$mon+=1;
print "$year/$mon/$mday ($wdays[$wno]) $hour:$min:$sec\n";

#�v���O���������s���C���s���ʂ��t�@�C���ɏ����ۑ�
#foreach my $mutate(@mutation){                                 #mutation�z��̗v�f���̉񐔃��[�v����
#  for($cross=0.1;$cross<=1;$cross+=0.1){                       #crossover_rate��0.1����0.1�����₷
#    open(OUT,"> data/mut_[$mutate]_crs_$cross.csv");            #100���s���̃f�[�^���i�[����t�@�C���̐���
#    print OUT "\"mutation_rate\", $mutate\n";                   #mutation_rate���t�@�C���o��
#    print OUT "\"crossover_rate\", $cross\n";	
#    close(OUT);
		
#    for($seed=0;$seed<$ind;$seed++){			
#      $result=system("./go -seed $seed -cross $cross -mutate $mutate >> data/mut_[$mutate]_crs_$cross.csv");  
#    }
#  }
#}

for($estimateUncomParameter=1;$estimateUncomParameter<=1;$estimateUncomParameter++){
	open(OUT,">calcCostData/calcCost.csv");
    print OUT "\"estimateUncomParameter\", $estimateUncomParameter\n";                   #mutation_rate���t�@�C���o��	
    close(OUT);
	
	for($seed=0;$seed<$ind;$seed++){
		$result=system("./go -seed $seed -cross 0.1 -mutate 0.1 -estimate $estimateUncomParameter >> calcCostData/calcCost.csv");  	
	}
}

open(OUT,"> calcCostData/calcCost_kekka.csv");		
#foreach(@mutation){		
#	print OUT ",$_";
#}
#print OUT "\n";

for($estimateUncomParameter=1;$estimateUncomParameter<=1;$estimateUncomParameter++){
	print OUT "$estimateUncomParameter,";		
    $sum_result=0;
    $i=0;
    open(DAT,"calcCostData/calcCost.csv");		
    while($line = <DAT>){	
      @strlist=split(/, /, $line);		
      if($i>=1){       #if line num over 2 start calculation		
        $sum_result+=$line;		
      }
      $i++;
    }
    $avg_result = $sum_result/$ind;		#average of result		
    close(DAT);
    print OUT "$avg_result,";		
  print OUT "\n";
}
close(OUT);


($sec,$min,$hour,$mday,$mon,$year,$wno) = localtime(time);
$year+=1900;
$mon+=1;
print "$year/$mon/$mday ($wdays[$wno]) $hour:$min:$sec\n";
print "end\n";

=comment
