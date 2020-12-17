reset
#
# output to png image file
set terminal png enhanced nocrop font "/usr/share/fonts/chinese/TrueType/ukai.ttf,12"
set output '/tmp/bug-CURRENT_DATE.png'
#

#
# output to screen
#
#set terminal x11 font "/usr/share/fonts/zh_TW/TrueType/bsmi00lp.ttf,10"

#
# We will plot 2 figure in one frame
#
set multiplot

#
# plot bug fix statistics
#
set size 1,0.5 ;
set grid
set origin 0,0
set boxwidth 0.9 absolute
set style fill solid 1.00
set key inside right top vertical Right noreverse noenhanced autotitles nobox
set style histogram clustered gap 1 title  offset character 0, 0, 0
set datafile missing '-'
set style data histograms
set xtics border in scale 0,0 nomirror offset character 0,0,0
set ytics 1
set title "每日處理情況統計 (fixed,Not Bug,Can't Reproduce,Closed) CURRENT_DATE" 
set yrange [ 0 : 10 ] noreverse nowriteback
plot '/tmp/bugfix.dat' using 2:xtic(1) ti col, '' u 3 ti col, '' u 4 ti col, '' u 5 ti col, '' u 6 ti col, '' u 7 ti col, '' u 8 ti col, '' u 9 ti col, '' u 10 ti col

#
# draw bug assignment statistics
#
set size 1,0.5 ;
set origin 0,0.5
set title "每日Bug分配統計 (Assigned,Postponed,Study) CURRENT_DATE" 
set yrange [ 0 : 10 ] noreverse nowriteback
plot '/tmp/bugassignment.dat' using 2:xtic(1) ti col, '' u 3 ti col, '' u 4 ti col, '' u 5 ti col, '' u 6 ti col, '' u 7 ti col, '' u 8 ti col, '' u 9 ti col, '' u 10 ti col
