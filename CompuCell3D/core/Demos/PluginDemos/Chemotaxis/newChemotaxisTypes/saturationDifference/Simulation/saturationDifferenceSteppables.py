
from PySteppables import *
import CompuCell
import sys
class saturationDifferenceSteppable(SteppableBasePy):

    def __init__(self,_simulator,_frequency=1):
        SteppableBasePy.__init__(self,_simulator,_frequency)
    def start(self):
        # any code in the start function runs before MCS=0
        pass
    def step(self,mcs):        
        #type here the code that will run every _frequency MCS
        pass
        
        for cell in self.cellList:
            
            cd = self.chemotaxisPlugin.getChemotaxisData(cell, "ATTR")
            print cd.si_fude_mano()
        
        
        
        
    def finish(self):
        # Finish Function gets called after the last MCS
        pass
        