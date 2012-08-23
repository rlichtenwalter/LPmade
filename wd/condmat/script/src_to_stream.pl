#! /usr/bin/perl

use strict;
use warnings FATAL => 'all';


my $processingEdges = 0;
while( my $line = <STDIN> ) {
	chomp( $line );
	if( $line =~ m/\*Hyperedges:.*/ ) {
		$processingEdges = 1;
		next;
	}
	if( $processingEdges == 1 ) {
		processEdge( $line );
	}
}

sub processEdge {
	my ($line) = @_;
	$line =~ m/(\d+) (\d+) \[ (.*) \]/;
	my $time = $1;
	my $edgeCount = $2;
	my $edgeList = $3;
	my @edgeArray = split( / /, $edgeList );
	foreach my $i (0..$#edgeArray) {
		foreach my $j ($i+1..$#edgeArray) {
			my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime( $time );
			$year += 1900;
			$mon += 1;
			printf STDOUT "$year-%02d-%02d %02d:%02d:%02d,$edgeArray[$i],$edgeArray[$j],%.6f\n", $mon, $mday, $hour, $min, $sec, 1 / ($edgeCount-1);
			printf STDOUT "$year-%02d-%02d %02d:%02d:%02d,$edgeArray[$j],$edgeArray[$i],%.6f\n", $mon, $mday, $hour, $min, $sec, 1 / ($edgeCount-1);
		}
	}
}
