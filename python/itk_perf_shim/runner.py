"""Invoke a benchmark executable, parse its jsonxx output, return mean seconds.

Environment contract (set by the ASV/GHA caller):
  ITK_BENCHMARK_BIN   — dir containing benchmark executables (required)
  ITK_BENCHMARK_DATA  — ExternalData root holding the BRAIN image fixture (required)
  ITK_BENCHMARK_SCRATCH — scratch dir for output images (optional; tempdir otherwise)
"""

from __future__ import annotations

import json
import os
import subprocess
import tempfile
from pathlib import Path

from .registry import BENCHMARKS


class BenchmarkError(RuntimeError):
    pass


def _resolve_env() -> tuple[Path, Path, Path]:
    bin_dir = os.environ.get("ITK_BENCHMARK_BIN")
    data_dir = os.environ.get("ITK_BENCHMARK_DATA")
    if not bin_dir:
        raise BenchmarkError("ITK_BENCHMARK_BIN not set")
    if not data_dir:
        raise BenchmarkError("ITK_BENCHMARK_DATA not set")
    scratch = os.environ.get("ITK_BENCHMARK_SCRATCH") or tempfile.mkdtemp(prefix="itkperf-")
    return Path(bin_dir), Path(data_dir), Path(scratch)


def _find_exe(bin_dir: Path, exe: str) -> Path:
    for candidate in (bin_dir / exe, bin_dir / f"{exe}.exe"):
        if candidate.is_file() and os.access(candidate, os.X_OK):
            return candidate
    for match in bin_dir.rglob(exe):
        if match.is_file() and os.access(match, os.X_OK):
            return match
    raise BenchmarkError(f"Executable {exe!r} not found under {bin_dir}")


def _mean_probe_seconds(timings_json: Path) -> float:
    """Parse HighPriorityRealTimeProbesCollector JSON; average all probes' means.

    The jsonxx output shape (per WriteExpandedReport + JSONReport) is roughly:
      { "Probes": [ { "Name": "...", "Mean": <sec>, "Min": ..., "Max": ... }, ... ],
        "SystemInformation": {...}, "ITKBuildInformation": {...}, ... }
    We reduce to a single scalar per benchmark by averaging probe means, since each
    C++ benchmark typically has one dominant probe. Multi-probe benchmarks can be
    parametrized later.
    """
    with timings_json.open() as f:
        doc = json.load(f)
    probes = doc.get("Probes") or doc.get("probes") or []
    if not probes:
        raise BenchmarkError(f"No probes in {timings_json}: keys={list(doc)}")
    means = []
    for p in probes:
        for key in ("Mean", "mean", "MeanTime", "Mean (s)"):
            if key in p:
                means.append(float(p[key]))
                break
    if not means:
        raise BenchmarkError(f"No Mean field in probes of {timings_json}")
    return sum(means) / len(means)


def run_benchmark(name: str) -> float:
    if name not in BENCHMARKS:
        raise BenchmarkError(f"Unknown benchmark {name!r}")
    spec = BENCHMARKS[name]
    bin_dir, data_dir, scratch = _resolve_env()
    exe = _find_exe(bin_dir, spec["exe"])
    scratch.mkdir(parents=True, exist_ok=True)

    with tempfile.NamedTemporaryFile(
        suffix=".json", dir=scratch, delete=False
    ) as tf:
        timings_json = Path(tf.name)

    subs = {
        "timings_json": str(timings_json),
        "iterations": str(spec["iterations"]),
        "brain": str(data_dir / "brainweb165a10f17.mha"),
        "brain_x45": str(data_dir / "brainweb165a10f17extract45i90z.mha"),
        "brain_x60": str(data_dir / "brainweb165a10f17extract60i50z.mha"),
        "output_dir": str(scratch),
    }
    argv = [str(exe)] + [a.format(**subs) for a in spec["args"]]
    try:
        proc = subprocess.run(argv, capture_output=True, text=True, check=True)
    except subprocess.CalledProcessError as e:
        raise BenchmarkError(
            f"{name} failed (rc={e.returncode}):\nstdout={e.stdout}\nstderr={e.stderr}"
        ) from e
    _ = proc  # stdout contains the human-readable report; we parse the JSON file
    return _mean_probe_seconds(timings_json)
