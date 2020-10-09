#!/usr/bin/python3 -B
import re,datetime,math,geomag, sys

if len(sys.argv) == 1:
  print('usage: %s <sample_out_IGRFXY.txt>' % sys.argv[0])
  sys.exit(1)

units = {'K':1000, 'M':1, 'F':.3048}

def deg2rad(s):
  m = re.match(r'(-?)(\d+)\,(\d+)\,(\d*)',s)
  if m:
    tmp = float(m.group(2))+float(m.group(3))/60+float('0'+m.group(4))/3600
    if m.group(1) == '-': tmp = -tmp
  else:
    tmp = float(s)
  return tmp/180*math.pi

for i,l in enumerate(open(sys.argv[1])):
  if i==0: continue
  print('% 2s ' % i, end='')
  (dat, cs, alt, lat, lon, dd, dm) = l.split()[:7]
  if cs == 'C':
    print('   (skipping geocentric coordinates)')
    continue
  m = re.match(r'(\d{4}),(\d{1,2}),(\d{1,2})',dat)
  if m:
    dat = datetime.datetime(*(int(d) for d in m.groups()))
    dat = dat.year + (float(dat.timetuple().tm_yday-1) / 
                     datetime.datetime(dat.year,12,31).timetuple().tm_yday)
  else:
    dat = float(dat)
  alt = units[alt[0]] * float(alt[1:])
  lat = deg2rad(lat)
  lon = deg2rad(lon)
  decl = geomag.thgeomag(lat,lon,alt,dat)
  if decl < 0: res = '-' 
  else: res = ''
  decl = abs(decl)
  res += "%dd %dm" % (int(decl),round((decl-int(decl)) * 60))
  correct = "%s %s" % (dd,dm)
  if res == correct: print('  ', end='')
  else: print('! ', end='')
  print ("calc: %8s, corr: %8s" % (res, correct))
