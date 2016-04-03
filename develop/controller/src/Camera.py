import bge


def reset():

    cont = bge.logic.getCurrentController()
    own = cont.owner
        
    own.localOrientation = (0, 0, 0)
