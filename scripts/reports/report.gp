#!/usr/bin/gnuplot -persist

set style data lines;
set datafile separator '|';
set xlabel 'date';
set ylabel 'score';
set xdata time;
set timefmt '%Y-%m-%d %H:%M:%S'; # 2025-08-31 20:58:51|20
set format x '%m/%y';
plot '< sqlite3 sleeper.db <report.sql' using 1:2 title 'foobar';


