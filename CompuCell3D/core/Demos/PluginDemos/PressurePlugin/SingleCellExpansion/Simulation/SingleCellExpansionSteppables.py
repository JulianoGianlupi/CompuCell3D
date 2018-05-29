
from PySteppables import *
import CompuCell
import sys
class SingleCellExpansionSteppable(SteppableBasePy):

    def __init__(self,_simulator,_frequency=1):
        SteppableBasePy.__init__(self,_simulator,_frequency)
    def start(self):
        # any code in the start function runs before MCS=0
        for cell in self.cellList:
            cell.Pressure = 1000.
    def step(self,mcs):        
        #type here the code that will run every _frequency MCS
        pass
    def finish(self):
        # Finish Function gets called after the last MCS
        pass
        