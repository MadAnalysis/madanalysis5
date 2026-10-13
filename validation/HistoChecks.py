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

"""Comparison of two MA5 histogram SAF files

This script supports two workflows.

1. Direct comparison of existing files without running MA5:
   Compare a reference ``histos.saf`` file against a produced histogram file.

   Usage:
       python3 HistoChecks.py compare <reference.saf> <produced.saf>

2. Run-and-compare mode
   Given a validation name ``NAME``
   - check that ``scripts/NAME.ma5`` exists,
   - run ``./bin/ma5 -s validation/scripts/NAME.ma5`` from the MA5 home directory,
   - detect the newly created ``ANALYSIS_X`` directory,
   - compare its histogram output against ``outputs/NAME.saf``.
   - if the reference output file is not available, it is downloaded from GitHub

   Usage:
       python3 HistoChecks.py run <NAME>

The comparison checks nominal positive/negative contributions separately,
histogram statistics, axes, types and region associations. Additional weight
columns are validated structurally but are not compared numerically, even when
both files contain them. Generated reports, their normalisation and
signal/background figures of merit are outside this comparison. CutflowChecks
and RegionChecks reuse this module.

Exit codes: 0 = agreement, 1 = execution/input error, 2 = comparison mismatch
(argparse also uses 2 for invalid arguments).
"""

from __future__ import annotations

import argparse
from decimal import Decimal
from SAFReader import read_blocks, numeric_row
import gzip
import math
import os
import subprocess
import shutil
import sys
import urllib.request
import urllib.error
from pathlib import Path
from typing import List, Sequence, Tuple

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
# Floating-point values agree if either the absolute or relative tolerance
# is satisfied. Event and entry counts are compared exactly.
ABS_TOL = 1e-7
REL_TOL = 1e-5
THRESHOLD_ORANGE = 1e-12
THRESHOLD_RED = 1e-6
DEFAULT_MAX_BINS_TO_PRINT = 12
REFERENCE_BASE_URL = "https://raw.githubusercontent.com/MadAnalysis/validation_data/main"

# ANSI colors for terminal output.
RED = "\033[91m"
ORANGE = "\033[93m"
GREEN = "\033[92m"
BOLD = "\033[1m"
RESET = "\033[0m"


# ---------------------------------------------------------------------------
# Ensure the reference SAF file exists locally
# ---------------------------------------------------------------------------
def ensure_reference_file(name: str, ma5dir: Path, output_dir: Path | None = None) -> Path:
    """Ensure validation/outputs/<name>.saf exists locally.

    If missing, download <name>.saf.gz from the validation_data GitHub repo
    and decompress it locally. An optional output directory allows other
    validation drivers to use an explicitly supplied reference location.
    Existing SAF files are reused unchanged.
    """

    # Init and safety
    local_dir = output_dir if output_dir is not None else ma5dir / "validation" / "outputs"
    local_dir.mkdir(parents=True, exist_ok=True)
    local_saf = local_dir / f"{name}.saf"
    if local_saf.exists(): return local_saf

    # Download needed
    remote_gz_url = f"{REFERENCE_BASE_URL}/{name}.saf.gz"
    local_gz = local_dir / f"{name}.saf.gz"

    print(f"{BOLD}Reference SAF not found locally.{RESET}")
    print(f"{BOLD}Downloading:{RESET} {remote_gz_url}")

    try:
        urllib.request.urlretrieve(remote_gz_url, local_gz)
    except urllib.error.HTTPError as exc:
        raise FileNotFoundError(f"Could not download reference file: {remote_gz_url} (HTTP {exc.code})") from exc
    except urllib.error.URLError as exc:
        raise RuntimeError(f"Failed to download reference file: {remote_gz_url} ({exc})") from exc

    # Decompress one gzip-compressed SAF file
    print(f"{BOLD}Decompressing:{RESET} {local_gz.name}")
    with gzip.open(local_gz, "rb") as fin, open(local_saf, "wb") as fout:
        shutil.copyfileobj(fin, fout)
        local_gz.unlink(missing_ok=True)

    # Check and exit
    if not local_saf.exists(): raise FileNotFoundError(f"Decompression failed, missing file: {local_saf}")
    return local_saf

