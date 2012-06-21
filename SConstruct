import platform

env = Environment(
    CPPPATH = ['lib/mongrel2-cpp/lib',
               'lib/',
               'lib/httplib/include',
               'lib/httplib/',
               '/usr/include/stlsoft/include',
               '/usr/include/pantheios/include',
               '/usr/include/soci/',
               ],
    CCFLAGS=["-std=gnu++0x",
             "-g", "-O0",
             "-D_LARGEFILE_SOURCE",
             "-D_FILE_OFFSET_BITS=64"])

env.SharedLibrary('build/lib/libhttplib.so',
                  ['lib/httplib/src/httplib.cpp'])

#env.SharedLibrary('build/lib/libMPFDParser.so',
#                  [
#        'lib/MPFDParser-0.1.1/Parser.cpp',
#        'lib/MPFDParser-0.1.1/Field.cpp',
#        'lib/MPFDParser-0.1.1/Exception.cpp',
#                   ])

pantheios_libs = [
                  'pantheios.1.core.gcc46',
                  'pantheios.1.be.fprintf.gcc46',
                  'pantheios.1.bec.fprintf.gcc46',
                  'pantheios.1.fe.all.gcc46',
                  'pantheios.1.util.gcc46',
                  ]

main_libs = ['m2pp','zmq','json',
                  'pthread','m','rt','pq',
                  'httplib','boost_system', 'boost_regex',
                  'curl',
                  'soci_core',
                  'soci_postgresql',
                  #'MPFDParser', 
                  'ctemplate'
                  ]

arch = platform.architecture()[0]
main_libs.extend([plib+".file64bit" if arch == "64bit" else plib for plib in pantheios_libs])

env.Program('build/main',
            ['src/cpp/main.cpp',],
            LIBS=main_libs,
            LIBPATH=['.'])

Repository(['build/lib/',
            'lib/mongrel2-cpp/',
            'lib/httplib',
            '/usr/lib/pantheios',
            ])

# vim: ft=python tabstop=8 expandtab shiftwidth=4 softtabstop=4
