"""Process-level tests. Expected dimensions are transcribed from drawing TC-01."""
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

PROBE = Path(sys.argv.pop(1)).resolve()
FIXTURES = Path(__file__).resolve().parents[2] / "fixtures"
STEP = FIXTURES / "cad/t-coupon.step"
ORACLE = FIXTURES / "expected/t-coupon.json"


def inspect(path):
    return subprocess.run([str(PROBE), str(path)], capture_output=True, text=True, timeout=15)


class StepDiagnostics(unittest.TestCase):
    def test_drawing_and_occurrences(self):
        run = inspect(STEP)
        self.assertEqual(run.returncode, 0, run.stderr)
        report = json.loads(run.stdout)
        oracle = json.loads(ORACLE.read_text())
        self.assertIs(report["release_eligible"], False)
        self.assertEqual(report["source_sha256"], hashlib.sha256(STEP.read_bytes()).hexdigest())
        self.assertEqual(report["file_length_unit"], "mm")
        self.assertEqual(report["root_count"], oracle["root_count"])
        actual = sorted(report["occurrences"], key=lambda item: item["min_m"][0])
        expected = oracle["occurrences"]
        self.assertEqual(len(actual), len(expected))
        self.assertEqual(len({tuple(item["occurrence_path"]) for item in actual}), len(expected))
        definitions = {}
        for item, ref in zip(actual, expected):
            part = ref["part"]
            self.assertEqual(len(item["occurrence_path"]), ref["depth"])
            self.assertEqual(definitions.setdefault(part, item["definition_id"]), item["definition_id"])
            for key, ref_key in (("min_m", "min_mm"), ("max_m", "max_mm"), ("translation_m", "datum_mm")):
                self.assertEqual(len(item[key]), 3)
                for measured, mm in zip(item[key], ref[ref_key]):
                    self.assertAlmostEqual(measured, mm / 1000, delta=1e-9)
            for row in range(3):
                for col in range(3):
                    value = ref["rotation_diagonal"][row] if row == col else 0
                    self.assertAlmostEqual(item["rotation"][row][col], value, delta=1e-12)
        self.assertEqual(len(set(definitions.values())), 2)

    def refused(self, data, reason):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "coupon.step"
            if data is not None:
                path.write_bytes(data)
            run = inspect(path)
            self.assertEqual(run.returncode, 1, run.stderr)
            self.assertEqual(run.stdout, "")
            self.assertIn(reason, run.stderr)

    def test_missing(self):
        self.refused(None, "input_unavailable")

    def test_corrupt(self):
        self.refused(b"invalid exchange file\nEND-ISO-10303-21;", "step_read_failed")

    def test_truncated(self):
        self.refused(STEP.read_bytes().split(b"END-ISO-10303-21;")[0], "incomplete_step_envelope")

    def test_missing_length_in_one_context(self):
        data = STEP.read_bytes()
        self.assertIn(b"(#24,#25,#26)", data)
        self.refused(data.replace(b"(#24,#25,#26)", b"(#25,#26)", 1),
                     "expected_millimetre_context")

    def test_oversize(self):
        self.refused(b" " * (8 * 1024 * 1024 + 1), "input_too_large")

    def test_unsupported_units(self):
        self.refused(STEP.read_bytes().replace(b".MILLI.,.METRE.", b".CENTI.,.METRE."),
                     "expected_millimetre_context")

    def test_mixed_units(self):
        self.refused(STEP.read_bytes().replace(b".MILLI.,.METRE.", b".CENTI.,.METRE.", 1),
                     "expected_millimetre_context")


if __name__ == "__main__":
    unittest.main()
