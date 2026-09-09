# python
import os
import sys
import xml.etree.ElementTree as ET

import pytest

# Absolute path to the directory where THIS script lives
script_dir = os.path.dirname(os.path.abspath(__file__))

# Build the absolute path to ../../tools/ReportGenerator/Services
relative_path = os.path.join(
    script_dir, "..", "..", "tools", "ReportGenerator", "Services"
)
absolute_path = os.path.abspath(relative_path)

# Add to sys.path
sys.path.append(absolute_path)

from .run_recast import RunRecast


class _DummyRecasting:
    def __init__(self):
        self.TACO_output = ""
        self.ma5tune = False
        self.delphes = False
        self.global_likelihoods_switch = False
        self.systematics = []
        self.extrapolated_luminosities = []
        self.store_events = False
        self.store_root = False
        self.stat_only_mode = False
        self.analysis_only_mode = False
        self.error_extrapolation = "sqrt"


class _DummyArchi:
    def __init__(self, ma5dir):
        self.ma5dir = str(ma5dir)
        self.has_fastjet = False
        self.delphes_inc_paths = []
        self.has_zlib = False


class _DummySession:
    def __init__(self):
        self.editor = "vi"
        self.has_simplify = False


class _DummyMain:
    def __init__(self, ma5dir):
        self.forced = False
        self.recasting = _DummyRecasting()
        self.archi_info = _DummyArchi(ma5dir)
        self.session_info = _DummySession()
        self.developer_mode = False
        # placeholders referenced by code but not used in tests
        class _FS:
            pass

        self.fastsim = _FS()
        self.superfastsim = _FS()
        self.datasets = []
        self.script = False


def test_fix_pileup_success(tmp_path):
    # Setup directories
    ma5dir = tmp_path / "ma5"
    pad_pileup_dir = ma5dir / "tools" / "PAD" / "Input" / "Pileup"
    pad_pileup_dir.mkdir(parents=True, exist_ok=True)

    # Create pileup file in expected new path
    pileup_file = pad_pileup_dir / "pileup1.root"
    pileup_file.write_text("dummy pileup content")

    # Create a delphes card file with a set PileUpFile line referencing an external path
    card_file = tmp_path / "card.tcl"
    card_content = (
        "# some header\n" " set PileUpFile /old/location/pileup1.root\n" " other stuff\n"
    )
    card_file.write_text(card_content)

    main = _DummyMain(ma5dir)
    rc = RunRecast(main, str(tmp_path))
    # default detector -> not delphesMA5tune -> use PAD path
    rc.detector = "delphes"
    # call fix_pileup
    ok = rc.fix_pileup(str(card_file))
    assert ok is True

    # original saved and new file updated
    assert (str(card_file) + ".original") and os.path.exists(str(card_file) + ".original")
    new_text = card_file.read_text()
    assert "PileUpFile" in new_text
    # new path should point to our pileup file basename replaced into newpath
    assert "Pileup/pileup1.root" in new_text


def test_fix_pileup_missing_pileup_returns_false(tmp_path):
    # Setup ma5dir but do NOT create pileup file
    ma5dir = tmp_path / "ma5"
    pad_pileup_dir = ma5dir / "tools" / "PAD" / "Input" / "Pileup"
    pad_pileup_dir.mkdir(parents=True, exist_ok=True)

    # Create a delphes card file referencing a pileup that does not exist
    card_file = tmp_path / "card2.tcl"
    card_content = " set PileUpFile /somewhere/missing.root\n"
    card_file.write_text(card_content)

    main = _DummyMain(ma5dir)
    rc = RunRecast(main, str(tmp_path))
    rc.detector = "delphes"
    ok = rc.fix_pileup(str(card_file))
    assert ok is False


def test_check_xml_scipy_methods_returns_et_module(tmp_path):
    main = _DummyMain(tmp_path)
    rc = RunRecast(main, str(tmp_path))
    ET_module = rc.check_xml_scipy_methods()
    # Should return a module (either lxml.etree or xml.etree.ElementTree)
    assert ET_module is not False
    # Ensure parse works on a simple xml string via a temp file
    xml_file = tmp_path / "tmp.xml"
    xml_file.write_text("<root><child>1</child></root>")
    parsed = ET_module.parse(str(xml_file))
    root = parsed.getroot()
    assert root.tag == "root"
    assert root.find("child").text == "1"


def test_parse_info_file_and_header_info_file(tmp_path):
    # Create pad analyzer info file structure
    pad = tmp_path / "pad"
    ana_dir = pad / "Build" / "SampleAnalyzer" / "User" / "Analyzer"
    ana_dir.mkdir(parents=True, exist_ok=True)

    info_path = ana_dir / "testana.info"
    # simple info xml content with one region
    info_xml = (
        '<analysis id="testana">\n'
        "  <lumi>36.1</lumi>\n"
        '  <region id="SR1">\n'
        "    <nobs>10</nobs>\n"
        "    <nb>8</nb>\n"
        "    <deltanb>1</deltanb>\n"
        "  </region>\n"
        "</analysis>\n"
    )
    info_path.write_text(info_xml)

    main = _DummyMain(tmp_path)
    rc = RunRecast(main, str(tmp_path))
    # set rc.pad to our pad directory
    rc.pad = str(pad)

    lumi, regions, regiondata = rc.parse_info_file(ET, "testana", "default")
    assert isinstance(lumi, float)
    assert abs(lumi - 36.1) < 1e-6
    assert regions == ["SR1"]
    assert "SR1" in regiondata
    # header_info_file stores scaled nobs/nb according to lumi_scaling (default -> unchanged)
    assert regiondata["SR1"]["nobs"] == pytest.approx(10.0)
    assert regiondata["SR1"]["nb"] == pytest.approx(8.0)
    assert regiondata["SR1"]["deltanb"] == pytest.approx(1.0)  # python
