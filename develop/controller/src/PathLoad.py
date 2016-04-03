import bge
from bge import logic
path = logic.expandPath("//paths\\")
    
nodes = []

def load():
    i = 0
    file = open(path+'path.csv','r')
    for line in file:
        nodes.append([int(x) for x in line.split(',')])
        i += 1

def getNodes():
    return nodes

def main():

    cont = bge.logic.getCurrentController()
    own = cont.owner

    mouseOver = cont.sensors["mouseOver"]
    mouseDown = cont.sensors["mouseDown"]
    if (mouseOver.positive and mouseDown.positive and own['added'] == False):
        load()
        own['added'] = True