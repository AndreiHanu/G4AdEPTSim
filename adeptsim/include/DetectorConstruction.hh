//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// ********************************************************************
// DetectorConstruction.hh
//
// Description: Definition of the detector construction for the AdEPT 
// simulation
// ********************************************************************

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4Region;
class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class G4ProductionCuts;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  	// constructor and destructor
    DetectorConstruction();
    virtual ~DetectorConstruction();
    
  public:
  	// virtual methods from G4VUserDetectorConstruction.
  	virtual G4VPhysicalVolume* Construct();
  	virtual void ConstructSDandField();
               
  private:
    // Standard Materials
    G4Material* fWorldMaterial;
    G4Material* fGasMaterial;
    
    // Logical Volumes
    G4LogicalVolume* WorldLogical;
    G4LogicalVolume* TrackerLogical;
    
    // Physical Volumes
    G4VPhysicalVolume* WorldPhysical;
    G4VPhysicalVolume* TrackerPhysical;
    
    // Geometry Parameters
    G4double DRIFT_GAS_length;
    G4double DRIFT_GAS_width;
    G4double DRIFT_GAS_height;
    
    G4ProductionCuts*  fTrackerCuts;
    G4Region* fRegGasDet;

    G4bool  fCheckOverlaps;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

