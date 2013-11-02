#!/usr/bin/python
import os
from os.path import basename
import glob

first = True
print "["
for h_file in glob.glob("*.hh"):
  if first:
    first = False
  else:
    print ","
  print "{"
  print "  \"directory\": \"" + os.getcwd() + "\","
  print "  \"command\": \"/usr/bin/g++ -std=c++0x -g -O0 -Wall " + basename(h_file) + " -c\","
  print "  \"file\": \"./" + basename(h_file) + "\""
  print "}"
print "]"

