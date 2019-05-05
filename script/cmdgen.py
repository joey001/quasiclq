import os
import sys
import os.path
import random
import itertools

fold = sys.argv[1];
cmdPath = sys.argv[2];
cmdfile = open(cmdPath,'w')

gammalst = (0.80, 0.85, 0.90, 0.95)
alphalst = [0.01]
betalst = [0.4]
runcnt = 10
tmcutoff = 120


def density(f):
	if not f.endswith('.clq'):
		return None
	for line in open(f).readlines():
		if line[0] == 'p':
			pstr,estr,vnstr,enstr = line.split()
			vn = int(vnstr)
			en = int(enstr)			
			return float(2*en)/(vn*(vn-1))
			
cnt = 0

if os.path.exists('output') == False:
	os.makedirs('output')

for root, dir, files in os.walk(fold):
	for f in files:
		fulpath = os.path.join(root, f)
		dens = density(fulpath)		
		print(dens)
		print(fulpath)
		fltgammalst = filter(lambda x: x>dens+0.05, gammalst)
		fltgammalst = list(fltgammalst)
		if len(fltgammalst) == 0:
			continue
		#				
		print ('inst %s, dens %.2f, %s'%(f,dens, str(fltgammalst)))
		for gamma,alpha,beta,itrcnt in itertools.product(fltgammalst, alphalst, betalst, range(runcnt)):			
			#print 'Inst (%s %d) alpha: %d, beta %.2f'%(f,gamma,alpha, beta)			
			randnum = random.randint(0,2147483647)
			appPath = '.' + os.sep + 'hc3' + os.sep + 'bin' + os.sep + 'hc3'
			cmdstr = appPath + r' -f %s -k %.2f -a %.2f -c %d -t %d'%(fulpath, gamma, alpha, randnum, tmcutoff)
			cmdstr = cmdstr + ' > output' + os.sep + str(cnt) + '.out'
			cnt = cnt + 1
			#print cmdstr
			cmdfile.write(cmdstr+'\n')

					
					
					
