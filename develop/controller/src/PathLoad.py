import bge
import PathInd
from bge import logic
path = logic.expandPath("//paths\\")
    
nodes = []

def load():
    cont = bge.logic.getCurrentController()
    own = cont.owner
    pathnameMsg = cont.sensors['pathnameMsg']
    if pathnameMsg.positive:
        for x in range(len(nodes)):
            nodes.pop() #reset global var
        filename = pathnameMsg.bodies[0]
        i = 0
        file = open(path+filename,'r')
        for line in file:
            nodes.append([int(x) for x in line.split(',')])
            i += 1
        own['added'] = False
        
        #Change text box
        scene = bge.logic.getCurrentScene()
        scene.objects['sideCamera']['sys'].layout.setPathBox(filename)
        
        #Display path indicator
        PathInd.update(nodes)
        

def getNodes():
    return nodes

def main():

    cont = bge.logic.getCurrentController()
    own = cont.owner
    addLoadScreen = cont.actuators['addLoadScreen']

    cont.activate(addLoadScreen)