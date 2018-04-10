/*************************************************************************
*    CompuCell - A software framework for multimodel simulations of     *
* biocomplexity problems Copyright (C) 2003 University of Notre Dame,   *
*                             Indiana                                   *
*                                                                       *
* This program is free software; IF YOU AGREE TO CITE USE OF CompuCell  *
*  IN ALL RELATED RESEARCH PUBLICATIONS according to the terms of the   *
*  CompuCell GNU General Public License RIDER you can redistribute it   *
* and/or modify it under the terms of the GNU General Public License as *
*  published by the Free Software Foundation; either version 2 of the   *
*         License, or (at your option) any later version.               *
*                                                                       *
* This program is distributed in the hope that it will be useful, but   *
*      WITHOUT ANY WARRANTY; without even the implied warranty of       *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    *
*             General Public License for more details.                  *
*                                                                       *
*  You should have received a copy of the GNU General Public License    *
*     along with this program; if not, write to the Free Software       *
*      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.        *
*************************************************************************/

#include <CompuCell3D/CC3D.h>

// // // #include <CompuCell3D/Simulator.h>
// // // #include <CompuCell3D/Potts3D/Potts3D.h>
// // // #include <CompuCell3D/Automaton/Automaton.h>
using namespace CompuCell3D;

using namespace std;

#include "PressurePlugin.h"


// PressurePlugin::PressurePlugin() : potts(0) {}

PressurePlugin::~PressurePlugin() {}

void PressurePlugin::init(Simulator *simulator, CC3DXMLElement *_xmlData){
	potts = simulator->getPotts();
	bool pluginAlreadyRegisteredFlag;
	Plugin *plugin=Simulator::pluginManager.get("VolumeTracker",&pluginAlreadyRegisteredFlag); //this will load VolumeTracker plugin if it is not already loaded


	pUtils=simulator->getParallelUtils();
	pluginName=_xmlData->getAttribute("Name");

	cerr<<"GOT HERE BEFORE CALLING INIT"<<endl;
	if(!pluginAlreadyRegisteredFlag)
		plugin->init(simulator);
	potts->registerEnergyFunctionWithName(this,toString());
	//save pointer to plugin xml element for later. Initialization has to be done in extraInit to make sure sutomaton (CelltypePlugin)
	// is already registered - we need it in the case of BYCELLTYPE
	xmlData=_xmlData;

	simulator->registerSteerableObject(this);
        

}

