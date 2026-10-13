#!/usr/bin/env python3
################################################################################
#
#  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
#  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
#
#  This file is part of MadAnalysis 5.
#  Official website: <https://github.com/MadAnalysis/madanalysis5>
#
#  MadAnalysis 5 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MadAnalysis 5 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
#
################################################################################

"""Compare nominal cutflows and histograms against main-branch SAF references.

This script supports two workflows.

1. Direct comparison of existing files without running MA5:
     python3 CutflowChecks.py compare <reference_region.saf> <produced_region.saf> [<reference_histos.saf> <produced_histos.saf>]

2. Run-and-compare mode given a validation name:
   - check that ``scripts/NAME.ma5`` exists,
   - run ``./bin/ma5 -s validation/scripts/NAME.ma5`` from the MA5 home directory,
   - detect the newly created ``ANALYSIS_X`` directory,
   - compare its cutflow output against ``outputs/<NAME>_region.saf``.
   - compare its histogram output against ``outputs/<NAME>_histos.saf``.
   - if the reference output file is not available, it is downloaded from GitHub

   Usage:
     python3 CutflowChecks.py run <NAME>

   This Run mode requires one dataset, one submission and one cutflow region.
   RegionChecks should be used for multiple datasets, regions or resubmission steps.

Cutflow counts, sums of weights and sums of squared weights are compared for
positive and negative nominal contributions separately. Histogram comparison
uses HistoChecks.compare_histos: histogram definitions, statistics and signed nominal bins.
Additional weight variations are not compared. In direct mode, either supply
both histogram paths or omit both; run mode always compares both output files.

Exit codes: 0 = agreement, 1 = execution/input error, 2 = comparison mismatch
(argparse also uses 2 for invalid arguments).
"""

from __future__ import annotations

import argparse
import math
import subprocess
import sys
from dataclasses import dataclass
from SAFReader import Block, read_blocks, numeric_row
from pathlib import Path
from HistoChecks import compare_histos, ensure_reference_file, extract_bin_values, find_new_analysis_dir

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
from HistoChecks import ABS_TOL
from HistoChecks import REL_TOL
COUNTER_ROWS = ("nentries", "sum of weights", "sum of weights^2")

# ANSI colors for terminal output
RED = "\033[91m"
ORANGE = "\033[93m"
GREEN = "\033[92m"
BOLD = "\033[1m"
RESET = "\033[0m"

# ---------------------------------------------------------------------------
# Structures holding parsed SAF blocks and cutflow counters
# ---------------------------------------------------------------------------
@dataclass
class Record:
    """
    One initial/cut counter: block type, displayed name and numeric rows.
    Each row is (label, values, exact), as returned by SAFReader.numeric_row.
    """
    kind: str
    name: str
    rows: list

# ---------------------------------------------------------------------------
# SAF parsing and validation
# ---------------------------------------------------------------------------
def read_cutflows(path: Path) -> list:
    """
    Return ordered Record objects for one region's plain SAF file.

    Each block has a name and three rows: entries, sum of weights and sum of
    squared weights. Preserve all pairs and the signs stored in the file;
    entry counts are exact integers. Empty cuts remain part of the sequence.
    """
    records = []
    for index, block in enumerate(read_blocks(path)):
        # The first block is the initial counter; subsequent blocks follow the cut order.
        # Each contains one name row followed by three numeric rows.
        expected = "InitialCounter" if index == 0 else "Counter"
        if block.tag != expected or block.children or len(block.lines) != 4: raise ValueError(f"{path}: malformed {expected} block {index}")
        if len(block.lines[0]) != 1: raise ValueError(f"{path}: expected a quoted counter name")

        # Only entry counts require exact integers; weighted sums are floating-point.
        rows = [numeric_row(tokens, label, exact=(i==0)) for i, (tokens, label) in enumerate(zip(block.lines[1:], COUNTER_ROWS))]
        records.append(Record(block.tag, block.lines[0][0], rows))
    check_weight_columns(records, path)
    return records


def check_weight_columns(records: list, path: Path) -> int:
    """Require one common row width within a file; return its number of weight pairs."""
    widths = {len(values) for record in records for _, values, _ in record.rows}
    if len(widths) != 1: raise ValueError(f"{path}: inconsistent number of weight columns")
    return next(iter(widths)) // 2

