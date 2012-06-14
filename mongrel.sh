mkdir -p var/mongrel/run
mkdir -p var/mongrel/log
mkdir -p var/mongrel/tmp
mkdir -p var/mongrel/proc 
mount --bind /proc var/mongrel/proc
m2sh load -config etc/site.conf  --db var/site.sqlie
m2sh start  -host localhost -sudo --db var/site.sqlie
