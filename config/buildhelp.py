from dependency   import Dependency, VALID_FIELDS
from ConfigParser import ConfigParser, NoOptionError
from SCons.Util   import Split
from sys import exit
from SCons import Environment

def read_dependencies(filename):
    cparse = ConfigParser()
    cparse.read(filename)
    
    dependencies = {}
    for dep_name in cparse.sections():
        try:
            libs   = cparse.get(dep_name, "libs")
            cheads = cparse.get(dep_name, "check_headers")
            if cparse.has_option(dep_name, 'core'):
                core = cparse.getboolean(dep_name, 'core')
            else:
                core = True
        except NoOptionError:
            print "Incomplete dependency spec for {0}, (needs libs & check_headers)".format(dep_name)
            raise
        dependencies[dep_name] = Dependency(dep_name, libs, cheads, core)
    return dependencies

def check_dependency(conf, dependency):
    print "\nChecking {0} dependencies..".format(dependency.name)
    exists = True
    for header in Split(dependency.check_headers):
        if not conf.CheckCXXHeader(header):
            exists = False
            print('!! Cannot locate header {0} ...'.format(header))
            if dependency.core:
                exit(0)
                
    # check libs
    for lib in Split(dependency.libs):
        if not conf.CheckLib(lib):
            exists = False
            print('!! Cannot locate library {0}'.format(lib))
            if dependency.core:
                exit(0)

    return exists


def sanity_checks(conf):
    if not conf.CheckCXX():
        print('!! C++ compiler is not available...')
        exit(0)


def parse_user_config(filename, dependencies):
    '''
    Read user_config and update the relevant fields
    '''
    cparse = ConfigParser()
    cparse.read(filename)
    for dep_name in cparse.sections():
        if not dep_name in dependencies.keys():
            print('!! user_config specifies unknown dependency "{0}"'.format(dep_name))
            exit(0)
        
        # loop over options for that dependencies
        for opt in cparse.options(dep_name):
            if opt in VALID_FIELDS:
                dependencies[dep_name].__setattr__(opt, cparse.get(dep_name, opt))
            else:
                print "Unknown build option: {0} for dependency {1}".format(opt, dep_name)
    
def update_and_check_env(conf, dependencies):
    '''
    Update the build environment with the dependencies
    '''
    for dep in dependencies.values():
        if dep.header_path:
            conf.env.Append(CPPPATH = [dep.header_path])
        if dep.flags:
            conf.env.Append(CFLAGS = [dep.flags])
        if dep.lib_path:
            conf.env.Append(LIBPATH = [dep.lib_path])
            conf.env.Append(RPATH   = [dep.lib_path])
        if check_dependency(conf, dep) and dep.libs:
            conf.env.Append(LIBS = Split(dep.libs))
    
