import os

env = Environment()
env.ParseConfig('pkg-config --libs --cflags libxml++-2.6')
env.Append(CPPPATH=['#/include/psxml'])
env.Append(CPPPATH=['#/include'])
env.Append(CPPFLAGS=['-Wall','-O2'])


# get env variables
prefix=ARGUMENTS.get('PREFIX','/usr/local')
#destdir=ARGUMENTS.get('DESTDIR`','/usr/local')
env.Append(CPPFLAGS=[os.getenv('CPPFLAGS')])
env.Append(LINKFLAGS=[os.getenv('LDFLAGS')])
env["ENV"]["PKG_CONFIG_PATH"]=os.getenv('PKG_CONFIG_PATH')

Export("env")

SConscript("src/SConscript")
