"""CLI integration, persisted contract and independent analytic oracle."""
import copy
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from jsonschema import Draft202012Validator

ROOT = Path(__file__).resolve().parents[1]
EXE = Path(sys.argv.pop(1)).resolve()

class OfflineTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.run_demo = lambda: subprocess.run([str(EXE), self.temp.name], capture_output=True, text=True)
        result = self.run_demo()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.output = json.loads(result.stdout)
        self.validator = Draft202012Validator(json.loads((ROOT/'libs/contracts/draft.schema.json').read_text()))

    def test_persisted_contract_and_oracle(self):
        self.validator.validate(self.output)
        disk = json.loads((Path(self.temp.name)/'JOB-SYNTHETIC-001/draft.json').read_text())
        self.assertEqual(self.output, disk)
        oracle = json.loads((ROOT/'fixtures/expected/straight-line.json').read_text())
        self.assertEqual(len(disk['points_m']),len(oracle['points_m']))
        for actual, expected in zip(disk['points_m'],oracle['points_m']):
            for a,e in zip(actual,expected): self.assertAlmostEqual(a,e,places=12)
        self.assertNotEqual(disk['travel_speed_m_s'],disk['wire_feed_speed_m_s'])

    def test_idempotent_rerun(self):
        again=self.run_demo()
        self.assertEqual(again.returncode,0,again.stderr)
        self.assertEqual(json.loads(again.stdout),self.output)

    def test_modified_draft_refused(self):
        file=Path(self.temp.name)/'JOB-SYNTHETIC-001/draft.json'
        file.write_text('tampered')
        self.assertNotEqual(self.run_demo().returncode,0)
        self.assertEqual(file.read_text(),'tampered')

    def test_schema_refuses_release_and_bad_units(self):
        for field,value in [('release_eligible',True),('synthetic',False),('units','mm'),('travel_speed_m_s',0),('wire_feed_speed_m_s',1),('job_id','../bad')]:
            with self.subTest(field=field):
                bad=copy.deepcopy(self.output);bad[field]=value
                self.assertTrue(list(self.validator.iter_errors(bad)))

if __name__ == '__main__': unittest.main()
