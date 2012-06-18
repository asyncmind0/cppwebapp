mkdir -p var/run
mkdir -p var/log
mkdir -p var/tmp
mkdir -p var/proc 
mkdir -p var/static/javascript 
umount var/proc
umount var/static/javascript
mount --bind /proc var/proc
mount --bind src/javascript var/static/javascript/
m2sh load -config etc/site.conf  --db var/site.sqlite
m2sh start  -host localhost -sudo --db var/site.sqlite
