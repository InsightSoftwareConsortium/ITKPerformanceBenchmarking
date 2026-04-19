"""ASV benchmarks for ITK Filtering-group filters."""

from itk_perf_shim import run_benchmark


class FilteringSuite:
    timeout = 600.0
    unit = "seconds"
    number = 1
    repeat = 1

    def track_binary_add(self):
        return run_benchmark("filtering.binary_add")

    track_binary_add.unit = "seconds"

    def track_unary_add(self):
        return run_benchmark("filtering.unary_add")

    track_unary_add.unit = "seconds"

    def track_gradient_magnitude(self):
        return run_benchmark("filtering.gradient_magnitude")

    track_gradient_magnitude.unit = "seconds"

    def track_median(self):
        return run_benchmark("filtering.median")

    track_median.unit = "seconds"

    def track_min_max_curvature_flow(self):
        return run_benchmark("filtering.min_max_curvature_flow")

    track_min_max_curvature_flow.unit = "seconds"
