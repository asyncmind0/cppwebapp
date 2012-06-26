#!/usr/bin/env python
import os
import pyinotify

def _fullpath(paths):
    return map(lambda x:os.path.abspath(x), handlers_watch)

handlers_watch = [
    "src/cpp/form.h",
    "src/cpp/handlers/*.cpp",
    ]

scss_watch = [
    "src/scss/*.scss",
    ]

def compile_scss(event):
    os.system('compass compile -c etc/config.rb')

def compile_handlers(event):
    if os.system('scons -j3 --handlers') == 0:
        print os.system('wget http://localhost:9090/reload_libs -q -T 1 -O-')
    

wm = pyinotify.WatchManager()
notifier = pyinotify.Notifier(wm)

wm.add_watch(_fullpath(scss_watch), pyinotify.IN_MODIFY| pyinotify.IN_CREATE | pyinotify.IN_MOVED_TO, compile_scss, do_glob=True)
wm.add_watch(_fullpath(handlers_watch), pyinotify.IN_MODIFY| pyinotify.IN_CREATE | pyinotify.IN_MOVED_TO, compile_handlers, do_glob=True)

notifier.loop()