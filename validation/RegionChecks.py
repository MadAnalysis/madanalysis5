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
#  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

"""Compare regions, their nominal cutflows and their associated histograms.

Usage:
    python3 RegionChecks.py run NAME
    python3 RegionChecks.py compare REFERENCE_DIRECTORY PRODUCED_DIRECTORY

References default to flat files validation/outputs/NAME_<region>.saf,
NAME_histos.saf and NAME_regions.saf (region declarations). Directories can
still be complete ANALYSIS_X jobs. The run mode downloads the missing
reference files from the repository
  https://github.com/MadAnalysis/validation_data
and the direct comparison remains local. Any script name is accepted, with one
or more datasets, and dcripts using resubmit compare each retained
MadAnalysis5job_N output with NAME_stepN references.

Cutflow parsing and numerical comparisons reuse CutflowChecks; histogram bins
reuse HistoChecks. Region declarations, cutflow filenames and histogram region
associations are also compared. Additional weights, histogram statistics and
axis metadata are not compared.

Exit codes: 0 = agreement, 1 = execution/input error, 2 = comparison mismatch
(argparse also uses 2 for invalid arguments).
"""

from __future__ import annotations

import argparse
import json
import re
import shlex
import subprocess
import sys
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

from CutflowChecks import compare_records, positive_integer, read_blocks, read_cutflows
from HistoChecks import (BOLD, GREEN, RED, RESET, DEFAULT_MAX_BINS_TO_PRINT, REFERENCE_BASE_URL,
                         compare_histos, ensure_reference_file, find_new_analysis_dir)

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
DEFAULT_MAX_DIFFERENCES = DEFAULT_MAX_BINS_TO_PRINT


# ---------------------------------------------------------------------------
# Output discovery: preserve dataset and analyzer identities across branches
# ---------------------------------------------------------------------------
def labelled_reference(reference: Path, path: Path) -> tuple | None:
    """Decode PREFIX_DATASET_MadAnalysis5job_N_regions.saf."""
    stem = path.name[:-len('_regions.saf')]
    tail = stem[len(reference.name) + 1:]
    if '__MadAnalysis5job_' in tail:
        return None
    match = re.fullmatch(r'(.+)_(MadAnalysis5job_\d+)', tail)
    if match is None:
        return None
    return '/'.join(match.groups()), path.with_name(stem)


def find_analyses(directory: Path) -> dict:
    """Accept a job, SAF tree or flat reference prefix.

    A single flat reference has no dataset label and matches one produced analysis.
    Multiple analyses use PREFIX_DATASET_ANALYZER_<suffix>.saf.
    """
    if not directory.is_dir():
        single = directory.with_name(directory.name + "_regions.saf")
        multiple = [labelled_reference(directory, path)
                    for path in sorted(directory.parent.glob(directory.name + "_*_regions.saf"))]
        multiple = [entry for entry in multiple if entry is not None]
        if single.is_file():
            if multiple:
                raise ValueError(f"Mixed single- and multiple-dataset references: {directory}")
            return {"*": directory}
        analyses = {}
        for key, prefix in multiple:
            if key in analyses:
                raise ValueError(f"Duplicate reference for {key}: {directory}")
            analyses[key] = prefix
        if analyses:
            return analyses
        raise FileNotFoundError(f"Missing reference/output directory or flat reference prefix: {directory}")
    saf = directory / "Output" / "SAF"
    if not saf.is_dir():
        saf = directory
    if not saf.is_dir():
        raise FileNotFoundError(f"Missing reference/output directory: {directory}")
    analyses = {path.relative_to(saf).as_posix(): path
                for path in sorted(saf.glob("*/MadAnalysis5job_*")) if path.is_dir()}
    if not analyses:
        raise ValueError(f"No dataset/MadAnalysis5job_* directories found under {saf}")
    return analyses


def analysis_files(analysis: Path) -> tuple:
    """Resolve the same three kinds of SAF output from either storage layout."""
    if analysis.is_dir():
        return (analysis / "MadAnalysis5job.saf",
                {p.stem: p for p in (analysis / "Cutflows").glob("*.saf")},
                analysis / "Histograms" / "histos.saf")
    prefix = analysis.name + "_"
    regions = analysis.with_name(prefix + "regions.saf")
    histos = analysis.with_name(prefix + "histos.saf")
    cutflows = {p.name[len(prefix):-4]: p for p in analysis.parent.glob(prefix + "*.saf")
                if p not in (regions, histos)}
    return regions, cutflows, histos


