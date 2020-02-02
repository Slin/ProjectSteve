from PIL import Image as img
import os
import math

files = sorted(filter(lambda str: str[-4:] == ".png" , os.listdir('combo')))

z = float(len(files))
y = math.sqrt(z) * 3. / 4.
x = y * 16. / 9.

x = int(x)
y = int(y)



out = img.new('RGBA', (x * 128, y * 128 ))
for f in range(4):
	print ('f: ', f)
	for i in range(x):
		for j in range(y):
			image = img.open('combo/' + files[i * y + j]).crop((0,128 * f,128,(f+1) * 128));
			out.paste(image, (i * 128, j * 128, (i+1) * 128, (j+1)*128));
	out.save("out"+ str(f)  + ".png");