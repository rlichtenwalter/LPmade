#! /usr/bin/perl

use strict;
use warnings FATAL => 'all';

srand(1);

my %vertex_hash = ();

while( my $line = <STDIN>) {
	chomp( $line );
	my ($d1,$d2,$weight) = split( /\s+/, $line );
	$d1 =~ s/^0+//;
	$d2 =~ s/^0+//;
	
	# add vertices
	if( !defined( $vertex_hash{$d1} ) ) {
		$vertex_hash{$d1} = {};
	}
	if( !defined( $vertex_hash{$d2} ) ) {
		$vertex_hash{$d2} = {};
	}
	
	# determine unset weights
	if( $weight eq "f" ) {
		my $count = 0;
		$weight = 0;
		my $neighbor_hash_ref = $vertex_hash{$d1};
		foreach my $neighbor (keys(%{$neighbor_hash_ref})) {
			$count++;
			$weight += $neighbor_hash_ref->{$neighbor};
		}
		$neighbor_hash_ref = $vertex_hash{$d2};
		foreach my $neighbor (keys(%{$neighbor_hash_ref})) {
			$count++;
			$weight += $neighbor_hash_ref->{$neighbor};
		}
		$weight /= $count;
	}
	
	# add edge with weight
	if( !defined( $vertex_hash{$d1}->{$d2} ) ) {
		$vertex_hash{$d1}->{$d2} = $weight;
	}
	if( !defined( $vertex_hash{$d2}->{$d1} ) ) {
		$vertex_hash{$d2}->{$d1} = $weight;
	}
}

foreach my $vertex (sort(keys(%vertex_hash))) {
	my $neighbor_hash_ref = $vertex_hash{$vertex};
	foreach my $neighbor (sort(keys(%{$neighbor_hash_ref}))) {
		if( $vertex lt $neighbor ) {
			my $weight = $neighbor_hash_ref->{$neighbor};
			my $random = rand() * 10;
			foreach my $check (1..10) {
				if( $random <= $check ) {
					print STDOUT "$check,$vertex,$neighbor,$weight\n";
					print STDOUT "$check,$neighbor,$vertex,$weight\n";
					last;
				}
			}
		}
	}
}

