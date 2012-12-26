sudo httpd -k start
sleep 5
ps -ef|grep httpd
sleep 10
sudo httpd -k stop
sleep 5
ps -ef|grep httpd

