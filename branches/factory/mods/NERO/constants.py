# Dimensions of the arena
XDIM = 800
YDIM = 400

# Height of the wall
HEIGHT = 20
OFFSET = -HEIGHT/2

MAX_MOVEMENT_SPEED = 12
MAX_VISION_RADIUS = 100

# Population size
pop_size = 40

OBJECT_TYPE_OBSTACLE  = (1 << 0) # object type for walls
OBJECT_TYPE_AGENT = (1 << 1) # object type for agent
OBJECT_TYPE_FLAG = (1 << 2) # object type for the flag
OBJECT_TYPE_LEVEL_GEOM = (1 << 3) # object type for the level geometry

############################
### SENSOR CONFIGURATION ###
############################

# Wall Ray Sensors
# node 9 1 1 1 RaySensor 1 16777216 16777220 180 0 50
# node 10 1 1 1 RaySensor 1 16777216 16777220 135 0 50
# node 11 1 1 1 RaySensor 1 16777216 16777220 90 0 50
# node 12 1 1 1 RaySensor 1 16777216 16777220 45 0 50
# node 13 1 1 1 RaySensor 1 16777216 16777220 0 0 50
WALL_SENSORS = [-90, -45, 0, 45, 90]

# Flag Radar Sensors
# node 14 1 1 1 FlagRadarSensor 2 180 102 90 -90 300
# node 15 1 1 1 FlagRadarSensor 1 108 87 90 -90 300
# node 16 1 1 1 FlagRadarSensor 1 93 72 90 -90 300
# node 17 1 1 1 FlagRadarSensor 2 78 0 90 -90 300
# node 18 1 1 1 FlagRadarSensor 5 3 177 90 -90 300
FLAG_SENSORS = [(-90, -12), (-18, 3), (-3, 18), (12, 90), (87, -87)]

# Number of network inputs
NEAT_SENSORS = len(WALL_SENSORS) + len(FLAG_SENSORS)

# Number of network outputs
NEAT_ACTIONS = 2

# Network bias value
NEAT_BIAS = 0.3

FITNESS_STAND_GROUND = "Stand ground"
FITNESS_STICK_TOGETHER = "Stick together"
FITNESS_APPROACH_ENEMY = "Approach enemy"
FITNESS_APPROACH_FLAG = "Approach flag"
FITNESS_HIT_TARGET = "Hit target"
FITNESS_AVOID_FIRE = "Avoid fire"
FITNESS_DIMENSIONS = [FITNESS_STAND_GROUND, FITNESS_STICK_TOGETHER, 
    FITNESS_APPROACH_ENEMY, FITNESS_APPROACH_FLAG, FITNESS_HIT_TARGET, 
    FITNESS_AVOID_FIRE]

FITNESS_INDEX = dict([(f,i) for i,f in enumerate(FITNESS_DIMENSIONS)])