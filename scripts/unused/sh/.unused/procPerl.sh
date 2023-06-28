#!/bin/sh

set -efu

# list all running processes
ps -eo comm --sort=size | perl -e '
use strict;
use warnings FATAL => "all";
while (<>) {
    @F = split(" ");
    $total{$_}++ for (@F);

    END {
        for ( keys %total ) {
            if ( $total{$_} == 1 ) {
                print "$_\n" unless /^\(sd-pam\)$|^COMMAND$|^kworker/;
            }
            else {
                print "$_ [$total{$_}]\n";
            }
        }
    }
}
' | fzf
