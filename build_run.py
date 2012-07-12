#!/usr/bin/python
import os
import argparse
parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('--tests',   default=False, action="store_true",
                       help='show launcher')
parser.add_argument('--debug',   default=False, action="store_true",
                       help='debug with gdb')
args = parser.parse_args()

root = os.getcwd()
os.chdir("lib/mongrel2-cpp/")
os.system("make")
os.chdir(root)
os.system("createdb cppblog")
if args.tests:
    if os.system("scons -j3 --tests"):exit(1)
    os.system("scons -j3&& LD_LIBRARY_PATH=build/lib/:lib/cjango/ build/regex_tester")
else:
    os.system("compass compile -c etc/config.rb")
    if os.system("scons -j3 --handlers"):exit(1)
    if args.debug:
        os.system("scons -j3&& LD_LIBRARY_PATH=build/lib/:lib/cjango/ gdb -ex run --args build/main 'tcp://127.0.0.1:9997' 'tcp://127.0.0.1:9996'")
    else:
        os.system("scons -j3&& LD_LIBRARY_PATH=build/lib/:lib/cjango/ build/main 'tcp://127.0.0.1:9997' 'tcp://127.0.0.1:9996'")
#LD_LIBRARY_PATH=build/lib/:lib/cjango/ build/main 'tcp://127.0.0.1:9997' 'tcp://127.0.0.1:9996'
