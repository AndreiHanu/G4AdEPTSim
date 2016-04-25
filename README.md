## Welcome to AdEPTSim
## 
AdEPTSim is an application based on the Geant4 Monte Carlo toolkit for
simulating the response and readout of the Advanced Energetic Pair Telescope (AdEPT).
AdEPT is pair production telescope that uses a gaseous time projection chamber to measure the direction, energy, polarization, and
timing of gamma-rays with energy > 1.022 MeV. 

## Build Notes
## 
To build this application, you should have a working version of CMake
and Geant 4.10 or higher. You're also going to need OpenCV since it's used to generate the
XZ and YZ projection images at the end of a run.

Geant4: https://geant4.web.cern.ch/geant4/ CMake: https://cmake.org/

I recommend setting up your directory structure as follows:

- $G4WORKDIR/G4AdEPTSim : This is the source directory and is a clone of
the code from this GitHub - $G4WORKDIR/G4AdEPTSim-build : Contains
directories and files for each build. This directory also contains the
executable files and example macros.

## Steps to compile:
## 
### Step 1 - Source the Geant4 environment setup script
source /opt/local/libexec/Geant4/Geant4.10.1/geant4.sh

### Step 2 - Create the build directory and navigate to it
mkdir G4AdEPTSim-build && cd $_

### Step 3 - Setup CMake, make the build, and run the build
cmake -DGeant4_DIR=/opt/local/libexec/Geant4/Geant4.10.1
~/Geant4Work/G4AdEPTSim; make clean; make; ./AdEPTSim

To recompile, I typically just re-run the command in Step 3

## Scoring Physical Quantities
## 

At the end of each event, the location of primary ionizations (electrons) are scored by the Sensitive Detector class ("TrackerHitsCollection"). At the end of a run (multiple events), the 
TrackerHitsCollection is readout and the primary ionizations are binned in 400 um x 400 um x 400 um voxels. Following this, OpenCV is used to created lossless PNG images of the 
XZ and YZ projections. The images are available in the G4AdEPTSim-build directory at the end of the run.