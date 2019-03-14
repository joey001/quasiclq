'''
This script run executive files for multiple times (with different seeds) and make statisitcs to the solution
'''

import subprocess
import sys
import os.path
import random
import numpy as np

execFileName = 'x64\\Debug\\quasiclq.exe'
runTime = 10


if __name__ == '__main__':
	rerun = 0
	if len(sys.argv) == 2 and sys.argv[1] == '-r':
		rerun = 1
	lno = 1
	instance= None
	seeds = []
	gamma = None
	solutions = []
	objs=[]
	times=[]
	iters=[]
	
	for rtime in range(runTime):
		if rerun:
			fout = open('T%dsh.o'%lno,'w')
			ferr = open('T%dsh.e'%lno,'w')
			args = [execFileName]	
			seed = random.randint(0,2147483647)
			try:
				subprocess.call([args,'-c', str(seed)], stdout=fout, stderr=ferr)
			except OSError as e:
				print >>sys.stderr, "Excution failed:",e
			print 'Run %d time'%(rtime+1)
			fout.close()
			ferr.close()
		fout = open('T%dsh.o'%lno,'r')
		formatDict = {}
		for line in fout.readlines():
			line = line.strip()
			#print line
			if line.startswith('$') or line.startswith('@') or line.startswith('#'):
				key, param = line[1:].split('=')
				formatDict[key] = param
		try:
			if formatDict['seed'] in seeds:
				print >>sys.stderr, 'seed %s repeat in file %s'%(formatDict['seed'],lno)
			seeds.append(formatDict['seed'])
			#check instance
			if instance == None:
				instance = formatDict['para_file']
				gamma = float(formatDict['para_gamma'])
			elif formatDict['para_file'] != instance:
				print >>sys.stderr, 'file %s %.2f conflic'%(formatDict['para_file'], formatDict['para_gamma'])
			
			times.append(float(formatDict['besttime']))
			objs.append(int(formatDict['objv']))
			iters.append(int(formatDict['bestiter']))
		except KeyError, e:
			print >>sys.stderr, 'Key error:',e

		lno+=1
	print 'Solution report:'
	print 'Time Ave: %.3f; StdVar %.2f'%(np.mean(times),np.std(times))
	print 'Iter Ave: %.3f; StdVar %.2f'%(np.mean(iters),np.std(iters))
	print 'Obj Ave: %.2f; stdVar %.2f'%(np.mean(objs), np.std(objs))
	
	