#!/usr/bin/env python3

# set version information based on
#   1) git data
#   2) the first line of the file CHANGES (valid for releases)
#   3) use at least previous release info + current date

import subprocess, datetime, re, sys, pathlib

def run(s):
  return subprocess.check_output(s, shell=True, stderr=subprocess.DEVNULL).strip().decode('ascii')

if len(sys.argv) > 1:
  output_folder = pathlib.Path(sys.argv[1])
else:
  output_folder = pathlib.Path('.')

try:
  release = run('git tag --points-at HEAD') # check for a TAG in the current commit
  if run('git status -s'): # check for uncommitted changes
    uncommitted = '+dev'
    date = datetime.date.today().isoformat()
  else:
    uncommitted = ''
    date = run('git show -s --format="%cd" --date=short HEAD')
  if release.startswith('v'):   # release number if the commit is tagged
    ver = "%s (%s)" % (release[1:]+uncommitted, date) 
  else:                         # otherwise last known tag + short commit hash
    prevver = run('git describe --abbrev=0 --tags HEAD')
    commit = run('git rev-parse --short HEAD')
    ver = "%s+%s (%s)" % (prevver[1:], commit+uncommitted, date)
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
    ver = "%s+dev (compiled on %s)" % (ver1, datetime.date.today().isoformat())

try:
    oldver = open(output_folder / 'thversion.h').read()
except:
    oldver = ""

newver = '#define THVERSION "%s"\n' % ver
if (oldver != newver):
  with open(output_folder / 'thversion.h','w') as f:
    f.write(newver)
  (output_folder / 'thbook/').mkdir(parents=True, exist_ok=True)
  with open(output_folder / 'thbook/version.tex','w') as f:
    f.write('%s\n' % ver)
