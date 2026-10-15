#!/usr/bin/env python3
"""Coverage and honest topic classification for complete discussions."""
from dosbox_discussion import overview

headers=['Recent (2020-01-02 03:04) Alice','Earlier (2010-03-04 05:06) Bob','Oldest (2000-01-01 00:00) Alice']
bodies=['Thanks!', 'No sound, but the mouse works. Sound sound sound.', 'HIMEM and protected mode configuration']
o=overview(headers,bodies)
assert o['indexed_comments']==3 and len(o['entries'])==3 and o['contributors']==2
assert o['oldest']=='2000-01-01 00:00' and o['newest']=='2020-01-02 03:04'
assert o['unclassified']==1
counts={t['id']:t['comments'] for t in o['topics']}
assert counts['audio']==1 and counts['input']==1 and counts['memory']==1
assert o['entries'][0]['topics']==[]
assert o['entries'][1]['author']=='Bob' and o['entries'][1]['number']==2
assert 'No sound' not in o['summary'] and 'not fixes' in o['summary']
# Older comments must contribute; negation must not become an asserted fix.
assert 'audio' not in overview(headers[:1],bodies[:1])['summary'].lower()
assert 'Audio' in o['summary']
assert not overview(['Unknown'],['https://example.com/sound/mouse'])['topics']
assert overview(['Undated'],['Hello'])['entries'][0]['date']==''
assert overview([],[])['summary']==''
try:overview(headers,bodies[:1])
except ValueError:pass
else:raise AssertionError('Unpaired comments accepted')
print('PASS all-comment coverage, attribution, oldest/newest, unique mentions, negation, URL exclusion and unclassified fallback')
