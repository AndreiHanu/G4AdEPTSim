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
// RunAction.hh
//
// Description: Definition of the RunAction class
// ********************************************************************

#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Run;
class G4GenericMessenger;
class PrimaryGeneratorAction;

/// Run action class

class RunAction : public G4UserRunAction
{
public:
	RunAction(PrimaryGeneratorAction*);
	virtual ~RunAction();

	virtual void BeginOfRunAction(const G4Run*);
	virtual void EndOfRunAction(const G4Run*);
	
	// Methods
	void CreateEventFiles();
	void RecordEventPos(G4ThreeVector evtPos);
	void CloseEventFiles();
	
private:
	// Method for binning/histogramming a value
	G4int GetBin(G4double val, G4double min, G4double max, G4int numBins);

private:
	// Define a generic messenger class
	G4GenericMessenger* fMessenger;

	// Particle Gun
	PrimaryGeneratorAction* particleGun;

	// Define variables for image thresholding
	int SNR;
	int NoiseCenter;
	int NoiseSigma;
	
	// Projection Histogram Array
	int projXZ[2500][2500];
	int projYZ[2500][2500];
	
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

