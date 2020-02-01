from PIL import Image as img
from functools import reduce
import os

files = filter(lambda str: str[-4:] == ".png" , os.listdir())

def load(name, list) :
	return (filter(lambda str: str != name, list), img.open(name))

def name(attr):
	return  "combo/"+ reduce(lambda a,b: str(a) + str(b), attr, "") + ".png"

global antenne
(files, base) = load("base.png", files)
(files, antenne) = load("base_Antenna.png", files)

global attrsMeta
attrsMeta = {}

for  f in list(files):	
	p = f.find('_')
	key = f[0:p]
	level = int(f[p+1:p+2])
	skin = img.open(f)
	eAntenne = 0;
	if f[p+3:p+4] == "A":
		eAntenne = 1
	if key in attrsMeta:
		attrsMeta[key].append((skin, eAntenne))
	else:
		attrsMeta[key] = [(skin, eAntenne)]
global attrs
attrs = [0 for y in range(len(attrsMeta))]
attrsZ = [0 for y in range(len(attrsMeta))]
attrsList = list(map(lambda key: attrsMeta[key], list(attrsMeta)))

def addA():
	y = 0;
	carry = 1
	while carry and y < len(attrs):
		carry = 0
		attrs[y] += 1
		if  attrs[y] == len(attrsList[y]) + 1:
			attrs[y] = 0
			y += 1
			carry = 1
		
	return 0

addA()
while attrs != attrsZ:
	out = base.copy()
	eAntenne= 1
	for i in range(len(attrs)):
		if attrs[i]:
			out.alpha_composite(attrsList[i][attrs[i]-1][0])
			if not attrsList[i][attrs[i]-1][1]:
				eAntenne = 0
	if eAntenne:
		out = img.alpha_composite(antenne, out)
	out.save(name(attrs))	
	addA()

