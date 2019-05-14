
#include <CompuCell3D/CC3D.h>        
using namespace CompuCell3D;
using namespace std;

#include "ImplicitMotilityPlugin.h"


ImplicitMotilityPlugin::ImplicitMotilityPlugin():
pUtils(0),
lockPtr(0),
xmlData(0) ,
cellFieldG(0),
boundaryStrategy(0)
{}

ImplicitMotilityPlugin::~ImplicitMotilityPlugin() {
    pUtils->destroyLock(lockPtr);
    delete lockPtr;
    lockPtr=0;
}

void ImplicitMotilityPlugin::init(Simulator *simulator, CC3DXMLElement *_xmlData) {
    xmlData=_xmlData;
    sim=simulator;
    potts=simulator->getPotts();
    cellFieldG = (WatchableField3D<CellG *> *)potts->getCellFieldG();

	bool pluginAlreadyRegisteredFlag;
	Plugin *plugin = Simulator::pluginManager.get("CenterOfMass", &pluginAlreadyRegisteredFlag); //this will load CenterOfMass plugin if it is not already loaded
	if (!pluginAlreadyRegisteredFlag)
		plugin->init(simulator);
    
    pUtils=sim->getParallelUtils();
    lockPtr=new ParallelUtilsOpenMP::OpenMPLock_t;
    pUtils->initLock(lockPtr); 
   
   update(xmlData,true);
   
    potts->getCellFactoryGroupPtr()->registerClass(&implicitMotilityDataAccessor);
    potts->registerEnergyFunctionWithName(this,"ImplicitMotility");
    
	fieldDim = potts->getCellFieldG()->getDim();

	boundaryStrategy = BoundaryStrategy::getInstance();
	adjNeighbor.initialize(fieldDim);
	adjNeighbor_ptr = &adjNeighbor;

	if (potts->getBoundaryXName() == "Periodic") {
		adjNeighbor.setPeriodicX();
		boundaryConditionIndicator.x = 1;
	}
	if (potts->getBoundaryYName() == "Periodic") {
		adjNeighbor.setPeriodicY();
		boundaryConditionIndicator.y = 1;
	}
	if (potts->getBoundaryZName() == "Periodic") {
		adjNeighbor.setPeriodicZ();
		boundaryConditionIndicator.z = 1;
	}




    
    simulator->registerSteerableObject(this);
}

void ImplicitMotilityPlugin::extraInit(Simulator *simulator){
	update(xmlData, true);
}




double ImplicitMotilityPlugin::changeEnergyByCellType(const Point3D &pt,const CellG *newCell,const CellG *oldCell) {	

    double energy = 0;
    if (oldCell)
	{
		Coordinates3D<double> oldCOMAfterFlip = precalculateCentroid(pt, oldCell, -1, fieldDim, boundaryStrategy);

		if (oldCell->volume > 1)
		{
			oldCOMAfterFlip.XRef() = oldCOMAfterFlip.X() / (float)(oldCell->volume - 1);
			oldCOMAfterFlip.YRef() = oldCOMAfterFlip.Y() / (float)(oldCell->volume - 1);
			oldCOMAfterFlip.ZRef() = oldCOMAfterFlip.Z() / (float)(oldCell->volume - 1);
		}else
		{
			oldCOMAfterFlip = Coordinates3D<double>(oldCell->xCM / oldCell->volume, oldCell->zCM / oldCell->volume, oldCell->zCM / oldCell->volume);
		}

		Coordinates3D<double> oldCOMBeforeFlip(oldCell->xCM / oldCell->volume, oldCell->yCM / oldCell->volume, oldCell->zCM / oldCell->volume);
		Coordinates3D<double> distVector = distanceVectorCoordinatesInvariant(oldCOMAfterFlip, oldCOMBeforeFlip, fieldDim);
		
		//note, in the update bias vec function you're gonna write (juliano) multiply
		//the vector by lambda 
		//kindly, juliano from the past
		energy += distVector.X()*externalPotentialParamVector[oldCell->type].biasVec.X()
				+ distVector.Y()*externalPotentialParamVector[newCell->type].biasVec.Y()
				+ distVector.Z()*externalPotentialParamVector[newCell->type].biasVec.Z();

    }
    
    if(newCell){
        //PUT YOUR CODE HERE
    }else{
        //PUT YOUR CODE HERE
    }
    
    return energy;    
}            


void ImplicitMotilityPlugin::update(CC3DXMLElement *_xmlData, bool _fullInitFlag){
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


std::string ImplicitMotilityPlugin::toString(){
    return "ImplicitMotility";
}


std::string ImplicitMotilityPlugin::steerableName(){
    return toString();
}
