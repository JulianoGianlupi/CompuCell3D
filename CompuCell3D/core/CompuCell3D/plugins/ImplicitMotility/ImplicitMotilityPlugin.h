
#ifndef IMPLICITMOTILITYPLUGIN_H
#define IMPLICITMOTILITYPLUGIN_H

#include <CompuCell3D/CC3D.h>
#include "ImplicitMotilityData.h"

#include "ImplicitMotilityDLLSpecifier.h"

class CC3DXMLElement;

namespace CompuCell3D {
	/*
	@author jfg
	*/
    class Simulator;

    class Potts3D;
    class Automaton;
    //class AdhesionFlexData;
    class BoundaryStrategy;
    class ParallelUtilsOpenMP;
    
    template <class T> class Field3D;
    template <class T> class WatchableField3D;

	class IMPLICITMOTILITY_EXPORT ImplicitMotilityParam 
	{
	public:
		ImplicitMotilityParam()
		{
			biasVec.x = 0.0;
			biasVec.y = 0.0;
			biasVec.z = 0.0;
		}
		Coordinates3D<float> biasVec;
		double lambdaMotility;
	};

    class IMPLICITMOTILITY_EXPORT  ImplicitMotilityPlugin : public Plugin ,public EnergyFunction  {
        
    private:    
        BasicClassAccessor<ImplicitMotilityData> implicitMotilityDataAccessor;                
        CC3DXMLElement *xmlData;        
        
        Potts3D *potts;
        
        Simulator *sim;
        
        ParallelUtilsOpenMP *pUtils;            
        
        ParallelUtilsOpenMP::OpenMPLock_t *lockPtr;        

        Automaton *automaton;

        BoundaryStrategy *boundaryStrategy;
        WatchableField3D<CellG *> *cellFieldG;

		AdjacentNeighbor  adjNeighbor;
		Dim3D fieldDim;
		AdjacentNeighbor  * adjNeighbor_ptr;
		Point3D boundaryConditionIndicator;

		std::vector<ImplicitMotilityParam> externalPotentialParamVector;
        
    public:

        ImplicitMotilityPlugin();
        virtual ~ImplicitMotilityPlugin();
        
        BasicClassAccessor<ImplicitMotilityData> * getImplicitMotilityDataAccessorPtr(){return & implicitMotilityDataAccessor;}                

        
        //Energy function interface
        virtual double changeEnergyByCellType(const Point3D &pt, const CellG *newCell, const CellG *oldCell);

		double changeEnergyByCellId(const Point3D & pt, const CellG * newCell, const CellG * oldCell);
        
                
        
        virtual void init(Simulator *simulator, CC3DXMLElement *_xmlData=0);

        virtual void extraInit(Simulator *simulator);

        //Steerrable interface
        virtual void update(CC3DXMLElement *_xmlData, bool _fullInitFlag=false);
        virtual std::string steerableName();
        virtual std::string toString();

    };
};
#endif
        