void PressurePlugin::update(CC3DXMLElement *_xmlData, bool _fullInitFlag){
	//if there are no child elements for this plugin it means will use changeEnergyByCellId

	if(potts->getDisplayUnitsFlag()){
		Unit PressureUnit=powerUnit(potts->getLengthUnit(),3);
		Unit lambdaPressureUnit=potts->getEnergyUnit()/(Pressure);

		CC3DXMLElement * unitsElem=_xmlData->getFirstElement("Units"); 
		if (!unitsElem){ //add Units element
			unitsElem=_xmlData->attachElement("Units");
		}

		if(unitsElem->getFirstElement("PressureUnit")){
			unitsElem->getFirstElement("PressureUnit")->updateElementValue(PressureUnit.toString());
		}else{
			CC3DXMLElement * pressureUnitElem = unitsElem->attachElement("PressureUnit",PressureUnit.toString());
		}

		if(unitsElem->getFirstElement("LambdaPressureUnit")){
			unitsElem->getFirstElement("LambdaPressureUnit")->updateElementValue(lambdaPressureUnit.toString());
		}else{
			CC3DXMLElement * lambdaPressureUnitElem = unitsElem->attachElement("LambdaPressureUnit",lambdaPressureUnit.toString());
		}


	}

	

	if (_xmlData->findElement("PressureEnergyExpression")){
		unsigned int maxNumberOfWorkNodes=pUtils->getMaxNumberOfWorkNodesPotts();
		eed.allocateSize(maxNumberOfWorkNodes);
		vector<string> variableNames;
		variableNames.push_back("LambdaPressure");
		variableNames.push_back("Pressure");
		variableNames.push_back("Volume");

		eed.addVariables(variableNames.begin(),variableNames.end());
		eed.update(_xmlData->getFirstElement("PressureEnergyExpression"));
		energyExpressionDefined=true;
	}else{
		energyExpressionDefined=false;
	}


	if(!_xmlData->findElement("PressureEnergyParameters") && !_xmlData->findElement("Pressure")){

		functionType=BYCELLID;
	}else{
		if(_xmlData->findElement("PressureEnergyParameters"))
			functionType=BYCELLTYPE;
		else if (_xmlData->findElement("Pressure"))
			functionType=GLOBAL;
		else //in case users put garbage xml use changeEnergyByCellId
			functionType=BYCELLID;
	}
	Automaton *automaton=potts->getAutomaton();
	cerr<<"automaton="<<automaton<<endl;

	switch(functionType){
		case BYCELLID:
			//set fcn ptr
			changeEnergyFcnPtr=&PressurePlugin::changeEnergyByCellId;
			break;

		case BYCELLTYPE:
			{
				pressureEnergyParamVector.clear();
				vector<int> typeIdVec;
				vector<PressureEnergyParam> pressureEnergyParamVectorTmp;

				CC3DXMLElementList energyVec=_xmlData->getElements("PressureEnergyParameters");

				for (int i = 0 ; i<energyVec.size(); ++i){
					PressureEnergyParam preParam;

					preParam.Pressure=energyVec[i]->getAttributeAsDouble("Pressure");
					preParam.lambdaPressure=energyVec[i]->getAttributeAsDouble("LambdaPressure");
					preParam.typeName=energyVec[i]->getAttribute("CellType");
					cerr<<"automaton="<<automaton<<endl;
					typeIdVec.push_back(automaton->getTypeId(preParam.typeName));

					pressureEnergyParamVectorTmp.push_back(preParam);
				}
				vector<int>::iterator pos=max_element(typeIdVec.begin(),typeIdVec.end());
				int maxTypeId=*pos;
				pressureEnergyParamVector.assign(maxTypeId+1,PressureEnergyParam());
				for (int i = 0 ; i < pressureEnergyParamVectorTmp.size() ; ++i){
					pressureEnergyParamVector[typeIdVec[i]]=pressureEnergyParamVectorTmp[i];
				}

				//set fcn ptr
				changeEnergyFcnPtr=&PressurePlugin::changeEnergyByCellType;
			}
			break;

		case GLOBAL:
			//using Global Pressure Energy Parameters
			Pressure=_xmlData->getFirstElement("Pressure")->getDouble();
			lambdaPressure=_xmlData->getFirstElement("LambdaPressure")->getDouble();
			//set fcn ptr
			changeEnergyFcnPtr=&PressurePlugin::changeEnergyGlobal;
			break;

		default:
			//set fcn ptr
			changeEnergyFcnPtr=&PressurePlugin::changeEnergyByCellId;
	}
}


void PressurePlugin::extraInit(Simulator *simulator){
	update(xmlData);
}

void PressurePlugin::handleEvent(CC3DEvent & _event){
    if (_event.id==CHANGE_NUMBER_OF_WORK_NODES){    
    	update(xmlData);
    }

}
double PressurePlugin::customExpressionFunction(double _lambdaPressure,double _Pressure, double _volumeBefore,double _volumeAfter){////////

		int currentWorkNodeNumber=pUtils->getCurrentWorkNodeNumber();	
		ExpressionEvaluator & ev=eed[currentWorkNodeNumber];
		double energyBefore=0.0,energyAfter=0.0;

		//before
		ev[0]=_lambdaPressure;
		ev[1]=_volumeBefore;
		ev[2]=_Pressure;
		energyBefore=ev.eval();

		//after		
		ev[1]=_volumeAfter;
		energyAfter=ev.eval();

		return energyAfter-energyBefore;
}

