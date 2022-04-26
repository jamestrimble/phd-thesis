# vim: set et ft=gnuplot sw=4 :

set terminal tikz standalone color size 5.5cm,4.5cm font '\scriptsize' preamble '\usepackage{times,microtype,algorithm2e,algpseudocode,amssymb}'

set border 3
set grid
set logscale x
set logscale y
set xtics nomirror
set ytics nomirror
set size square
set xrange [1:1000000]
set yrange [1:1000000]

set xlabel "Krissinel and Henrick"
set format x '$10^{%T}$'
set format y '$10^{%T}$'

set output "plots/kh-vs-mcsplit.tex"
set ylabel "McSplit"
plot "runtimes.txt" u 6:2 notitle w points pointtype 7 pointsize .2, x notitle lc rgb '#444444'

set output "plots/kh-vs-mcsplit-prime.tex"
set ylabel "Modified McSplit"
plot "runtimes.txt" u 6:5 notitle w points pointtype 7 pointsize .2, x notitle lc rgb '#444444'



set size nosquare
set logscale x
unset logscale y
set format x '$10^{%T}$'
set xlabel "Run time (ms)"
set ylabel "Number of instances solved"
set xtics autofreq
set ytics autofreq
set xrange[1:1000000]
unset format y
unset yrange
set key right bottom
set output "plots/cumulative.tex"
plot "runtimes.txt" u 2:($2>=1e6?1e-10:1) smooth cumulative w l ti '\textproc{McSplit}' lc 1, \
     "runtimes.txt" u 3:($3>=1e6?1e-10:1) smooth cumulative w l ti '\textproc{McSplit}-1' lc 2