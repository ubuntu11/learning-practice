set terminal png

set title "PV平滑化結果"
set grid
set xlabel "秒"
set ylabel "功率(KW)"
set terminal png font " Times_New_Roman,12 "
set output "pv_smooth.png"
set yr [0:2000]
set xr [0:100]
set xtic 10
set key left

plot "dataset.txt" using 0:1 with linespoints title "PV輸出", "" using 0:2 with linespoints title "PCS輸出", "" using 0:4 with linespoints title "總輸出"
