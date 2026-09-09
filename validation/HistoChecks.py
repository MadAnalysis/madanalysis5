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

1. Direct comparison mode
   Compare a reference ``histos.saf`` file against a produced histogram file.

   Usage:
       python HistoChecks.py compare <reference.saf> <produced.saf>

2. Run-and-compare mode
   Given a validation name ``NAME``
   - check that ``scripts/NAME.ma5`` exists,
   - run ``./bin/ma5 -s scripts/NAME.ma5`` from the MA5 home directory,
   - detect the newly created ``ANALYSIS_X`` directory,
   - compare its histogram output against ``outputs/NAME.saf``.
   - if the output file is not available, it is downloaded from github

   Usage:
       python HistoChecks.py run <name>

The comparison is based on the *bin content* only. For each line of a SAF
<Data> block, the bin content is defined as the sum of the first two columns,
which correspond to the sum of the positive-weight and negative-weight
contributions.
"""

from __future__ import annotations

import argparse
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
#  Ensure the reference output SAF filesexists locally
# ---------------------------------------------------------------------------
def ensure_reference_file(name: str, ma5dir: Path) -> Path:
    """Ensure validation/outputs/<name>.saf exists locally.

    If missing, download <name>.saf.gz from the validation_data GitHub repo
    and decompress it locally.
    """

    # Init and safety
    local_dir = ma5dir / "validation" / "outputs"
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
        raise FileNotFoundError(
            f"Could not download reference file: {remote_gz_url} (HTTP {exc.code})"
        ) from exc
    except urllib.error.URLError as exc:
        raise RuntimeError(
            f"Failed to download reference file: {remote_gz_url} ({exc})"
        ) from exc

    # Unzipping
    print(f"{BOLD}Decompressing:{RESET} {local_gz.name}")
    with gzip.open(local_gz, "rb") as fin, open(local_saf, "wb") as fout:
        shutil.copyfileobj(fin, fout)
        local_gz.unlink(missing_ok=True)

    if not local_saf.exists():
        raise FileNotFoundError(f"Decompression failed, missing file: {local_saf}")

    return local_saf

# ---------------------------------------------------------------------------
# SAF parsing
# ---------------------------------------------------------------------------
def extract_bin_values(path: Path) -> List[Tuple[str | None, List[float]]]:
    """Extract histogram names and bin contents from a SAF histogram file.

    Each histogram is represented by a tuple ``(name, bins)`` where ``bins`` is
    the list of bin contents. The bin content is defined as
    ``positive_weight + negative_weight`` from the first two columns of each
    line in a ``<Data>`` block.
    """
    histos: List[Tuple[str | None, List[float]]] = []
    current_name: str | None = None
    in_description = False
    in_data = False
    current_bins: List[float] = []

    with path.open() as handle:
        for raw_line in handle:
            line = raw_line.strip()

            if line == "<Description>":
                in_description = True
                current_name = None
                continue
            if line == "</Description>":
                in_description = False
                continue
            if line == "<Data>":
                in_data = True
                current_bins = []
                continue
            if line == "</Data>":
                in_data = False
                histos.append((current_name, current_bins))
                continue

            if in_description and current_name is None:
                if line.startswith('"') and line.endswith('"'):
                    current_name = line.strip('"')
                continue

            if in_data:
                # Remove inline comments.
                if "#" in line:
                    line = line.split("#", 1)[0].strip()

                # Safety
                if not line: continue
                parts = line.split()
                if len(parts) < 2: continue

                # Data
                positive = float(parts[0])
                negative = float(parts[1])
                current_bins.append(positive + negative)

    return histos


# ---------------------------------------------------------------------------
# Histogram comparison
# ---------------------------------------------------------------------------
def compare_histos( file1: Path, file2: Path, max_bins_to_print: int = DEFAULT_MAX_BINS_TO_PRINT) -> Tuple[bool, List[str]]:
    """Compare two SAF histogram files.

    Returns ``(all_ok, reports)`` where ``reports`` only contains human-readable
    blocks for histograms with significant differences.
    """

    # Data massaging
    histos1 = extract_bin_values(file1)
    histos2 = extract_bin_values(file2)

    # Safety
    if len(histos1) != len(histos2): return False, [ f"Different number of histograms: {len(histos1)} vs {len(histos2)}" ]

    # Initialisation
    reports: List[str] = []
    all_ok = True

    # Main method
    for (name1, bins1), (name2, bins2) in zip(histos1, histos2):
        if name1 != name2:
            reports.append(f'Histogram name mismatch: "{name1}" vs "{name2}"')
            all_ok = False
            continue

        if len(bins1) != len(bins2):
            reports.append(f'Histogram "{name1}": different number of bins: {len(bins1)} vs {len(bins2)}')
            all_ok = False
            continue

        diffs: List[Tuple[int, float, float, float, float]] = []
        significant_diffs: List[Tuple[int, float, float, float, float]] = []
        max_abs = 0.0
        max_rel = 0.0
        worst_bin = -1

        for ibin, (value1, value2) in enumerate(zip(bins1, bins2)):
            abs_diff = abs(value1 - value2)
            rel_diff = abs_diff / max(abs(value1), abs(value2), 1e-300)

            if abs_diff > max_abs:
                max_abs = abs_diff
                max_rel = rel_diff
                worst_bin = ibin

            if not math.isclose(value1, value2, rel_tol=REL_TOL, abs_tol=ABS_TOL): diffs.append((ibin, value1, value2, abs_diff, rel_diff))
            if abs_diff > ABS_TOL and rel_diff > REL_TOL: significant_diffs.append((ibin, value1, value2, abs_diff, rel_diff))

        # Only report histograms with at least one *significant* differing bin.
        if significant_diffs:
            all_ok = False

            lines: List[str] = []
            lines.append(f"  Histogram: {name1}")
            lines.append(f"    differing bins : {len(diffs)}")
            lines.append(f"    worst bin      : {worst_bin}")
            lines.append(f"    max abs diff   : {max_abs:.3e}")
            lines.append(f"    max rel diff   : {max_rel:.3e}")
            lines.append(f"    shown bins     : {min(len(significant_diffs), max_bins_to_print)}/{len(significant_diffs)}")

            for ibin, value1, value2, abs_diff, rel_diff in significant_diffs[:max_bins_to_print]:
                if rel_diff > THRESHOLD_RED: color = RED
                elif rel_diff > THRESHOLD_ORANGE: color = ORANGE
                else: color = RESET

                if ibin == 0: label = "underflow"
                elif ibin == len(bins1) - 1: label = "overflow "
                else: label = f"bin {ibin:4d}"

                lines.append(f"{color}      {label}: {value1:.6e} vs {value2:.6e} (abs={abs_diff:.2e}, rel={rel_diff:.2e}){RESET}")

            if len(significant_diffs) > max_bins_to_print: lines.append(f"      ... {len(significant_diffs) - max_bins_to_print} more differing bins")

            reports.append("\n".join(lines))

    return all_ok, reports


# ---------------------------------------------------------------------------
# MA5 helpers
# ---------------------------------------------------------------------------
def find_new_analysis_dir(ma5dir: Path, before: Sequence[str]) -> Path:
    """Return the newly created ``ANALYSIS_X`` directory.

    The MA5 run creates the first available ``ANALYSIS_X`` directory. We detect
    it by comparing the directory list before and after the run.
    """
    after = {p.name for p in ma5dir.iterdir() if p.is_dir() and p.name.startswith("ANALYSIS_")}
    new_dirs = sorted(after - set(before))
    if len(new_dirs) != 1:
        raise RuntimeError(
            "Could not uniquely identify the new ANALYSIS_X directory. "
            f"Found new directories: {new_dirs}"
        )
    return ma5dir / new_dirs[0]


def find_dataset_histo_file(analysis_dir: Path) -> Path:
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
    """Run ``scripts/<name>.ma5`` inside ``ma5dir`` and return file paths.

    Returns ``(reference_histo_file, produced_histo_file)``.
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
    compare_parser.add_argument("--max-bins", type=int, default=DEFAULT_MAX_BINS_TO_PRINT, help="Maximum number of differing bins printed per histogram.")

    run_parser = subparsers.add_parser("run",
        help=(
            "Run scripts/<name>.ma5 in the given MA5 directory, detect the newly "
            "created ANALYSIS_X directory, and compare its histograms against "
            "outputs/<name>.saf."
        )
    )
    run_parser.add_argument("name", help="Base name of the MA5 script (without .ma5)")
    run_parser.add_argument("--max-bins", type=int, default=DEFAULT_MAX_BINS_TO_PRINT, help="Maximum number of differing bins printed per histogram.")

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
            ma5dir = Path.cwd().parent.resolve()
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
