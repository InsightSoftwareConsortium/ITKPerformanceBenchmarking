"""ASV benchmarks for ITK Segmentation-group filters."""

from itk_perf_shim import run_benchmark


class SegmentationSuite:
    timeout = 1200.0
    unit = "seconds"
    number = 1
    repeat = 1

    def track_region_growing(self):
        return run_benchmark("segmentation.region_growing")

    track_region_growing.unit = "seconds"

    def track_watershed(self):
        return run_benchmark("segmentation.watershed")

    track_watershed.unit = "seconds"

    def track_morphological_watershed(self):
        return run_benchmark("segmentation.morphological_watershed")

    track_morphological_watershed.unit = "seconds"

    def track_level_set(self):
        return run_benchmark("segmentation.level_set")

    track_level_set.unit = "seconds"
