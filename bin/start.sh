killall -9 oracle_http_proxy
sleep 1
nohup ./oracle_http_proxy 0.0.0.0:15210 conf.ini &
