#!/usr/bin/env python3
"""Compare completed physical Doom Demo3 runs; VGA refresh is not game FPS."""
import argparse
import json
import re
import statistics
from pathlib import Path

p = argparse.ArgumentParser()
p.add_argument("directory", nargs="?", type=Path, default=Path(__file__).resolve().parent)
a = p.parse_args()
rows = []
for path in sorted(a.directory.glob("doom-*.log")):
    if not re.fullmatch(r"doom-(base|vfp)-\d+\.log", path.name):
        continue
    log = path.read_text()
    for marker in ("timed 2134 gametics", "CPU_CYCLES applied=10000",
                   "SHELL result=0 guest_exit=0", "SHELL PASS", "context_equal=1",
                   "register_mismatches=00000000", "witness_bad=0",
                   "LAUNCHER exit=0; owned memory released",
                   "SETTINGS memory_mb=8 machine=0 cycles=10000 live=1"):
        assert marker in log, (path, marker)
    assert not re.search(r"^PROFILE ", log, re.M), path
    clock = re.search(r"DOOMCLOCK magic=44424d31 signature=1 rejected=0 first=(\d+) last=(\d+) sim_first=\d+ sim_last=\d+ start=([0-9a-f]+) end=([0-9a-f]+)", log)
    assert clock, path
    first, last = int(clock[1]), int(clock[2])
    assert first in (0, 1) and last == 2134, path
    seconds = (int(clock[4], 16) - int(clock[3], 16)) / 333333333
    def fields(prefix):
        line = re.search(r"^" + prefix + r" .*", log, re.M)
        assert line, (path, prefix)
        return dict(re.findall(r"(\w+)=([a-f0-9]+)", line[0]))
    jit, pc, shell, scan = map(fields, ("JIT", "PC", "SHELL", "SCANLINES"))
    assert jit["witness"] == "1" and jit["cache_bytes"] == "4194304", path
    assert pc["timed_done"] == "0", path
    assert scan["skipped_rows"] == scan["cleared_rows"] == "0", path
    workload = {"source_frames": int(pc["frames"]), "emulated_ms": int(pc["emulated_ms"]),
                "jit_commits": int(jit["commits"]), "jit_dispatches": int(jit["dispatches"]),
                "guest_slices": int(shell["slices"]), "guest_output_hash": shell["hash"]}
    present = re.search(r"PRESENT magic=50525331 source=(\d+) shown=(\d+) skipped=(\d+) divisor=(\d+) changes=(\d+) mode=(\d+)", log)
    assert present and int(present[1]) == int(present[2]) and int(present[3]) == 0 and int(present[4]) == 1 and int(present[6]) == 0, path
    rows.append({"run": path.stem, "variant": "baseline" if "-base-" in path.name else "pic_vfp",
                 "game_tics": last-first, "seconds": seconds, "game_fps": (last-first)/seconds,
                 "clean_teardown": True, "profiling": False, "workload": workload})
assert rows, "No completed Doom runs"
means = {v: statistics.mean(r["game_fps"] for r in rows if r["variant"] == v)
         for v in {r["variant"] for r in rows}}
result = {"measurement": "Doom Demo3 game-tic interval / A9 global timer (333333333 Hz)",
          "settings": "8 MiB guest; 10000 cycles; Direct; Normal presentation; no profiling, trace, overlay or scanlines",
          "runs": rows, "mean_game_fps": means,
          "workload_counters_identical": all(r["workload"] == rows[0]["workload"] for r in rows)}
if len(means) == 2:
    result["mean_gain_percent"] = (means["pic_vfp"] / means["baseline"] - 1) * 100
    result["paired_gain_percent"] = {
        str(n): (next(r["game_fps"] for r in rows if r["run"] == f"doom-vfp-{n}") /
                 next(r["game_fps"] for r in rows if r["run"] == f"doom-base-{n}") - 1) * 100
        for n in (1, 2) if all(any(r["run"] == f"doom-{v}-{n}" for r in rows) for v in ("base", "vfp"))}
startup = []
for game in ("vette", "empire"):
    for variant in ("base", "vfp"):
        path = a.directory / f"{game}-{variant}.log"
        if not path.exists():
            continue
        log = path.read_text()
        for marker in ("SHELL result=2 guest_exit=0", "SHELL PASS", "context_equal=1",
                       "register_mismatches=00000000", "witness_bad=0",
                       "LAUNCHER exit=0; owned memory released", "written=0; handles closed",
                       "SETTINGS memory_mb=8 machine=0 cycles=3000 live=0"):
            assert marker in log, (path, marker)
        pc = re.search(r"PC frames=(\d+) emulated_ms=8000 wall_ms=(\d+) timed_done=1", log)
        assert pc, path
        startup.append({"run": path.stem, "emulated_ms": 8000, "wall_seconds": int(pc[2])/1000,
                        "display_frames": int(pc[1]), "clean_timed_cancellation": True})
result["startup_checks"] = startup
result["startup_scope"] = "Timed startup and cancellation only; no gameplay FPS, driving or Empire combat validation"
(a.directory / "physical-results.json").write_text(json.dumps(result, indent=2) + "\n")
print(json.dumps(result, indent=2))
