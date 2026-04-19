"""ASV benchmarks for ITK Core-group iteration.

Each `track_*` function returns seconds measured *inside* the C++ executable
via HighPriorityRealTimeProbesCollector. We use `track_*` rather than `time_*`
because the timing comes from the subprocess, not from ASV's wall-clock
instrumentation of the Python callable.
"""

from itk_perf_shim import run_benchmark


class CoreSuite:
    timeout = 300.0
    unit = "seconds"
    number = 1
    repeat = 1

    def track_copy_iteration(self):
        return run_benchmark("core.copy_iteration")

    track_copy_iteration.unit = "seconds"

    def track_vector_iteration(self):
        return run_benchmark("core.vector_iteration")

    track_vector_iteration.unit = "seconds"
