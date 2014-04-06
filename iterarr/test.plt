set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'cpp.png'
set yrange [0:800]
plot 'cpp.out' using 1:3 with lines,\
'cpp.out' using 1:4 with lines,'cpp.out' using 1:5 with lines,'cpp.out' using 1:6 with lines
#pause -1