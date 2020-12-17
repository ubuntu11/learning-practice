at now +1 minute <<EOF
mvn exec:java -Dexec.args="http://shopping.pchome.com.tw pchome 100 20000"
EOF

tshark -f "port not 53 and not arp" -f "src net 192.168.i62.47/32" -i 6 -p -a duration:300 -w pchome.pcap -F pcap