# ---------------------------------------------------------------------------
# Cutflow comparison: nominal counts and weighted sums, separated by sign
# ---------------------------------------------------------------------------
def compare_records(reference: list, produced: list) -> list:
    """
    Return all nominal discrepancies in block order, without printing.

    Match names/types and row labels before comparing both nominal columns.
    Counts use exact equality; weighted sums use the shared float tolerances.
    Reference and produced files may have different numbers of variation pairs.
    """
    differences = []

    # Compare cuts in their original order, including cuts with zero surviving events.
    if len(reference) != len(produced): differences.append(f"Different number of blocks: {len(reference)} vs {len(produced)}")
    for index, (ref, out) in enumerate(zip(reference, produced)):
        location = f"block {index + 1} ({ref.name})"
        if (ref.kind, ref.name) != (out.kind, out.name): differences.append(f"{location}: type/name mismatch: {out.kind} {out.name!r}")
        if len(ref.rows) != len(out.rows): differences.append(f"{location}: different number of rows: {len(ref.rows)} vs {len(out.rows)}")
        for (label, values, exact), (other_label, other_values, _) in zip(ref.rows, out.rows):
            if label != other_label: differences.append(f"{location}: row mismatch: {label!r} vs {other_label!r}")
            # The first two columns are the nominal positive/negative contributions.
            # Additional weight pairs may differ in number and are not compared.
            for sign, value, other in zip(("positive", "negative"), values[:2], other_values[:2]):
                equal = value == other if exact else math.isclose(value, other, rel_tol=REL_TOL, abs_tol=ABS_TOL)
                if not equal: differences.append(f"{location}, {label}, {sign}: reference={value}, produced={other} (abs difference={abs(value - other):.6g})")
    return differences

# ---------------------------------------------------------------------------
# Combined comparison: cutflows and optional histograms
# Histogram comparison is delegated to HistoChecks.
# ---------------------------------------------------------------------------
def compare_outputs(reference_region: Path, produced_region: Path, reference_histos: Path | None = None, produced_histos: Path | None = None, max_differences: int = 12) -> bool:
    """Compare cutflows, optionally delegating histogram comparison to HistoChecks."""
    if (reference_histos is None) != (produced_histos is None): raise ValueError("Provide both histogram paths or omit both.")

    ref = read_cutflows(reference_region)
    out = read_cutflows(produced_region)
    differences = compare_records(ref, out)
    color = RED if differences else GREEN
    print(f"{color}Cutflows: {'FAIL' if differences else 'PASS'} ({len(ref)} reference blocks){RESET}")
    print(f"  Reference: {reference_region}\n  Produced : {produced_region}")
    print(f"  Comparing nominal positive/negative columns separately; weight pairs: reference={check_weight_columns(ref, reference_region)}, produced={check_weight_columns(out, produced_region)}")

    # Limit printed differences only; every discrepancy contributes to failure.
    for difference in differences[:max_differences]: print(f"{RED}  {difference}{RESET}")
    if len(differences) > max_differences: print(f"{RED}  ... {len(differences) - max_differences} more differences{RESET}")
    all_ok = not differences

    if reference_histos is not None:
        histos_ok, reports = compare_histos(reference_histos, produced_histos, max_bins_to_print=max_differences)
        color = GREEN if histos_ok else RED
        print(f"{color}Histograms: {'PASS' if histos_ok else 'FAIL'}{RESET}")
        print(f"  Reference: {reference_histos}\n  Produced : {produced_histos}")
        print("  Using HistoChecks: separate nominal signs, statistics and definitions.")
        for report in reports: print(report)
        all_ok = all_ok and histos_ok
    return all_ok

# ---------------------------------------------------------------------------
# MA5 execution and output discovery
# ---------------------------------------------------------------------------
def find_outputs(analysis_dir: Path) -> tuple:
    """Require one dataset, one generated analyzer and one cutflow region."""
    saf_dir = analysis_dir / "Output" / "SAF"
    histograms = sorted(saf_dir.glob("*/MadAnalysis5job_*/Histograms/histos.saf"))
    if len(histograms) != 1: raise RuntimeError(f"Expected one histogram output under {saf_dir}; found {len(histograms)}")
    histos = histograms[0]
    regions = sorted((histos.parent.parent / "Cutflows").glob("*.saf"))
    if len(regions) != 1: raise RuntimeError(f"Expected one cutflow region next to {histos}; found {len(regions)}")
    return regions[0], histos