# ---------------------------------------------------------------------------
# Region declarations and histogram associations, using the shared SAF parser
# ---------------------------------------------------------------------------
def read_regions(analysis: Path) -> list:
    path, _, _ = analysis_files(analysis)
    blocks = read_blocks(path)
    selections = [block for block in blocks if block.tag == "RegionSelection"]
    if len(selections) != 1 or selections[0].children:
        raise ValueError(f"{path}: expected one RegionSelection block")
    rows = selections[0].lines
    if not rows or any(len(row) != 1 for row in rows):
        raise ValueError(f"{path}: expected nonempty region declarations")
    regions = [row[0] for row in rows]
    if len(regions) != len(set(regions)):
        raise ValueError(f"{path}: duplicate region declarations")
    return regions


def read_associations(path: Path) -> list:
    """Return histogram type, name and region set in histogram file order."""
    associations = []
    for block in read_blocks(path):
        if block.tag not in ("Histo", "HistoLogX", "HistoFrequency"):
            raise ValueError(f"{path}: unexpected histogram block {block.tag}")
        descriptions = [child for child in block.children if child.tag == "Description"]
        if len(descriptions) != 1:
            raise ValueError(f"{path}: expected one histogram Description")
        rows = descriptions[0].lines
        # Ordinary and logarithmic histograms have an axis row after the name.
        start = 1 if block.tag == "HistoFrequency" else 2
        if len(rows) < start or len(rows[0]) != 1:
            raise ValueError(f"{path}: malformed histogram Description")
        if any(len(row) != 1 for row in rows[start:]):
            raise ValueError(f"{path}: malformed histogram region list")
        regions = [row[0] for row in rows[start:]]
        if len(regions) != len(set(regions)):
            raise ValueError(f"{path}: duplicate histogram region association")
        associations.append((block.tag, rows[0][0], frozenset(regions)))
    return associations


# ---------------------------------------------------------------------------
# Comparison and coloured reporting
# ---------------------------------------------------------------------------
def report_differences(label: str, differences: list, limit: int) -> bool:
    color = RED if differences else GREEN
    print(f"{color}{label}: {'FAIL' if differences else 'PASS'}{RESET}")
    for difference in differences[:limit]:
        print(f"{RED}  {difference}{RESET}")
    if len(differences) > limit:
        print(f"{RED}  ... {len(differences) - limit} more differences{RESET}")
    return not differences


def compare_analysis(reference: Path, produced: Path, max_differences: int) -> bool:
    ref_regions, out_regions = read_regions(reference), read_regions(produced)
    differences = []
    if ref_regions != out_regions:
        differences.append(f"Region names/order: {ref_regions} vs {out_regions}")
    _, ref_files, ref_histos = analysis_files(reference)
    _, out_files, out_histos = analysis_files(produced)
    for label, regions, files in (("Reference", ref_regions, ref_files),
                                  ("Produced", out_regions, out_files)):
        missing, extra = set(regions) - files.keys(), files.keys() - set(regions)
        if missing:
            differences.append(f"{label}: missing cutflow files for {sorted(missing)}")
        if extra:
            differences.append(f"{label}: unexpected cutflow files for {sorted(extra)}")
    all_ok = report_differences("Regions", differences, max_differences)

    # Compare each region separately, including counters after complete rejection.
    for name in sorted(ref_files.keys() & out_files.keys()):
        differences = compare_records(read_cutflows(ref_files[name]), read_cutflows(out_files[name]))
        ok = report_differences(f"Cutflow {name}", differences, max_differences)
        all_ok = all_ok and ok

    ref_links, out_links = read_associations(ref_histos), read_associations(out_histos)
    differences = []
    if len(ref_links) != len(out_links):
        differences.append(f"Histogram count: {len(ref_links)} vs {len(out_links)}")
    for ref, out in zip(ref_links, out_links):
        if ref[:2] != out[:2]:
            differences.append(f"Histogram type/name/order: {ref[:2]} vs {out[:2]}")
        if ref[2] != out[2]:
            differences.append(f"{ref[1]} regions: {sorted(ref[2])} vs {sorted(out[2])}")
    for label, links, regions in (("Reference", ref_links, ref_regions),
                                  ("Produced", out_links, out_regions)):
        for _, name, associated in links:
            unknown = associated - set(regions)
            if unknown:
                differences.append(f"{label}: {name} uses undeclared regions {sorted(unknown)}")
    ok = report_differences("Histogram region associations", differences, max_differences)
    all_ok = all_ok and ok

    # Reuse the existing bin comparison, including its tolerances and colours.
    ok, reports = compare_histos(ref_histos, out_histos, max_bins_to_print=max_differences)
    color = GREEN if ok else RED
    print(f"{color}Histogram bins: {'PASS' if ok else 'FAIL'}{RESET}")
    for report in reports:
        print(report)
    return all_ok and ok


