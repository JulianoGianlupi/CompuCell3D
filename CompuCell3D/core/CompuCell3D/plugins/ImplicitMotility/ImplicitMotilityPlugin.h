
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
			
			lambdaMotility = 0.0;
		}
		
		double lambdaMotility;
		std::string typeName;
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

		std::vector<ImplicitMotilityParam> motilityParamVector;
		Coordinates3D<double> biasVecTmp;


		enum FunctionType { GLOBAL = 0, BYCELLTYPE = 1, BYCELLID = 2 };
		FunctionType functionType;
        
    public:

        ImplicitMotilityPlugin();
        virtual ~ImplicitMotilityPlugin();
        
        BasicClassAccessor<ImplicitMotilityData> * getImplicitMotilityDataAccessorPtr(){return & implicitMotilityDataAccessor;}                

        
        //Energy function interface
		typedef double (ImplicitMotilityPlugin::*changeEnergy_t)(const Point3D &pt, const CellG *newCell, const CellG *oldCell);
		ImplicitMotilityPlugin::changeEnergy_t changeEnergyFcnPtr;
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
        