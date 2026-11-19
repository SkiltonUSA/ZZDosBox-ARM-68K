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

if __name__ == '__main__':
    unittest.main()