// I'm as lambda pressure and pressure are pretty much the same thing I'm removing pressure.
double PressurePlugin::changeEnergyGlobal(const Point3D &pt, const CellG *newCell,const CellG *oldCell){

	double energy = 0;

	if (oldCell == newCell) return 0;

	if (!energyExpressionDefined){

		if (newCell){
			energy -= lambdaPressure/* * Pressure*/;

		}
		if (oldCell){
			energy += lambdaPressure /* Pressure*/;

		}
		return energy;
	}else{
/* DONT FORGET TO UNCOMENT THIS!!!!!
		if (newCell){
			energy+=customExpressionFunction(lambdaPressure,Pressure,newCell->volume,newCell->volume+1);
		}
		
		if (oldCell){
			energy+=customExpressionFunction(lambdaPressure,Pressure,oldCell->volume,oldCell->volume-1);

		}		
		return energy;
*/
		return 0; //AND TO DELETE THIS!
	}


}

double PressurePlugin::changeEnergyByCellType(const Point3D &pt,const CellG *newCell,const CellG *oldCell) {

	/// E = -   Pressure

	double energy = 0;

	if (oldCell == newCell) return 0;

	if (!energyExpressionDefined){
		if (newCell)
			energy -= pressureEnergyParamVector[newCell->type].lambdaPressure; //* pressureEnergyParamVector[newCell->type].Pressure;
			//(1 + 2 * (newCell->volume - fabs(volumeEnergyParamVector[newCell->type].targetVolume)));

		if (oldCell)
			energy += pressureEnergyParamVector[oldCell->type].lambdaPressure; //* pressureEnergyParamVector[newCell->type].Pressure;
			//energy += volumeEnergyParamVector[oldCell->type].lambdaVolume  *
			//(1 - 2 * (oldCell->volume - fabs(volumeEnergyParamVector[oldCell->type].targetVolume)));


		//cerr<<"VOLUME CHANGE ENERGY NEW: "<<energy<<endl;
		return energy;


	}else{
/*DONT FORGET TO UNCOMENT THIS!!!!!
		if (newCell){
			energy+=customExpressionFunction(pressureEnergyParamVector[newCell->type].lambdaPressure,fabs(pressureEnergyParamVector[newCell->type].Pressure),newCell->volume,newCell->volume+1);
		}

		if (oldCell){
			energy+=customExpressionFunction(pressureEnergyParamVector[oldCell->type].lambdaPressure,fabs(pressureEnergyParamVector[oldCell->type].Pressure),oldCell->volume,oldCell->volume-1);

		}		
		return energy;
*/
		return 0;
	}


}


double PressurePlugin::changeEnergyByCellId(const Point3D &pt,const CellG *newCell,const CellG *oldCell) {

	/// E = - lambda

	double energy = 0;

	if (oldCell == newCell) return 0;

	if (!energyExpressionDefined){

		if (newCell){
			energy -=  (newCell->lambdaPressure);//*(newCell->Pressure);
			//energy +=  newCell->lambdaVolume*
				//(1 + 2 * ((int)newCell->volume - newCell->targetVolume));
		}
		if (oldCell){
			energy +=  (oldCell->lambdaPressure);//*(newCell->Pressure);
			//energy += oldCell->lambdaVolume*
				//(1 - 2 * ((int)oldCell->volume - oldCell->targetVolume));
		}



		return energy;


	}else{
/*DONT FORGET TO UNCOMENT THIS!!!!!
		if (newCell){
			energy+=customExpressionFunction(newCell->lambdaPressure,newCell->Pressure,newCell->volume,newCell->volume+1);
		}

		if (oldCell){
			energy+=customExpressionFunction(oldCell->lambdaPressure,oldCell->Pressure,oldCell->volume,oldCell->volume-1);

		}		
		return energy;
*/
		return 0;
	}


}




double PressurePlugin::changeEnergy(const Point3D &pt,const CellG *newCell,const CellG *oldCell) {

	/// E = lambda * (volume - targetVolume) ^ 2 
	return (this->*changeEnergyFcnPtr)(pt,newCell,oldCell);


}


std::string PressurePlugin::steerableName(){
	return pluginName;
	//return "Volume";
}

std::string PressurePlugin::toString(){
	return pluginName;
	//return "Volume";
}




