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
// RunAction.cc
//
// Description: Implementation of the RunAction class for the for the AdEPT 
// simulation
// ********************************************************************

#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4UIcommand.hh"

#include <stdio.h>
#include <time.h>

#include "G4SystemOfUnits.hh"

// Messenger classes
#include "G4GenericMessenger.hh"

// Include OpenCV to generate the projection image files
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

RunAction::RunAction(PrimaryGeneratorAction* primary):G4UserRunAction(), particleGun(primary), SNR(5), NoiseCenter(0), NoiseSigma(3) {
	// Set starting seed for the Random Number Generator
	long seeds[2];
	time_t systime = time(NULL);
	seeds[0] = (long) systime;
	seeds[1] = (long) (systime*G4UniformRand());  
    G4Random::setTheSeeds(seeds);

	// Set the parameters to control image noise and SNR
	fMessenger = new G4GenericMessenger(this,"/AdEPTSim/Image/", "Image generation controls");
 	fMessenger->DeclareProperty("SNR", SNR, "Set the signal-to-noise level");
	fMessenger->DeclareProperty("NoiseCenter", NoiseCenter, "Set the center value for the noise in electrons");
	fMessenger->DeclareProperty("NoiseSigma", NoiseSigma, "Set the sigma for the noise in electrons");
}

RunAction::~RunAction() {
	delete fMessenger;
}


void RunAction::BeginOfRunAction(const G4Run* run)
{ 

	// Print Run ID
	G4cout << "\n--------------------- Start of Run "<< run->GetRunID() << " -----------------------------\n" << G4endl;
	
	// Zero out the projection arrays
	memset(projXZ, 0, sizeof projXZ);
	memset(projYZ, 0, sizeof projYZ);

}

G4int RunAction::GetBin(G4double val, G4double min, G4double max, G4int numBins)
{
	// Method for binning/histogramming a value
	// val - input value
	// min,max - minimum and maximum binning limits
	// numBins - number of Bins within the range 
	
	if ( val >= min && val < max) {
		// If the input value is within range, return the appropriate bin
		return floor(numBins*(val-min)/(max-min));
	} 
	else if ( val >= max ) { 
		// If the input value is greater than or equal to the maximum value, put it in the last bin
		return numBins - 1;
	}
	else {
		// For all other cases, return the first bin
		return 0;
	}
}

void RunAction::RecordEventPos(G4ThreeVector evtPos)
{
	// Increment X-Z projection
	projXZ[GetBin(evtPos.z(),-126.6,126.6,211)][GetBin(evtPos.x(),-126.6,126.6,211)] += 1;
	
	// Increment Y-Z projection
	projYZ[GetBin(evtPos.z(),-126.6,126.6,211)][GetBin(evtPos.y(),-126.6,126.6,211)] += 1;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
	// Initialize OpenCV Mat object using the projection data
	Mat XZ = Mat(211, 211, CV_32SC1, projXZ);
	Mat YZ = Mat(211, 211, CV_32SC1, projYZ);

	// Check if there was an event recorded and write the image to file
	if (sum(XZ)[0] > 0. && sum(YZ)[0] > 0.){
		// Initialize OpenCV Mat object for adding Gaussian noise to the images
		Mat noiseXZ = Mat(XZ.size(), CV_32SC1);
		Mat noiseYZ = Mat(YZ.size(), CV_32SC1);
		randn(noiseXZ, NoiseCenter, NoiseSigma);
		randn(noiseYZ, NoiseCenter, NoiseSigma);

		// Scale the track images by the SNR
		XZ *= SNR;
		YZ *= SNR;

		// Add the noise to the track images
		XZ += noiseXZ;
		YZ += noiseYZ;
		
		// Image parameters (PNG) lossless
		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(1);
<<<<<<< HEAD

		// =============================================================================================== //
		// Filenaming convention for training dataset
		// event_#_XZ/YZ_particle.png

		G4String eventFileXZ = 	"event_" + G4UIcommand::ConvertToString(run->GetRunID()) + "_XZ_" + 
								particleGun->GetGPS()->GetParticleDefinition()->GetParticleName() + ".png"; 
=======
		
		// Generate file naming format for images
		static char outputFileTemplate[FILENAME_SIZE];
		time_t now = time(0);
		strftime(outputFileTemplate, sizeof(outputFileTemplate), FILENAME_FORMAT, localtime(&now));
		G4String outputFile = outputFileTemplate;
		//G4UIcommand::ConvertToString allows for conversion to be made from G4int to string useable by non Geant applications
		G4String eventFileXZ = 	"event_" + G4UIcommand::ConvertToString(run->GetRunID()) + "_" + 
								particleGun->GetGPS()->GetParticleDefinition()->GetParticleName() + 
								"_XZ_.png"; 
>>>>>>> dd9485abd03d875d3a355c943f82cf411886385c

		G4String eventFileYZ = 	"event_" + G4UIcommand::ConvertToString(run->GetRunID()) + "_" + 
								particleGun->GetGPS()->GetParticleDefinition()->GetParticleName() + 
								"_YZ_.png"; 

		// =============================================================================================== //
		// Filenaming convention for training dataset
		// event_#_particle.png

		// G4String eventFileXZ = 	"event_" + G4UIcommand::ConvertToString(run->GetRunID()*2) + "_" + 
		// 						particleGun->GetGPS()->GetParticleDefinition()->GetParticleName() + ".png"; 
		
		// G4String eventFileYZ = 	"event_" + G4UIcommand::ConvertToString(run->GetRunID()*2+1) + "_" + 
		// 						particleGun->GetGPS()->GetParticleDefinition()->GetParticleName() + ".png"; 

		// Write out the images
		imwrite(eventFileXZ, XZ, compression_params);
    	imwrite(eventFileYZ, YZ, compression_params);
	}
	
	// Print End of Run
	G4cout << "\n---------------------- End of Run "<< run->GetRunID() << " ------------------------------\n" << G4endl;
	
}
