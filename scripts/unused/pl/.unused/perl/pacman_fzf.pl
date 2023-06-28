#!/usr/bin/perl

# Use statements {{{1
use strict;

# Halt the script when any type of warning rises.
use warnings 'all', 'FATAL';

# Parse command-line options; similar to the shell builtin command `getopts'.
use Getopt::Std;

# Subroutines {{{1
sub read_flags {
    my %opts;
    getopts( '1EP:Rabceimor', \%opts );

    my $order_method =
      $opts{1}
      ? 'ascending'
      : 'descending';

    my $output_method =
        !-f '/usr/bin/fzf' ? 'raw'
      : $opts{R}           ? 'raw'
      :                      'fzf';

    my $sort_method =
        $opts{a} ? 'avail_opt_deps'
      : $opts{b} ? 'build_date'
      : $opts{c} ? 'bins'
      : $opts{e} ? 'explicit'
      : $opts{i} ? 'install_date'
      : $opts{m} ? 'manpages'
      : $opts{o} ? 'opt_deps'
      : $opts{r} ? 'rev_deps'
      :            'size';

    $opts{P} && fzf_preview( $opts{P} );

    my $show_explicit_only = $opts{E} ? 1 : 0;

    return $order_method, $output_method, $sort_method, $show_explicit_only;
}

sub fzf_preview {
    my ($package) = @_;

    my $size_cmd = "LC_ALL=C pacman -Rcs --print-format '%s' $package";

    my $total_size = 0;
    open my $PH, '-|', $size_cmd;
    $_ = <$PH>;
    if (/^::/) {
        $total_size = 0;
    }
    else {
        $total_size += $_;
        while (<$PH>) {
            $total_size += $_;
        }
    }
    close $PH;

    printf "Total size\t: %.2f MiB\n", $total_size / 1_048_576;

    my @query_cmd = `LC_ALL=C /usr/bin/pacman -Qilq $package`;

    my @manpages;
    my @binaries;
    my $flag = 0;
    foreach (@query_cmd) {
        if (/^Provides/) {
            print;
            $flag = 1;
        }
        elsif ( $flag && /^Replaces/ ) {
            print;
            $flag = 0;
        }
        elsif ($flag) {
            print;
        }
        elsif (/^Description|^Build|^Install.[Dd]/) {
            print;
        }
        elsif (m'^/usr/bin.*/([^/]+)\n$') {
            push @binaries, $1;
        }
        elsif (m'^/usr/share/man/man[1-8]/(.*?)\.') {
            push @manpages, $1;
        }
    }

    my $manpages_number = scalar @manpages;
    my $binaries_number = scalar @binaries;

    my $m;
    my $n;
    if ( $manpages_number > $binaries_number ) {
        ( $m, $n ) = ( $binaries_number, $manpages_number );
        for ( my $i = $m ; $i < $n ; ++$i ) {
            push @binaries, '';
        }
    }
    else {
        ( $m, $n ) = ( $manpages_number, $binaries_number );
        for ( my $i = $m ; $i < $n ; ++$i ) {
            push @manpages, '';
        }
    }

    printf "\n%-40s%s", 'Binaries', "Manpages\n";
    for ( my $i = 0 ; $i < $n ; ++$i ) {
        printf "%-40s%s\n", $binaries[$i], $manpages[$i];
    }

    exit;
}

sub convert_pkg_size_to_mib {
    my ( $pkg_size, $unit_used ) = @_;
    my $divisor =
        $unit_used eq 'KiB' ? 1024
      : $unit_used eq 'MiB' ? 1
      :

      # The only unit left to check is "B" (there is no ``GiB'').
      1_048_576;
    return $pkg_size / $divisor;
}