def compare_analyses(ref: dict, out: dict, max_differences: int) -> bool:
    """Compare one set of dataset/analyzer outputs, possibly a single step."""
    # Unlabelled flat references are only meaningful for a single analysis.
    if "*" in ref and len(out) == 1:
        ref = {next(iter(out)): ref["*"]}
    if "*" in out and len(ref) == 1:
        out = {next(iter(ref)): out["*"]}
    differences = []
    missing, extra = ref.keys() - out.keys(), out.keys() - ref.keys()
    if missing:
        differences.append(f"Missing dataset/analyzer outputs: {sorted(missing)}")
    if extra:
        differences.append(f"Unexpected dataset/analyzer outputs: {sorted(extra)}")
    all_ok = report_differences("Datasets/analyses", differences, max_differences)
    for key in sorted(ref.keys() & out.keys()):
        print(f"\n{BOLD}{key}{RESET}")
        ok = compare_analysis(ref[key], out[key], max_differences)
        all_ok = all_ok and ok
    return all_ok


# ---------------------------------------------------------------------------
# Submission steps: MA5 keeps each execution in MadAnalysis5job_N
# ---------------------------------------------------------------------------
def submission_count(script: Path) -> int:
    """Count submit/resubmit commands in the validation script, ignoring comments."""
    commands = []
    for line in script.read_text().replace('\\\n', '').splitlines():
        lexer = shlex.shlex(line, posix=True, punctuation_chars=';')
        lexer.whitespace_split = True
        words = list(lexer)
        if not words or words[0] == 'shell' or words[0].startswith('!'):
            continue
        first = True
        for word in words:
            if word and set(word) == {';'}:
                first = True
            elif first:
                if word in ('submit', 'resubmit'):
                    commands.append(word)
                first = False
    if not commands or commands[0] != 'submit' or commands.count('submit') != 1:
        raise ValueError(f"{script}: expected one submit followed by zero or more resubmit commands")
    return len(commands)


def output_steps(analyses: dict) -> dict:
    steps = {}
    for key, path in analyses.items():
        match = re.fullmatch(r'.+/MadAnalysis5job_(\d+)', key)
        if match is None:
            raise ValueError(f"Cannot identify submission step from {key}")
        steps.setdefault(int(match.group(1)), {})[key] = path
    return steps


def reference_steps(reference: Path) -> dict:
    """Find flat step prefixes independently of the produced results."""
    if reference.is_dir():
        return {}
    pattern = re.compile(re.escape(reference.name) + r'_step(\d+)(?:_.*)?_regions\.saf')
    steps = {}
    for path in reference.parent.glob(reference.name + '_step*_regions.saf'):
        match = pattern.fullmatch(path.name)
        if match:
            number = int(match.group(1))
            steps[number] = reference.with_name(reference.name + f'_step{number}')
    return steps


def compare_outputs(reference: Path, produced: Path,
                    max_differences: int = DEFAULT_MAX_DIFFERENCES) -> bool:
    print(f"{BOLD}Reference:{RESET} {reference}*.saf\n{BOLD}Produced :{RESET} {produced}")
    steps = reference_steps(reference)
    if not steps:
        return compare_analyses(find_analyses(reference), find_analyses(produced), max_differences)
    if sorted(steps) != list(range(max(steps) + 1)):
        raise ValueError(f"{reference}: reference steps must start at 0 and be consecutive")
    outputs = output_steps(find_analyses(produced))
    differences = []
    if steps.keys() - outputs.keys():
        differences.append(f"Missing submission steps: {sorted(steps.keys() - outputs.keys())}")
    if outputs.keys() - steps.keys():
        differences.append(f"Unexpected submission steps: {sorted(outputs.keys() - steps.keys())}")
    all_ok = report_differences('Submission steps', differences, max_differences)
    for number in sorted(steps.keys() & outputs.keys()):
        print(f"\n{BOLD}Step {number}{RESET}")
        ok = compare_analyses(find_analyses(steps[number]), outputs[number], max_differences)
        all_ok = all_ok and ok
    return all_ok


