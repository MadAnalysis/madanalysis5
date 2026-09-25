# Validation of MadAnalysis 5

This directory contains scripts and reference outputs for checking that changes
to MadAnalysis 5 preserve analysis results. For backward-compatibility checks,
generate and inspect references on the `main` branch, then run the same scripts
with the same input samples on any other branch.

Currently, only histogram comparisons are implemented. Cutflow, region and
negative-weight validation are planned. Other tests are deferred.

## Contents

- [Directory layout](#directory-layout)
- [Available validation scripts](#available-validation-scripts)
- [Running the histogram checks](#running-the-histogram-checks)
- [What the histogram checker compares](#what-the-histogram-checker-compares)
- [Planned extensions](#planned-extensions)

## Directory layout

```text
validation/
  README.md
  HistoChecks.py          Histogram comparison driver
  scripts/
    lhe_histos_0.ma5
    lhe_histos_1.ma5
  outputs/
    lhe_histos_0.saf      Reference histogram output
    lhe_histos_1.saf      Reference histogram output
```

Input event samples are located in `samples/` at the MA5 repository root, and can be
downloaded by typing in the MadAnalysis5 CLI `install samples`.  Generated jobs
remain in their `ANALYSIS_X` directories; the checker does not delete them.

## Available validation scripts

| Script | Required sample | Coverage |
| --- | --- | --- |
| [lhe_histos_0.ma5](scripts/lhe_histos_0.ma5) | `samples/ttbar_sl_1.lhe.gz` | Parton-level event observables, particle IDs and weights; electron, muon, b-parton and jet kinematics; leading/subleading objects; angular observables and same-species/mixed-object combinations. |
| [lhe_histos_1.ma5](scripts/lhe_histos_1.ma5) | `samples/lljj.lhe.gz` | The same broad histogram coverage on another sample, with additional logarithmic-x histogram declarations. The input contains additional event weights, but the current checker only compares the first weight's bin contents. |

Both scripts use the dataset name `testset`, set `main.normalize = none` and
`main.lumi = 1.0`, and finish with `submit`. Neither defines selection cuts or
explicit signal regions. The current copies of both samples have only positive
nominal event weights.

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


## What the histogram checker compares

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


## Planned extensions

Cutflow and region validation is planned to be deployed through 5 scripts.

| Proposed script | Intended coverage |
| --- | --- |
| `lhe_cutflows_0.ma5` | Event-level `select` and `reject`, multiplicities, kinematic cuts, compound conditions, and cuts accepting all or rejecting all remaining events. |
| `lhe_cutflows_1.ma5` | Object-level selection and rejection, followed by multiplicity cuts and histograms verifying the modified object collections. |
| `lhe_regions_0.ma5` | Cuts and histograms associated with all regions, one region, or a subset; independent region survival, overlapping selections, and regions with no surviving events. |
| `lhe_regions_1.ma5` | Analysis editing with `swap`, removal of selections and regions, and `resubmit`; comparison with an equivalent directly constructed analysis. Supporting scripts may be needed. |
| `lhe_cutflows_weights.ma5` | Event weights enabled and disabled, including multiple weights where available, with comparisons of initial and surviving counters. |

The `CutflowChecks.py` driver will provide explicit reference recording, run-and-compare,
and comparison of existing outputs. Its CLI will be documented once implemented. It will
compare:

- Dataset and region identities, detecting missing or extra outputs.
- Cut names and order within each region.
- Initial and surviving event counts, separately by weight sign, with exact equality.
- Sums of weights and squared weights, separately by sign, with numerical tolerances.
- All weights when both outputs support them, with an explicit nominal-only mode
  for comparisons with the earlier code.

A sample containing both positive and negative nominal weights will be provided.
It will be added here when available. One or more scripts will use it to test
both histograms and cutflows, including selections retaining and rejecting events
of both signs and a run with event weights disabled. The histogram checker must
first be extended to compare the positive and negative contributions separately
and relevant statistics, including squared-weight sums. Comparing only the net
bin contents is insufficient.


