# Validation of MadAnalysis 5

This directory contains scripts and reference outputs for checking that changes
to MadAnalysis 5 preserve the analysis results. For backward-compatibility
checks, the scripts run template analyses on the current branch and inspect the
output relative to references built from the `main` branch.

Currently, histogram, cutflow and region comparisons are available in
parton-level mode, including samples with positive and negative nominal weights.
Other tests are deferred.

## Contents

- [Directory layout](#directory-layout)
- [Available validation scripts](#available-validation-scripts)
- [Shared conventions](#shared-conventions)
- [Running the histogram checks](#running-the-histogram-checks)
- [Running the cutflow checks](#running-the-cutflow-checks)
- [Running the region checks](#running-the-region-checks)
- [Planned extensions](#planned-extensions)

## Directory layout

```text
validation/
  README.md
  HistoChecks.py         Histogram comparison driver
  CutflowChecks.py       Cutflow (and histogram) comparison driver
  RegionChecks.py        Multiple-region and multiple-dataset comparison driver
  SAFReader.py           Shared SAF parsing for the three comparison drivers
  scripts/
    lhe_histos_?.ma5
    lhe_cutflows_?.ma5
    lhe_regions_?.ma5
  outputs/      Reference SAF files, downloaded when needed
```

Input event samples are located in `samples/` at the MA5 repository root, and can be
downloaded by typing in the MadAnalysis 5 CLI `install samples`.  Generated jobs
remain in their `ANALYSIS_X` directories; the checker does not delete them.

## Available validation scripts

Use `HistoChecks.py` for `lhe_histos_*`, `CutflowChecks.py` for
`lhe_cutflows_*`, and `RegionChecks.py` for `lhe_regions_*`. Pass the script
name without `.ma5` to the checker.

| Script | Required sample | Coverage |
| --- | --- | --- |
| [lhe_histos_0.ma5](scripts/lhe_histos_0.ma5) | `ttbar_sl_1.lhe.gz` | Parton-level event observables, particle IDs and weights; electron, muon, b-parton and jet kinematics; leading/subleading objects; angular observables and same-species/mixed-object combinations. |
| [lhe_histos_1.ma5](scripts/lhe_histos_1.ma5) | `lljj.lhe.gz` | The same broad histogram coverage on another sample, with additional logarithmic-x histogram declarations. The input contains additional event weights; only nominal results are compared. |
| [lhe_histos_2.ma5](scripts/lhe_histos_2.ma5) | `ttbar_nlo.lhe.gz` | Broad existing histogram suite with signed weights: event observables, particle IDs, lepton/b/jet kinematics, ranked objects, angular and mixed-object combinations, linear/logarithmic bins, underflow and overflow. |
| [lhe_histos_3.ma5](scripts/lhe_histos_3.ma5) | `ttbar_nlo.lhe.gz` | All eight ordering choices, positive/negative ranks, all four particle-status choices, scalar/vector sums and differences, ratios, impact-parameter observables, histogram setters and rendering shortcuts. |
| [lhe_cutflows_0.ma5](scripts/lhe_cutflows_0.ma5) | `ttbar_sl_1.lhe.gz` | Event-level selection and rejection, ordered objects, compound conditions, and cuts preserving or rejecting all remaining events. |
| [lhe_cutflows_1.ma5](scripts/lhe_cutflows_1.ma5) | `ttbar_sl_1.lhe.gz` | Object filtering, before/after histograms, and event cuts on the resulting collections. |
| [lhe_cutflows_2.ma5](scripts/lhe_cutflows_2.ma5) | `lljj.lhe.gz` | Event-level cuts on a multiweight sample: multiplicities, ordered objects, dilepton mass, compound conditions, and complete rejection; no explicit regions. |
| [lhe_cutflows_3.ma5](scripts/lhe_cutflows_3.ma5) | `lljj.lhe.gz` | Object filtering on a multiweight sample, before/after histograms, and dilepton and jet event cuts; no explicit regions. |
| [lhe_cutflows_4.ma5](scripts/lhe_cutflows_4.ma5) | `ttbar_nlo.lhe.gz` | Event `select`/`reject`, comparisons, `and`/`or`, multiplicities, kinematic cuts, an identity cut, complete rejection and empty downstream histograms/counters. |
| [lhe_cutflows_5.ma5](scripts/lhe_cutflows_5.ma5) | `ttbar_nlo.lhe.gz` | Object `select`/`reject`, compound object criteria, before/after multiplicities and spectra, subsequent event cuts. |
| [lhe_regions_0.ma5](scripts/lhe_regions_0.ma5) | `ttbar_sl_1.lhe.gz` | Region subsets, common and region-specific cuts, independent survival, empty regions, and region-associated histograms. |
| [lhe_regions_1.ma5](scripts/lhe_regions_1.ma5) | `ttbar_sl_1.lhe.gz` | Region and selection removal, swapping cuts, and a separate resubmission after each edit. The initial submission and every resubmission are checked against separate references. |
| [lhe_regions_2.ma5](scripts/lhe_regions_2.ma5) | `lljj.lhe.gz` | The same events in two datasets, with weights enabled and disabled; common and region-specific cuts and histograms. |
| [lhe_regions_3.ma5](scripts/lhe_regions_3.ma5) | `lljj.lhe.gz` | Independent, overlapping and empty regions on a multiweight sample, using dilepton mass; shared cuts, subset associations and region-specific histograms. |
| [lhe_regions_4.ma5](scripts/lhe_regions_4.ma5) | `ttbar_nlo.lhe.gz` | Same sample with event weights enabled/disabled in signal/background datasets; positive/negative input-weight partitions; common and region-local cuts; object filtering; overlapping associations; automatic histogram splitting; empty regions. |
| [lhe_regions_5.ma5](scripts/lhe_regions_5.ma5) | `ttbar_nlo.lhe.gz` | Six output steps: initial submission, swapped cuts, removed cut, edited histogram binning, disabled event weights, restored event weights. |

All scripts use parton-level events, `main.normalize = none` and `main.lumi = 1.0`.
The histogram-only scripts define no cuts or explicit regions. The supplied
`ttbar_sl_1.lhe.gz` and `lljj.lhe.gz` samples have only positive nominal weights.
The supplied `ttbar_nlo.lhe.gz` sample contains 10,000 events (7,720 positive
and 2,280 negative nominal weights) and 27 additional weights.

## Shared conventions

The examples below start in `validation/`. Each run reads `scripts/<name>.ma5`
and compares its output with references in `outputs/`. Missing flat references
are downloaded as `.saf.gz` files from
[MadAnalysis/validation_data](https://github.com/MadAnalysis/validation_data)
and decompressed locally, with the existing reference files being preserved.
Input event samples must already be installed and the comparison uses local
files only.

The drivers run `./bin/ma5 -s validation/scripts/<name>.ma5` from the repository
root and identify one newly created `ANALYSIS_X` directory. All drivers locate
the MadAnalysis 5 installation relative to their own files, so they can also be
invoked from another directory using the path to the checker.

All three checkers share [SAFReader.py](SAFReader.py), independently of MA5's
report reader. This helper reads SAF blocks and numeric weight pairs, preserves
exact integer counts and raises errors for malformed input. Only nominal results
are compared numerically, keeping positive and negative contributions separate.
Additional weight variations and generated reports are not compared.

Floating-point tolerances are `ABS_TOL=1e-7` and `REL_TOL=1e-5`, and a numerical
difference is considered significant when it exceeds both tolerances. Histogram
event/entry counts and cutflow entry counts are compared exactly. Display limits
do not affect the pass/fail results.

Undefined kinematic moments in old references (for example massless-particle
`GAMMA`) are accepted only when the produced moment has the same NaN/infinity
state. Nonfinite bins, counts, weight sums and squared-weight sums are rejected.

| Exit code | Meaning |
| --- | --- |
| `0` | Agreement for the quantities checked. |
| `1` | Execution, input or download error. |
| `2` | Comparison mismatch or invalid command-line arguments. |

## Running the histogram checks

**Run.** From `validation/`:

```bash
python3 HistoChecks.py run <name>
```

**References.** Each test uses `outputs/<name>.saf`. This mode supports one
dataset and one job submission. The region driver should be used for multiple
datasets or scripts including job resubmissions.

**Comparison.** Histogram number, names, order, types, axes, region associations
and frequency keys (PDG codes) must match within the applicable tolerances.
Positive and negative nominal contributions are compared separately in ordinary
and logarithmic bins, underflow/overflow and histogram statistics. Counts are
exact; other numerical quantities use the shared tolerances. Histogram region
association order does not matter.

**Direct comparison.** Compare two histogram files generated by MadAnalysis 5:

```bash
python3 HistoChecks.py compare <histo SAF file 1> <histo SAF file 2>
```

**Options.** Both modes accept `--max-bins N` (default 12), limiting displayed
differences per histogram, including bins, statistics and definitions.

## Running the cutflow checks

**Run.** From `validation/`:

```bash
python3 CutflowChecks.py run <name>
```

**References.** Each test uses `outputs/<name>_region.saf` and
`outputs/<name>_histos.saf`, and it supports one dataset, one job submission and
one cutflow region, with its histogram file.

**Comparison.** Initial-counter and cut names, order and number must match.
Entry counts are compared exactly, and sums of weights and squared weights with
the shared tolerances. Positive and negative nominal contributions are compared
separately. Histogram comparison reuses `HistoChecks.py`, with the scope above.

**Direct comparison.** The histogram paths are optional; omit both for a
cutflow-only comparison:

```bash
python3 CutflowChecks.py compare <cutflow SAF file 1> <cutflow SAF file 2> \
  <histo SAF file 1> <histo SAF file 2>
```

**Options.** Both modes accept `--max-differences N` (default 12), limiting
printed cutflow differences and histogram differences per histogram.

## Running the region checks

**Run.** From `validation/`:

```bash
python3 RegionChecks.py run <name>
```

The driver supports one new job containing multiple datasets and regions. It
runs the original script once, in one MA5 session. Successive resubmissions are
allowed and all steps are compared.

**References.** For one submission and one dataset, the references are named
`<name>_regions.saf` (the region declarations), `<name>_histos.saf` and
`<name>_<region>.saf` for each region, all files being directly available in the
folder `outputs/`. For scripts containing `resubmit`, the references use the
prefix `<name>_stepN`, starting at `step0` for the initial submission, with the
step number matching the suffix of the `MadAnalysis5job_N` folder in the job
output. For multiple datasets, the reference names append the dataset directory
name:
`<prefix>_<dataset>_<suffix>.saf`.
Here `<suffix>` is `regions`, `histos` or a region name.

**Comparison.** Dataset/analyser names, region declarations and their order,
and the matching cutflow files are checked. A single unlabelled flat reference
matches exactly one dataset/analyser without checking its name. Every region's
nominal cutflow is compared with `CutflowChecks.py`, including empty regions and
counters after complete rejection. Histogram types, names, order and associated
region sets are checked. Histogram bins, axes and statistics are compared with
`HistoChecks.py`. Association order within a histogram does not matter.

Each step and dataset is compared independently. A difference in an intermediate
step fails the test even if the final step agrees. Missing or extra submission
steps also fail the test.

**Direct comparison.** It is also possible to run this script directly on two
MadAnalysis 5 output folders:

```bash
python3 RegionChecks.py compare <analysis 1> <analysis 2>
```

**Options.** Both modes accept `--max-differences N` (default 12), limiting
printed differences per check and histogram differences per histogram. The run
mode also accepts `--reference PATH`, selecting a flat reference prefix, an
existing job directory or a SAF tree.


## Planned extensions

- Extend multiple-dataset validation to two different LHE samples, to detect
  accidental mixing of dataset results. Separately add checks of combined-report
  quantities and additional weight variations, which the current SAF comparisons
  do not cover.
- Add HepMC-based validation covering event reading, weight handling, reconstruction
  with FastJet, Delphes and DelphesMA5tune, and recasting with the corresponding
  PAD collections. Compare reconstructed observables, histograms, cutflows and
  region yields, including runs that save and reuse reconstructed events.

