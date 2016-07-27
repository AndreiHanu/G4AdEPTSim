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
// PhysicsList.cc
//
// Description: Implementation of the PhysicsList class using standard Physics Lists and the
// 				possibility to add the PAI model to only the gas region
//
// Author:      Andrei R. Hanu
//
// ********************************************************************

#include "PhysicsList.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsXS.hh"
#include "G4HadronElasticPhysicsHP.hh"
//#include "G4HadronElasticPhysicsLHEP.hh"
//#include "G4HadronQElasticPhysics.hh"
#include "G4ChargeExchangePhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4NeutronCrossSectionXS.hh"
//#include "G4QStoppingPhysics.hh"
#include "G4StoppingPhysics.hh"
//#include "G4LHEPStoppingPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
//#include "G4IonLHEPPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmProcessOptions.hh"
#include "G4OpticalPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"

#include "G4PAIModel.hh"
#include "G4PAIPhotModel.hh"

#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsFTF_BIC.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsQGSP_FTFP_BERT.hh"
#include "G4HadronPhysicsQGS_BIC.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4LossTableManager.hh"
#include "G4ProductionCutsTable.hh"

#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

//biasing physics list
#include "G4GenericBiasingPhysics.hh"
#include "FTFP_BERT.hh"
#include "G4GammaConversion.hh"
using namespace std;

