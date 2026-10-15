#!/usr/bin/env python3
"""Validate game-detail extraction without fetching third-party comments."""
from scrape_dosbox_details import parse

def version(v,status,rating):
    graphic=f'<img class="status" alt="{rating}% ({status})">' if rating else '<td>0% supported</td>'
    return f'<table>{graphic}<span>DOSBox version:</span> {v} (<span class="{status}">{status}</span>)</table><hr>'
# Synthetic page: a game title containing a dash, broken historical version,
# repeated version, comment markup, and a footer sharing the comment colour.
head='<div>Game details </div><div><span>Test - Game - Publisher - Division (1995)<br>Tested By: Tester</span>'
versions=version('0.74-3','supported',100)+version('0.71','broken',0)+version('0.71','playable',50)
comment='<table bgcolor="#355787"><tr><td>Advice (2010-01-02 03:04)</td><td>Reviewer</td></tr></table><table bgcolor="#0D2E5D"><tr><td>'+('word '*30)+'</td></tr></table>'
footer='<div class="caption">Compatibility statistics</div><table bgcolor="#0D2E5D"><tr><td>Footer</td></tr></table>'
raw=(head+versions+'</div><br/>'+comment+footer).encode()
game=dict(game_id=123,title='Test - Game',year='1995')
d=parse(raw,game)
assert d['publisher']=='Publisher - Division'
assert d['tested_by']=='Tester' and d['year']=='1995'
assert len(d['versions'])==3
assert d['versions'][1]['status']=='broken' and d['versions'][1]['rating_percent']==0
assert d['comment_count']==1 and d['latest_comment_author']=='Reviewer'
assert d['discussion']['indexed_comments']==1 and d['discussion']['entries'][0]['author']=='Reviewer'
assert d['latest_comment_at']=='2010-01-02 03:04'
assert len(d['note_preview'].removesuffix(' ...').split())==20
empty=parse((head+versions+'</div><br/>'+footer).encode(),game)
assert empty['comment_count']==0 and empty['note_preview']==''
try:parse(b'<html>Error</html>',game)
except ValueError:pass
else:raise AssertionError('Error page accepted')
print('PASS detail versions, conflicting history, publisher parsing, bounded previews, dates and footer exclusion')
