import sys
from os import environ
from os import getcwd
import string

sys.path.append(environ["PYTHON_MODULE_PATH"])


import CompuCellSetup



sim,simthread = CompuCellSetup.getCoreSimulationObjects()

#Create extra player5 fields here or add attributes

CompuCellSetup.initializeSimulationObjects(sim,simthread)

#Add Python steppables here
steppableRegistry=CompuCellSetup.getSteppableRegistry()


from cellsort_2D_steering_steppables import ContactSteeringAndTemperature
contactSteeringAndTemperature=ContactSteeringAndTemperature(_simulator=sim,_frequency=10)
steppableRegistry.registerSteppable(contactSteeringAndTemperature)

CompuCellSetup.mainLoop(sim,simthread,steppableRegistry)

