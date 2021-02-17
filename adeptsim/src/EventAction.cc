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
// EventAction.cc
//
// Description: Implementation of the EventAction construction for the AdEPT 
// simulation
// ********************************************************************

#include "EventAction.hh"
#include "RunAction.hh"
#include "TrackerHit.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"

#include "G4SystemOfUnits.hh"

EventAction::EventAction(RunAction* runAction):G4UserEventAction(), 
fPrintModulo(50),
fRunAct(runAction)
{}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event* evt)
{
	G4int evtNb = evt->GetEventID();
  
	if (evtNb == 0) {
		G4SDManager* SDMan = G4SDManager::GetSDMpointer(); 
		TrackerID = SDMan->GetCollectionID("TrackerSD/TrackerHitsCollection");
		 
	}

	if (evtNb%fPrintModulo == 0) { 
		G4cout << "\n---> Begin of event: " << evtNb << G4endl;
	}
}

void EventAction::EndOfEventAction(const G4Event* evt)
{
	G4int evtNb = evt->GetEventID();

	//Hits collections
	G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
	if(!HCE) return;
	
	TrackerHitsCollection* eventTracker = static_cast<TrackerHitsCollection*>(HCE->GetHC(TrackerID));
	
	// Print the number of hits in the drift chamber
    G4int nofHits = eventTracker->entries();
    
    // Export the every data point
	if (nofHits > 0)
	{
	
		G4cout << G4endl << "Event " << evtNb << " # of secondary ionizations: " << nofHits ;
    	
		G4int hitID = 0;
		
		while (hitID < nofHits){
			// Grab the hit information
			TrackerHit* hitCurrent = (*eventTracker)[hitID];
       		G4ThreeVector evtPos = hitCurrent->GetPos();
       		
       		// Store the hit information
       		fRunAct->RecordEventPos(evtPos);
       		
       		// Increment the current hitID
       		hitID++;
       	}
	
	}
	
}
