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
// $Id: TrackerHit.hh $
//
/// \file TrackerHit.hh
/// \brief Definition of the TrackerHit class

#ifndef TrackerHit_h
#define TrackerHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

/// Tracker hit class
///
/// It defines data members to store the position of a particle in a selected volume:
/// - fPos

class TrackerHit : public G4VHit
{
public:
    TrackerHit();
    TrackerHit(const TrackerHit&);
    virtual ~TrackerHit();
    
    // operators
    const TrackerHit& operator=(const TrackerHit&);
    G4int operator==(const TrackerHit&) const;
    
    inline void* operator new(size_t);
    inline void operator delete(void*);
    
    // methods from base class
    virtual void Draw();
    virtual void Print();
	
	// Set methods
    void SetPos(G4ThreeVector xyz){ fPos = xyz; };

    // Get methods
    G4ThreeVector GetPos() const { return fPos; };
    
private:
      G4ThreeVector fPos;
    
};


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<TrackerHit> TrackerHitsCollection;

extern G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* TrackerHit::operator new(size_t)
{
    if (!TrackerHitAllocator)
        TrackerHitAllocator = new G4Allocator<TrackerHit>;
    return (void*)TrackerHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void TrackerHit::operator delete(void* aHit)
{
    TrackerHitAllocator->FreeSingle((TrackerHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
