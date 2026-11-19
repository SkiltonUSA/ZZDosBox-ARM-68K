"""Resolved pixels must survive rectangle grouping and capacity rejection."""
import importlib.util
from pathlib import Path
import random
import unittest
ROOT = Path(__file__).resolve().parents[4]
spec = importlib.util.spec_from_file_location('spans', ROOT / 'scripts/analyze_uae_copper_spans.py')
spans = importlib.util.module_from_spec(spec)
spec.loader.exec_module(spans)

class SpansTest(unittest.TestCase):
    def check_image(self, rows):
        result = spans.plan(rows)
        self.assertIsNotNone(result['rectangles'])
        out = [[None for _ in row] for row in rows]
        for x0, y0, x1, y1, colour in result['rectangles']:
            self.assertTrue(0 <= x0 < x1 <= len(rows[0]))
            self.assertTrue(0 <= y0 < y1 <= len(rows))
            for y in range(y0, y1):
                for x in range(x0, x1):
                    self.assertIsNone(out[y][x], 'overlapping rectangles')
                    out[y][x] = colour
        self.assertEqual(out, rows)
        return result

    def test_solid_merges_vertically(self):
        r = self.check_image([[0xF800] * 7 for _ in range(5)])
        self.assertEqual(r['rectangles'], [(0, 0, 7, 5, 0xF800)])
        self.assertEqual(r['horizontal_runs'], 5)

    def test_edges_and_changed_palettes(self):
        self.check_image([[1, 1, 2], [1, 2, 2], [3, 3, 3], [1, 1, 2]])
        self.check_image([[0], [0xFFFF], [0]])

    def test_alternating_and_random_pixels(self):
        self.check_image([[(x + y) % 2 for x in range(17)] for y in range(9)])
        rng = random.Random(712)
        for _ in range(80):
            self.check_image([[rng.randrange(5) for _ in range(13)] for _ in range(7)])

    def test_capacity_rejects_entire_frame(self):
        r = spans.plan([[1, 2], [2, 1]], max_words=37)
        self.assertIsNone(r['rectangles'])
        self.assertEqual(r['required_drawing_words'], 38)
        self.assertEqual(len(spans.plan([[1, 2], [2, 1]], max_words=38)['rectangles']), 4)

    def test_invalid_frame(self):
        for rows in ([], [[]], [[1], [2, 3]], [[-1]], [[65536]], [[1] * 1025]):
            with self.assertRaises(ValueError):
                spans.plan(rows)

    def test_selective_regions_preserve_cpu_fallback(self):
        rows = [[1] * 8 + [2, 3], [1] * 8 + [3, 2]]
        result = spans.select_regions(rows, min_area=4, max_words=17)
        self.assertEqual(result['gpu_rectangles'], [(0, 0, 8, 2, 1)])
        self.assertEqual(result['gpu_pixels'], 16)
        self.assertEqual(result['drawing_words'], 17)
        spans.verify(rows, result['gpu_rectangles'] + result['cpu_rectangles'])
        for budget in (0, 10, 16):
            fallback = spans.select_regions(rows, min_area=4, max_words=budget)
            self.assertEqual(fallback['gpu_rectangles'], [])
            self.assertEqual(fallback['drawing_words'], 0)
            spans.verify(rows, fallback['cpu_rectangles'])

    def test_selective_budget_prefers_large_regions_deterministically(self):
        rows = [[1, 2, 2, 3, 3, 3], [1, 2, 2, 3, 3, 3]]
        result = spans.select_regions(rows, min_area=1, max_words=24)
        self.assertEqual(result['gpu_pixels'], 10)
        self.assertEqual(len(result['gpu_rectangles']), 2)
        spans.verify(rows, result['gpu_rectangles'] + result['cpu_rectangles'])
        self.assertEqual(result, spans.select_regions(rows, min_area=1, max_words=24))
        self.assertEqual(spans.select_regions(rows, min_area=7)['gpu_pixels'], 0)
        with self.assertRaises(ValueError):
            spans.select_regions(rows, min_area=0)

    def test_selective_random_partitions(self):
        rng = random.Random(741)
        for _ in range(80):
            rows = [[rng.randrange(3) for _ in range(11)] for _ in range(7)]
            budget, threshold = rng.randrange(100), rng.randrange(1, 5)
            r = spans.select_regions(rows, min_area=threshold, max_words=budget)
            spans.verify(rows, r['gpu_rectangles'] + r['cpu_rectangles'])
            self.assertLessEqual(r['drawing_words'], budget)
            self.assertEqual(r['gpu_pixels'], sum((x1-x0)*(y1-y0)
                for x0, y0, x1, y1, colour in r['gpu_rectangles']))
            for x0, y0, x1, y1, colour in r['gpu_rectangles']:
                self.assertGreaterEqual((x1-x0)*(y1-y0), threshold)

if __name__ == '__main__':
    unittest.main()
