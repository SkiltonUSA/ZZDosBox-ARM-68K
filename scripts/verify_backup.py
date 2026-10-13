#!/usr/bin/env python3
"""Verify saved source/artifact bytes without contacting hardware."""
from pathlib import Path
import hashlib,json
ROOT=Path(__file__).resolve().parents[1]
def verify():
 manifest=json.loads((ROOT/'backup-manifest.json').read_text())
 for name,digest in manifest.items():
  p=ROOT/name
  if not p.is_file() or hashlib.sha256(p.read_bytes()).hexdigest()!=digest:
   raise SystemExit('Backup checksum mismatch: '+name)
 print(f'PASS: {len(manifest)} backup files verified')
if __name__=='__main__':verify()
