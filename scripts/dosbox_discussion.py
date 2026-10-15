"""Deterministic topic overviews of complete cached DOSBox discussions.

This is a topic index, not a language model or a recommendation engine. A
mention may be a question, a failure or a successful workaround. Never infer
success or turn quoted commands into suggested settings. Source prose stays
in the private HTML cache; exported entries contain attribution and topic IDs.
"""
from collections import Counter
import re

TOPICS = (
    ('installation', 'Installation / startup', r'\b(?:install\w*|setup|start(?:up|ing)?|launch\w*|execut\w*)\b'),
    ('discs', 'Discs / mounting / paths', r'\b(?:cd(?:rom)?|dvd|iso|cue|imgmount|mount\w*|path(?:cd)?|director\w*|folder\w*|drive\w*|disk\w*|disc\w*)\b'),
    ('audio', 'Audio / MIDI', r'\b(?:sound\w*|audio|music|midi|adlib|blaster|gus|ultrasound|roland|mt[ -]?32|speaker\w*)\b'),
    ('input', 'Mouse / keyboard / joystick', r'\b(?:mous\w*|keyboard\w*|joystick\w*|gamepad\w*|cursor\w*|control(?:s|ler)?|key(?:s|map\w*|bind\w*)?)\b'),
    ('display', 'Graphics / display', r'\b(?:graphic\w*|display\w*|screen\w*|video|vga|svga|vesa|cga|ega|hercules|resolut\w*|render\w*|scaler\w*|palette\w*)\b'),
    ('speed', 'Speed / CPU cycles', r'\b(?:slow\w*|fast\w*|speed\w*|cycles?|fps|frameskip|lag\w*|stutter\w*|performance|timing|cpu|processor\w*)\b'),
    ('memory', 'Memory / DOS extenders', r'\b(?:memory|ram|ems|xms|umb|dpmi|dos4gw|dos4g|dos32a|loadfix|himem|emm386|protected[ -]mode)\b'),
    ('errors', 'Errors / crashes / freezes', r'\b(?:error\w*|crash\w*|freez\w*|frozen|hang(?:s|ing)?|lockup\w*|fault\w*|exception\w*)\b'),
    ('configuration', 'Configuration / commands', r'\b(?:config\w*|cfg|conf|autoexec|command\w*|setting\w*|core)\b'),
    ('patches', 'Patches / game versions', r'\b(?:patch\w*|updat\w*|fix\w*|version\w*|releas\w*)\b'),
    ('saves', 'Saved games / progress', r'\b(?:sav(?:e|es|ed|ing)|savegame\w*|checkpoint\w*)\b'),
    ('network', 'Network / multiplayer', r'\b(?:network\w*|multiplayer|ipx|modem|serial|tcp)\b'),
    ('host', 'Host OS / hardware', r'\b(?:windows|linux|ubuntu|debian|mac(?:os)?|osx|vista|win(?:95|98|xp|7|10)|pentium|athlon|raspberry|android)\b'),
    ('help', 'Guides / downloads', r'\b(?:download\w*|manual\w*|guide\w*|tutorial\w*|instruction\w*|readme|pdf)\b'),
    ('play', 'Gameplay', r'\b(?:cheat\w*|weapon\w*|sword\w*|level\w*|mission\w*|puzzle\w*|fight\w*|gameplay|combat)\b'),
)
RULES = [(key, label, re.compile(pattern, re.I)) for key, label, pattern in TOPICS]

def overview(headers, bodies):
    if len(headers) != len(bodies):
        raise ValueError('Unpaired comments')
    entries = []
    for number, (header, body) in enumerate(zip(headers, bodies), 1):
        stamp = re.search(r'\((\d{4}-\d{2}-\d{2} \d{2}:\d{2})\)', header)
        subject = header[:stamp.start()] if stamp else ''
        # Do not classify a URL or the author's username as discussion content.
        text = re.sub(r'https?://\S+', ' ', subject + ' ' + body)
        topics = [key for key, _, pattern in RULES if pattern.search(text)]
        entries.append(dict(number=number, date=stamp[1] if stamp else '',
                            author=header[stamp.end():].strip() if stamp else '', topics=topics))
    counts = Counter(key for entry in entries for key in entry['topics'])
    dates = sorted(e['date'] for e in entries if e['date'])
    authors = {e['author'] for e in entries if e['author']}
    unclassified = sum(not e['topics'] for e in entries)
    topics = [dict(id=key, label=label, comments=counts[key]) for key, label, _ in TOPICS if counts[key]]
    topics.sort(key=lambda t: (-t['comments'], t['label']))
    lines = []
    if entries:
        lines.append(f"All {len(entries)} comments indexed; {len(authors)} named contributors.")
        if dates:
            lines.append(f"Dates: {dates[0][:10]} to {dates[-1][:10]}.")
        lines.append('Automatic topic overview (mentions, not fixes):')
        lines.extend(f"{t['label']}: {t['comments']}" for t in topics)
        if unclassified:
            lines.append(f'Other / unclassified comments: {unclassified}')
        lines.append('Topics can overlap. Read the original report for context and complete comments.')
    return dict(method='topic-index-v1', indexed_comments=len(entries), contributors=len(authors),
                oldest=dates[0] if dates else '', newest=dates[-1] if dates else '',
                topics=topics, unclassified=unclassified, entries=entries,
                summary='\n'.join(lines))
