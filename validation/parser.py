#!/usr/bin/env python

# TestDEDX2 portal parser: metadata() reads the macro, parse() the per-run .dat
# tables. This application does not simulate particle transport: it computes
# dE/dX analytically with G4EmCalculator (and G4ESTARStopping for electrons) in
# RunAction::BeginOfRunAction, over a fixed 242-bin log scan from 1 keV to
# ~1 TeV. /run/beamOn only needs to trigger a Run; the event count is otherwise
# irrelevant to the output.
import os

from geantval import getJSON, one_command, single_run

PHYSLISTS = {"emstandard_opt0", "emstandard_opt2", "emstandard_opt3",
             "empenelope", "emlivermore", "pai", "pai_photon"}

# Suffix of the .dat file -> human-readable production cut used to compute it.
# cut100kev and cutE0 are written for every particle; cut1km only for e-, and
# only meaningful if the macro set /testem/phys/setCuts 1 km (an unrestricted,
# "total" stopping power comparable to the ESTAR table).
CUT_FILES = {"cut100kev": "100 keV", "cutE0": "E0", "cut1km": "1 km"}


def extract_table(filename):
    x, y = [], []
    with open(filename) as myfile:
        next(myfile)  # header: "numBin Emin Emax", not needed here
        for line in myfile:
            fields = line.split()
            if len(fields) != 2:
                raise ValueError("Expected energy/dE-dX columns: " + line)
            x.append(float(fields[0]))
            y.append(float(fields[1]))
    if not x:
        raise ValueError("Empty dE/dX table: " + filename)
    return x, y


def plot(job, target, model, suffix, cut_label):
    filepath = os.path.join(job["path"], "%s_%s_%s.dat" % (job["PARTICLE"], job["MATERIAL"], suffix))
    if not os.path.exists(filepath):
        return None
    xvalues, yvalues = extract_table(filepath)
    return getJSON(job, "chart",
                   mctool_name="GEANT4",
                   mctool_model=model,
                   observableName="dE/dX",
                   targetName=target,
                   beamParticle=job["PARTICLE"],
                   beamEnergies=xvalues[:3],
                   secondaryParticle="None",
                   title="dE/dX",
                   xAxisName="E, MeV",
                   yAxisName="dE/dX, MeV cm2/g",
                   xValues=xvalues,
                   yValues=yvalues,
                   parameters=[{"names": "CUT", "values": cut_label}])


def parse(job):
    target = job["MATERIAL"].removeprefix("G4_")
    found = False
    for suffix, cut_label in CUT_FILES.items():
        record = plot(job, target, job["PHYSLIST"], suffix, cut_label)
        if record is not None:
            found = True
            yield record

    if job["PARTICLE"] == "e-":
        record = plot(job, target, "ESTAR", "ESTAR", "1 km")
        if record is not None:
            found = True
            yield record

    if not found:
        raise ValueError("No dE/dX tables found for %s in %s" % (job["PARTICLE"], job["MATERIAL"]))


def metadata(commands):
    single_run(commands)
    physlist = one_command(commands, "/testem/phys/addPhysics")
    if physlist not in PHYSLISTS:
        raise ValueError("Unsupported physics list for this validation: " + physlist)
    return {"TEST": "TestDEDX2",
            "PHYSLIST": physlist,
            "MATERIAL": one_command(commands, "/testem/det/setMat"),
            "PARTICLE": one_command(commands, "/gun/particle")}
