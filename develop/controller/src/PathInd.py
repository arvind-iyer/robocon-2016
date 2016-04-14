import bge
import math

x = 0
y = 0

x_offset = -6.5
y_offset = -6.5

def update(nodes):
    global x, y
    scene = bge.logic.getCurrentScene()
    spawner = scene.objects['arrowSpawner']
    
    for node in nodes:
        x_target = node[1]/1000
        y_target = node[2]/1000
        dist = math.sqrt(math.pow((x_target-x),2)+math.pow((y_target-y),2))
        arrowNo = math.floor(dist*4)
        dir = math.atan2((x_target-x),(y_target-y))
        print(arrowNo, dir/(math.pi)*180)
        spawner.localPosition = [0,0,0]
        spawner.worldPosition = [x+x_offset,y+y_offset,0]
        spawner.localOrientation = (0,0,-dir)
        for i in range(0, arrowNo):
            scene.addObject('Arrow', spawner)
            spawner.localPosition[0] += 0.25*math.sin(dir)
            spawner.localPosition[1] += 0.25*math.cos(dir)
        x = x_target
        y = y_target