#!/usr/bin/env perl

# my @files_and_directories = glob $downloadDir . '*';
# my @files                 = grep -f, @files_and_directories;
# print @files;

# XXX guaranteed to work
# XXX the script will work as long as the filenames do not contain newlines
# who does that anyway?
# XXX it is actually better to implement this script with File::Find
# but some systems may not have that module so it is better to fall back
# to recursion anyway

use strict;
use warnings FATAL => 'all';
use Digest::MD5 'md5_hex';

my $depth = $ARGV[0];
die "Usage: $0 [DEPTH]\n" if @ARGV != 1;
my $path = ".";

# get the number of slashes in the path, it will be used later for $depth
my $c = () = $path =~ /\//g;

# this is a recursive subroutine
sub find {

    # the hash that will contain md5 hashes as keys and the files as values
    # separated by a new line
    my %hash;

    # `@_/*` is the absolute path (do not change it)
    foreach my $case ( glob "@_/*" ) {

        # check if the case is a file
        if ( -f $case ) {

            # get the md5 hash (the next two lines are needed, otherwise
            # the hash of path itself will be used as string, we do
            # not want that)
            open( my $fh, '<', $case ) or die "Can't open '$case': $!";
            binmode($fh);
            $hash{ Digest::MD5->new->addfile($fh)->hexdigest } .= "\n$case";

        }

        # in case it is a directory
        elsif ( -d $case ) {

            # get the number of slash in the directory path
            my $count = () = $case =~ /\//g;

            # dumb comparison, but I want to make $depth similar to GNU find
            # depth
            if ( $count + 1 <= $c + $depth ) {
                find($case);
            }

        }
    }

    foreach my $name ( keys %hash ) {

        # split the string based on the newlines in it
        my @F = split /\n/, $hash{$name};

        # sort based on the length of the strings
        @F = sort { length($b) cmp length($a) } @F;

      # remove the last two elements from the array (which are the shortest line
      # and the blank line)
        $#F -= 2;

        foreach (@F) {

            # print "rm $_\n";

            unlink $_;
        }
    }

}

find($path);