# ---------------------------------------------------------------------------
# MA5 execution: references remain independent of script filenames
# ---------------------------------------------------------------------------
def ensure_region_references(reference: Path, ma5dir: Path) -> None:
    """Fetch missing flat SAF references without replacing existing files.

    Region declarations determine the required cutflow filenames, independently
    of the produced output. On a fresh multi-dataset test, GitHub's directory
    listing supplies the labelled reference prefixes.
    """
    if reference.is_dir():
        return

    def fetch(name):
        return ensure_reference_file(name, ma5dir, output_dir=reference.parent)

    # Existing declaration files already identify the reference analyses.
    try:
        analyses = find_analyses(reference)
    except FileNotFoundError:
        try:
            fetch(reference.name + "_regions")
        except FileNotFoundError as missing:
            # Only a remote 404 triggers discovery of multi-dataset references.
            if not isinstance(missing.__cause__, urllib.error.HTTPError) or missing.__cause__.code != 404:
                raise
            owner, repo, branch, *subdir = urllib.parse.urlparse(REFERENCE_BASE_URL).path.strip('/').split('/')
            index = f"https://api.github.com/repos/{owner}/{repo}/contents/" + '/'.join(subdir)
            index += '?' + urllib.parse.urlencode({'ref': branch})
            try:
                with urllib.request.urlopen(index) as response:
                    entries = json.load(response)
            except urllib.error.URLError as exc:
                raise RuntimeError(f"Could not list region references: {index}: {exc}") from exc
            names = sorted(entry['name'][:-len('.saf.gz')] for entry in entries
                           if entry.get('type') == 'file'
                           and entry['name'].startswith(reference.name + '_')
                           and entry['name'].endswith('_regions.saf.gz')
                           and labelled_reference(reference, Path(entry['name'][:-3])) is not None)
            if not names:
                raise missing
            for name in names:
                fetch(name)
        analyses = find_analyses(reference)

    # Fetch the declaration's cutflows, including regions missing from a bad run.
    for prefix in analyses.values():
        for region in read_regions(prefix):
            fetch(prefix.name + '_' + region)
        fetch(prefix.name + '_histos')


def run_ma5_script(name: str, ma5dir: Path, reference: Path | None = None) -> tuple:
    script = ma5dir / "validation" / "scripts" / f"{name}.ma5"
    if not script.is_file():
        raise FileNotFoundError(f"Missing MA5 script: {script}")
    reference = reference if reference is not None else ma5dir / "validation" / "outputs" / name
    # Each resubmission needs its own reference set; single submissions keep their names.
    count = submission_count(script)
    references = [reference]
    if count > 1 and not reference.is_dir():
        references = [reference.with_name(reference.name + f'_step{i}') for i in range(count)]
    analyses = []
    for prefix in references:
        ensure_region_references(prefix, ma5dir)
        analyses.extend(find_analyses(prefix).values())
    if reference.is_dir() and set(output_steps(find_analyses(reference))) != set(range(count)):
        raise ValueError(f"{reference}: expected {count} reference submission steps")
    # Check reference completeness before launching MA5.
    for analysis in analyses:
        regions = read_regions(analysis)
        _, files, histos = analysis_files(analysis)
        if set(regions) != files.keys():
            raise ValueError(f"{analysis}: reference cutflow files do not match declared regions")
        for path in files.values():
            read_cutflows(path)
        read_associations(histos)
    before = [p.name for p in ma5dir.iterdir() if p.is_dir() and p.name.startswith("ANALYSIS_")]
    command = ["./bin/ma5", "-s", str(script.relative_to(ma5dir))]
    result = subprocess.run(command, cwd=ma5dir)
    if result.returncode:
        raise RuntimeError(f"MA5 execution failed with exit code {result.returncode}")
    return reference, find_new_analysis_dir(ma5dir, before)


# ---------------------------------------------------------------------------
# Command-line interface
# ---------------------------------------------------------------------------
def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    modes = parser.add_subparsers(dest="mode", required=True)
    run = modes.add_parser("run", help="Download missing references, run one script and compare all datasets and regions.")
    run.add_argument("name", help="Validation script name, without .ma5.")
    run.add_argument("--reference", type=Path, help="Flat reference prefix, job or SAF tree; default: validation/outputs/NAME.")
    compare = modes.add_parser("compare", help="Compare flat references, existing jobs or SAF trees.")
    compare.add_argument("reference", type=Path)
    compare.add_argument("produced", type=Path)
    for mode in (run, compare):
        mode.add_argument("--max-differences", type=positive_integer, default=DEFAULT_MAX_DIFFERENCES,
                          help="Maximum differences per check, or bins per histogram (default: 12).")
    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    try:
        reference = args.reference.resolve() if args.reference is not None else None
        if args.mode == "run":
            reference, produced = run_ma5_script(
                args.name, Path(__file__).resolve().parent.parent, reference)
        else:
            produced = args.produced.resolve()
        ok = compare_outputs(reference, produced, args.max_differences)
        color = GREEN if ok else RED
        message = "Nominal region outputs agree." if ok else "Validation differences found."
        print(f"{color}{message}{RESET}")
        return 0 if ok else 2
    except (OSError, ValueError, RuntimeError) as exc:
        print(f"{RED}Error: {exc}{RESET}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
