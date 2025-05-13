#!/usr/bin/env python3

import re, datetime, sys, subprocess, requests, json

repo = re.search(r'\:(\w+/\w+)\.', subprocess.check_output('git config --get remote.origin.url', shell=True).decode('ascii')).group(1)

banner = f'''

  Warning!

  This script makes a new release and pushes it to github [{repo}].

  All changes should already be committed.

  Continue? (no/yes): '''

def err(s):
  print('\nfatal: %s\n' % s)
  sys.exit(1)

def rel_notes():
  s = ''
  with open('CHANGES') as f:
    for l in f:
      if l.startswith('-'*70):
        break
      if m := re.match(r'(Therion \d+\..+:)\s+$',l):
        s += '## %s\n' % m.group(1)
        continue
      if m := re.match(r'(\S.+:)\s+$',l):
        s += '#### %s\n' % m.group(1)
        continue
      s += l.replace('<','\\<')
  return s

# get the github token
token = input('github token: ')
if not token or not token.startswith('gh'):
  err("couldn't initialize the github token")

if input(banner) == 'yes':
  # check CHANGES to assure that the release is ready and to get the tag message
  isfirst = True
  msg = ''
  ver = ''
  gh_url = ''
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
  # check the branch
  if subprocess.check_output('git rev-parse --abbrev-ref HEAD', shell=True) != b'master\n':
    err('you are not on master branch')
  # check for uncommited changes
  subprocess.check_call('git pull', shell=True)
  if subprocess.check_output('git status -s', shell=True):
    err('there are uncommitted changes on your drive')
  # add a signed tag and push
  subprocess.check_call('git tag -s -a v%s -m "%s"' % (ver,msg), shell=True)
  subprocess.check_call('git push origin v%s' % ver, shell=True)
  # create a release and upload the signature
  rel_data = {'tag_name': 'v%s' % ver,
              'name': 'Release v%s' % ver,
              'body': rel_notes()}
  signature = subprocess.check_output('git archive --prefix=therion-%s/ v%s | gzip --no-name | gpg --armor --detach-sign --default-key B4FFC641 --default-key 6F0F704B -o -' % (ver, ver), shell=True)
  headers = {'Authorization': 'token ' + token, 'Accept': 'application/vnd.github.v3+json'}
  res = requests.post('https://api.github.com/repos/%s/releases' % repo, headers=headers, data=json.dumps(rel_data)).json()
  if 'id' not in res:
    err('unexpected response while creating a release:\n\n%s\n' % res.text)
  headers = {'Authorization': 'token ' + token, 'Accept': 'application/vnd.github.v3+json', 'Content-Type': 'text/plain'}
  res = requests.post('https://uploads.github.com/repos/%s/releases/%s/assets?name=%s' % (repo, res['id'], 'therion-%s.tar.gz.asc' % ver),
                headers=headers, data=signature).json()
  if 'id' not in res:
    err('unexpected response while uploading the signature:\n\n%s\n' % res.text)
  # update CHANGES to assure that subsequent commits would not be mistaken for a release by set_version.py 
  m = re.match(r'(.+\.)(\d+)$', ver)
  assert m
  newver = '%s%d' % (m.group(1), int(m.group(2))+1)
  with open('CHANGES') as f:
    oldchanges = f.read()
  with open('CHANGES', 'w') as f:
    f.write("Therion %s (in progress):\n\n\n%s\n\n" % (newver, '-'*80) + oldchanges)
  subprocess.check_call('git add CHANGES', shell=True)
  subprocess.check_call('git commit -m "update CHANGES"', shell=True)
  subprocess.check_call('git push', shell=True)
else:
  print('bye')