sub convert_to_yyyymmddhhmmss {
    my ( $b, $e, $H, $M, $S, $Y ) = @_;
    my $m =
        $b eq 'Jan' ? 1
      : $b eq 'Feb' ? 2
      : $b eq 'Mar' ? 3
      : $b eq 'Apr' ? 4
      : $b eq 'May' ? 5
      : $b eq 'Jun' ? 6
      : $b eq 'Jul' ? 7
      : $b eq 'Aug' ? 8
      : $b eq 'Sep' ? 9
      : $b eq 'Oct' ? 10
      : $b eq 'Nov' ? 11
      :               12;
    return sprintf '%04d%02d%02d%02d%02d%02d', $Y, $m, $e, $H, $M, $S;

    # I originally used the Unix timestamp instead of yyyymmddhhmmss
    # in order to compare build/installation dates of packages, but I found
    # out that this method is faste, has no drawbacks, does not require
    # using modules like Time::Piece
}

sub occurances_count {
    my ($string) = @_;
    my $word_count = () = $string =~ /\S+/g;
    return $word_count;
}

sub parse {
    my (@command) = @_;

    # Initialize the hash.
    my %pkgs;

    # Will be used as a key for the hash above.
    my $package_name;

    # Will be used to calculate the number of dots to use in order to
    # dotfill the empty spaces that come after the name of a package.
    my $longest_package_name = 0;

    # Will be later joined to form a single regex.
    my $mm     = qr/\s+:\s\w{3}\s(\w{3})/;
    my $dd     = qr/\s{1,2}(\d{1,2})/;
    my $HHMMSS = qr/\s(\d{2}):(\d{2}):(\d{2})/;
    my $yyyy   = qr/\s(\d{4})/;

    # Will make line checking a bit faster.
    my $flag = 8;
    foreach (@command) {
        if ( $flag == 8 && /^Name\s+:\s(.*)/ ) {
            $package_name = $1;
            if ( length $package_name > $longest_package_name ) {
                $longest_package_name = length $package_name;
            }
            $pkgs{$package_name}{'bins'}           = 0;
            $pkgs{$package_name}{'manpages'}       = 0;
            $pkgs{$package_name}{'opt_deps'}       = 0;
            $pkgs{$package_name}{'avail_opt_deps'} = 0;
            $flag                                  = 1;
        }
        elsif ( $flag == 1 && /^Depends\sOn\s+:\s(.*)/ ) {
            $pkgs{$package_name}{'deps'} =
              $1 eq 'None'
              ? 0
              : occurances_count($1);

            $flag = 2;
        }
        elsif ( $flag == 2 && /^Optional\sDeps.*\s(\S+)/ ) {
            $1 eq 'None'
              ? (
                $pkgs{$package_name}{'opt_deps'},
                $pkgs{$package_name}{'avail_opt_deps'}
              )
              =
              ( 0, 0 )
              : $1 eq '[installed]' ? (
                $pkgs{$package_name}{'opt_deps'},
                $pkgs{$package_name}{'avail_opt_deps'}
              )
              =
              ( 1, 1 )
              : $pkgs{$package_name}{'opt_deps'} = 1;
            $flag = 3;
        }
        elsif ( $flag == 3 && /^\s.*\s(\S+)/ ) {
            ++$pkgs{$package_name}{'opt_deps'};
            if ( $1 eq '[installed]' ) {
                ++$pkgs{$package_name}{'avail_opt_deps'};
            }
        }
        elsif ( $flag == 3 && /^Required\sBy\s+:\s(.*)/ ) {
            $pkgs{$package_name}{'rev_deps'} =
              $1 eq 'None'
              ? 0
              : occurances_count($1);

            $flag = 4;
        }
        elsif ( $flag == 4 && /^Installed\sSize\s\s:\s(\S+)\s(\w+)$/ ) {
            $pkgs{$package_name}{'size'} = convert_pkg_size_to_mib( $1, $2 );
            $flag = 5;
        }
        elsif ( $flag == 5 && /^Build\sDate$mm$dd$HHMMSS$yyyy/ ) {
            $pkgs{$package_name}{'build_date'} =
              convert_to_yyyymmddhhmmss( $1, $2, $3, $4, $5, $6 );
            $flag = 6;
        }
        elsif ( $flag == 6 && /^Install\sDate$mm$dd$HHMMSS$yyyy/ ) {
            $pkgs{$package_name}{'install_date'} =
              convert_to_yyyymmddhhmmss( $1, $2, $3, $4, $5, $6 );
            $flag = 7;
        }
        elsif ( $flag == 7 && /^Install\sReason\s+:\s(\w+)/ ) {

            $pkgs{$package_name}{'explicit'} = $1 eq 'Explicitly'

              # ASCII code for ``E''.
              ? 69

              # ASCII code for a space.
              : 32;

            $flag = 8;
        }
        elsif ( $flag == 8 && m'^/usr/bin.*/[^/]+\n$' ) {
            ++$pkgs{$package_name}{'bins'};
        }
        elsif ( $flag == 8 && m'^/usr/share/man/man[1-8]/.' ) {
            ++$pkgs{$package_name}{'manpages'};
        }
    }
    return $longest_package_name, %pkgs;
}

