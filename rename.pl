#!/usr/bin/perl
use strict;
use warnings;

my ($infile, $outfile) = @ARGV;
die "Usage: $0 <input_file> <output_file>\n" unless $infile && $outfile;

local $/ = undef;
open my $fh, '<', $infile or die "Cannot open $infile: $!";
my $text = <$fh>;
close $fh;

# Then handle plain _init
$text =~ s{
    (void\s+(\w+)_init(?!initToPool)\s*\()
    (.*?)  # argument list
    (\);)
}{
    my $func = $2;
    "void    ${func}_create               (tMempool** const mempool, ${func}** const);"
}egx;

# Utility: clean and split arguments safely
sub split_args {
    my ($arglist) = @_;
    my @args;
    my $buf = '';
    my $depth = 0;
    foreach my $c (split //, $arglist) {
        if ($c eq ',' && $depth == 0) {
            push @args, $buf;
            $buf = '';
        } else {
            $buf .= $c;
            $depth++ if $c eq '(';
            $depth-- if $c eq ')';
        }
    }
    push @args, $buf if $buf =~ /\S/;
    @args = map { s/^\s+|\s+$//gr } @args;
    return @args;
}

# --- 1. Handle _initToPool ---
$text =~ s{
    \bvoid\s+(\w+)_initToPool\s*\((.*?)\);
}{
    my $base = $1;
    my $args = $2;
    my @args = split_args($args);

    # Find and move tMempool** arg to the front, rename to LEAF* const leaf
    my ($mempool_idx) = grep { $args[$_] =~ /tMempool\s*\*\*/ } 0..$#args;
    if (defined $mempool_idx) {
        my $leaf_arg = $args[$mempool_idx];
        $leaf_arg =~ s/tMempool\s*\*\*.*$/LEAF* const leaf/;
        splice(@args, $mempool_idx, 1);
        unshift @args, $leaf_arg;
    }
    # # Work on a local copy so we don't mutate @args globally
    # my @converted = map {
    #     my $a = $_;
    #     # Convert only TYPE** const → TYPE* const
    #     $a =~ s/(\bt\w+)\s*\*\*\s*const\b/$1* const/;
    #     $a;
    # } @args;
    #
    # my $newargs = join(', ', @converted);

     my $newargs = join(', ', @args);
    "void    ${base}_init                 ($newargs);"
}egxs;


# === 4. Final pass: convert TYPE** → TYPE* in _init functions ===
$text =~ s{
    (                   # capture prefix
        \bvoid\s+\w+_init(?!ToPool)\s*\(  # matches void Something_init( but not initToPool
    )
    ([^)]*)               # capture arguments
    (\))                  # closing parenthesis
}{
    my ($prefix, $args, $suffix) = ($1, $2, $3);
    my @args = split /,/, $args;

    my @converted = map {
        my $a = $_;
        # convert TYPE** const → TYPE* const, but skip tMempool and LEAF
        $a =~ s/\b(t(?!Mempool|LEAF)\w+)\s*\*\*\s*const\b/$1* const/g;
        $a;
    } @args;

    $prefix . join(',', @converted) . $suffix;
}egxs;
open my $out, '>', $outfile or die "Cannot write $outfile: $!";
print $out $text;
close $out;

print "Wrote transformed output to $outfile\n";

