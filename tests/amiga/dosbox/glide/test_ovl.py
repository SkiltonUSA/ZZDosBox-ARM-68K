"""Guest ABI generation checks; no game files required."""
import importlib.util
from pathlib import Path
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[4]
spec=importlib.util.spec_from_file_location('ovl',ROOT/'scripts/build_dosbox_glide_ovl.py')
ovl=importlib.util.module_from_spec(spec);spec.loader.exec_module(ovl)
class OvlTests(unittest.TestCase):
 def test_layout(self):
  calls=ovl.read_api(ROOT/'amiga/dosbox/glide/abi/glide2x.api')
  names={c['name']:c for c in calls}
  self.assertEqual(len(calls),146)
  self.assertEqual(names['grDrawTriangle']['bytes'],12)
  self.assertEqual(names['guTexAllocateMemory']['bytes'],60)
  self.assertEqual(names['ConvertAndDownloadRle']['bytes'],64)
  self.assertEqual(names['grGlideInit']['bytes'],0)
  self.assertEqual(names['guFogTableIndexToW']['return'],'float')
 def test_invalid_api(self):
  with tempfile.TemporaryDirectory() as d:
   p=Path(d)/'api';p.write_text('bad | void | double x | 1 | | stub\n')
   with self.assertRaises(ValueError):ovl.read_api(p)
 def test_export_names(self):
  c=ovl.read_api(ROOT/'amiga/dosbox/glide/abi/glide2x.api')
  asm,link,header=ovl.generate(c)
  self.assertRegex(link,r"export '_GRDRAWTRIANGLE@12'=zz_[0-9]+")
  self.assertIn('ret 64',asm)
  self.assertIn('fldz',asm) # float-return ABI even when renderer absent
  self.assertIn('lea ecx,[esp+4]',asm)
  self.assertIn('GRX_CALL_grDrawTriangle',header)
if __name__=='__main__':unittest.main()
