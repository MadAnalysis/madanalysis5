# Validation of MadAnalysis 5

This directory contains scripts and reference outputs for checking that changes
to MadAnalysis 5 preserve the analysis results. For backward-compatibility
checks, the scripts run template analyses on the current branch and inspect the
output relative to references built from the `main` branch.

Currently, only histogram, cutflow and region comparisons in the parton-level
mode are available. Negative-weight validation will be added when corresponding
samples become available. Other tests are deferred.

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
  scripts/
    lhe_histos_?.ma5
    lhe_cutflows_?.ma5
    lhe_regions_?.ma5
  outputs/      Reference SAF files, downloaded when needed
```

Input event samples are located in `samples/` at the MA5 repository root, and can be
downloaded by typing in the MadAnalysis5 CLI `install samples`.  Generated jobs
remain in their `ANALYSIS_X` directories; the checker does not delete them.

## Available validation scripts

| Script | Required sample | Coverage |
| --- | --- | --- |
| [lhe_histos_0.ma5](scripts/lhe_histos_0.ma5) | `samples/ttbar_sl_1.lhe.gz` | Parton-level event observables, particle IDs and weights; electron, muon, b-parton and jet kinematics; leading/subleading objects; angular observables and same-species/mixed-object combinations. |
| [lhe_histos_1.ma5](scripts/lhe_histos_1.ma5) | `samples/lljj.lhe.gz` | The same broad histogram coverage on another sample, with additional logarithmic-x histogram declarations. The input contains additional event weights, but the current checker only compares the first weight's bin contents. |
| [lhe_cutflows_0.ma5](scripts/lhe_cutflows_0.ma5) | `samples/ttbar_sl_1.lhe.gz` | Event-level selection and rejection, ordered objects, compound conditions, and cuts preserving or rejecting all remaining events. |
| [lhe_cutflows_1.ma5](scripts/lhe_cutflows_1.ma5) | `samples/ttbar_sl_1.lhe.gz` | Object filtering, before/after histograms, and event cuts on the resulting collections. |
| [lhe_cutflows_2.ma5](scripts/lhe_cutflows_2.ma5) | `samples/lljj.lhe.gz` | Event-level cuts on a multiweight sample: multiplicities, ordered objects, dilepton mass, compound conditions, and complete rejection; no explicit regions. |
| [lhe_cutflows_3.ma5](scripts/lhe_cutflows_3.ma5) | `samples/lljj.lhe.gz` | Object filtering on a multiweight sample, before/after histograms, and dilepton and jet event cuts; no explicit regions. |
| [lhe_regions_0.ma5](scripts/lhe_regions_0.ma5) | `samples/ttbar_sl_1.lhe.gz` | Region subsets, common and region-specific cuts, independent survival, empty regions, and region-associated histograms. |
| [lhe_regions_1.ma5](scripts/lhe_regions_1.ma5) | `samples/ttbar_sl_1.lhe.gz` | Region and selection removal, swapping cuts, and a separate resubmission after each edit. The initial submission and every resubmission are checked against separate references. |
| [lhe_regions_2.ma5](scripts/lhe_regions_2.ma5) | `samples/lljj.lhe.gz` | The same events in two datasets, with weights enabled and disabled; common and region-specific cuts and histograms. |
| [lhe_regions_3.ma5](scripts/lhe_regions_3.ma5) | `samples/lljj.lhe.gz` | Independent, overlapping and empty regions on a multiweight sample, using dilepton mass; shared cuts, subset associations and region-specific histograms. |

All scripts use parton-level events, `main.normalize = none` and `main.lumi = 1.0`.
The histogram-only scripts define no cuts or explicit regions. The current copies
of both input samples have only positive nominal event weights.

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
the MadAnalysis5 installation relative to their own files.

Only nominal weights are compared. Floating-point tolerances are `ABS_TOL=1e-7`
and `REL_TOL=1e-5`, and a numerical difference is considered significant when it
exceeds both tolerances. Cutflow entry counts are however compared exactly.
Moreover, display limits do not affect the pass/fail results.

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

**Comparison.** Histogram number, names, order and data-row counts must match.
The checker compares the sum of the first two columns in each bin, including
underflow and overflow. These columns contain the positive- and negative-weight
contributions for the nominal weight. Histogram types, axis metadata, statistics
and region associations are not checked.

**Direct comparison.** Compare two histogram files generated by MadAnalyssi5:

```bash
python3 HistoChecks.py compare <histo SAF file 1> <histo SAF file 2>
```

**Options.** Both modes accept `--max-bins N` (default 12), limiting displayed
bin differences per histogram.

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
printed cutflow differences and differing bins per histogram.

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
output. For multiple datasets, the reference names append the original dataset
and analyser directory names:
`<prefix>_<dataset>_<analyser>_<suffix>.saf`.

**Comparison.** Dataset/analyser names, region declarations and their order,
and the matching cutflow files are checked. A single unlabelled flat reference
matches exactly one dataset/analyser without checking its name. Every region's
nominal cutflow is compared with `CutflowChecks.py`, including empty regions and
counters after complete rejection. Histogram types, names, order and associated
region sets are checked, and bins are compared with `HistoChecks.py`. Association
order within a histogram does not matter. Extra weights, histogram statistics,
axis metadata and generated reports are also not compared.

Each step and dataset is compared independently. A difference in an intermediate
step fails the test even if the final step agrees. Missing or extra submission
steps also fail the test.

**Direct comparison.** It is also possible to run this script directly on two
MadAnalysis 5 output folders:

```bash
python3 RegionChecks.py compare <analysis 1> <analysis 2>
```

**Options.** Both modes accept `--max-differences N` (default 12), limiting printed
differences per check and differing bins per histogram.

## Planned extensions

- Add histogram and cutflow tests using a sample with negative nominal weights.
- Add a mixed signal/background analysis using the two LHE samples as separate
  datasets. Check their individual histograms and cutflows together with the
  combined report; compare nominal results against `main` and additional weights
  where supported.
- Add HepMC-based validation covering event reading, weight handling, reconstruction
  with FastJet, Delphes and DelphesMA5tune, and recasting with the corresponding
  PAD collections. Compare reconstructed observables, histograms, cutflows and
  region yields, including runs that save and reuse reconstructed events.

