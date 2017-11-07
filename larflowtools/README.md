# LArFlow tools

Utilities for the pixel flow-based 3D track reco. project

Tools
* program to check the ground truth labels (check_flow)

### building

* clone repo
* go into folder and run `git submodule init`
* run `source configure.sh`
* go into larlite folder and run `make`
* go into larcv folder and run `make -j4`
* finally, go into larflowtools folder and run `make`