def run_ma5_script(name: str, ma5dir: Path) -> tuple:
    """Fetch/check references, run one script and locate its single-region output.

    Return reference cutflow, produced cutflow, reference histograms and produced
    histograms, in the argument order expected by compare_outputs. Leave the
    generated job on disk; do not overwrite or create reference results.
    """
    # Locate the requested validation script.
    script = ma5dir / "validation" / "scripts" / f"{name}.ma5"
    if not script.is_file(): raise FileNotFoundError(f"Missing MA5 script: {script}")

    # Download missing references before running the analysis
    reference_region = ensure_reference_file(name + "_region", ma5dir)
    reference_histos = ensure_reference_file(name + "_histos", ma5dir)

    # Reject incomplete references before launching a potentially expensive job.
    # This parses and validates the histograms; the summed bin values are not used for comparison which keeps both nominal signs separate.
    read_cutflows(reference_region)
    if not extract_bin_values(reference_histos): raise ValueError(f"No histograms found in reference file: {reference_histos}")

    # Record existing jobs so that only the newly created output is selected.
    before = [p.name for p in ma5dir.iterdir() if p.is_dir() and p.name.startswith("ANALYSIS_")]

    # Run MA5 from its installation directory.
    command = ["./bin/ma5", "-s", str(script.relative_to(ma5dir))]
    result = subprocess.run(command, cwd=ma5dir)
    if result.returncode: raise RuntimeError(f"MA5 execution failed with exit code {result.returncode}")

    # Locate the cutflow and histogram files produced by this run.
    analysis_dir = find_new_analysis_dir(ma5dir, before)
    produced_region, produced_histos = find_outputs(analysis_dir)
    print(f"Analysis directory: {analysis_dir}")
    return reference_region, produced_region, reference_histos, produced_histos

# ---------------------------------------------------------------------------
# Command-line interface
# ---------------------------------------------------------------------------
def positive_integer(value: str) -> int:
    """Validate a positive CLI display limit; also used by RegionChecks."""
    number = int(value)
    if number < 1: raise argparse.ArgumentTypeError("must be positive")
    return number


def main() -> int:
    """Dispatch run/compare mode and translate errors or mismatches into exit codes."""
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    modes = parser.add_subparsers(dest="mode", required=True)
    run = modes.add_parser("run", help="Run one cutflow script and compare both outputs.")
    run.add_argument("name", help="Name of the validation script, without .ma5.")
    compare = modes.add_parser("compare", help="Compare cutflows, optionally with a pair of histogram files.")
    for name in ("reference_region", "produced_region"): compare.add_argument(name, type=Path)
    for name in ("reference_histos", "produced_histos"): compare.add_argument(name, type=Path, nargs="?", help="Optional; supply both histogram paths.") # Accepts either both histogram paths or neither
    for mode in (run, compare): mode.add_argument("--max-differences", type=positive_integer, default=12, help="Maximum displayed cutflow differences and differences per histogram (default: 12).")
    args = parser.parse_args()
    if args.mode == "compare" and ((args.reference_histos is None) != (args.produced_histos is None)): parser.error("Provide both histogram paths or omit both.")
    try:
        if args.mode == "run": files = run_ma5_script(args.name, Path(__file__).resolve().parent.parent)
        else: files = tuple(getattr(args, name).resolve() if getattr(args, name) is not None else None for name in ("reference_region", "produced_region", "reference_histos", "produced_histos"))
        ok = compare_outputs(*files, max_differences=args.max_differences)
        quantities = "cutflows and histograms" if files[2] is not None else "cutflows"
        color = GREEN if ok else RED
        message = f"Nominal {quantities} agree." if ok else "Validation differences found."
        print(f"{color}{message}{RESET}")
        return 0 if ok else 2
    except (OSError, ValueError, RuntimeError) as exc:
        print(f"{RED}Error: {exc}{RESET}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
