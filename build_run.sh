scons && \
#LD_LIBRARY_PATH=build/lib/:lib/cjango/ gdb --args build/main 'tcp://127.0.0.1:9997' 'tcp://127.0.0.1:9996'
LD_LIBRARY_PATH=build/lib/:lib/cjango/ build/main 'tcp://127.0.0.1:9997' 'tcp://127.0.0.1:9996'