PhysicsList::PhysicsList() : G4VModularPhysicsList(),
  fConfig(0),
  fEmPhysicsList(0),
  fDecayPhysicsList(0),
  fBiasingPhysics(0)
{
  fConfig = G4LossTableManager::Instance()->EmConfigurator();
  G4LossTableManager::Instance()->SetVerbose(0);
  
  // defaultCutValue = 205*um;
  defaultCutValue  = 50.*um;
  fCutForGamma     = defaultCutValue;
  fCutForElectron  = defaultCutValue;
  fCutForPositron  = defaultCutValue;
  fCutForProton    = defaultCutValue;

  // Decay Physics is always defined
  fDecayPhysicsList = new G4DecayPhysics();
  
  // Hadron Physics
  fHadronPhys.push_back( new G4HadronPhysicsFTFP_BERT_HP());
  fHadronPhys.push_back( new G4HadronElasticPhysicsHP() );
  fHadronPhys.push_back( new G4StoppingPhysics());
  fHadronPhys.push_back( new G4IonBinaryCascadePhysics());
  fHadronPhys.push_back( new G4IonPhysics());
  fHadronPhys.push_back( new G4NeutronTrackingCut());

  // EM physics (General)
  //fEmPhysicsList = new G4EmLivermorePhysics();
  //fEmPhysicsList = new G4EmPenelopePhysics();
  //fEmPhysicsList = new G4EmStandardPhysics_option3();
  fEmPhysicsList = new G4EmStandardPhysics();
  
  //initialize fBiasingPhysics and make it type G4GenericBiasingPhysics then set particles to bias
  fBiasingPhysics = new G4GenericBiasingPhysics();
  //Add the proccess' needed for biasing; keywords for processes can be found in chapter 5.2: Physics Processes of geant manual
  //Search class reference of process wanted and keyword is found there
  processesToBias.push_back("conv");
  // processesToBias.push_back("compt");
  // processesToBias.push_back("phot");
  fBiasingPhysics->Bias("gamma", processesToBias);
  //std::vector < const G4ParticleDefinition* > fParticlesToBias; for reference
  
  // EM physics in gas cavity (Photoabsorption Ionization Model)
  //fEmName = G4String("pai_photon");
  fEmName = G4String("pai");
  
  SetVerboseLevel(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{
  delete fDecayPhysicsList;
  delete fEmPhysicsList;
  for(size_t i=0; i<fHadronPhys.size(); ++i) { delete fHadronPhys[i]; }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructParticle()
{
  fDecayPhysicsList->ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructProcess()
{
  AddTransportation();
  fEmPhysicsList->ConstructProcess();
  fDecayPhysicsList->ConstructProcess();
  
  //Construct the biasing physics list
  fBiasingPhysics->ConstructProcess();
  
  for(size_t i=0; i<fHadronPhys.size(); ++i) { 
    fHadronPhys[i]->ConstructProcess(); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddPhysicsList(const G4String& name)
{
  
  if (verboseLevel>1) {
    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
  }

  if (name == fEmName) {
    return;

  } else if (name == "emstandard_opt1") {

    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmStandardPhysics_option1();

  } else if (name == "emstandard_opt2") {

    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmStandardPhysics_option2();

  } else if (name == "emstandard_opt3") {

    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmStandardPhysics_option3();

  } else if (name == "emstandard_opt4") {

    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmStandardPhysics_option4();

  } else if (name == "emlivermore") {

    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmLivermorePhysics();

  } else if (name == "empenelope") {

    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmPenelopePhysics();

  } else if (name == "pai") {

    fEmName = name;
    AddPAIModel(name);

  } else if (name == "pai_photon") { 

    fEmName = name;
    AddPAIModel(name);

  } else {

    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
           << " is not defined"
           << G4endl;
  }
}

void PhysicsList::SetCuts()
{
   	// Set the low limit on the production cut based on the first ionisation potential of the gas
	// Argon = 15.76	
	// Set the low limit on the production cut based on the first excitation potential of the gas
	// Argon = 11.6	
	G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(11.6*eV,100*GeV);
	if (verboseLevel >0){
    	G4cout << "PhysicsList::SetCuts:";
    	G4cout << "CutLength : " << defaultCutValue/mm << " (mm)" << G4endl;
  	}

  	// set cut values for gamma at first and for e- second and next for e+,
  	// because some processes for e+/e- need cut values for gamma

  	SetCutValue(fCutForGamma, "gamma");
  	SetCutValue(fCutForElectron, "e-");
  	SetCutValue(fCutForPositron, "e+");
  	SetCutValue(fCutForProton, "proton");

  	if ( verboseLevel > 0 ) { DumpCutValuesTable(); }
}

void PhysicsList::SetCutForGamma(G4double cut)
{
  fCutForGamma = cut;
  SetParticleCuts(fCutForGamma, G4Gamma::Gamma());
}

void PhysicsList::SetCutForElectron(G4double cut)
{
  fCutForElectron = cut;
  SetParticleCuts(fCutForElectron, G4Electron::Electron());
}

void PhysicsList::SetCutForPositron(G4double cut)
{
  fCutForPositron = cut;
  SetParticleCuts(fCutForPositron, G4Positron::Positron());
}

void PhysicsList::SetCutForProton(G4double cut)
{
  fCutForProton = cut;
  SetParticleCuts(fCutForProton, G4Proton::Proton());
}

void PhysicsList::AddPAIModel(const G4String& modname)
{
	theParticleIterator->reset();
  	while ((*theParticleIterator)())
  	{
    	G4ParticleDefinition* particle = theParticleIterator->value();
    	G4String partname = particle->GetParticleName();
    	if(partname == "e-" || partname == "e+") {
      		NewPAIModel(particle, modname, "eIoni");

    	} else if(partname == "mu-" || partname == "mu+") {
      		NewPAIModel(particle, modname, "muIoni");

    	} else if(partname == "proton" || partname == "pi+" || partname == "pi-") {
     		NewPAIModel(particle, modname, "hIoni");
    	}
  	}
}

void PhysicsList::NewPAIModel(const G4ParticleDefinition* part, 
                              const G4String& modname,
                              const G4String& procname)
{
	G4String partname = part->GetParticleName();
  	if(modname == "pai") {
    	G4PAIModel* pai = new G4PAIModel(part,"PAIModel");
    	fConfig->SetExtraEmModel(partname,procname,pai,"GasRegion", 0.0,100.*TeV,pai);
  	} else if(modname == "pai_photon") {
    	G4PAIPhotModel* pai = new G4PAIPhotModel(part,"PAIPhotModel");
    	fConfig->SetExtraEmModel(partname,procname,pai,"GasRegion", 0.0,100.*TeV,pai);
  	}
}

