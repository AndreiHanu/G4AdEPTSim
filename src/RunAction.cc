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

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4UIcommand.hh"

#include <stdio.h>
#include <time.h>

#include "G4SystemOfUnits.hh"

// Include OpenCV to generate the projection image files
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Output filename format
#define FILENAME_FORMAT "ArgonBox_Geant4_%Y%m%d_%H%M_Proj_"
#define FILENAME_SIZE 60

using namespace std;
using namespace cv;

RunAction::RunAction():G4UserRunAction() {}

RunAction::~RunAction() {}


void RunAction::BeginOfRunAction(const G4Run* run)
{ 

	// Print Run ID
	G4cout << "\n--------------------- Start of Run "<< run->GetRunID() << " -----------------------------\n" << G4endl;

	// Set Starting Seed
	long seeds[2];
	time_t systime = time(NULL);
	seeds[0] = (long) systime;
	seeds[1] = (long) (systime*G4UniformRand());  
	CLHEP::HepRandom::setTheSeeds(seeds);
	
	// Create Filename Template
	static char outputFileTemplate[FILENAME_SIZE];
    time_t now = time(0);
    strftime(outputFileTemplate, sizeof(outputFileTemplate), FILENAME_FORMAT, localtime(&now));
    
	outputFile = outputFileTemplate;
	
	// Zero out the projection arrays
	memset(projXZ, 0, sizeof projXZ);
	memset(projYZ, 0, sizeof projYZ);

}

void RunAction::CreateEventFiles()
{
	eventFileXZ = outputFile + "XZ.csv"; 
	eventFileYZ = outputFile + "YZ.csv"; 
	
	pFileXZ = fopen(eventFileXZ,"w");
	pFileYZ = fopen(eventFileYZ,"w");
	
	//std::ofstream outFileXZ(eventFileXZ);
	//std::ofstream outFileYZ(eventFileYZ);

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
	projXZ[GetBin(evtPos.x(),-126.6,126.6,633)][GetBin(evtPos.z(),-126.6,126.6,633)] += 1;
	
	// Increment Y-Z projection
	projYZ[GetBin(evtPos.y(),-126.6,126.6,633)][GetBin(evtPos.z(),-126.6,126.6,633)] += 1;
}

void RunAction::CloseEventFiles()
{
	// Close file
	fclose(pFileXZ);
	fclose(pFileYZ);
}

void RunAction::EndOfRunAction(const G4Run* run)
{
	// Initialize OpenCV Mat object using the projection data
	Mat XZ = Mat(633, 633, CV_32SC1, projXZ );
	Mat YZ = Mat(633, 633, CV_32SC1, projYZ );
	
	// Image parameters (JPEG)
	// std::vector<int> params;
	// params.push_back(CV_IMWRITE_JPEG_QUALITY);
	// params.push_back(100);   // that's percent, so 100 == no compression, 1 == full 
	
	// Image parameters (PNG) lossless
	vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(1);
	
	//Generate file naming format for images
	static char outputFileTemplate[FILENAME_SIZE];
    time_t now = time(0);
    strftime(outputFileTemplate, sizeof(outputFileTemplate), FILENAME_FORMAT, localtime(&now));
	outputFile = outputFileTemplate;
	//G4UIcommand::ConvertToString allows for conversion to be made from G4int to string useable by non Geant applications
	eventFileXZ = outputFile + "XZ_event_" + G4UIcommand::ConvertToString(run->GetRunID()) + ".png"; 
	eventFileYZ = outputFile + "YZ_event_" + G4UIcommand::ConvertToString(run->GetRunID()) + ".png"; 
	
    // Check that there was an event recorded and write the image to file
	double psumxz = sum(XZ)[0];
	double psumyz = sum(YZ)[0];
	if (psumxz!=0. && psumyz!=0.){
		imwrite(eventFileXZ, XZ, compression_params);
    	imwrite(eventFileYZ, YZ, compression_params);
	}

	//use for trial and error when checking results on small samples, generates only one image in current directory
	// imwrite("XZ.png", XZ, compression_params);
    // imwrite("YZ.png", YZ, compression_params);
	
	// imwrite("XZ.jpg", XZ, params);
	// imwrite("YZ.jpg", YZ, params);
	
	// Print End of Run
	G4cout << "\n---------------------- End of Run "<< run->GetRunID() << " ------------------------------\n" << G4endl;
	
}

