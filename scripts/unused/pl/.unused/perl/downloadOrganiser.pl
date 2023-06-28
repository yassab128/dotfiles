#!/usr/bin/env perl

# XXX guaranteed to work
use strict;
use warnings FATAL => 'all';

# needed in order to move the files
use File::Copy;

# removing the trailing slash with break the whole script
my $downloadDir = '/home/pactrag/Downloads/';

# my $downloadDir = '/home/pactrag/Programme/couldron/Downloads/';

# I concatenated $downloadDir manually to each directory since I could not find
# a better way to do it performance-wise (stop trying, enough time has been
# wasted; the script will perform a bit better this way anyway; hardcoding FTW)
my %filetype = (
    $downloadDir . 'Compressed',
    '|zip|rar|7z|gzip|gz|tar|arj|gz|xz|iso|',

    $downloadDir . 'Documents', '|ps|docx|pdf|doc|txt|rtf|odt|tex|
	|docm|wks|wps|ppt|ods|pptx|xlr|xlt|xls|xlsx|xml|key|rss|cer|djvu|fb2|fb3|
	|mobi|epub|azw|lit|odf|kfx|',

    $downloadDir . 'Executables',
    '|exe|apk|com|deb|msi|dmg|bin|vcd|pl|
	|cgi|jar|py|wsf|rpm|',

    $downloadDir . 'Music',
    '|mp3|aac|wav|ogg|flac|wma|ac3|pcm|aiff|
	|alac|wpl|aa|act|ape|m4a|m4p|oga|mogg|tta|',

    $downloadDir . 'Pictures',
    '|jpg|jpeg|peg|png|gif|tiff|tif|svg|bmp|
	|ai|ico|icns|ppm|pgm|pnm|pbm|bgp|webp|',

    $downloadDir . 'Videos',
    '|mkv|avi|3gp|3g2|mov|bik|wmv|flv|swf|m2v|,
	|m2p|vob|ifo|mp4|m4v|mpg|asf|mpeg|mpv|qt|webm|ogv|',
);

# create the directories if they do not exist
foreach my $i ( keys %filetype ) {
    unless ( -d $i ) {
        mkdir $i;
    }
}

my %hash;

# open the download directory
opendir DIR, $downloadDir || die $!;

# iterate over the contents of the download directory
while ( my $file = readdir DIR ) {

    # make $file the absolute path of the file (which is the basename)
    $file = $downloadDir . $file;

    # skip to the next iteration unless it is a file that is being processed
    next unless -f $file;

    # extract the extension from the file (two steps required)
    my $fileExtension = $file;
    $fileExtension =~ s/.*\.//;

    # remove .aria2 .part (for firefox) extension
    $file =~ s/\.(aria2|part)$//;

    # we got to rid of of duplicates
    unless ( exists $hash{$file} ) {
        $hash{$file} = $fileExtension;
    }
    else {
        delete $hash{$file};
    }
}

# close the download directory
closedir DIR;

foreach my $i ( keys %hash ) {

    # iterate over the extensions to see to which filetype the file belongs
    # based on its extension
    foreach my $j ( keys %filetype ) {

        # see whether these extension include fileExtension
        # $hash{$i} is the extension, $filetype{$j} are the extensions
        if ( $filetype{$j} =~ /\|$hash{$i}\|/ ) {

            # print "mv '$i' $j\n";
            # print "execute the command? (y/n): ";
            # chomp( my $choice = <STDIN> );
            # if ( $choice eq 'y' ) {
            move( $i, $j )
              || die 'issue with either the original file or ',;

            # }
        }
    }
}
