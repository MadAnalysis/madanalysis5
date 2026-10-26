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

"""
Shared SAF block parsing and signed numeric rows for validation drivers.

This validation-only reader accepts explicit file paths and raises errors for
malformed input. It is independent of MA5's report reader in job_reader.py.
Histogram, cutflow and region interpretation remains in the respective checkers.
Numeric rows retain all weight pairs; the checkers compare the nominal positive
and negative contributions separately against the reference.
"""
from __future__ import annotations

import math
import re
import shlex
from dataclasses import dataclass, field
from decimal import Decimal, InvalidOperation
from pathlib import Path


# ---------------------------------------------------------------------------
# SAF block structure and file parsing
# ---------------------------------------------------------------------------
@dataclass
class Block:
    """One SAF tag, its tokenised text rows and its ordered child blocks"""
    tag: str
    lines: list = field(default_factory=list)
    children: list = field(default_factory=list)


def read_blocks(path: Path) -> list:
    """
    Return the ordered payload blocks of an uncompressed SAF file.

     Header/footer blocks are checked and omitted from the result. Rows remain
     lists of string tokens for the caller to interpret as histogram, counter
     or region data. Unbalanced tags, missing wrappers and text outside blocks
     raise ValueError.
    """
    root = Block("root")
    stack = [root]
    with path.open() as handle:
        for number, raw in enumerate(handle, 1):
            line = raw.strip()
            if not line or line.startswith("#"): continue

            # Only a standalone tag changes nesting; a quoted cut may contain < or >.
            tag = re.fullmatch(r"<(/?)(\w+)>\s*(?:#.*)?", line)
            if tag:
                closing, name = tag.groups()
                if closing:
                    if len(stack) == 1 or stack[-1].tag != name: raise ValueError(f"{path}:{number}: unmatched closing tag {name}")
                    stack.pop()
                else:
                    block = Block(name)
                    stack[-1].children.append(block)
                    stack.append(block)
            # Preserve quoted names/expressions while removing unquoted comments.
            else:
                tokens = shlex.split(line, comments=True)
                if len(stack) == 1: raise ValueError(f"{path}:{number}: content outside a SAF block")
                if tokens: stack[-1].lines.append(tokens)
    if len(stack) != 1: raise ValueError(f"{path}: unclosed <{stack[-1].tag}> block")
    tags = [block.tag for block in root.children]
    if len(tags) < 3 or tags[0] != "SAFheader" or tags[-1] != "SAFfooter": raise ValueError(f"{path}: expected a complete, nonempty SAF file")
    return root.children[1:-1]


# ---------------------------------------------------------------------------
# Numeric rows: positive/negative pairs for the nominal and variation weights
# ---------------------------------------------------------------------------
def numeric_row(tokens: list, label: str, exact: bool = False, allow_nonfinite: bool = False) -> tuple:
    """
    Return (label, values, exact) for a row of positive/negative pairs.

    The first pair is nominal; later pairs are variations. Preserve the stored
    signs and all columns. With exact=True, require nonnegative integer counts
    and parse via Decimal to avoid rounding large integers through float.
    allow_nonfinite is reserved for histogram kinematic moments; weight sums,
    counts and bins must be finite. Malformed rows raise ValueError.
    """

    if len(tokens) < 2 or len(tokens) % 2: raise ValueError(f"{label}: expected positive/negative weight pairs")
    values = []
    for token in tokens:
        if exact:
            try: number = Decimal(token)
            except InvalidOperation as exc: raise ValueError(f"{label}: invalid count {token!r}") from exc
            if not number.is_finite() or number < 0 or number != number.to_integral_value(): raise ValueError(f"{label}: invalid count {token!r}")
            values.append(int(number))
        else:
            number = float(token)
            if not allow_nonfinite and not math.isfinite(number): raise ValueError(f"{label}: nonfinite value {token!r}")
            values.append(number)
    return label, tuple(values), exact

