You can find a complete PDF manual in manual/manual.pdf. An abbreviated startup guide is contained below.

The procedure for installation is simple. The distribution is constructed such that it is unnecessary to seek and learn how to build and integrate any libraries. The only possible prerequisite for successful operation on supported systems is version 1.5 or higher of the Java Runtime Environment (JRE). We recommend Oracle/Sun Java, but the OpenJDK may work, and the default Java binary path is set to assume a standard Linux OpenJDK distribution. Setting up a working JRE or JDK is only required if your goals include using the bundled non-standard WEKA distribution.

Navigate to the directory into which you expanded the gzipped tarball. Here, you will find two files containing the license terms and a file called 'make_all.sh'. To build LPmade and prepare it for use, you must execute 'make_all.sh'.

This will perform the following actions within the working directory.
	-Compile all components of the network and link prediction library.
	-Compile all evaluation code.
	-Compile the bundled version of gnuplot and install into the gnuplot subdirectory.

After this process completes without errors, you can make use of any of the binaries in the evaluation or network libraries individually or you can use the work flow management scripts.

The following series of steps describes the process to quickly use the automated build system for link prediction and related analysis on sample data sets:



1. Follow the instructions above to prepare the system.
2. Navigate to one of the sample networks under 'wd'.
3. Run 'make -j X <target>' where X is the number of simultaneous jobs to run and target is the build target. For the list of targets, see the manual, but you can try 'sm', 'growth', or 'plots'.

The following series of steps describes the process to quickly use the automated build system for link prediction and related analysis on a new network data set:

1. Follow the instructions above to prepare the system.
2. Create a subdirectory under 'wd' with the name of your network . For purposes of example, let us call it 'mynet', so 'wd/mynet'.
3. Create a 'src' subdirectory, 'wd/mynet/src'. Move all necessary source files into this directory.
4. Copy a sample task-specific Makefiles from the condmat or disease-g network. Modify this file so that it indicates how to interpret the raw source data, possibly referencing external scripts, which can go into a subdirectory 'wd/mynet/script'. See the manual for more details if necessary.
5. Change directory to 'wd/mynet'.
6. Type 'make -j X <target>' where target is the build target. For the list of targets, see the manual, but you can try 'sm', 'growth', or 'plots'.
