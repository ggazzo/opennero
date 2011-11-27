from OpenNero import *
from random import seed, randint

# add the key and mouse bindings
from inputConfig import createInputMapping

from common import *
import common.gui as gui

from BlocksTower.module import getMod, delMod
from BlocksTower.constants import *

AGENTS = [
    ('Problem reduction', lambda: getMod().start_tower1() ),
    ('State-space search', lambda: getMod().start_tower2() ),
    ('Goal stacking', lambda: getMod().start_tower3() ),
]

class UI:
    pass

def CreateGui(guiMan):
    print 'CreateGui'
    guiMan.setTransparency(1.0)
    guiMan.setFont("data/gui/fonthaettenschweiler.bmp")

    ui = UI() # a collection of all the UI elements

    window_width = 250 # width
    control_height = 30  # height

    # AGENT SELECTION BOX
    x, y = 5, 2 * control_height + 5
    w, h = window_width - 15, control_height - 10
    ui.agentBoxLabel = gui.create_text(guiMan, 'agentLabel', Pos2i(x,y), Pos2i(3*w/10,h), 'Agent Type:')
    ui.agentComboBox = gui.create_combo_box(guiMan, "agentComboBox", Pos2i(x + 5 + 3*w/10, y), Pos2i(7*w/10, h))
    for agent_name, agent_function in AGENTS:
        ui.agentComboBox.addItem(agent_name)

    # START/RESET AND PAUSE/CONTINUE AGENT BUTTONS AND HELP BUTTON
    x, y = 5, 0 * control_height + 5
    w, h = (window_width - 20) / 3, control_height - 5
    ui.startAgentButton = gui.create_button(guiMan, 'startAgentButton', Pos2i(x, y), Pos2i(w, h), '')
    ui.pauseAgentButton = gui.create_button(guiMan, 'pauseAgentButton', Pos2i(x + w + 5, y), Pos2i(w, h), '')
    ui.helpButton = gui.create_button(guiMan, 'helpButton', Pos2i(x + 2*w + 10, y), Pos2i(w, h), '')
    ui.startAgentButton.text = 'Start'
    ui.pauseAgentButton.text = 'Pause'
    ui.helpButton.text = 'Help'
    ui.pauseAgentButton.enabled = False
    ui.startAgentButton.OnMouseLeftClick = startAgent(ui)
    ui.pauseAgentButton.OnMouseLeftClick = pauseAgent(ui)
    ui.helpButton.OnMouseLeftClick = openWiki('BlocksWorldMod')

    # SPEEDUP SLIDER
    x, y = 5, 1 * control_height + 5
    w, h = window_width - 20, control_height - 5
    ui.speedupLabel = gui.create_text(guiMan, 'speedupLabel', Pos2i(x, y), Pos2i(3*w/10, h), 'Speedup:')
    ui.speedupScroll = gui.create_scroll_bar(guiMan, 'speedupScroll', Pos2i(x + 5 + 3*w/10, y), Pos2i(3*w/5, h-5), True)
    ui.speedupValue = gui.create_text(guiMan, 'speedupEditBox', Pos2i(x + 10 + 9*w/10, y), Pos2i(w/10, h), str(0))
    ui.speedupScroll.setMax(100)
    ui.speedupScroll.setLargeStep(10)
    ui.speedupScroll.setSmallStep(1)
    ui.speedupScroll.setPos(0)
    getMod().set_speedup(0)
    ui.speedupScroll.OnScrollBarChange = speedup_adjusted(ui)
    
    # THE WINDOW THAT HOLDS ALL THE CONTROLS ABOVE
    ui.agentWindow = gui.create_window(guiMan, 'agentWindow', Pos2i(10, 10), Pos2i(window_width, 3*control_height+25), 'Agent')
    ui.agentWindow.addChild(ui.agentBoxLabel)
    ui.agentWindow.addChild(ui.agentComboBox)
    ui.agentWindow.addChild(ui.startAgentButton)
    ui.agentWindow.addChild(ui.pauseAgentButton)
    ui.agentWindow.addChild(ui.helpButton)
    ui.agentWindow.addChild(ui.speedupLabel)
    ui.agentWindow.addChild(ui.speedupScroll)
    ui.agentWindow.addChild(ui.speedupValue)

def speedup_adjusted(ui):
    """generate a closure that will be called whenever the speedup slider is adjusted"""
    ui.speedupValue.text = str(ui.speedupScroll.getPos())
    getMod().set_speedup(float(ui.speedupScroll.getPos())/100)
    def closure():
        """called whenever the speedup slider is adjusted"""
        ui.speedupValue.text = str(ui.speedupScroll.getPos())
        getMod().set_speedup(float(ui.speedupScroll.getPos())/100)
    return closure

def startAgent(ui):
    """ return a function that starts or stops the agent """
    def closure():
        """starts or stops the agent"""
        if ui.startAgentButton.text == 'Start':
            i = ui.agentComboBox.getSelected()
            (agent_name, agent_function) = AGENTS[i]
            print 'Starting', agent_name
            agent_function()
            ui.pauseAgentButton.text = 'Pause'
            ui.pauseAgentButton.enabled = True
            ui.startAgentButton.text = 'Reset'
            ui.agentComboBox.enabled = False
        else:
            getMod().stop_agent()
            disable_ai()
            get_environment().cleanup()
            ui.startAgentButton.text = 'Start'
            ui.pauseAgentButton.text = 'Pause'
            ui.pauseAgentButton.enabled = False
            ui.agentComboBox.enabled = True
    return closure

def pauseAgent(ui):
    """ return a function that pauses and continues the agent """
    def closure():
        """pauses and continues the agent"""
        if ui.pauseAgentButton.text == 'Continue':
            ui.pauseAgentButton.text = 'Pause'
            enable_ai()
        else:
            ui.pauseAgentButton.text = 'Continue'
            disable_ai()
    return closure

def recenter(cam):
    def closure():
        cam.setPosition(Vector3f(NUDGE_X, NUDGE_Y, 30))
        cam.setTarget(Vector3f(NUDGE_X + GRID_DX * ROWS / 2, NUDGE_Y + GRID_DY * COLS / 2, 5))
    return closure

def ClientMain():
    # create fog effect
    getSimContext().setFog()

    # don't show physics
    # disable_physics()

    # add a camera
    camRotateSpeed = 100
    camMoveSpeed   = 3000
    camZoomSpeed   = 100
    cam = getSimContext().addCamera(camRotateSpeed, camMoveSpeed, camZoomSpeed)
    cam.setFarPlane(5000)
    cam.setEdgeScroll(False)
    recenter_cam = recenter(cam) # create a closure to avoid having a global variable
    recenter_cam() # call the recenter function

    # load the background
    #addObject("data/terrain/Sea.xml", Vector3f(-3000 + NUDGE_X,-3000 + NUDGE_Y,-20))
    addObject("data/terrain/FlatTerrain.xml", Vector3f(-500,-500,0), Vector3f(0,0,0)) #Vector3f(-1100 + NUDGE_X, -2400 + NUDGE_Y, -17), Vector3f(0,0,-45))
    addSkyBox("data/sky/irrlicht2")

    # load the maze
    getSimContext().addLightSource(Vector3f(-500,-500,1000), 1500)
    getMod().add_maze()

    # load the GUI
    CreateGui(getGuiManager())

    # create the key binding
    ioMap = createInputMapping()
    ioMap.BindKey( "KEY_SPACE", "onPress", recenter_cam )
    getSimContext().setInputMapping(ioMap)