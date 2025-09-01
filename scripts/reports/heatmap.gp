#!/usr/bin/gnuplot -persist

set datafile separator '|'
set title 'Wake up hours'
set xlabel 'month'
set ylabel 'hour'
set zlabel 'times'
set yrange [0:23]
set zrange [0:4]

set xdata time
set timefmt '%Y-%m' # 2025-08-31 20:58:51|20
set format x '%m/%y'

set dgrid3d 20, 20
set pm3d map

splot '< sqlite3 sleeper.db <heatmap.sql' using 1:2:3 title 'foobar'