sub show {
    my ( $sort_method, $order_method, $show_explicit_only ) = @_;
    my @cmd = `LC_ALL=C /usr/bin/pacman --query --info --list --quiet`;
    my ( $dots_number, %packages ) = parse(@cmd);
    my $output = '';
    for (
        sort {
                $order_method eq 'descending'
              ? $packages{$b}->{$sort_method} <=> $packages{$a}->{$sort_method}
              : $packages{$a}->{$sort_method} <=> $packages{$b}->{$sort_method}
        }
        keys %packages
      )
    {
        if ( $show_explicit_only && $packages{$_}{'explicit'} == 32 ) {
            next;
        }

        $output .= sprintf "%c| %.*s %-3s %-6s | %-4s | %-4s | %-5s | %s\n",
          $packages{$_}{'explicit'},
          $dots_number,
          "$_ ......................................................",
          "$packages{$_}{'deps'}D",
          "($packages{$_}{'avail_opt_deps'}/$packages{$_}{'opt_deps'})",
          "$packages{$_}{'rev_deps'}R",
          "$packages{$_}{'bins'}B",
          "$packages{$_}{'manpages'}M",
          sprintf '%.2f MiB', $packages{$_}{'size'};
    }
    chomp $output;
    return $output;
}

sub show_raw {
    my ($output) = @_;
    print "$output\n";
    exit;
}

sub show_fzf {
    my ($output) = @_;

    my $fzf_header_message =
      'Mark with <Tab>, run *pacman -Rcns pkgs* with <Enter>, exit with <C-c>';

    my $fzf_command_string = <<~"EOF";
    /usr/bin/fzf \\
    --header="$fzf_header_message" \\
    --preview="exec $0 -P {2}" \\
    --reverse \\
    --nth=2 \\
    --exact \\
    --multi <<-FZF_EOF
    $output
    FZF_EOF
    EOF

    my $marked4removal = '';
    open my $PH, '-|', $fzf_command_string || return 0;
    while (<$PH>) {
        /^[E\s]\|\s(\S+)/;
        $marked4removal .= " $1";
    }
    close $PH;

    my $uninstallation_coommand =
      "/usr/bin/su -c \"LC_ALL=C /usr/bin/pacman -Rcns$marked4removal\"";

    $marked4removal
      ? my $exit_code = system $uninstallation_coommand
      : return 0;

    # ``su'' has an exit code of 2 if <C-c> is pressed while being prompted to
    # insert the password. Such a SIGINT somehow messes up the terminal, we
    # got to remidy that.
    $exit_code == 2 && system '/usr/bin/tput reset';

    return 1;
}

# Entry point {{{1

my ( $order_by, $display_output, $sort_by, $show_explicit_only ) = read_flags();
my $output = show( $sort_by, $order_by, $show_explicit_only );

$display_output eq 'raw' && show_raw($output);

while ( show_fzf($output) ) {
    $output = show( $sort_by, $order_by );
}

# vi:fdm=marker
