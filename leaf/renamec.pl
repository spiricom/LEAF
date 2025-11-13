#!/usr/bin/perl
use strict;
use warnings;

my ($infile, $outfile) = @ARGV;
die "Usage: $0 <input_file> <output_file>\n" unless $infile && $outfile;

local $/ = undef;
open my $fh, '<', $infile or die "Cannot open $infile: $!";
my $text = <$fh>;
close $fh;

# --- Utility: safely split C argument lists ---
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

# === 1. Handle _init → _create FIRST ===
$text =~ s{
    \bvoid\s+(\w+)_init(?!ToPool)\s*\((.*?)\)\s*\{(.*?)\}
}{
    my $base = $1;  # Struct name
    my $args = $2;
    my $body = $3;
    my @args = split_args($args);

    # Extract argument names
    my ($ptr_arg)     = map { /(\w+)\s*$/ ? $1 : () } grep { /t\w+\s*\*\*/ } @args;
    my ($mempool_arg) = map { /(\w+)\s*$/ ? $1 : () } grep { /tMempool\s*\*\*/ } @args;
    $mempool_arg ||= 'mp';
    $ptr_arg     ||= 'ef';

    # Replace body with only ALLOC_FROM_POOL
    my $call = "    ALLOC_FROM_POOL($base, $ptr_arg, $mempool_arg);";

    "void ${base}_create(tMempool** const $mempool_arg, ${base}** const $ptr_arg)\n{\n$call\n}"
}egxs;

# === 2. Handle _initToPool → _init SECOND ===
$text =~ s{
    \bvoid\s+(\w+)_initToPool\s*\((.*?)\)\s*\{(.*?)\}
}{
    my $base = $1;
    my $args = $2;
    my $body = $3;

    my @args = split_args($args);

    # Find tMempool** argument, rename it to LEAF* const leaf, move to front
    my ($mempool_idx) = grep { $args[$_] =~ /tMempool\s*\*\*/ } 0..$#args;
    my $leaf_arg = 'LEAF* const leaf';
    splice(@args, $mempool_idx, 1) if defined $mempool_idx;
    unshift @args, $leaf_arg;

    # Convert any TYPE** const → TYPE* const (single pointer)
    foreach (@args) {
        s/(\bt\w+)\s*\*\*\s*const\b/$1* const/;
    }

    my $newargs = join(', ', @args);
    "void ${base}_init($newargs)\n{\n$body\n}"
}egxs;

# === 3. Clean up allocation blocks inside any _init ===
# Remove any code block matching mpool_alloc / mempool setup
$text =~ s{
    ^[ \t]*tMempool\*\s+\w+\s*=\s*\*.*?;\s*\n      # tMempool* m = *mp;
    [ \t]*\w+\s*\*\s*\w+\s*=\s*\*.*mpool_alloc.*?;\s*\n # TYPE* e = *ef = (TYPE*) mpool_alloc(...);
    [ \t]*\w+->mempool\s*=\s*\w+;\s*\n?             # e->mempool = m;
}{}mgx;

open my $out, '>', $outfile or die "Cannot write $outfile: $!";
print $out $text;
close $out;

print "✅ Wrote transformed definitions to $outfile\n";
