#!/usr/bin/env python

# set version information based on
#   1) git data
#   2) the first line of the file CHANGES (valid for releases)
#   3) use at least previous release info + current date

import subprocess, datetime, re

def run(s):
  return subprocess.check_output(s, shell=True).strip().decode('ascii')

try:
  release = run('git tag --points-at HEAD 2> /dev/null') # check for a TAG in the current commit
  date = run('git show -s --format="%cd" --date=short HEAD')
  if release.startswith('v'):   # release number if the commit is tagged
    ver = "%s (%s)" % (release[1:], date) 
  else:                         # otherwise last known tag + short commit hash
    prevver = run('git describe --abbrev=0 --tags master')
    commit = run('git rev-parse --short HEAD')
    ver = "%s+%s (%s)" % (prevver[1:], commit, date)
except:   # no git version available
  patt = r'Therion (\d+\.\d+(?:\.\d+)?) \((\d{4}-\d\d-\d\d)\):$'
  try:   # check the file CHANGES for version info on the first line (this would be a released version)
    with open('CHANGES') as f:
      txt = f.readline().strip()
    m = re.match(patt,txt)
    if m:
      ver = '%s (%s)' % (m.group(1),m.group(2))
    else:
      raise ValueError()
  except:   # find the most recent release in the file CHANGES
    with open('CHANGES') as f:
      for l in f:
        m = re.match(patt,l.strip())
        if m:
          ver1 = m.group(1)
          break
    ver = "%s+? (compiled on %s)" % (ver1, datetime.date.today().isoformat())

with open('thversion.h','w') as f:
  f.write('#define THVERSION "%s"\n' % ver)
with open('thbook/version.tex','w') as f:
  f.write('%s\n' % ver)
