
from PySteppables import *
import CompuCell
import sys
import numpy as np

class Log10DivisionFormulaSteppable(SteppableBasePy):

    def __init__(self,_simulator,_frequency=1):
        SteppableBasePy.__init__(self,_simulator,_frequency)
    def start(self):
        # any code in the start function runs before MCS=0
        self.pW_chemoExtr = self.addNewPlotWindow(_title='Chemotaxis energy',
                                                  _xAxisTitle='( c(max x)+1 )/( c(min x)+1 ) ',
                                                  _yAxisTitle='E/lambda',
                                                  _xScaleType='linear',
                                                  _yScaleType='linear')
        self.pW_chemoExtr.addPlot('Over All Cell', _style='Dots', _color='red', _size=5)
        self.pW_chemoExtr.addPlot('To Center Of Mass', _style='Dots', _color='blue',_size=5)
    def step(self,mcs):        
        #type here the code that will run every _frequency MCS
        field = self.getConcentrationField('attr')
        for cell in self.cellList:
            
            max_x = -1
            min_x = 999
            x_com = cell.xCOM
            pixelList = self.getCellBoundaryPixelList(cell)
            for boundaryPixelTrackerData in pixelList:
                p = boundaryPixelTrackerData.pixel
                max_x = max(max_x,p.x)
                min_x = min(min_x,p.x)
        
        c1 = field[max_x,self.dim.y//2,0]
        c2 = field[min_x,self.dim.y//2,0]
        
        cCOM = field[x_com,self.dim.y//2,0]
        
        energy = self.calculate_chemotaxis(c1,c2)
        
        self.pW_chemoExtr.addDataPoint("Over All Cell", (c1+1.)/(c2+1.), energy)  
        
        
        energy = self.calculate_chemotaxis(c1,cCOM)
        self.pW_chemoExtr.addDataPoint("To Center Of Mass", (c1+1.)/(c2+1.), energy)  
        
        
        
    def calculate_chemotaxis(self,c1,c2):
        return np.log10((c1+1.)/(c2+1.))
    def finish(self):
        # Finish Function gets called after the last MCS
        pass
        