# Portal JSON parser

Adapted from `geant-config-generator/tests/geant4/TestDEDX2/parser.py` at commit
`0bf42953c9c98ac6ecccf077e7c7fe2c4ceb7307`, with queue discovery replaced by the
shared ci-workflows exporter.

This application does not simulate particle transport. `RunAction::BeginOfRunAction`
computes dE/dX directly with `G4EmCalculator` (and `G4ESTARStopping` for electrons)
over a fixed 242-bin log-spaced energy scan from 1 keV to ~1 TeV, for whichever
particle, material and physics list the macro selected; `/run/beamOn` only needs
to trigger a Run; the exported dE/dX tables do not depend on the event count.

`config.json` selects seven macros, written by hand (the original repository only
ships placeholder templates with `PARTICLE`/`MATERIAL`/`PHYSLIST` substitutions,
expanded at run time by its own `run.py`, not self-contained macros): electrons in
water, aluminum, lead and gold with `emstandard_opt0`, water with `empenelope` and
`emlivermore` for a low-energy model comparison, and one non-electron run (proton
in water) to exercise the rest of the application. The full list of supported
physics lists is `emstandard_opt0`, `emstandard_opt2`, `emstandard_opt3`,
`empenelope`, `emlivermore`, `pai` and `pai_photon`; the `emstandard_opt4` entry in
the original `params.conf` is not implemented by this application's `PhysicsList`.

For each macro, the parser reads whichever of `<PARTICLE>_<MATERIAL>_cut100kev.dat`,
`_cutE0.dat` and (electrons only) `_cut1km.dat` and `_ESTAR.dat` the run produced,
one plot per file (`cut1km` is only physically meaningful as an unrestricted/total
stopping power if the macro set `/testem/phys/setCuts 1 km`, as the electron macros
here do). Unlike the original parser, `beamEnergies` is the first three energies of
each scan (as in the `Attenuation-ilantest` parser) rather than the hardcoded,
unrelated `[1, 2, 3, 4, 5]` placeholder of the original.

The pipeline uploads `validation-json/results.json` (an array of plot objects)
and `plots/*.json` (the original one-object-per-file format), plus raw artifacts.
See [ci-workflows](https://github.com/G4Med-test/ci-workflows) for local CLI usage.
