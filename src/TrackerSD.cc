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
// TrackerSD.cc
//
// Description: Implementation of the TrackerSD class
// ********************************************************************


#include "TrackerSD.hh"
#include "TrackerHit.hh"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName) 
: G4VSensitiveDetector(name), fHitsCollection(NULL), particleDef(0)
{
	collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackerSD::~TrackerSD() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackerSD::Initialize(G4HCofThisEvent* hce)
{
  	// Create hits collection
  	fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 

  	// Add this collection in hce
  	if(hcID<0){ hcID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);}
  	hce->AddHitsCollection( hcID, fHitsCollection ); 
  	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
                                     
G4bool TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)                                     
{  
	
	// Check for newly produced secondary particle ( e.g. first step )
	if ( aStep->GetTrack()->GetCurrentStepNumber() != 1) return FALSE;
	
	// Check if this is the primary particle ( e.g. ParentID > 0 )
	if ( aStep->GetTrack()->GetParentID() == 0 ) return FALSE;
	
	// Check if the particle definition is given
	if ( particleDef && particleDef != aStep->GetTrack()->GetDefinition() ) return FALSE;
          
	// Add the information to the hits collection
    TrackerHit* hit = new TrackerHit();
    hit->SetPos(aStep->GetPreStepPoint()->GetPosition());
    
    // Update the hit map for this event
    fHitsCollection->insert(hit);
	
	return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackerSD::EndOfEvent(G4HCofThisEvent*)
{
	if ( verboseLevel>1 ) { 
     	G4int nofHits = fHitsCollection->entries();
     	
     	G4cout << G4endl << "Secondary ionizations: " << nofHits << G4endl;
  	}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackerSD::SetParticle(const G4String& particleName)
{
	G4ParticleDefinition* pd = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
	if(!pd) {
		G4String msg = "Particle <";
		msg += particleName;
		msg += "> not found.";
		G4Exception("TrackerSD::SetParticle", SensitiveDetectorName ,FatalException,msg);
	}
	
	// Notify of filter set
	G4cout 
		<< G4endl 
        << "NOTICE: A particle filter for " << pd->GetParticleName() << " has been set for TrackerSD." << G4endl << G4endl;
	
	// Set particle to filter
	particleDef = pd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
