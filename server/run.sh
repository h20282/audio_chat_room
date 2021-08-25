echo run udp server
./udp_server > udp_cout.log
echo udp server started
./tcp_server
echo finish