# ---------------------------------------------------------------------------
# SAF parsing: retain both signs, including the PDG key in frequency histograms
# ---------------------------------------------------------------------------
def read_histograms(path: Path) -> list:
    """Read nominal histogram bins, statistics and definitions.

    SAF stores the magnitude of negative contributions separately. Frequency
    rows start with a PDG key, followed by the positive/negative weight pairs.
    Extra weight pairs are validated but not compared with the old-branch file.

    Return dictionaries in file order with name, kind, axis, regions, stats and
    data fields. Statistics retain all weight columns; data rows retain the
    nominal pair; regions are sets. Reject malformed definitions, statistics or
    bin layouts with ValueError.
    """
    histograms = []
    for block in read_blocks(path):
        # Safety tests
        if block.tag not in ("Histo", "HistoLogX", "HistoFrequency"): raise ValueError(f"{path}: unexpected histogram block {block.tag}")
        children = {child.tag: child for child in block.children}
        if len(children) != 3 or len(block.children) != 3 or set(children) != {"Description", "Statistics", "Data"}: raise ValueError(f"{path}: expected Description, Statistics and Data")
        description = children["Description"].lines
        frequency = block.tag == "HistoFrequency"

        # The description starts with the name, followed by an axis row for ordinary/log-X histograms; the remaining rows list associated regions
        start = 1 if frequency else 2
        if len(description) < start or len(description[0]) != 1: raise ValueError(f"{path}: malformed histogram description")
        name = description[0][0]
        axis = ()
        if not frequency:
            if len(description[1]) != 3: raise ValueError(f"{path}: malformed axis for {name}")
            axis = tuple(float(value) for value in description[1])
            if not all(math.isfinite(x) for x in axis) or axis[0] <= 0 or not axis[0].is_integer() or axis[1] >= axis[2]: raise ValueError(f"{path}: invalid axis for {name}")
        regions = description[start:]
        if any(len(row) != 1 for row in regions) or len({row[0] for row in regions}) != len(regions): raise ValueError(f"{path}: malformed region associations for {name}")

        # Remark: frequency histograms have four statistics rows; ordinary/log-X have seven
        statistics = children["Statistics"].lines
        if len(statistics) != (4 if frequency else 7): raise ValueError(f"{path}: malformed statistics for {name}")

        # Rows 0 and 2 are integer counts. Only the two kinematic moments; rows 5 and 6 may contain undefined or infinite values
        stats = [numeric_row(row, label, exact=(i in (0, 2)), allow_nonfinite=(i >= 5)) for i, (label, row) in enumerate(zip(STATISTICS, statistics))]

        # A single histogram must use the same number of weight pairs in every row.
        data = []
        widths = {len(row[1]) for row in stats}
        for i, row in enumerate(children["Data"].lines):
            if frequency:
                key = Decimal(row[0])
                if not key.is_finite() or key != key.to_integral_value(): raise ValueError(f"{path}: invalid frequency key for {name}")
                label, values = f"PDG {int(key)}", row[1:]
            else:
                label = "underflow" if i == 0 else f"bin {i}"
                values = row
            _, values, _ = numeric_row(values, f"{path}: {name}, {label}")
            widths.add(len(values))
            data.append((label, values[:2]))
        if len(widths) != 1: raise ValueError(f"{path}: inconsistent weight columns for {name}")
        if frequency:
            if len({label for label, _ in data}) != len(data): raise ValueError(f"{path}: duplicate frequency key for {name}")
        else:
            if len(data) != int(axis[0]) + 2: raise ValueError(f"{path}: inconsistent bin count for {name}")
            data[-1] = ("overflow", data[-1][1])
        histograms.append(dict(name=name, kind=block.tag, axis=axis, regions=frozenset(row[0] for row in regions), stats=stats, data=data))
    return histograms

# SAF statistics row order; rows 0 and 2 contain exact event/entry counts.
STATISTICS = ("nevents", "sum of event-weights over events", "nentries", "sum of event-weights over entries", "sum weights^2", "sum value*weight", "sum value^2*weight")


def extract_bin_values(path: Path) -> List[Tuple[str | None, List[float]]]:
    """Compatibility helper returning names and sums of the two nominal columns.

    These sums are absolute-weight contents, not signed yields. The comparator
    below checks both columns separately; callers of this helper keep its API.
    """
    return [(h["name"], [sum(pair) for _, pair in h["data"]]) for h in read_histograms(path)]


