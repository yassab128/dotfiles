#!/usr/bin/env perl

use strict;
use warnings FATAL => 'all';
use HTTP::Tiny;

if ( $ARGV[0] eq '-g' ) {

    # print "cyka\n";
    my $response = HTTP::Tiny->new->request(
        'POST',
        'http://localhost:6800/jsonrpc',
        {
            content => '{
					"id": "",
					"method": "aria2.getGlobalStat"
				}',
        }
    );

    die "Failed: $response->{reason}\n" unless $response->{success};

    my $count = 0;
    while (
        $response->{content} =~ /
				(?<="downloadSpeed":")(.*?)(?=",")|
				(?<="numActive":")(.*?)(?=",")|
				(?<="numStopped":")(.*?)(?=",")|
				(?<="numStoppedTotal":")(.*?)(?=",")|
				(?<="numWaiting":")(.*?)(?=",")
				/xg
      )
    {
        if ( ++$count == 1 ) {
            printf "downloadSpeed: %dk\n", $& / 1024;
        }
        elsif ( $count == 2 ) {
            print "numActive: $&\n";
        }
        elsif ( $count == 3 ) {
            print "numStopped: $&\n";
        }
        elsif ( $count == 4 ) {
            print "numStoppedTotal: $&\n";
        }
        else {
            print "numWaiting: $&\n";
        }
    }
}
elsif ( $ARGV[0] eq '-t' ) {
    my $response = HTTP::Tiny->new->request(
        'POST',
        'http://localhost:6800/jsonrpc',
        {
            content => '{
					"id": "",
					"method": "aria2.tellActive"
				}',
        }
    );

    die "Failed: $response->{reason}\n" unless $response->{success};

    # will be used to check whether downloads are running or not
    my $emptyResponse = '{"id":"","jsonrpc":"2.0","result":[]}';

    # this step is necessary -- the string comparison (the next if) will not
    # work unless I reassigned $response as $response->{content}
    $response = $response->{content};
    if ( $response eq $emptyResponse ) {
        my $response = HTTP::Tiny->new->request(
            'POST',
            'http://localhost:6800/jsonrpc',
            {
                content => '{
					"id": "",
					"method": "aria2.unpauseAll"
				}',
            }
        );
        print "resumed\n";
    }
    else {
        my $response = HTTP::Tiny->new->request(
            'POST',
            'http://localhost:6800/jsonrpc',
            {
                content => '{
					"id": "",
					"method": "aria2.pauseAll"
				}',
            }
        );
        print "stopped\n";
    }
}
elsif ( $ARGV[0] eq '-a' ) {

    my $uri = "\"$ARGV[1]\"";

    my $response = HTTP::Tiny->new->request(
        'POST',
        'http://localhost:6800/jsonrpc',
        {
            content => '{
					"id":"",
					"method":"aria2.addUri",
					"params": [[' . $uri . ']]
			}',
        }
    );
    print "$response->{content}\n";
    my $shit = 5;
    print "$shit\n";
}
