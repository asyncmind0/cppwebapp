#!/usr/bin/env python
import os
import pyinotify

def _fullpath(paths):
    return map(lambda x:os.path.abspath(x), paths)

handlers_watch = [
    "src/cpp/handlers/*.cpp",
    ]

scss_watch = [
    "src/scss/*.scss",
    ]

def compile_scss(event):
    os.system('compass compile -c etc/config.rb')

def compile_handlers(event):
    if os.system('scons -j3 --handlers') == 0:
        print os.system('wget http://localhost/reload_libs -q --timeout 1  --tries 1 -O-')
    return 0
    

wm = pyinotify.WatchManager()
notifier = pyinotify.Notifier(wm)

wm.add_watch(_fullpath(scss_watch), pyinotify.IN_MODIFY| pyinotify.IN_CREATE | pyinotify.IN_MOVED_TO, compile_scss, do_glob=True)
wm.add_watch(_fullpath(handlers_watch), pyinotify.IN_MODIFY| pyinotify.IN_CREATE | pyinotify.IN_MOVED_TO, compile_handlers, do_glob=True)

notifier.coalesce_events(True)
notifier.loop()
