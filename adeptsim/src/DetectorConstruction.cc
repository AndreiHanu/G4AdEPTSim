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
// DetectorConstruction.cc
//
// Description: Implementation of the detector model for the AdEPT 
// simulation
// ********************************************************************

#include <math.h>

#include "DetectorConstruction.hh"
#include "TrackerSD.hh"

#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4GeometryManager.hh"
#include "G4ProductionCuts.hh"

// Scoring Components
#include "G4SDManager.hh"
#include "G4SDParticleFilter.hh"

// need to include for forcing interactions
#include "GB02BOptrMultiParticleForceCollision.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DetectorConstruction::DetectorConstruction(): G4VUserDetectorConstruction(), fRegGasDet(0), fCheckOverlaps(true)
{
	// Default Parameter Values
	DRIFT_GAS_length = 1000.00*mm;			// Drift gas volume length
	DRIFT_GAS_width = 1000.00*mm;			// Drift gas volume width
	DRIFT_GAS_height = 1000.00*mm;			// Drift gas volume height
	
	// Production cuts for secondary particle generation
	G4double cut = 50.*um;
  	fTrackerCuts = new G4ProductionCuts();
  	fTrackerCuts->SetProductionCut(cut,"gamma");
  	fTrackerCuts->SetProductionCut(cut,"e-");
  	fTrackerCuts->SetProductionCut(cut,"e+");
  	fTrackerCuts->SetProductionCut(cut,"proton");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
	delete fTrackerCuts; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	////////////////////////////////////////////////////////////////////////
	// Material Definitions
	G4NistManager* nistManager = G4NistManager::Instance();
	
	// NIST materials at STP conditions
  	//G4Material* Argon = nistManager->FindOrBuildMaterial("G4_Ar");
  	G4Material* galactic = nistManager->FindOrBuildMaterial("G4_Galactic");
  	
  	// NIST gases at non STP conditions
  	G4Material* Ar_293K_1p5atm = nistManager->ConstructNewGasMaterial("Ar_293K_1p5atm","G4_Ar",293.15*kelvin,1.5*atmosphere);
	
	// Set the materials for the Geometry
  	fWorldMaterial = galactic;
  	fGasMaterial = Ar_293K_1p5atm;
  	
  	// Print all the materials defined.
  	G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
  	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
	
	////////////////////////////////////////////////////////////////////////
	// Cleanup old geometry
  	G4GeometryManager::GetInstance()->OpenGeometry();

  	if(fRegGasDet) { delete fRegGasDet; }
  	fRegGasDet = new G4Region("GasRegion");
  	fRegGasDet->SetProductionCuts(fTrackerCuts);
	
	// Cleanup old geometry
  	G4GeometryManager::GetInstance()->OpenGeometry();
  	G4PhysicalVolumeStore::GetInstance()->Clean();
  	G4LogicalVolumeStore::GetInstance()->Clean();
  	G4SolidStore::GetInstance()->Clean();	
	
	////////////////////////////////////////////////////////////////////////
	// Construct The World Volume (Vacuum)

	G4double world_X = 2*DRIFT_GAS_length;
	G4double world_Y = 2*DRIFT_GAS_width;
	G4double world_Z = 2*DRIFT_GAS_height;
	
	G4Box* WorldSolid = new G4Box("World", world_X/2, world_Y/2, world_Z/2);
  
	WorldLogical = 
		new G4LogicalVolume(WorldSolid,						// The Solid
							fWorldMaterial,					// Material
							"World");						// Name
  
	WorldPhysical = 
		new G4PVPlacement(	0,								// Rotation
							G4ThreeVector(),				// Translation vector
							WorldLogical,					// Logical volume
							"World",						// Name
							0,								// Mother volume
							false,							// Unused boolean parameter
							0,								// Copy number
							fCheckOverlaps);				// Overlap Check
	
	////////////////////////////////////////////////////////////////////////
	// Tracking Capable Volume (Cube)
	
	G4Box* TrackerSolid = new G4Box("TrackingVolumeSolid", DRIFT_GAS_length/2, DRIFT_GAS_width/2, DRIFT_GAS_height/2);
	
	TrackerLogical = 
		new G4LogicalVolume(TrackerSolid,					// The Solid
							fGasMaterial,					// Material
							"TrackingVolumeLogical");		// Name
	
	TrackerPhysical = 
		new G4PVPlacement(	0, 								// Rotation
							G4ThreeVector(), 				// Translation vector
							TrackerLogical, 				// Logical volume
							"TrackingVolumePhysical", 		// Name
							WorldLogical, 					// Mother volume
							false, 							// Unused boolean parameter
							0, 								// Copy number
							fCheckOverlaps);				// Overlap Check
	
	////////////////////////////////////////////////////////////////////////
	// Register the Tracker Volume
	
	fRegGasDet->AddRootLogicalVolume(TrackerLogical);
	  
	////////////////////////////////////////////////////////////////////////						
	// Visualization attributes
	
	// World Volume
  	G4VisAttributes* WorldAttributes = new G4VisAttributes(G4Colour(1.,1.,1.,0.3));
  	WorldAttributes->SetForceWireframe(true);
  	WorldLogical->SetVisAttributes(WorldAttributes);
	//WorldLogical->SetVisAttributes(G4VisAttributes::Invisible);
  	
  	// Tracking Volume Vessel
  	G4VisAttributes* TrackerAttributes = new G4VisAttributes(G4Colour(1.,1.,0.,0.6));
  	TrackerAttributes->SetForceWireframe(true);
  	TrackerLogical->SetVisAttributes(TrackerAttributes);
  	
	////////////////////////////////////////////////////////////////////////
	// Return world volume
	return WorldPhysical; 
}

void DetectorConstruction::ConstructSDandField() {
	
	////////////////////////////////////////////////////////////////////////
	// Sensitive Detector for the Tracker Volume
	TrackerSD* myTrackerSD = new TrackerSD("AdEPTSim/TrackerSD","TrackerHitsCollection");
	
	// Create a particle filter
	myTrackerSD->SetParticle("e-");
    
    // Setting myTrackerSD to all logical volumes with the same name 
  	// of "TrackingVolumeLogical".
  	SetSensitiveDetector("TrackingVolumeLogical", myTrackerSD, true);
	
	//Grabbing the logical volume where we will apply biasing  
	G4LogicalVolume* logicTest = G4LogicalVolumeStore::GetInstance()->GetVolume("TrackingVolumeLogical");
	
	GB02BOptrMultiParticleForceCollision* testMany = new GB02BOptrMultiParticleForceCollision();
	
	testMany->AddParticle("gamma");
	testMany->AddParticle("neutron");
	
	testMany->AttachTo(logicTest);
	
	G4cout << " Attaching biasing operator " << testMany->GetName()
    	   << " to logical volume " << logicTest->GetName()
    	   << G4endl;
}


