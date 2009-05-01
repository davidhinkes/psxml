env = Environment()
env.ParseConfig('pkg-config --libs --cflags libxml++-2.6')
env.Append(CPPPATH=['#/include/psxml'])
env.Append(CPPPATH=['#/include'])
env.Append(CPPFLAGS=['-Wall','-O3'])

Export("env")

SConscript("src/SConscript")
