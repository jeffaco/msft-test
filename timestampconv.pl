#!/usr/bin/perl -w

# Converts Pegasus timestamps into readable form
# Use like
#  ./timestampconv.pl cimserver.trc > outfile.trc

use Time::gmtime;

while (<>) {
    if (/(^\d+)(s-.*)/) {
	print gmctime($1);
	print "$2\n";
    }
    else {
	print "$_";
    }
}
