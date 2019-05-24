

#include <CompuCell3D/CC3D.h>

using namespace CompuCell3D;

using namespace std;

#include "BiasVectorSteppable.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <BasicUtils/BasicRandomNumberGenerator.h>

BiasVectorSteppable::BiasVectorSteppable() : cellFieldG(0),sim(0),potts(0),xmlData(0),boundaryStrategy(0),automaton(0),cellInventoryPtr(0){}

BiasVectorSteppable::~BiasVectorSteppable() {
}


void BiasVectorSteppable::init(Simulator *simulator, CC3DXMLElement *_xmlData) {
	cerr << "fot into biasvec step" << endl; //it gets to here <-
	
	xmlData=_xmlData;//the problem was the xml data being a nullptr
  
  potts = simulator->getPotts();
  cellInventoryPtr=& potts->getCellInventory();
  sim=simulator;
  cellFieldG = (WatchableField3D<CellG *> *)potts->getCellFieldG();
  fieldDim=cellFieldG->getDim();

  potts->getCellFactoryGroupPtr()->registerClass(&biasVectorSteppableDataAccessor);
  simulator->registerSteerableObject(this);

  update(_xmlData,true);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BiasVectorSteppable::extraInit(Simulator *simulator){
    //PUT YOUR CODE HERE
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BiasVectorSteppable::start(){

  //PUT YOUR CODE HERE

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BiasVectorSteppable::step(const unsigned int currentStep){
    //REPLACE SAMPLE CODE BELOW WITH YOUR OWN
	CellInventory::cellInventoryIterator cInvItr;
	CellG * cell=0;

	BasicRandomNumberGenerator *rand = BasicRandomNumberGenerator::getInstance();

	
    
    cerr<<"currentStep="<<currentStep<<endl;
	for(cInvItr=cellInventoryPtr->cellInventoryBegin() ; cInvItr !=cellInventoryPtr->cellInventoryEnd() ;++cInvItr )
	{
		cell=cellInventoryPtr->getCell(cInvItr);
		
		//if it is a 2D simulation
		if (fieldDim.x == 1 || fieldDim.y == 1 || fieldDim.z == 1)			
		{
			// need to add a switch to select which of the ccordinates will be 0ed
			double angle = rand->getRatio() * 2 * M_PI;

			double x = std::cos(angle);
			double y = std::sin(angle);

			cell->biasVecX = x;
			cell->biasVecY = y;
			cell->biasVecZ = 0;

		}

		else
		{
			//method for getting random unitary vector in sphere from Marsaglia 1972
			//example and reason for not using a uniform distribution
			//can be found @ mathworld.wolfram.com/SpherePointPicking.html
			double tx = 2 * rand->getRatio() - 1;
			double ty = 2 * rand->getRatio() - 1;

			double dist = std::sqrt(tx*tx + ty*ty);

			while (dist > 1)
			{
				double tx = 2 * rand->getRatio() - 1;
				double ty = 2 * rand->getRatio() - 1;


				dist = std::sqrt(tx*tx + ty*ty);

			}

			if (dist <= 1)
			{
				double x = 2 * tx*std::sqrt(1 - tx*tx - ty*ty);
				double y = 2 * ty * std::sqrt(1 - tx*tx - ty*ty);
				double z = 1 - 2 * (tx*tx + ty*ty);

				cell->biasVecX = x;
				cell->biasVecY = y;
				cell->biasVecZ = z;
			}
		}
        //cerr<<"cell.id="<<cell->id<<" vol="<<cell->volume<<endl;
    }

}


void BiasVectorSteppable::update(CC3DXMLElement *_xmlData, bool _fullInitFlag){

    //PARSE XML IN THIS FUNCTION
    //For more information on XML parser function please see CC3D code or lookup XML utils API
    automaton = potts->getAutomaton();
    ASSERT_OR_THROW("CELL TYPE PLUGIN WAS NOT PROPERLY INITIALIZED YET. MAKE SURE THIS IS THE FIRST PLUGIN THAT YOU SET", automaton)
   set<unsigned char> cellTypesSet;

    CC3DXMLElement * exampleXMLElem=_xmlData->getFirstElement("Example");
    if (exampleXMLElem){
        double param=exampleXMLElem->getDouble();
        cerr<<"param="<<param<<endl;
        if(exampleXMLElem->findAttribute("Type")){
            std::string attrib=exampleXMLElem->getAttribute("Type");
            // double attrib=exampleXMLElem->getAttributeAsDouble("Type"); //in case attribute is of type double
            cerr<<"attrib="<<attrib<<endl;
        }
    }
    
    //boundaryStrategy has information aobut pixel neighbors 
    boundaryStrategy=BoundaryStrategy::getInstance();

}

std::string BiasVectorSteppable::toString(){
   return "BiasVectorSteppable";
}

std::string BiasVectorSteppable::steerableName(){
   return toString();
}
        
