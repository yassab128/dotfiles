#!/usr/bin/env perl

# XXX working great
use strict;
use warnings FATAL => 'all';
use HTTP::Tiny;

my $ariaLogFile = '/tmp/ariaParser.txt';

sub cleanup {
    open( my $FH, '>', '/home/pactrag/Programme/sysinfo/misc/ariaProgress' )
      || die "cannot open file";
    print $FH "\n";
    close $FH;
}

# SIGINT is what happens when you do CTRL-C from the terminal
# SIGTERM is the default signal sent by kill (-15)
$SIG{TERM} = $SIG{INT} = sub {
    cleanup();
    exit;
};

while (1) {

    cleanup();

    my $last_modified_time = ( stat $ariaLogFile )[9];
    while ( ( stat $ariaLogFile )[9] eq $last_modified_time ) {
        sleep 1;
    }

   # the backticks are better; system() somehow clogs the whole script and makes
   # it unresponsive to kill signals

    while (1) {
        my $response = HTTP::Tiny->new->request(
            'POST',
            'http://localhost:6800/jsonrpc',
            {
                content => '{
					"id":"",
					"method":"aria2.tellActive"
				}',
            }
        );

        die "Failed: $response->{reason}\n" unless $response->{success};

        my $count           = 0;
        my $completedLength = 0;
        my $downloadSpeed   = 0;
        my $totalLength     = 0;

        while (
            $response->{content} =~ /
				(?<=,"completedLength":")(.*?)(?=")|
				(?<="downloadSpeed":")(.*?)(?=")|
				(?<="totalLength":")(.*?)(?=")
				/xg
          )
        {
            if ( ++$count == 1 ) {
                $completedLength += $&;
                $count = 1;

            }
            elsif ( $count == 2 ) {
                $downloadSpeed += $&;

            }
            else {
                $totalLength += $&;
            }
        }

        if ( $totalLength == 0 ) {
            last;
        }

        open( my $FH, '>', '/home/pactrag/Programme/sysinfo/misc/ariaProgress' )
          || die "cannot open file";
        printf $FH "%dk | %s | %.02f%%\n",
          $downloadSpeed / 1024,
          $count / 3,
          $completedLength / $totalLength * 100;
        close $FH;
        sleep 1;
    }
}
