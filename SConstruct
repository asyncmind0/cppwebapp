import platform
AddOption('--handlers',
          dest='handlers',
          action='store_true',
          help='rebuild only handlers')
AddOption('--tests',
          dest='tests',
          action='store_true',
          help='rebuild only tests')

env = Environment(
    CPPPATH = ['lib/mongrel2-cpp/lib',
               'lib/',
               'lib/httplib/include',
               'lib/httplib/',
               'lib/csv_iterator/include',
               'lib/plustache/include',
               '/usr/include/stlsoft/include',
               '/usr/include/pantheios/include',
               '/usr/include/soci/',
               'lib/plustache/'
               ],
    CCFLAGS=["-std=gnu++0x",
             "-g", "-O0","-fPIC",
             "-D_LARGEFILE_SOURCE",
             "-D_FILE_OFFSET_BITS=64"],
             LINKFLAGS=["-fPIC"])

env.SharedLibrary('build/lib/libhttplib.so',
                  ['lib/httplib/src/httplib.cpp'])

env.SharedLibrary('build/lib/libplustache.so',
                  [
        'lib/plustache/src/template.cpp',
        'lib/plustache/src/context.cpp'
        ])
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
             'httplib',
             'plustache',
             'boost_system',
             'boost_regex',
             'curl',
             'soci_core',
             'soci_postgresql',
             #'MPFDParser', 
             'dl',
             'uuid'
             ]
arch = platform.architecture()[0]
main_libs.extend([plib+".file64bit" if arch == "64bit" else plib for plib in pantheios_libs])
handlers = ["handlers","nutrition","dataloader","home", "auth"]
if GetOption('handlers'):
    for handler in handlers:
        env.SharedLibrary('build/lib/lib%s.so'%handler,
                  ['src/cpp/handlers/%s.cpp'%handler],
                  CC='-fPIC',
                  LIBS=main_libs,
                  LIBPATH=['.'])

if GetOption('tests'):
    env.Program('build/regex_tester',
            ['src/cpp/regex_tester.cpp',],
            LIBS=main_libs,
            LIBPATH=['.'])
else:
    env.Program('build/main',
            ['src/cpp/main.cpp',],
            LIBS=main_libs,
            LIBPATH=['.'])

Repository(['build/lib/',
            'lib/mongrel2-cpp/',
            'lib/httplib',
            'lib/plustache',
            '/usr/lib/pantheios',
            ])

# vim: ft=python tabstop=8 expandtab shiftwidth=4 softtabstop=4
