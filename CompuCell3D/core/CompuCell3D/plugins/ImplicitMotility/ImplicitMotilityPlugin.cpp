
//jfg

#include <CompuCell3D/CC3D.h>        
using namespace CompuCell3D;
using namespace std;

#include "ImplicitMotilityPlugin.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <BasicUtils/BasicRandomNumberGenerator.h>

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

    
	//I'm going to declare the bias vector here for now, however it updating every spin flip attempt is no bueno.
	//It should update n times every mcs for each cell.
	//So bias vector needs to be a cell object thing.
	BasicRandomNumberGenerator *rand = BasicRandomNumberGenerator::getInstance();
	//std::uniform_real_distribution<double> dist(0.0, 1.0);
	//spherical coordinates using ISO convention (ISO 80000-2). theta is polar angle, phi is azimuth.
	double tmp_theta = M_PI * rand->getRatio(); //random theta and phi in their respective ranges
	double tmp_phi = 2 * M_PI * rand->getRatio();

	//need to add check 2D, then set phi = pi/2
	Coordinates3D<double> biasVec(std::cos(tmp_theta) * std::sin(tmp_phi),
		std::sin(tmp_theta) * std::sin(tmp_phi),
		std::cos(tmp_phi));
	
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
		
		
		energy -= externalPotentialParamVector[oldCell->type].lambdaMotility*
			(distVector.X()*biasVec.X() + distVector.Y()*biasVec.X() + distVector.Z()*biasVec.X());

    }
    
    if(newCell)
	{
		Coordinates3D<double> newCOMAfterFlip = precalculateCentroid(pt, newCell, 1, fieldDim, boundaryStrategy);


		newCOMAfterFlip.XRef() = newCOMAfterFlip.X() / (float)(newCell->volume + 1);
		newCOMAfterFlip.YRef() = newCOMAfterFlip.Y() / (float)(newCell->volume + 1);
		newCOMAfterFlip.ZRef() = newCOMAfterFlip.Z() / (float)(newCell->volume + 1);


		Coordinates3D<double> newCOMBeforeFlip(newCell->xCM / newCell->volume, newCell->yCM / newCell->volume, newCell->zCM / newCell->volume);
		Coordinates3D<double> distVector = distanceVectorCoordinatesInvariant(newCOMAfterFlip, newCOMBeforeFlip, fieldDim);
		//externalPotentialParamVector[newCell->type]
		energy -= externalPotentialParamVector[newCell->type].lambdaMotility*
			(distVector.X()*biasVec.X() + distVector.Y()*biasVec.X() + distVector.Z()*biasVec.X());
    }
    
    return energy;    
}            

double ImplicitMotilityPlugin::changeEnergyByCellId(const Point3D &pt, const CellG *newCell,
															const CellG *oldCell)
{
	
	//I'm going to declare the bias vector here for now, however it updating every spin flip attempt is no bueno.
	//It should update n times every mcs for each cell.
	//So bias vector needs to be a cell object thing.
	BasicRandomNumberGenerator *rand = BasicRandomNumberGenerator::getInstance();
	//std::uniform_real_distribution<double> dist(0.0, 1.0);
	//spherical coordinates using ISO convention (ISO 80000-2). theta is polar angle, phi is azimuth.
	double tmp_theta = M_PI * rand->getRatio(); //random theta and phi in their respective ranges
	double tmp_phi = 2 * M_PI * rand->getRatio();

	//need to add check 2D, then set phi = pi/2
	Coordinates3D<double> biasVec(std::cos(tmp_theta) * std::sin(tmp_phi),
						 std::sin(tmp_theta) * std::sin(tmp_phi),
						  std::cos(tmp_phi));
	
	
	double energy = 0.0;
	if (oldCell) {
		Coordinates3D<double> oldCOMAfterFlip = precalculateCentroid(pt, oldCell, -1, fieldDim, boundaryStrategy);

		if (oldCell->volume>1) {
			oldCOMAfterFlip.XRef() = oldCOMAfterFlip.X() / (float)(oldCell->volume - 1);
			oldCOMAfterFlip.YRef() = oldCOMAfterFlip.Y() / (float)(oldCell->volume - 1);
			oldCOMAfterFlip.ZRef() = oldCOMAfterFlip.Z() / (float)(oldCell->volume - 1);
		}
		else {

			oldCOMAfterFlip = Coordinates3D<double>(oldCell->xCM / oldCell->volume, oldCell->zCM / oldCell->volume, oldCell->zCM / oldCell->volume);

		}

		Coordinates3D<double> oldCOMBeforeFlip(oldCell->xCM / oldCell->volume, oldCell->yCM / oldCell->volume, oldCell->zCM / oldCell->volume);
		Coordinates3D<double> distVector = distanceVectorCoordinatesInvariant(oldCOMAfterFlip, oldCOMBeforeFlip, fieldDim);
		energy -= oldCell->lambdaMotility*
			(distVector.X()*biasVec.X() + distVector.Y()*biasVec.X() + distVector.Z()*biasVec.X());
		//negative because it'd be confusing for users to have to define a negative lambda to go to a positive direction
	}


	if (newCell) {

		Coordinates3D<double> newCOMAfterFlip = precalculateCentroid(pt, newCell, 1, fieldDim, boundaryStrategy);


		newCOMAfterFlip.XRef() = newCOMAfterFlip.X() / (float)(newCell->volume + 1);
		newCOMAfterFlip.YRef() = newCOMAfterFlip.Y() / (float)(newCell->volume + 1);
		newCOMAfterFlip.ZRef() = newCOMAfterFlip.Z() / (float)(newCell->volume + 1);


		Coordinates3D<double> newCOMBeforeFlip(newCell->xCM / newCell->volume, newCell->yCM / newCell->volume, newCell->zCM / newCell->volume);
		Coordinates3D<double> distVector = distanceVectorCoordinatesInvariant(newCOMAfterFlip, newCOMBeforeFlip, fieldDim);

		energy -= newCell->lambdaMotility*
			(distVector.X()*biasVec.X() + distVector.Y()*biasVec.X() + distVector.Z()*biasVec.X());
		//negative because it'd be confusing for users to have to define a negative lambda to go to a positive direction
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
