#!python
import os
import sys
from build_tools import get_gsl_flags, get_arma_flags, get_root_flags, write_compile_script, get_hdf_flags

BOOST = '/Users/matthewmottram/snoPlus/software/boost_1_59_0'
BOOSTLIBPATH = BOOST + "/libs"
#BOOST = '/usr/local/Cellar/boost/1.58.0'
#BOOSTLIBPATH = BOOST + "/lib"

# Get the locations of root, armadillo and gsl from root-config and env.sh
root_incs, root_libs  = get_root_flags()
hdf5_incs, hdf5_libs  = get_hdf_flags()
root_incs, root_libs = Split(root_incs), Split(root_libs)
armadillo_include, armadillo_lib = get_arma_flags()
gsl_include, gsl_lib = get_gsl_flags()

hdf_lib_names = ["hdf5_hl_cpp", "hdf5_cpp", "hdf5_hl", "hdf5"]
###############################
# Building the static library #
###############################

# Put built object files in build/ 
VariantDir("build", "src", duplicate=0)

# Compile all .cpp files in source tree
source_dirs  = [x[0] for x in os.walk("src")]
source_files = []
for x in source_dirs:
    source_files += Glob(os.path.join(x,"*.cpp").replace("src", "build"))

#Copy headers into include dir
hdrs = []
for x in source_dirs:
    hdrs += Glob(os.path.join(x, "*.h")) + Glob(os.path.join(x, "*.hpp")) + Glob(os.path.join(x, "*.hh"))

env = Environment()
headers = [Command(os.path.join("include", os.path.basename(y.rstr())), 
                   y, Copy("$TARGET", "$SOURCE")) for y in hdrs]

env.Default(headers)
# Create the build environment
print hdf5_incs
env = Environment(CCFLAGS = '-O2', 
                  CPPPATH = ["include", hdf5_incs, armadillo_include, gsl_include, BOOST, '/usr/include/python2.7'] + root_incs,
                  LIBPATH = ['./', BOOSTLIBPATH, "/usr/lib"],
                  RPATH = ['./', BOOSTLIBPATH]
                  )

# Build the library
objects = [env.Object(x) for x in source_files]

lib = env.Library("build/liboxsx", objects)

#shlib = env.SharedLibrary('oxsx', source_files)
shlib = env.SharedLibrary('oxsx', ['src/bindings/BoostBindings.cpp', 'src/pdf/Pdf.cpp', 'src/pdf/IntegrablePdf.cpp', 'src/pdf/analytic/Gaussian.cpp', 'src/data/DataRepresentation.cpp'], 
                          LIBPATH=['build', gsl_lib],
                          LIBS=['python', 'oxsx', 'boost_python', 'gsl', 'gslcblas'], 
                          SHLIBPREFIX='', SHLIBSUFFIX='.so')

#, 'src/pdf/Pdf.cpp', 'src/data/DataRepresentation.cpp'], SHLIBPREFIX=''
#, LIBS=[BOOST_PYTHON_LIB])

env.Default([objects, lib])#, shlib])
env.Default(shlib)



############################################################
# For compiling tests and user scripts against the library #
############################################################
                      
testenv = Environment(parse_flags = root_libs + root_incs,
                      CCFLAGS = "-O2",
                      CPPPATH = ["Catch/include", "include", hdf5_incs, gsl_include, armadillo_include],
                      LIBS = ["armadillo", "gsl", "oxsx", "Minuit2"] + hdf_lib_names,
                      LIBPATH = [gsl_lib, armadillo_lib, "build"]
                      )

unit_test_files = Glob("test/unit/*/*.cpp") + Glob("test/unit/*.cpp")
unit_tests      = [testenv.Object(x) for x in unit_test_files]

unit_test_executible = testenv.Program("test/RunUnits", 
                                       source = [unit_tests, lib],
                                       )
testenv.Alias("units", unit_test_executible)


######################################
# Create a compiler for user scripts #
######################################
write_compile_script(testenv, "./examples")
