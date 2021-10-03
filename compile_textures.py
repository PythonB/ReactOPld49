import os
from PIL import Image

def convert(name):
    im = Image.open("textures/"+name, 'r').convert("RGBA")
    data = im.getdata()
    out = open("data/"+name.split(".")[0]+".dat", "wb")
    for pixel in data:
        r, g, b, i = pixel
        if i==0:
            r = 40
            b = 40
            g = 40
        out.write(r.to_bytes(1, byteorder='big'))
        out.write(g.to_bytes(1, byteorder='big'))
        out.write(b.to_bytes(1, byteorder='big'))
        out.write(i.to_bytes(1, byteorder='big'))

files = [f for f in os.listdir("textures/") if os.path.isfile(os.path.join("textures/", f))]
images = []

for file in files:
    if file.split(".")[1] == "png" or file.split(".")[1] == "PNG":
        images.append(file)
    else:
        print("Non-suitable file found: {0}".format(file))

for file in images:
    print("Compiling {0} -> {1}".format(file, file.split(".")[0]+".dat"))
    convert(file)