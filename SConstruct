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

env.SharedLibrary('build/lib/libMPFDParser.so',
                  [
        'lib/MPFDParser-0.1.1/Parser.cpp',
        'lib/MPFDParser-0.1.1/Field.cpp',
        'lib/MPFDParser-0.1.1/Exception.cpp',
                   ])

env.Program('build/main',
            ['src/cpp/main.cpp',],
            LIBS=['m2pp','zmq','json',
                  'pthread','m','rt','pq',
                  'httplib','boost_system', 'boost_regex',
                  'MPFDParser', 'curl',
                  'pantheios.1.core.gcc46.file64bit',
                  'pantheios.1.be.fprintf.gcc46.file64bit',
                  'pantheios.1.bec.fprintf.gcc46.file64bit',
                  'pantheios.1.fe.all.gcc46.file64bit',
                  'pantheios.1.util.gcc46.file64bit',
                  'soci_core',
                  'soci_postgresql',
                  'ctemplate'
                  ],
            LIBPATH=['.'])

Repository(['build/lib/',
            'lib/mongrel2-cpp/',
            'lib/httplib',
            '/usr/lib/pantheios',
            ])
