set terminal png ; set output "/tmp/pg_conn.png" ; set xdata time ; set timefmt "%H:%M" ; set format x "%H" ; set grid ; set size 1,0.5 ; set origin 0,0 ; set ylabel 'db conn' ; set xlabel 'Date: 2016-06-16' ; set yr [0:100] ; set xr ["00":"24"] ; plot 50 title "pool size", "pgconn-20160616.dat" using 2:3  with line title "mtsv6 tm10";
