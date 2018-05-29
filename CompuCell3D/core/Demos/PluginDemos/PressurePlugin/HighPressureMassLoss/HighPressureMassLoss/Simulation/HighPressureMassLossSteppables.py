from PlayerPython import * 
import CompuCellSetup

from PySteppables import *
import CompuCell
import sys
class HighPressureMassLossSteppable(SteppableBasePy):

    def __init__(self,_simulator,_frequency=1):
        SteppableBasePy.__init__(self,_simulator,_frequency)
    def start(self):
        # any code in the start function runs before MCS=0
        
        for cell in self.cellListByType(self.HIGHPRESSURE):  
            cell.Pressure = 15
        for cell in self.cellListByType(self.LOWPRESSURE):  
            cell.Pressure = 10
    def step(self,mcs):        
        #type here the code that will run every _frequency MCS
        for cell in self.cellList:
            print "cell.id=",cell.id
    def finish(self):
        # Finish Function gets called after the last MCS
        pass
        
from PySteppables import *
import CompuCell
import sys

from PlayerPython import *
import CompuCellSetup
from math import *


class pressureField(SteppableBasePy):
    def __init__(self,_simulator,_frequency=1):
        SteppableBasePy.__init__(self,_simulator,_frequency)
        self.scalarCLField = self.createScalarFieldCellLevelPy("Pressure")
    def start(self):
        pass
    def step(self,mcs):
        self.scalarCLField.clear()
        for cell in self.cellList:
            self.scalarCLField[cell] = cell.Pressure
    def finish(self):
        # this function may be called at the end of simulation - used very infrequently though
        return
    
