//Set's up the class for biasing, use function AddParticle in GB02DetectorConstruction after added to a logical volume
// ** Instantiated in DetectorConstruction
//
//
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
#include "GB02BOptrMultiParticleForceCollision.hh"
#include "G4BiasingProcessInterface.hh"

#include "G4BOptrForceCollision.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

GB02BOptrMultiParticleForceCollision::GB02BOptrMultiParticleForceCollision()
: G4VBiasingOperator("TestManyForceCollision")
{
}

void GB02BOptrMultiParticleForceCollision::AddParticle(G4String particleName)
{// look for "particleName" in particle table and assign that to "particle" which is class G4ParticleDefinition
  const G4ParticleDefinition* particle =
    G4ParticleTable::GetParticleTable()->FindParticle( particleName );

  if ( particle == 0 ) //if FindParticle returns 0 from G4ParticleTable "particle" is void, this is an error, provide report
    {
      G4ExceptionDescription ed;
      ed << "Particle `" << particleName << "' not found !" << G4endl;
      G4Exception("GB02BOptrMultiParticleForceCollision::AddParticle(...)",
                  "exGB02.01",
                  JustWarning,
                  ed);
      return;
    }
  // base class is G4VBiasingOperator; is responsible for forcing collision by splitting track into interacting and non-interacting
  G4BOptrForceCollision* optr = new G4BOptrForceCollision(particleName,
                                                          "ForceCollisionFor"+particleName);
  // takes particle and adds it to the end of the vector according to std::vector
  fParticlesToBias.push_back( particle );
  // accesses the element "particle" that was added to the vector according to std::map
  fBOptrForParticle[ particle ] = optr;

}
//////////////////////////////////////////////////////////////////////////////////////////////////
// calls each of the functions from base class of GB02BOptrMultiParticleForceCollision and gets perameters from fCurrentOperator or returns 0 if 
// fCurrentOperator isn't defined 
G4VBiasingOperation* 
GB02BOptrMultiParticleForceCollision::
ProposeOccurenceBiasingOperation(const G4Track* track, 
                                 const G4BiasingProcessInterface* callingProcess)
{
  if ( fCurrentOperator ) return fCurrentOperator->
                            GetProposedOccurenceBiasingOperation(track, callingProcess);
  else                    return 0;
}

G4VBiasingOperation*
GB02BOptrMultiParticleForceCollision::
ProposeNonPhysicsBiasingOperation(const G4Track* track,
                                  const G4BiasingProcessInterface* callingProcess)
{
  if ( fCurrentOperator ) return fCurrentOperator->
                            GetProposedNonPhysicsBiasingOperation(track, callingProcess);
  else                    return 0;
}

G4VBiasingOperation*
GB02BOptrMultiParticleForceCollision::
ProposeFinalStateBiasingOperation(const G4Track* track,
                                  const G4BiasingProcessInterface* callingProcess)
{
  if ( fCurrentOperator ) return fCurrentOperator->
                            GetProposedFinalStateBiasingOperation(track, callingProcess);
  else                    return 0;
}


void GB02BOptrMultiParticleForceCollision::StartTracking( const G4Track* track )
{
  const G4ParticleDefinition* definition = track->GetParticleDefinition();
  std::map < const G4ParticleDefinition*, G4BOptrForceCollision* > :: iterator
    it = fBOptrForParticle.find( definition );
  fCurrentOperator = 0;
  if ( it != fBOptrForParticle.end() ) fCurrentOperator = (*it).second;
}

void 
GB02BOptrMultiParticleForceCollision::
OperationApplied( const G4BiasingProcessInterface*         callingProcess,
                  G4BiasingAppliedCase                        biasingCase,
                  G4VBiasingOperation*                   operationApplied,
                  const G4VParticleChange*         particleChangeProduced )
{
  if ( fCurrentOperator ) fCurrentOperator->ReportOperationApplied( callingProcess,
                                                                    biasingCase,
                                                                    operationApplied,
                                                                    particleChangeProduced );
  
}

void
GB02BOptrMultiParticleForceCollision::
OperationApplied( const G4BiasingProcessInterface*        callingProcess,
                  G4BiasingAppliedCase                       biasingCase,
                  G4VBiasingOperation*         occurenceOperationApplied,
                  G4double                 weightForOccurenceInteraction,
                  G4VBiasingOperation*        finalStateOperationApplied, 
                  const G4VParticleChange*        particleChangeProduced )
{
  if ( fCurrentOperator ) fCurrentOperator->ReportOperationApplied( callingProcess,
                                                                    biasingCase,
                                                                    occurenceOperationApplied,
                                                                    weightForOccurenceInteraction,
                                                                    finalStateOperationApplied, 
                                                                    particleChangeProduced );
}

void
GB02BOptrMultiParticleForceCollision::
ExitBiasing( const G4Track*                           track,
             const G4BiasingProcessInterface* callingProcess )
{
  if ( fCurrentOperator ) fCurrentOperator->ExitingBiasing( track, callingProcess );
}
