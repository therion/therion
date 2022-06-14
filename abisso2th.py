#!/usr/bin/python3
#
# copyright 2009 Martin Budaj

import sys,os

if len(sys.argv) < 2:
  print("usage: ss2th <file.ss>")
  sys.exit()

file_in = open(sys.argv[1]);
file_out = open(os.path.splitext(sys.argv[1])[0]+'.th', 'w')

f_prev = 0
m = 2

file_out.write('centreline\n')

for i,l in enumerate(file_in):
  if i < 8: continue
  toks = l.split()
  if i < 18 and float(toks[0]) < 0 and sum(abs(float(x)) for x in toks[1:4]):
    file_out.write('    fix')
    for j in range(4): file_out.write(" {}".format(toks[j]))
    file_out.write('\n')
  if i == 18:
    file_out.write('  units clino compass deg\n')
    file_out.write('  calibrate tape 0 0.01\n')
    file_out.write('  data normal from to compass clino length\n')
  if len(toks) < 9: continue
  l,s,a,f,t = [float(x) for x in toks[0:5]]
  if l == s == f == a == t == 0: continue
  t = m if t==0 else t
  f = t_prev if f == 0 else f
  t_prev = t
  m += 1
  file_out.write('%8d%8d%8s%8s%8s\n' % (f,t,a,s,l))

file_out.write('endcentreline\n')
