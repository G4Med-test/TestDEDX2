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
// $Id: RunAction.cc,v 1.15 2010-05-10 13:45:49 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4Run.hh"
#include "G4ProcessManager.hh"
#include "G4UnitsTable.hh"
#include "G4EmCalculator.hh"
#include "G4Electron.hh"
#include "G4SystemOfUnits.hh"
#include "G4ESTARStopping.hh"

#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(DetectorConstruction* det, PrimaryGeneratorAction* kin)
:detector(det), primary(kin)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*)
{
  //instanciate EmCalculator
  G4EmCalculator emCal;
  G4ESTARStopping estar("long");
  
  // get particle 
  G4ParticleDefinition* particle = primary->GetParticleGun()
                                          ->GetParticleDefinition();
  partName = particle->GetParticleName();
  G4double energy   = primary->GetParticleGun()->GetParticleEnergy();
 
  // get material
  G4Material* material = detector->GetMaterial();
  G4String matName     = material->GetName();
  G4double density     = material->GetDensity();

  G4double Ecut = 100*keV;

  G4double Emin = 1*keV;
  G4int numBin = 242;
  G4double delta_E = 0.037345;
  G4double Emax = std::pow(10,std::log10(Emin)+delta_E*241);
  G4double log10energy;
  G4double cDEDXto1;
  G4double cDEDXto2;

  std::ostringstream ost1;
  std::ostringstream ost2;

  ost1 << partName + "_" + matName + "_cut100kev.dat";
  ost2 << partName + "_" + matName + "_cutE0.dat"; 

  std::ofstream fout1(ost1.str().c_str());
  std::ofstream fout2(ost2.str().c_str());

  fout1 << numBin << " " << Emin << " " << Emax << std::endl;
  fout2 << numBin << " " << Emin << " " << Emax << std::endl;

  if (partName == "e-") {
    G4double cDEDXto3;
    G4double cDEDXto4;
    std::ostringstream ost3;
    std::ostringstream ost4;
    ost3 << partName + "_" + matName + "_cut1km.dat";
    ost4 << partName + "_" + matName + "_ESTAR.dat";
    std::ofstream fout3(ost3.str().c_str());
    std::ofstream fout4(ost4.str().c_str());
    fout3 << numBin << " " << Emin << " " << Emax << std::endl;
    fout4 << numBin << " " << Emin << " " << Emax << std::endl;
    for (G4int i=0; i<numBin; i++){
      log10energy = std::log10(Emin) + delta_E*i;
      energy = std::pow(10,log10energy);
      cDEDXto3 = emCal.ComputeTotalDEDX(energy,particle,material);
      cDEDXto3 /= density;
      fout3 << energy << " " << cDEDXto3/(MeV / (g/cm2)) << std::endl;
      cDEDXto4 = estar.GetElectronicDEDX(material,energy);
      fout4 << energy << " " << cDEDXto4/(MeV / (g/cm2)) << std::endl;
    }
  }

  for (G4int i=0; i<numBin; i++){
    log10energy = std::log10(Emin) + delta_E*i;
    energy = std::pow(10,log10energy);
    cDEDXto1 = emCal.ComputeTotalDEDX(energy,particle,material,Ecut);
    cDEDXto1 /= density;
    fout1 << energy << " " << cDEDXto1/(MeV / (g/cm2)) << std::endl;
    cDEDXto2 = emCal.ComputeTotalDEDX(energy,particle,material,energy);
    cDEDXto2 /= density;
    fout2 << energy << " " << cDEDXto2/(MeV / (g/cm2)) << std::endl;
    //    cDEDXto3 = emCal.ComputeTotalDEDX(energy,particle,material);
    //cDEDXto3 /= density;
    //fout3 << energy << " " << cDEDXto3/(MeV / (g/cm2)) << std::endl;
    //    if (partName == "e-") {
    // cDEDXto4 = estar.GetElectronicDEDX(material,energy);
    //  fout4 << energy << " " << cDEDXto4/(MeV / (g/cm2)) << std::endl;
    //}
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* )
{ 
    if (partName == "e-") {
      G4ESTARStopping stop("basic");
      G4cout << "### ESTAR test E = 1 MeV water  stop= " <<  
	stop.GetElectronicDEDX(179, MeV)/(MeV / (g/cm2)) << " MeV*cm2/g" << G4endl;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
