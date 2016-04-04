import bge
import os
path = bge.logic.expandPath("//paths\\")

def main():

    cont = bge.logic.getCurrentController()
    own = cont.owner
    scene = bge.logic.getCurrentScene()
    files = [f for f in os.listdir(path) if f.endswith(".csv")]
        
    i = 0
    for file in files:
        obj = "File"+('%02d' % i)
        scene.objects[obj].text = files[i]
        i += 1
        if (i == 12):
            break
    own["max"] = i
    if (i < 12):
        for e in range(i, 12):
            obj = "File"+('%02d' % e)
            scene.objects[obj].text = ''
    
main()