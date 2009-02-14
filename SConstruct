env = Environment()
env.ParseConfig('pkg-config --libs --cflags libxml++-2.6')
env.Append(CPPPATH=['#/include'])
env.Append(CPPFLAGS=['-Wall'])

Export("env")

SConscript("src/SConscript")
