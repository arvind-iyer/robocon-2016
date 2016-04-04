import bge
import math

def update():

    cont = bge.logic.getCurrentController()
    own = cont.owner
    pos = bge.logic.getCurrentScene().objects["Window"]["select"]
    
    own.localPosition = [0, (0.89 - 0.15*pos), 0.05]
    
def mouseOver():
    cont = bge.logic.getCurrentController()
    own = cont.owner
    mouseOver = cont.sensors['mouseOver']
    
    if mouseOver.positive:
        mousePos = bge.logic.mouse.position[1]
        select = math.floor((mousePos-0.210)/(0.745-0.210)*12)
        own["select"] = max(min(select, own["max"]-1),0)

def mouseDown():
    cont = bge.logic.getCurrentController()
    own = cont.owner
    
    scene = bge.logic.getCurrentScene()
    obj = "File"+('%02d' % scene.objects["Window"]["select"])
    
    mouseOver = cont.sensors['mouseOver']
    mouseDown = cont.sensors['mouseDown']
    selected = cont.actuators['selected']
    
    if mouseOver.positive and mouseDown.positive:
        selected.body = scene.objects[obj].text
        cont.activate(selected)