# ---------------------------------------------------------------------------
# Histogram comparison shared by all three drivers
# ---------------------------------------------------------------------------
def compare_histos(file1: Path, file2: Path, max_bins_to_print: int = DEFAULT_MAX_BINS_TO_PRINT) -> Tuple[bool, List[str]]:
    """Compare nominal signs separately, statistics, axes and region sets.

    The public call signature and return value are unchanged. Display limits
    never hide a failure. Nonfinite bins/counts/weights are input errors;
    matching nonfinite kinematic moments are accepted for older references.

    Return (all_ok, reports) without printing; callers choose how to display
    reports. Invalid input raises an exception rather than becoming a mismatch.
    """
    # Initilisation and checks
    first, second = read_histograms(file1), read_histograms(file2)
    reports = []

    # Compare histograms in declaration order, so reordered histograms also fail
    if len(first) != len(second): reports.append(f"Different number of histograms: {len(first)} vs {len(second)}")
    for ref, out in zip(first, second):
        differences = []
        for field in ("name", "kind", "regions"):
            if ref[field] != out[field]: differences.append(f"{field} mismatch: {ref[field]} vs {out[field]}")
        if len(ref["axis"]) != len(out["axis"]) or any(not math.isclose(a, b, rel_tol=REL_TOL, abs_tol=ABS_TOL) for a, b in zip(ref["axis"], out["axis"])):
            differences.append(f"axis mismatch: {ref['axis']} vs {out['axis']}")
        if len(ref["stats"]) != len(out["stats"]): differences.append("different number of statistics rows")

        # Compare the nominal pair even if the produced file has more variations
        rows = []
        for (label, values, exact), (_, other, _) in zip(ref["stats"], out["stats"]): rows.append((label, values[:2], other[:2], exact))
        if [label for label, _ in ref["data"]] != [label for label, _ in out["data"]]: differences.append("bin count/order or frequency keys differ")
        else:
            rows.extend((label, values, other, False) for (label, values), (_, other) in zip(ref["data"], out["data"]))
        for label, values, other, exact in rows:
            for sign, a, b in zip(("positive", "negative"), values, other):
                # Older references can have undefined kinematic moments (e.g. GAMMA for massless particles). Match NaN/inf only in those moments; counts, weights and bins must always be finite
                if label in STATISTICS[5:] and (not math.isfinite(a) or not math.isfinite(b)): equal = (math.isnan(a) and math.isnan(b)) or a == b
                else: equal = a == b if exact else math.isclose(a, b, rel_tol=REL_TOL, abs_tol=ABS_TOL)
                if not equal: differences.append(f"{label}, {sign}: reference={a}, produced={b} (abs difference={abs(a-b):.6g})")
        # Limit printed differences only, including metadata, statistics and bins. Every detected difference contributes to the failure
        if differences:
            lines = [f"  Histogram: {ref['name']}"]
            lines.extend(f"{RED}    {line}{RESET}" for line in differences[:max_bins_to_print])
            if len(differences) > max_bins_to_print: lines.append(f"    ... {len(differences)-max_bins_to_print} more differences")
            reports.append("\n".join(lines))
    return not reports, reports


# ---------------------------------------------------------------------------
# MA5 helpers
# ---------------------------------------------------------------------------
def find_new_analysis_dir(ma5dir: Path, before: Sequence[str]) -> Path:
    """
    Return the newly created ``ANALYSIS_X`` directory.

    The MA5 run creates the first available ``ANALYSIS_X`` directory. We detect
    it by comparing the directory list before and after the run.
    """
    after = {p.name for p in ma5dir.iterdir() if p.is_dir() and p.name.startswith("ANALYSIS_")}
    new_dirs = sorted(after - set(before))
    if len(new_dirs) != 1: raise RuntimeError(f"Could not uniquely identify the new ANALYSIS_X directory. Found new directories: {new_dirs}")
    return ma5dir / new_dirs[0]


