import bge
import bgui
import bgui.bge_utils
import PathLoad
import Sender

class SimpleLayout(bgui.bge_utils.Layout):
    def __init__(self, sys, data):
        super().__init__(sys, data)
        
        self.frame = bgui.Frame(self, border=1, size=[1,0.15])
        self.frame.colors = [(0,0,0,0) for i in range(4)]
        
        #self.lbl = bgui.Label(self.frame, text = 'I sure wish someone would push that button...', pt_size=70, pos=[0,0.7], options = bgui.BGUI_CENTERX)
        self.confBtn = bgui.FrameButton(self.frame, text='Config', size=[0.14,0.3], pos=[0.05,0.55])
        self.debugBtn = bgui.FrameButton(self.frame, text='Debug', size=[0.14,0.3], pos=[0.20,0.55])
        self.loadBtn = bgui.FrameButton(self.frame, text='Load', size=[0.14,0.3], pos=[0.35,0.55])
        self.sendBtn = bgui.FrameButton(self.frame, text='Send to Hybrid', size=[0.44,0.3], pos=[0.05,0.15])
        
        self.pathBox = bgui.TextInput(self.frame, text='None', size=[0.2,0.3], pos=[0.51,0.55], input_options = bgui.BGUI_INPUT_NONE, options = bgui.BGUI_DEFAULT)
        self.pathBox.frozen = True
        
        self.confBtn.frozen = True
        self.debugBtn.frozen = True
        
        self.loadBtn.on_click = self.loadBtn_click
        self.sendBtn.on_click = self.sendBtn_click
    
    def loadBtn_click(self, widget):
        PathLoad.main()
    
    def sendBtn_click(self, widget):
        Sender.start()

def main(cont):

    own = cont.owner
    mouse = bge.logic.mouse
    
    if 'sys' not in own:
        own['sys'] = bgui.bge_utils.System('themes/default')
        own['sys'].load_layout(SimpleLayout, None)
        mouse.visible = True
    else:
        own['sys'].run()
