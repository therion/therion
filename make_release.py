#!/usr/bin/env python

import re, datetime, sys, subprocess

if sys.version_info.major > 2:
  raw_input = input

banner = '''

  Warning!

  This script makes a new release and pushes it to github. 

  All changes should already be committed.

  Continue? (no/yes): '''

def err(s):
  print('\nfatal: %s\n' % s)
  sys.exit(1)

if raw_input(banner) == 'yes':
  # check CHANGES to assure that the release is ready and to get the tag message
  isfirst = True
  msg = ''
  ver = ''
  with open('CHANGES') as f:
    for l in f:
      if isfirst:
        isfirst = False
        m = re.match(r'Therion (\d+\.\d+(?:\.\d+)?) \((\d{4}-\d\d-\d\d)\):\s+$',l)
        if m:
          if m.group(2) != datetime.date.today().isoformat():
            err('release date in the CHANGES is not today')
          ver = m.group(1)
        else:
          err('no version info in the file CHANGES')
        msg += l
      else:
        if l.startswith('-'*70):
          break
        else:
          msg += l.replace('"',r'\"').replace('$',r'\$')
  # check for uncommited changes
  subprocess.check_call('git pull', shell=True)
  if subprocess.check_output('git status -s', shell=True):
    err('there are uncommitted changes on your drive')
  # add a tag and push
  subprocess.check_call('git tag -a v%s -m "%s"' % (ver,msg), shell=True)
  subprocess.check_call('git push origin v%s' % ver, shell=True)
  # update CHANGES to assure that subsequent commits would not be mistaken for a release by set_version.py 
#  m = re.match(r'(.+\.)(\d+)$', ver)
#  assert m
#  ver = '%s%d' % (m.group(1), int(m.group(2))+1)
#  with open('CHANGES') as f:
#    oldchanges = f.read()
#  with open('CHANGES', 'w') as f:
#    f.write("Therion %s (in progress):\n\n\n%s\n\n" % (ver, '-'*80) + oldchanges)
#  subprocess.check_call('git add CHANGES', shell=True)
#  subprocess.check_call('git commit -m "update CHANGES"', shell=True)
#  subprocess.check_call('git push', shell=True)
else:
  print('bye')
