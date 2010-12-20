env = Environment()
if env.subst('$CXX') == 'cl':
    env.Append(CXXFLAGS=['/GX', '/MT', '/O1'])
if env.subst('$CXX') == 'g++':
    env.Append(CXXFLAGS=['-Wall'])
env.Program('cpuinfo', ['Main.cpp', 'CPUInfo.cpp'])
