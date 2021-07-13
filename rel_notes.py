#!/usr/bin/env python3

# this script generates release notes in the Markdown format for the most recent release
# based on the data in the file CHANGES; it's used by Github Actions when creating a release

import re

f_out = open('rel_notes.md','w')

with open('CHANGES') as f:
  for l in f:
    if l.startswith('-'*70):
      break
    m = re.match(r'(Therion \d+\..+:)\s+$',l)
    if m:
      f_out.write('## %s\n' % m.group(1))
      continue
    m = re.match(r'(\S.+:)\s+$',l)
    if m:
      f_out.write('#### %s\n' % m.group(1))
      continue
    f_out.write(l.replace('<','\\<'))

f_out.close()
