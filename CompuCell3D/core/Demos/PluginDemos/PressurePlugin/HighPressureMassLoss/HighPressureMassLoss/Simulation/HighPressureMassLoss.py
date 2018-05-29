
import sys
from os import environ
from os import getcwd
import string

sys.path.append(environ["PYTHON_MODULE_PATH"])


import CompuCellSetup


sim,simthread = CompuCellSetup.getCoreSimulationObjects()
        
# add extra attributes here
        
CompuCellSetup.initializeSimulationObjects(sim,simthread)
# Definitions of additional Python-managed fields go here
        
#Add Python steppables here
steppableRegistry=CompuCellSetup.getSteppableRegistry()
        
from HighPressureMassLossSteppables import HighPressureMassLossSteppable
steppableInstance=HighPressureMassLossSteppable(sim,_frequency=1)
steppableRegistry.registerSteppable(steppableInstance)
        

from HighPressureMassLossSteppables import pressureField
instanceOfpressureField=pressureField(_simulator=sim,_frequency=1)
steppableRegistry.registerSteppable(instanceOfpressureField)

CompuCellSetup.mainLoop(sim,simthread,steppableRegistry)
        
        