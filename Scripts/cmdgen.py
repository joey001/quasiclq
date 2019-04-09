import os
import sys
import os.path
import random
import itertools

fold = sys.argv[2];
cmdPath = sys.argv[3];
cmdfile = open(cmdPath,'w')

gammalst = (0.80, 0.85, 0.90, 0.95)
alphalst = (1,2,4,8,16,32,64)
betalst = (0.1,0.2,0.4,0.8,1.6,3.2,6.4)
runcnt = 10
tmcutoff = 240


def density(f):
	if not f.endswith('.clq'):
		return None
	for line in open(f).readlines():
		if line[0] == 'p':
			pstr,estr,vnstr,enstr = line.split()
			vn = int(vnstr)
			en = int(enstr)			
			return float(2*en)/(vn*(vn-1))
			
for root, dir, files in os.walk(fold):
	for f in files:
		fulpath = os.path.join(root, f)
		dens = density(fulpath)		
		fltgammalst = filter(lambda x: x>dens+0.05, gammalst)
		#				
		print ('inst %s, dens %.2f, %s'%(f,dens, fltgammalst))
		for gamma,alpha,beta,itrcnt in itertools.product(fltgammalst, alphalst, betalst, range(5)):			
			#print 'Inst (%s %d) alpha: %d, beta %.2f'%(f,gamma,alpha, beta)			
			randnum = random.randint(0,2147483647)
			cmdstr = r'./mqcp -f %s -k %.2f -a %d -b %.2f -c %d -t %d'%(fulpath, gamma, alpha, beta, randnum, tmcutoff)
			#print cmdstr
			cmdfile.write(cmdstr+'\n')

					
					
					
