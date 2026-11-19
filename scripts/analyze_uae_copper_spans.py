#!/usr/bin/env python3
"""Offline RGB565 span feasibility probe. No GPU submission or timing claims.

Input is a completed image, not a Copper execution trace. Command volume models
opaque scissored clears: five initial register writes plus seven words per
rectangle. Fences, padding, uploads and presentation are excluded. The supplied
budget is for drawing words only, not a hardware allocation or packet limit.
"""
import argparse
import hashlib
import json
from pathlib import Path


def horizontal_runs(row):
    start = 0
    while start < len(row):
        end = start + 1
        while end < len(row) and row[end] == row[start]:
            end += 1
        yield start, end, row[start]
        start = end


def plan(rows, max_words=32768):
    if not rows or not rows[0] or len(rows) > 768 or len(rows[0]) > 1024:
        raise ValueError('Expected a nonempty image up to 1024x768')
    width = len(rows[0])
    if max_words < 0 or any(len(row) != width for row in rows):
        raise ValueError('Invalid budget or inconsistent row width')
    if any(not isinstance(c, int) or not 0 <= c <= 65535 for row in rows for c in row):
        raise ValueError('Pixels must be RGB565 integers')
    rectangles, previous, runs = [], {}, 0
    for y, row in enumerate(rows):
        current = {}
        for x0, x1, colour in horizontal_runs(row):
            runs += 1
            key = (x0, x1, colour)
            if key in previous:
                index = previous[key]
                old = rectangles[index]
                rectangles[index] = (x0, old[1], x1, y + 1, colour)
            else:
                index = len(rectangles)
                rectangles.append((x0, y, x1, y + 1, colour))
            current[key] = index
        previous = current
    words = 10 + 7 * len(rectangles)
    return {'width': width, 'height': len(rows), 'horizontal_runs': runs,
            'rectangle_count': len(rectangles), 'required_drawing_words': words,
            'drawing_budget_words': max_words,
            'rectangles': rectangles if words <= max_words else None}


def select_regions(rows, min_area=256, max_words=32768):
    """Offline partition: largest solid regions first, with exact CPU fallback."""
    if min_area < 1 or max_words < 0:
        raise ValueError('Expected positive area and nonnegative budget')
    full = plan(rows, max_words=10 + 7 * 1024 * 768)
    rectangles = full['rectangles']
    def area(rect):
        return (rect[2] - rect[0]) * (rect[3] - rect[1])
    eligible = [i for i, rect in enumerate(rectangles) if area(rect) >= min_area]
    eligible.sort(key=lambda i: (-area(rectangles[i]), i))
    capacity = max(0, (max_words - 10) // 7)
    selected = set(eligible[:capacity])
    gpu = [rect for i, rect in enumerate(rectangles) if i in selected]
    cpu = [rect for i, rect in enumerate(rectangles) if i not in selected]
    return {'gpu_rectangles': gpu, 'cpu_rectangles': cpu,
            'gpu_pixels': sum(area(rect) for rect in gpu),
            'drawing_words': 10 + 7 * len(gpu) if gpu else 0}


def verify(rows, rectangles):
    """Independent exhaustive coverage/pixel check, outside any timing claim."""
    out = [[None] * len(rows[0]) for _ in rows]
    for x0, y0, x1, y1, colour in rectangles:
        for y in range(y0, y1):
            for x in range(x0, x1):
                if out[y][x] is not None:
                    raise ValueError('Overlapping rectangles')
                out[y][x] = colour
    if out != rows:
        raise ValueError('Reconstructed RGB565 image differs')


def analyze(path, budget):
    from PIL import Image
    with Image.open(path) as image:
        width, height = image.size
        if not 0 < width <= 1024 or not 0 < height <= 768:
            raise ValueError('Input must be an emulator capture up to 1024x768')
        # Quantization is explicit; equality is against this RGB565 reference,
        # not the original RGB888 image or a physical display.
        data = image.convert('RGB').tobytes()
        pixels = [((data[i] >> 3) << 11) | ((data[i + 1] >> 2) << 5) | (data[i + 2] >> 3)
                  for i in range(0, len(data), 3)]
    rows = [pixels[y * width:(y + 1) * width] for y in range(height)]
    full = plan(rows, max_words=10 + 7 * width * height)
    verify(rows, full.pop('rectangles'))
    words = full['required_drawing_words']
    full.update(input=path.name, sha256=hashlib.sha256(path.read_bytes()).hexdigest(),
                drawing_budget_words=budget, fits_drawing_budget=words <= budget,
                drawing_bytes=words * 4, raw_rgb565_bytes=width * height * 2,
                command_to_pixel_bytes_ratio=round(words * 4 / (width * height * 2), 4),
                rgb565_reconstruction='exact',
                limitation='Offline image grouping only; no Copper trace, GPU execution or FPS measurement')
    selective = []
    for threshold in (64, 256, 1024):
        chosen = select_regions(rows, min_area=threshold, max_words=budget)
        gpu, cpu = chosen.pop('gpu_rectangles'), chosen.pop('cpu_rectangles')
        verify(rows, gpu + cpu)
        chosen.update(min_area=threshold, gpu_rectangle_count=len(gpu),
                      cpu_rectangle_count=len(cpu), drawing_bytes=chosen['drawing_words'] * 4,
                      gpu_coverage_percent=round(chosen['gpu_pixels'] * 100 / (width * height), 2),
                      rgb565_reconstruction='exact')
        selective.append(chosen)
    full['selective_regions'] = selective
    return full


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('images', type=Path, nargs='+')
    parser.add_argument('--drawing-words', type=int, default=32768)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    if args.drawing_words < 0:
        parser.error('drawing budget must be nonnegative')
    result = {'scope': 'Post-render RGB565 rectangle feasibility; excludes fences and presentation',
              'captures': [analyze(p, args.drawing_words) for p in args.images]}
    text = json.dumps(result, indent=2) + '\n'
    if args.output:
        args.output.write_text(text)
    else:
        print(text, end='')


if __name__ == '__main__':
    main()
