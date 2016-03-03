<h1> OXSX </h1>
Signal Extraction framework for the SNO+ experiment


<h2> Dependencies </h2>
1. [Armadillo](http://arma.sourceforge.net/) is a linear algebra package used for quick matrix multiplication
2. [GSL](https://gcc.gnu.org/libstdc++/) - likely you already have this installed, especially if you are running RAT
3. [SCons](http://www.scons.org/) Is used for the build, also a dependency for RAT
4. [HDF5](https://www.hdfgroup.org/HDF5/release/obtain5.html)
5. [ROOT](https://root.cern.ch/downloading-root) Should be installed with Minuit2 enabled `./configure --enable-minuit2`


<h2>Installation Instructions </h2>
Follow the installation instructions for each of the above using either the default install location or a different directory if you would prefer.

1. Clone this repository with ```git clone https://github.com/jackdunger/oxsx.git --recursive``` if you've already cloned without the recursive flag just run ```git submodule update --init```

2. If your dependencies are somewhere the compiler can't find them, copy `config/userconfig.ini.template` to `config/userconfig.ini` and add the relevant paths. Missing entries are assumed to be in standard locations. e.g.
    ```
    [root]
    header_path : <path/to/headers>
    lib_path    : <path/to/libraries>
     ```
  
3. Run ```scons && scons units```

4. Test the build was sucessful with ```./tests/RunUnits```


<h3> Compiling Your Own Scripts</h3>

scons auto-generates a script that compiles and links your c++ against the source code and dependencies just run ```. <oxsx root>/bin/compile.sh <your cpp file>``` to produce an executible of the same name

Look in `<oxsx root>/examples` for help getting started



<h2> Creating ntuples </h2>
One way to read in data is using a ROOT ntuple. If you are looking at SNO+ data, you probably have a flat root tree that can be easily pruned down into this format with only the branches you are interested in.

To create ntuples for oxsx run ```./util/prune_flat_tree <path_to_file_to_convert> -treename <name_of_the_tree> <branch1> <branch2> <branch3> -newtreename <name_of_your_tree> -outputfile <name_of_your_file> -nevents <number_of_events>```

 * The name of the tree in an input file is optional, as a default it is "output"
 * The name of the output file is optional, as a default is is <the_name_of_input_file>+"_oxsx.root"
 * The name of the tree in an output file is optional, as a default it is "oxsx"
 * The number of events of an output file is optional 

