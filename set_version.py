#!/usr/bin/env python

import subprocess, datetime

def run(s):
  return subprocess.check_output(s, shell=True).strip().decode('ascii')

try:
  release = run('git tag --points-at HEAD') # check for a TAG in the current commit
  date = run('git show -s --format="%cd" --date=short HEAD')
  if release.startswith('v'):   # release number if the commit is tagged
    ver = "%s (%s)" % (release[1:], date) 
  else:                         # otherwise last known tag + short commit hash
    prevver = run('git describe --abbrev=0 --tags master')
    commit = run('git rev-parse --short HEAD')
    ver = "%s+%s (%s)" % (prevver[1:], commit, date)
except:
  ver = "[no version info] (%s)" % datetime.date.today().isoformat()

with open('thversion.h','w') as f:
  f.write('#define THVERSION "%s"\n' % ver)
with open('thbook/version.tex','w') as f:
  f.write('%s\n' % ver)
