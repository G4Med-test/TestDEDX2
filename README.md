# TestDEDX2

**Detailed dE/dX (electron and heavy-charged-particle stopping power) validation
across Geant4's EM physics lists**, including comparison against NIST ESTAR
stopping-power tables for electrons. Author: A. Bagulya; based on the `TestEm0`
example.

[![CI Pipeline](https://github.com/G4Med-test/TestDEDX2/actions/workflows/ci.yml/badge.svg)](https://github.com/G4Med-test/TestDEDX2/actions/workflows/ci.yml)

---

## 🔬 What this test does

Unlike most tests in this organization, `TestDEDX2` does not track particles
through a simulated geometry: `RunAction::BeginOfRunAction` calls `G4EmCalculator`
directly to compute the total dE/dX of the configured particle in the configured
material, over 242 log-spaced energy points from 1 keV to ~1 TeV, for whichever
physics list the macro selected. `/run/beamOn` only needs to trigger a Run; the
event count does not affect the output. Three production-cut variants are
written for every particle (`_cut100kev.dat`, `_cutE0.dat`, where `E0` is each
scan point's own energy), and for electrons specifically, two more: `_cut1km.dat`
(an unrestricted/"total" stopping power, meaningful only when the macro sets
`/testem/phys/setCuts 1 km`) and `_ESTAR.dat` (the NIST ESTAR reference, via
`G4ESTARStopping`, for direct comparison).

Supported physics lists (`PhysicsList::AddPhysicsList`): `emstandard_opt0`,
`emstandard_opt2`, `emstandard_opt3`, `empenelope`, `emlivermore`, `pai` and
`pai_photon`.

## 🛠️ Fill in the repository (from the template)

| Path | What it contains |
| --- | --- |
| `main.cc`, `src/`, `include/` | The Geant4 application, imported unchanged from `geant-validation-tests/src/TestDEDX2` (it already used the standard single-macro-argument invocation). |
| `macro/unit.mac` | One-event smoke test (electron in aluminum, `emstandard_opt0`). |
| `macro/*.mac` | Seven self-contained macros written by hand from the original `PARTICLE`/`MATERIAL`/`PHYSLIST` templates (which were only ever expanded by the original `run.py`, never committed as concrete macros): electrons in water, aluminum, lead and gold, plus a low-energy model comparison (water with `empenelope`/`emlivermore`), and one proton run. |
| `validation/config.json` | Lists all seven macros above. |
| `validation/parser.py` | Reads whichever `<PARTICLE>_<MATERIAL>_<cut>.dat` tables the run produced and exports each as a dE/dX plot. |

See [`validation/README.md`](validation/README.md) for the parser's provenance,
exact behaviour and the one deliberate deviation from the original (`beamEnergies`
is no longer a hardcoded, unrelated placeholder).

## 🚀 How to run the container

```bash
apptainer build --build-arg PROJECT_NAME=TestDEDX2 TestDEDX2.sif Apptainer.def
apptainer run TestDEDX2.sif macro/e-_G4_WATER_emstandard_opt0.mac
```

No Geant4 datasets are needed for this test (no hadronic/EM data files are read
by `G4EmCalculator` beyond what ships with the physics lists themselves), but the
shared container still mounts `/g4data` as usual.

See the top-level [template README](https://github.com/G4Med-test/template#readme)
for the full local workflow (pulling the CI-built container and running
`ci-workflows/validation/export.py`).

## 📦 Container on GHCR

[![Container on GHCR](https://img.shields.io/badge/Container-GHCR-blue?logo=github)](https://github.com/orgs/G4Med-test/packages?repo_name=TestDEDX2)

```bash
apptainer pull oras://ghcr.io/g4med-test/testdedx2:<tag>
```
> Replace `<tag>` with the desired release tag or commit hash.

## ✅ GitHub Actions workflow

Defined in the shared [`ci-workflows`](https://github.com/G4Med-test/ci-workflows)
repository and invoked from [`.github/workflows/ci.yml`](.github/workflows/ci.yml).
