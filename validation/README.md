# Validation of MadAnalysis 5

This directory contains scripts and reference outputs for checking that changes
to MadAnalysis 5 preserve analysis results. For backward-compatibility checks,
generate and inspect references on the `main` branch, then run the same scripts
with the same input samples on any other branch.

Currently, only histogram, cutflow and region comparisons in the parton-level
mode are available. Negative-weight validation will be added when the
corresponding sample is available. Other tests are deferred.

## Contents

- [Directory layout](#directory-layout)
- [Available validation scripts](#available-validation-scripts)
- [Running the histogram checks](#running-the-histogram-checks)
- [Running the cutflow checks](#running-the-cutflow-checks)
- [Running the region checks](#running-the-region-checks)
- [Planned extensions](#planned-extensions)

## Directory layout

```text
validation/
  README.md
  HistoChecks.py          Histogram comparison driver
  CutflowChecks.py        Cutflow (and histogram) comparison driver
  scripts/
    lhe_histos_?.ma5
    lhe_cutflows_?.ma5
    lhe_regions_?.ma5
  outputs/
    lhe_histos_?.saf              Reference histogram outputs
    lhe_cutflows_?_histos.ma5     Reference histogram outputs
    lhe_cutflows_?_regions.ma5    Reference cutflow outputs
    lhe_regions_?.saf             Reference histo/cutflow outputs for multiple regions
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
| [lhe_regions_1.ma5](scripts/lhe_regions_1.ma5) | `samples/ttbar_sl_1.lhe.gz` | Region and selection removal, swapping cuts, and a separate resubmission after each edit. Requires the region-removal and resubmission fixes. |
| [lhe_regions_2.ma5](scripts/lhe_regions_2.ma5) | `samples/lljj.lhe.gz` | The same events in two datasets, with weights enabled and disabled; common and region-specific cuts and histograms. |
| [lhe_regions_3.ma5](scripts/lhe_regions_3.ma5) | `samples/lljj.lhe.gz` | Independent, overlapping and empty regions on a multiweight sample, using dilepton mass; shared cuts, subset associations and region-specific histograms. |

All scripts use parton-level events, `main.normalize = none` and `main.lumi = 1.0`.
The histogram-only scripts define no cuts or explicit regions. The current copies
of both input samples have only positive nominal event weights.

## Running the histogram checks

Use an MA5 installation that can already compile and run parton-level analyses,
and provide the input sample required by the script. The Python comparison driver
uses only the standard library; running MA5 still requires its usual dependencies.

From the repository root:

```bash
cd validation
python3 HistoChecks.py run lhe_histos_0
python3 HistoChecks.py run lhe_histos_1
```

**Run mode must be invoked from the `validation/` directory.** The current driver
uses the parent of the working directory as the MA5 repository root.

For each test, the driver:
1. Looks for `scripts/<name>.ma5` and `outputs/<name>.saf`.
2. If the reference is absent, downloads `<name>.saf.gz` from
   [MadAnalysis/validation_data](https://github.com/MadAnalysis/validation_data)
   and decompresses it into `outputs/`. Input samples are not downloaded.
3. Runs `./bin/ma5 -s validation/scripts/<name>.ma5` from the repository root.
4. Identifies the single newly created `ANALYSIS_X` directory.
5. Finds the dataset's `MadAnalysis5job_0/Histograms/histos.saf` and compares it
   with the reference.

This workflow supports one dataset and one newly created analysis directory per
script. It is not yet a driver for multiple datasets or `resubmit` tests.

To compare existing files without running MA5, use explicit paths. For example,
from `validation/`, replacing `ANALYSIS_X` with the actual job directory:

```bash
python3 HistoChecks.py compare outputs/lhe_histos_0.saf \
  ../ANALYSIS_X/Output/SAF/testset/MadAnalysis5job_0/Histograms/histos.saf
```

Both modes accept `--max-bins N` to limit the number of differing bins printed
per histogram; the default is 12. This option does not change the comparison.

| Exit code | Meaning |
| --- | --- |
| `0` | No differences detected by the implemented comparison. |
| `1` | An execution, file-access, or parsing exception occurred. |
| `2` | A comparison mismatch was found. Argument-parsing errors also use this code. |


[HistoChecks.py](HistoChecks.py) reads each histogram's name and its `<Data>`
block, then compares histograms in file order.

| Quantity | Current comparison |
| --- | --- |
| Number of histograms | Exact match. |
| Histogram names and order | Exact match. |
| Number of data rows per histogram | Exact match. |
| Bin contents, including underflow and overflow | Sum of the first two columns of each row, compared numerically. These columns hold the positive- and negative-weight contributions for the first weight. |

The tolerances are `ABS_TOL = 1e-7` and `REL_TOL = 1e-5`. A numerical difference
is reported when it exceeds both the absolute and relative tolerances.
A successful comparison therefore establishes agreement only for the quantities
listed in the table. Separate sign comparisons and broader validation are planned.


## Running the cutflow checks

Run a cutflow validation from the repository root:

```bash
cd validation
python3 CutflowChecks.py run lhe_cutflows_0
```

Each test requires two references: `outputs/lhe_cutflow_N_region.saf` and
`outputs/lhe_cutflow_N_histos.saf`. Missing references are downloaded from the
same validation-data repository used by `HistoChecks.py`.

For each test, the driver:
1. Looks for `scripts/<name>.ma5` and the reference outputs.
2. Runs `./bin/ma5 -s validation/scripts/<name>.ma5` from the repository root.
3. Identifies the single newly created `ANALYSIS_X` directory.
4. Finds the dataset's histogram and cutflow files, and compares it
   with the reference.

This comparison requires one dataset, one newly created analysis directory per
script, the associated histograms and and cutflow regions.

The comparison checks:
- The initial-counter and cut names, order and number.
- The event/entry counts exactly.
- The sums of weights and squared weights.
- The histogram names, types, binning and region associations.
- All histogram statistics and bins, including underflow and overflow.

Positive and negative nominal contributions are compared separately.
Additional weight columns are not compared against the main-branch
references. Floating-point tolerances are `ABS_TOL = 1e-7` and
`REL_TOL = 1e-5`.

Existing outputs can also be compared without running MA5:

```bash
python3 CutflowChecks.py compare \
  validation/outputs/lhe_cutflow_0_region.saf \
  ANALYSIS_X/Output/SAF/testset/MadAnalysis5job_0/Cutflows/myregion.saf \
  validation/outputs/lhe_cutflow_0_histos.saf \
  ANALYSIS_X/Output/SAF/testset/MadAnalysis5job_0/Histograms/histos.saf
```

Both modes accept `--max-differences N`, limiting printed differences per
file without changing pass/fail. Exit codes are `0` for agreement, `1` for
execution/input errors and `2` for mismatches. Invalid command-line
arguments also return `2`.

## Running the region checks



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