def find_dataset_histo_file(analysis_dir: Path) -> Path:
    """
    Find the initial-submission histogram file for exactly one dataset.
    Later submission steps are not inspected; use RegionChecks to compare them.
    """
    saf_dir = analysis_dir / "Output" / "SAF"
    if not saf_dir.is_dir(): raise FileNotFoundError(f"Missing SAF directory: {saf_dir}")

    candidates = []
    for d in sorted(saf_dir.iterdir()):
        if not d.is_dir(): continue
        histo = d / "MadAnalysis5job_0" / "Histograms" / "histos.saf"
        if histo.is_file(): candidates.append(histo)

    if len(candidates) == 0: raise FileNotFoundError(f"No histos.saf found under {saf_dir}")
    if len(candidates) > 1:raise RuntimeError("Ambiguous output: several dataset histogram files found:\n  " + "\n  ".join(str(x) for x in candidates))

    return candidates[0]

def run_ma5_script(name: str, ma5dir: Path) -> Tuple[Path, Path]:
    """Run ``validation/scripts/<name>.ma5`` from ``ma5dir`` and return file paths.

    Returns ``(reference_histo_file, produced_histo_file)``. The reference is
    obtained before launch. Exactly one new job is expected and is left on disk.
    """
    # Is the script existing?
    script_path = ma5dir / "validation/scripts" / f"{name}.ma5"
    if not script_path.exists(): raise FileNotFoundError(f"Missing MA5 script: {script_path}")

    # Is the reference output existing?
    reference_path = ensure_reference_file(name, ma5dir)

    # Run
    before = [p.name for p in ma5dir.iterdir() if p.is_dir() and p.name.startswith("ANALYSIS_")]
    cmd = ["./bin/ma5", "-s", str(script_path.relative_to(ma5dir))]
    result = subprocess.run(cmd, cwd=ma5dir, text=True)
    if result.returncode != 0: raise RuntimeError(f"MA5 execution failed with exit code {result.returncode}")
    analysis_dir = find_new_analysis_dir(ma5dir, before)
    produced_histos = find_dataset_histo_file(analysis_dir)
    if not produced_histos.exists(): raise FileNotFoundError(f"Produced histogram file not found: {produced_histos}")

    return reference_path, produced_histos


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------
def build_parser() -> argparse.ArgumentParser:
    """Create the command-line parser."""
    parser = argparse.ArgumentParser(description="Compare MA5 histogram SAF files.")
    subparsers = parser.add_subparsers(dest="mode", required=True)

    compare_parser = subparsers.add_parser("compare", help="Compare two explicit SAF histogram files.")
    compare_parser.add_argument("reference", type=Path, help="Reference histos.saf file")
    compare_parser.add_argument("produced", type=Path, help="Produced histos.saf file")
    compare_parser.add_argument("--max-bins", type=int, default=DEFAULT_MAX_BINS_TO_PRINT, help="Maximum number of differences printed per histogram.")

    run_parser = subparsers.add_parser("run",
        help=(
            "Run scripts/<name>.ma5 in the given MA5 directory, detect the newly "
            "created ANALYSIS_X directory, and compare its histograms against "
            "outputs/<name>.saf."
        )
    )
    run_parser.add_argument("name", help="Base name of the MA5 script (without .ma5)")
    run_parser.add_argument("--max-bins", type=int, default=DEFAULT_MAX_BINS_TO_PRINT, help="Maximum number of differences printed per histogram.")

    return parser



def main() -> int:
    """Program entry point."""
    parser = build_parser()
    args = parser.parse_args()

    try:
        if args.mode == "compare":
            reference = args.reference.resolve()
            produced = args.produced.resolve()
        elif args.mode == "run":
            ma5dir = Path(__file__).resolve().parent.parent
            reference, produced = run_ma5_script(args.name, ma5dir)
            print(f"{BOLD}Reference:{RESET} {reference}")
            print(f"{BOLD}Produced :{RESET} {produced}")
        else:
            parser.error("Unknown mode")
            return 1

        ok, reports = compare_histos(reference, produced, max_bins_to_print=args.max_bins)

        if ok:
            print(f"{GREEN}No significant histogram differences found.{RESET}")
            return 0

        print(f"{RED}Histogram differences found:{RESET}")
        for report in reports:
            print(report)
            print()
        return 2

    except Exception as exc:  # pragma: no cover - top-level CLI safeguard
        print(f"{RED}Error:{RESET} {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
