#!/usr/bin/env python
import os
watched_files = [
    "src/cpp/form.h",
    "src/cpp/handlers/*.cpp",
    "src/scss/*.scss"
    ]
    
while True:
    if os.system("inotifywait -e modify %s" % " ".join(watched_files)) == 0:
        if os.system('compass compile -c etc/config.rb') == 0 :
            pass
        if os.system('scons -j3 --handlers') == 0:
            print os.system('wget http://localhost:9090/reload_libs -q -T 1 -O-')
    else:
        break
