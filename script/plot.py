import matplotlib.pyplot as plt
import os
import re
import csv
import seaborn as sns
import pandas as pd

sns.set()

solutions = {}
root = 'output'

def getName(contents):
	name = re.findall("/([^/]*?).clq", contents)
	return name[0]

def getGamma(contents):
	gamma = re.findall("para_gamma=(.*?)\n", contents)
	return gamma[0]

def getAlpha(contents):
	alpha = re.findall("para_alpha=(.*?)\n", contents)
	return alpha[0]

def getBeta(contents):
	beta = re.findall("para_beta=(.*?)\n", contents)
	return beta[0]

def getObjv(contents):
	objv = re.findall("objv=(.*?)\n", contents)
	return objv[0]

def dealFile(path):
	file = open(path)
	contents = file.read()

	name = getName(contents)
	gamma = getGamma(contents)
	alpha = getAlpha(contents)
	beta = getBeta(contents)
	objv = getObjv(contents)

	objv = int(objv)

	t = (name, gamma, alpha, beta)

	# print(t)
	if (t in solutions):
		solutions[t] = solutions[t] + objv
	else:
		solutions[t] = objv

def writeToCsv(name, gamma):
	headers = ['alpha', 'beta', 'objv']
	with open('solution.csv', 'w') as f:
		f_csv = csv.writer(f)
		f_csv.writerow(headers)
		for key in solutions:
			if (key[0] == name and key[1] == gamma):
				f_csv.writerow([key[2], key[3], solutions[key]])
		f.close()


for parent, dirnames, filenames in os.walk(root):
	for filename in filenames:
		path = os.path.join(parent,filename)
		if (path[-3 : len(path)] == 'out'):
			dealFile(path)

for key in solutions:
	solutions[key] = solutions[key] / 5

# writeToCsv('C4000.5', '0.95')

nameGamma = set()

for key in solutions:
	ng = (key[0], key[1])
	if (ng in nameGamma):
		continue
	
	nameGamma.add(ng)

	print(ng)

	writeToCsv(ng[0], ng[1])

	df = pd.read_csv('solution.csv')
	dfs = df.pivot('alpha', 'beta', 'objv')

	print(dfs)

	f, ax = plt.subplots(figsize=(9, 6))
	sns.heatmap(dfs,cmap="YlGnBu")
	ax.invert_yaxis()
	plt.title(ng[0]+ '_' + ng[1])
	figPath = 'image' + os.sep + ng[0] + '_' + ng[1] + '.png'
	print(figPath)
	plt.savefig(figPath)
plt.show()
os.remove('solution.csv')