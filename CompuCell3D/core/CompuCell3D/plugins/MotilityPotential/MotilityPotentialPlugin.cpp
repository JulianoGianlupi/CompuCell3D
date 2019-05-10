#include <CompuCell3D/CC3D.h>
// // // #include <CompuCell3D/Simulator.h>
// // // #include <CompuCell3D/Potts3D/Potts3D.h>
// // // #include <CompuCell3D/Field3D/Field3D.h>
// // // #include <CompuCell3D/Field3D/AdjacentNeighbor.h>
// // // #include <CompuCell3D/Automaton/Automaton.h>
// // // #include <BasicUtils/BasicClassGroup.h>

// // // #include <PublicUtilities/NumericalUtils.h>
// // // #include <Utils/Coordinates3D.h>
// // // #include <PublicUtilities/StringUtils.h>


using namespace CompuCell3D;
using namespace std;


#include "MotilityPotentiallPlugin.h"

namespace CompuCell3D {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	MotilityPotentiallPlugin::MotilityPotentiallPlugin():lambdaVec(Coordinates3D<float>(0.,0.,0.)),xmlData(0),boundaryStrategy(0)
{}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MotilityPotentiallPlugin::~MotilityPotentiallPlugin()
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MotilityPotentiallPlugin::init(Simulator *_simulator, CC3DXMLElement *_xmlData){
  
  xmlData=_xmlData;
  potts = _simulator->getPotts();
  cellFieldG=(WatchableField3D<CellG *> * )potts->getCellFieldG();
  simulator=_simulator;

	bool pluginAlreadyRegisteredFlag;
	Plugin *plugin=Simulator::pluginManager.get("CenterOfMass",&pluginAlreadyRegisteredFlag); //this will load VolumeTracker plugin if it is not already loaded
	if(!pluginAlreadyRegisteredFlag)
		plugin->init(simulator);

  
  potts->registerEnergyFunctionWithName(this,"MotilityPotentiall");
  
  fieldDim=potts->getCellFieldG()->getDim();
   
   
   
  boundaryStrategy=BoundaryStrategy::getInstance();
  adjNeighbor.initialize(fieldDim);
  adjNeighbor_ptr=&adjNeighbor;

  if(potts->getBoundaryXName()=="Periodic"){
      adjNeighbor.setPeriodicX();
	  boundaryConditionIndicator.x=1;
  }
  if(potts->getBoundaryYName()=="Periodic"){
      adjNeighbor.setPeriodicY();
	  boundaryConditionIndicator.y=1;
  }
  if(potts->getBoundaryZName()=="Periodic"){
      adjNeighbor.setPeriodicZ();
	  boundaryConditionIndicator.z=1;
  }

  _simulator->registerSteerableObject(this);



}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MotilityPotentiallPlugin::extraInit(Simulator *_simulator){
   // check why registering in init gives segfault
//    _simulator->registerSteerableObject(this);

		update(xmlData,true);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void MotilityPotentiallPlugin::update(CC3DXMLElement *_xmlData, bool _fullInitFlag){


	if(potts->getDisplayUnitsFlag()){
		Unit extternalPotentialLambdaUnit=potts->getEnergyUnit()/potts->getLengthUnit();




		CC3DXMLElement * unitsElem=_xmlData->getFirstElement("Units"); 
		if (!unitsElem){ //add Units element
			unitsElem=_xmlData->attachElement("Units");
		}

		if(unitsElem->getFirstElement("LambdaUnit")){
			unitsElem->getFirstElement("LambdaUnit")->updateElementValue(extternalPotentialLambdaUnit.toString());
		}else{
			unitsElem->attachElement("LambdaUnit",extternalPotentialLambdaUnit.toString());
		}

	}

	bool comBasedAlgorithm=false;
	if(_xmlData->findElement("Algorithm")){ 

		string algorithm=_xmlData->getFirstElement("Algorithm")->getText();
		
		changeToLower(algorithm);

		if(algorithm=="centerofmassbased"){
			comBasedAlgorithm=true;
		}else if (algorithm=="pixelbased"){
			comBasedAlgorithm=false;
		}

	}

	if(!_xmlData->getNumberOfChildren() || (_xmlData->getNumberOfChildren()==1 && _xmlData->findElement("Algorithm"))){ 
		functionType=BYCELLID;
	}else{
		if(_xmlData->findElement("MotilityPotentiallParameters"))
			functionType=BYCELLTYPE;
		else if (_xmlData->findElement("Lambda"))
			functionType=GLOBAL;
		else //in case users put garbage xml use changeEnergyByCellId
			functionType=BYCELLID;
	}

	Automaton *automaton=potts->getAutomaton();
	//cerr<<"automaton="<<automaton<<endl;

	switch(functionType){
		case BYCELLID:
			//set fcn ptr
			if (comBasedAlgorithm){				
				changeEnergyFcnPtr=&MotilityPotentiallPlugin::changeEnergyByCellIdCOMBased;
			}else{
				changeEnergyFcnPtr=&MotilityPotentiallPlugin::changeEnergyByCellId;				
			}
			break;

		case BYCELLTYPE:
			{
				MotilityPotentiallParamVector.clear();
				vector<int> typeIdVec;
				vector<MotilityPotentiallParam> MotilityPotentiallParamVectorTmp;

				CC3DXMLElementList energyVec=_xmlData->getElements("MotilityPotentiallParameters");

				for (int i = 0 ; i<energyVec.size(); ++i){
					MotilityPotentiallParam extPotentialParam;

					extPotentialParam.lambdaVec.x=energyVec[i]->getAttributeAsDouble("x");
					extPotentialParam.lambdaVec.y=energyVec[i]->getAttributeAsDouble("y");
					extPotentialParam.lambdaVec.z=energyVec[i]->getAttributeAsDouble("z");

					extPotentialParam.typeName=energyVec[i]->getAttribute("CellType");
					//cerr<<"automaton="<<automaton<<endl;
					typeIdVec.push_back(automaton->getTypeId(extPotentialParam.typeName));
					participatingTypes.insert(automaton->getTypeId(extPotentialParam.typeName));

					MotilityPotentiallParamVectorTmp.push_back(extPotentialParam);				
				}
				vector<int>::iterator pos=max_element(typeIdVec.begin(),typeIdVec.end());
				int maxTypeId=*pos;
				MotilityPotentiallParamVector.assign(maxTypeId+1,MotilityPotentiallParam());
				for (int i = 0 ; i < MotilityPotentiallParamVectorTmp.size() ; ++i){
					MotilityPotentiallParamVector[typeIdVec[i]]=MotilityPotentiallParamVectorTmp[i];
				}

				//set fcn ptr
				if (comBasedAlgorithm){
					changeEnergyFcnPtr=&MotilityPotentiallPlugin::changeEnergyByCellTypeCOMBased;					
				}else{
					changeEnergyFcnPtr=&MotilityPotentiallPlugin::changeEnergyByCellType;					
				}
				
			}
			break;

		case GLOBAL:
			//using Global Volume Energy Parameters
			lambdaVec=Coordinates3D<float>(_xmlData->getFirstElement("Lambda")->getAttributeAsDouble("x"),
													 _xmlData->getFirstElement("Lambda")->getAttributeAsDouble("y"),
													 _xmlData->getFirstElement("Lambda")->getAttributeAsDouble("z")
				);
			//set fcn ptr
			//changeEnergyFcnPtr=&MotilityPotentiallPlugin::changeEnergyGlobal;
			if (comBasedAlgorithm){								
				changeEnergyFcnPtr=&MotilityPotentiallPlugin::changeEnergyGlobalCOMBased;
			}else{
				changeEnergyFcnPtr=&MotilityPotentiallPlugin::changeEnergyGlobal;
			}

			
			break;

		default:
			//set fcn ptr
			if (comBasedAlgorithm){				
				changeEnergyFcnPtr=&MotilityPotentiallPlugin::changeEnergyByCellIdCOMBased;
			}else{
				changeEnergyFcnPtr=&MotilityPotentiallPlugin::changeEnergyByCellId;				
			}
	}

}

double MotilityPotentiallPlugin::changeEnergyGlobalCOMBased(const Point3D &pt,  const CellG *newCell,
                                                const CellG *oldCell)
{
	
	double energy=0.0;	
	if (oldCell){
		Coordinates3D<double> oldCOMAfterFlip=precalculateCentroid(pt, oldCell, -1,fieldDim, boundaryStrategy);

      if(oldCell->volume>1){
         oldCOMAfterFlip.XRef()=oldCOMAfterFlip.X()/(float)(oldCell->volume-1);
         oldCOMAfterFlip.YRef()=oldCOMAfterFlip.Y()/(float)(oldCell->volume-1);
         oldCOMAfterFlip.ZRef()=oldCOMAfterFlip.Z()/(float)(oldCell->volume-1);
	  }else{

		 oldCOMAfterFlip=Coordinates3D<double>(oldCell->xCM/oldCell->volume,oldCell->zCM/oldCell->volume,oldCell->zCM/oldCell->volume);
		 
	  }

		Coordinates3D<double> oldCOMBeforeFlip(oldCell->xCM/oldCell->volume, oldCell->yCM/oldCell->volume, oldCell->zCM/oldCell->volume);
		Coordinates3D<double> distVector = distanceVectorCoordinatesInvariant(oldCOMAfterFlip ,oldCOMBeforeFlip,fieldDim);
		energy += distVector.X()*lambdaVec.X() + distVector.Y()*lambdaVec.Y() + distVector.Z()*lambdaVec.Z();
	}

	if (newCell){

		Coordinates3D<double> newCOMAfterFlip=precalculateCentroid(pt, newCell, 1,fieldDim, boundaryStrategy);

      
         newCOMAfterFlip.XRef()=newCOMAfterFlip.X()/(float)(newCell->volume+1);
         newCOMAfterFlip.YRef()=newCOMAfterFlip.Y()/(float)(newCell->volume+1);
         newCOMAfterFlip.ZRef()=newCOMAfterFlip.Z()/(float)(newCell->volume+1);


		Coordinates3D<double> newCOMBeforeFlip(newCell->xCM/newCell->volume, newCell->yCM/newCell->volume, newCell->zCM/newCell->volume);
		Coordinates3D<double> distVector = distanceVectorCoordinatesInvariant(newCOMAfterFlip ,newCOMBeforeFlip,fieldDim);

		energy += distVector.X()*lambdaVec.X() + distVector.Y()*lambdaVec.Y() + distVector.Z()*lambdaVec.Z();

	}

	return energy;
}

double MotilityPotentiallPlugin::changeEnergyByCellTypeCOMBased(const Point3D &pt,  const CellG *newCell,
                                                const CellG *oldCell)
{

	double energy=0.0;	
	if (oldCell){
		Coordinates3D<double> oldCOMAfterFlip=precalculateCentroid(pt, oldCell, -1,fieldDim, boundaryStrategy);

      if(oldCell->volume>1){
         oldCOMAfterFlip.XRef()=oldCOMAfterFlip.X()/(float)(oldCell->volume-1);
         oldCOMAfterFlip.YRef()=oldCOMAfterFlip.Y()/(float)(oldCell->volume-1);
         oldCOMAfterFlip.ZRef()=oldCOMAfterFlip.Z()/(float)(oldCell->volume-1);
	  }else{

		 oldCOMAfterFlip=Coordinates3D<double>(oldCell->xCM/oldCell->volume,oldCell->zCM/oldCell->volume,oldCell->zCM/oldCell->volume);
		 
	  }

		Coordinates3D<double> oldCOMBeforeFlip(oldCell->xCM/oldCell->volume, oldCell->yCM/oldCell->volume, oldCell->zCM/oldCell->volume);
		Coordinates3D<double> distVector = distanceVectorCoordinatesInvariant(oldCOMAfterFlip ,oldCOMBeforeFlip,fieldDim);
		energy += distVector.X()*MotilityPotentiallParamVector[oldCell->type].lambdaVec.X() 
				  + distVector.Y()*MotilityPotentiallParamVector[oldCell->type].lambdaVec.Y()
				  + distVector.Z()*MotilityPotentiallParamVector[oldCell->type].lambdaVec.Z();
	}

	if (newCell){

		Coordinates3D<double> newCOMAfterFlip=precalculateCentroid(pt, newCell, 1,fieldDim, boundaryStrategy);

      
         newCOMAfterFlip.XRef()=newCOMAfterFlip.X()/(float)(newCell->volume+1);
         newCOMAfterFlip.YRef()=newCOMAfterFlip.Y()/(float)(newCell->volume+1);
         newCOMAfterFlip.ZRef()=newCOMAfterFlip.Z()/(float)(newCell->volume+1);


		Coordinates3D<double> newCOMBeforeFlip(newCell->xCM/newCell->volume, newCell->yCM/newCell->volume, newCell->zCM/newCell->volume);
		Coordinates3D<double> distVector = distanceVectorCoordinatesInvariant(newCOMAfterFlip ,newCOMBeforeFlip,fieldDim);

		energy += distVector.X()*MotilityPotentiallParamVector[newCell->type].lambdaVec.X() 
			      + distVector.Y()*MotilityPotentiallParamVector[newCell->type].lambdaVec.Y() 
				  + distVector.Z()*MotilityPotentiallParamVector[newCell->type].lambdaVec.Z();

	}

	return energy;
}

double MotilityPotentiallPlugin::changeEnergyByCellIdCOMBased(const Point3D &pt,  const CellG *newCell,
                                                const CellG *oldCell)
{
	
	double energy=0.0;	
	if (oldCell){
		Coordinates3D<double> oldCOMAfterFlip=precalculateCentroid(pt, oldCell, -1,fieldDim, boundaryStrategy);

      if(oldCell->volume>1){
         oldCOMAfterFlip.XRef()=oldCOMAfterFlip.X()/(float)(oldCell->volume-1);
         oldCOMAfterFlip.YRef()=oldCOMAfterFlip.Y()/(float)(oldCell->volume-1);
         oldCOMAfterFlip.ZRef()=oldCOMAfterFlip.Z()/(float)(oldCell->volume-1);
	  }else{

		 oldCOMAfterFlip=Coordinates3D<double>(oldCell->xCM/oldCell->volume,oldCell->zCM/oldCell->volume,oldCell->zCM/oldCell->volume);
		 
	  }

		Coordinates3D<double> oldCOMBeforeFlip(oldCell->xCM/oldCell->volume, oldCell->yCM/oldCell->volume, oldCell->zCM/oldCell->volume);
		Coordinates3D<double> distVector = distanceVectorCoordinatesInvariant(oldCOMAfterFlip ,oldCOMBeforeFlip,fieldDim);
		energy += distVector.X()*oldCell->lambdaVecX + distVector.Y()*oldCell->lambdaVecY + distVector.Z()*oldCell->lambdaVecZ;
	}

	if (newCell){

		Coordinates3D<double> newCOMAfterFlip=precalculateCentroid(pt, newCell, 1,fieldDim, boundaryStrategy);

      
         newCOMAfterFlip.XRef()=newCOMAfterFlip.X()/(float)(newCell->volume+1);
         newCOMAfterFlip.YRef()=newCOMAfterFlip.Y()/(float)(newCell->volume+1);
         newCOMAfterFlip.ZRef()=newCOMAfterFlip.Z()/(float)(newCell->volume+1);


		Coordinates3D<double> newCOMBeforeFlip(newCell->xCM/newCell->volume, newCell->yCM/newCell->volume, newCell->zCM/newCell->volume);
		Coordinates3D<double> distVector = distanceVectorCoordinatesInvariant(newCOMAfterFlip ,newCOMBeforeFlip,fieldDim);

		energy += distVector.X()*newCell->lambdaVecX + distVector.Y()*newCell->lambdaVecY + distVector.Z()*newCell->lambdaVecZ;

	}

	return energy;
}



double MotilityPotentiallPlugin::changeEnergyGlobal(const Point3D &pt,  const CellG *newCell,
                                                const CellG *oldCell)
{

   double deltaEnergyOld=0.0;
   double deltaEnergyNew=0.0;
   CellG *neighborPtr;

   Dim3D fieldDim=cellFieldG->getDim();


   const vector<Point3D>  & neighborsOffsetVec = adjNeighbor_ptr->getAdjFace2FaceNeighborOffsetVec(pt);
   unsigned int neighborSize=neighborsOffsetVec.size();

   Point3D ptAdj;
   

   //x, y,or z depending in which direction we want potential gradient to act
   //I add 1 to avoid zero change of energy when z is at the boundary
   short prefferedCoordinate=pt.z+1;
   //short deltaCoordinate;
   Coordinates3D<short> deltaCoordinate(0,0,0);
   
   ///COMMENT TO deltaCoordinate calculations
   // have to do fieldDim.z-2 because the neighbor of pt with max_z can be a pt with z=0 but they are spaced by delta z=1
   // thus you need to do deltaCoordinate % (fieldDim.z-2). Otherwise if you do not do fieldDim.z-2
   // then you may get the following (max_z-0) % max_z =0, whereas it should be 1 !

//    cerr<<"******************CHANGE BEGIN**************************"<<endl;
   int counter=0;
   Point3D ptFlipNeighbor=potts->getFlipNeighbor();
   Point3D deltaFlip;
   
   deltaFlip=pt;
   deltaFlip-=ptFlipNeighbor;
      
   for(unsigned int i = 0 ; i < neighborSize ; ++i){
      ptAdj=pt;
      ptAdj+=neighborsOffsetVec[i];
      
      if( cellFieldG->isValid(ptAdj) ){
         ++counter;
         neighborPtr=cellFieldG->get(ptAdj);
         
         ///process old energy
         if(/*neighborPtr &&*/ oldCell && neighborPtr!=oldCell){

            deltaCoordinate.XRef()=(ptAdj.x-pt.x);
            deltaCoordinate.YRef()=(ptAdj.y-pt.y);
            deltaCoordinate.ZRef()=(ptAdj.z-pt.z);

            if(fabs(static_cast<double>(deltaCoordinate.X()))>1){

               deltaCoordinate.XRef()=
               ( deltaCoordinate.X()>0 ? -(deltaCoordinate.X()+1) % (fieldDim.x-1) :-(deltaCoordinate.X()-1) % (fieldDim.x-1) );


             }

            if(fabs(static_cast<double>(deltaCoordinate.Y()))>1){
               deltaCoordinate.YRef()=
               ( deltaCoordinate.Y()>0 ? -(deltaCoordinate.Y()+1) % (fieldDim.y-1) :-(deltaCoordinate.Y()-1) % (fieldDim.y-1) );
            }


            if(fabs(static_cast<double>(deltaCoordinate.Z()))>1){
               deltaCoordinate.ZRef()=
               ( deltaCoordinate.Z()>0 ? -(deltaCoordinate.Z()+1) % (fieldDim.z-1) :-(deltaCoordinate.Z()-1) % (fieldDim.z-1) );
            }
            deltaEnergyOld+=  deltaCoordinate.X()*lambdaVec.X()+
                              deltaCoordinate.Y()*lambdaVec.Y()+
                              deltaCoordinate.Z()*lambdaVec.Z();

         }

         ///process new energy
         if(/*neighborPtr && */newCell && neighborPtr!=newCell){

            deltaCoordinate.XRef()=(ptAdj.x-pt.x);
            deltaCoordinate.YRef()=(ptAdj.y-pt.y);
            deltaCoordinate.ZRef()=(ptAdj.z-pt.z);
            
            if(fabs(static_cast<double>(deltaCoordinate.X()))>1){
               deltaCoordinate.XRef()=
               ( deltaCoordinate.X()>0 ? -(deltaCoordinate.X()+1) % (fieldDim.x-1) :-(deltaCoordinate.X()-1) % (fieldDim.x-1) );
               
            }

            if(fabs(static_cast<double>(deltaCoordinate.Y()))>1){
               deltaCoordinate.YRef()=
               ( deltaCoordinate.Y()>0 ? -(deltaCoordinate.Y()+1) % (fieldDim.y-1) :-(deltaCoordinate.Y()-1) % (fieldDim.y-1) );
            }
                        

            if(fabs(static_cast<double>(deltaCoordinate.Z()))>1){
               deltaCoordinate.ZRef()=
               ( deltaCoordinate.Z()>0 ? -(deltaCoordinate.Z()+1) % (fieldDim.z-1) :-(deltaCoordinate.Z()-1) % (fieldDim.z-1) );
            }
            
            deltaEnergyNew+=  deltaCoordinate.X()*lambdaVec.X()+
                              deltaCoordinate.Y()*lambdaVec.Y()+
                              deltaCoordinate.Z()*lambdaVec.Z();
            
         }


      }
       
   }


   return deltaEnergyNew-deltaEnergyOld;
   
}      


double MotilityPotentiallPlugin::changeEnergyByCellType(const Point3D &pt,  const CellG *newCell,
                                                const CellG *oldCell)
{

	

   double deltaEnergyOld=0.0;
   double deltaEnergyNew=0.0;
   CellG *neighborPtr;

   Dim3D fieldDim=cellFieldG->getDim();


   const vector<Point3D>  & neighborsOffsetVec = adjNeighbor_ptr->getAdjFace2FaceNeighborOffsetVec(pt);
   unsigned int neighborSize=neighborsOffsetVec.size();

   Point3D ptAdj;
   

   //x, y,or z depending in which direction we want potential gradient to act
   //I add 1 to avoid zero change of energy when z is at the boundary
   short prefferedCoordinate=pt.z+1;
   //short deltaCoordinate;
   Coordinates3D<short> deltaCoordinate(0,0,0);
   
   ///COMMENT TO deltaCoordinate calculations
   // have to do fieldDim.z-2 because the neighbor of pt with max_z can be a pt with z=0 but they are spaced by delta z=1
   // thus you need to do deltaCoordinate % (fieldDim.z-2). Otherwise if you do not do fieldDim.z-2
   // then you may get the following (max_z-0) % max_z =0, whereas it should be 1 !

//    cerr<<"******************CHANGE BEGIN**************************"<<endl;
   int counter=0;
   Point3D ptFlipNeighbor=potts->getFlipNeighbor();
   Point3D deltaFlip;
   
   deltaFlip=pt;
   deltaFlip-=ptFlipNeighbor;
      
   for(unsigned int i = 0 ; i < neighborSize ; ++i){
      ptAdj=pt;
      ptAdj+=neighborsOffsetVec[i];
      
      if( cellFieldG->isValid(ptAdj) ){
         ++counter;
         neighborPtr=cellFieldG->get(ptAdj);
         
         ///process old energy		 
         if(/*neighborPtr &&*/ oldCell && neighborPtr!=oldCell && participatingTypes.find(oldCell->type)!=participatingTypes.end()){		
            deltaCoordinate.XRef()=(ptAdj.x-pt.x);
            deltaCoordinate.YRef()=(ptAdj.y-pt.y);
            deltaCoordinate.ZRef()=(ptAdj.z-pt.z);

            if(fabs(static_cast<double>(deltaCoordinate.X()))>1){

               deltaCoordinate.XRef()=
               ( deltaCoordinate.X()>0 ? -(deltaCoordinate.X()+1) % (fieldDim.x-1) :-(deltaCoordinate.X()-1) % (fieldDim.x-1) );


             }

            if(fabs(static_cast<double>(deltaCoordinate.Y()))>1){
               deltaCoordinate.YRef()=
               ( deltaCoordinate.Y()>0 ? -(deltaCoordinate.Y()+1) % (fieldDim.y-1) :-(deltaCoordinate.Y()-1) % (fieldDim.y-1) );
            }


            if(fabs(static_cast<double>(deltaCoordinate.Z()))>1){
               deltaCoordinate.ZRef()=
               ( deltaCoordinate.Z()>0 ? -(deltaCoordinate.Z()+1) % (fieldDim.z-1) :-(deltaCoordinate.Z()-1) % (fieldDim.z-1) );
            }
				
            deltaEnergyOld+=  deltaCoordinate.X()*MotilityPotentiallParamVector[oldCell->type].lambdaVec.X()+
                              deltaCoordinate.Y()*MotilityPotentiallParamVector[oldCell->type].lambdaVec.Y()+
                              deltaCoordinate.Z()*MotilityPotentiallParamVector[oldCell->type].lambdaVec.Z();

         }

         ///process new energy
         if(/*neighborPtr && */newCell && neighborPtr!=newCell && participatingTypes.find(newCell->type)!=participatingTypes.end()){

            deltaCoordinate.XRef()=(ptAdj.x-pt.x);
            deltaCoordinate.YRef()=(ptAdj.y-pt.y);
            deltaCoordinate.ZRef()=(ptAdj.z-pt.z);
            
            if(fabs(static_cast<double>(deltaCoordinate.X()))>1){
               deltaCoordinate.XRef()=
               ( deltaCoordinate.X()>0 ? -(deltaCoordinate.X()+1) % (fieldDim.x-1) :-(deltaCoordinate.X()-1) % (fieldDim.x-1) );
               
            }

            if(fabs(static_cast<double>(deltaCoordinate.Y()))>1){
               deltaCoordinate.YRef()=
               ( deltaCoordinate.Y()>0 ? -(deltaCoordinate.Y()+1) % (fieldDim.y-1) :-(deltaCoordinate.Y()-1) % (fieldDim.y-1) );
            }
                        

            if(fabs(static_cast<double>(deltaCoordinate.Z()))>1){
               deltaCoordinate.ZRef()=
               ( deltaCoordinate.Z()>0 ? -(deltaCoordinate.Z()+1) % (fieldDim.z-1) :-(deltaCoordinate.Z()-1) % (fieldDim.z-1) );
            }
            deltaEnergyNew+=  deltaCoordinate.X()*MotilityPotentiallParamVector[newCell->type].lambdaVec.X()+
                              deltaCoordinate.Y()*MotilityPotentiallParamVector[newCell->type].lambdaVec.Y()+
                              deltaCoordinate.Z()*MotilityPotentiallParamVector[newCell->type].lambdaVec.Z();
            
         }


      }
       
   }

	//if((deltaEnergyNew-deltaEnergyOld) != 0.0)
		//cerr<<"\t\t\t\tMotilityPotentiall energy="<<deltaEnergyNew-deltaEnergyOld<<endl;
		

   return deltaEnergyNew-deltaEnergyOld;
   
}      

double MotilityPotentiallPlugin::changeEnergyByCellId(const Point3D &pt,  const CellG *newCell,
                                                const CellG *oldCell)
{

	

   double deltaEnergyOld=0.0;
   double deltaEnergyNew=0.0;
   CellG *neighborPtr;

   Dim3D fieldDim=cellFieldG->getDim();


   const vector<Point3D>  & neighborsOffsetVec = adjNeighbor_ptr->getAdjFace2FaceNeighborOffsetVec(pt);
   unsigned int neighborSize=neighborsOffsetVec.size();

   Point3D ptAdj;
   

   //x, y,or z depending in which direction we want potential gradient to act
   //I add 1 to avoid zero change of energy when z is at the boundary
   short prefferedCoordinate=pt.z+1;
   //short deltaCoordinate;
   Coordinates3D<short> deltaCoordinate(0,0,0);
   
   ///COMMENT TO deltaCoordinate calculations
   // have to do fieldDim.z-2 because the neighbor of pt with max_z can be a pt with z=0 but they are spaced by delta z=1
   // thus you need to do deltaCoordinate % (fieldDim.z-2). Otherwise if you do not do fieldDim.z-2
   // then you may get the following (max_z-0) % max_z =0, whereas it should be 1 !

//    cerr<<"******************CHANGE BEGIN**************************"<<endl;
   int counter=0;
   Point3D ptFlipNeighbor=potts->getFlipNeighbor();
   Point3D deltaFlip;
   
   deltaFlip=pt;
   deltaFlip-=ptFlipNeighbor;
      
   for(unsigned int i = 0 ; i < neighborSize ; ++i){
      ptAdj=pt;
      ptAdj+=neighborsOffsetVec[i];
      
      if( cellFieldG->isValid(ptAdj) ){
         ++counter;
         neighborPtr=cellFieldG->get(ptAdj);
         
         ///process old energy
         if(/*neighborPtr &&*/ oldCell && neighborPtr!=oldCell){

            deltaCoordinate.XRef()=(ptAdj.x-pt.x);
            deltaCoordinate.YRef()=(ptAdj.y-pt.y);
            deltaCoordinate.ZRef()=(ptAdj.z-pt.z);

            if(fabs(static_cast<double>(deltaCoordinate.X()))>1){

               deltaCoordinate.XRef()=
               ( deltaCoordinate.X()>0 ? -(deltaCoordinate.X()+1) % (fieldDim.x-1) :-(deltaCoordinate.X()-1) % (fieldDim.x-1) );


             }

            if(fabs(static_cast<double>(deltaCoordinate.Y()))>1){
               deltaCoordinate.YRef()=
               ( deltaCoordinate.Y()>0 ? -(deltaCoordinate.Y()+1) % (fieldDim.y-1) :-(deltaCoordinate.Y()-1) % (fieldDim.y-1) );
            }


            if(fabs(static_cast<double>(deltaCoordinate.Z()))>1){
               deltaCoordinate.ZRef()=
               ( deltaCoordinate.Z()>0 ? -(deltaCoordinate.Z()+1) % (fieldDim.z-1) :-(deltaCoordinate.Z()-1) % (fieldDim.z-1) );
            }
				
            deltaEnergyOld+=  deltaCoordinate.X()*oldCell->lambdaVecX+
                              deltaCoordinate.Y()*oldCell->lambdaVecY+
                              deltaCoordinate.Z()*oldCell->lambdaVecZ;

         }

         ///process new energy
         if(/*neighborPtr && */newCell && neighborPtr!=newCell){

            deltaCoordinate.XRef()=(ptAdj.x-pt.x);
            deltaCoordinate.YRef()=(ptAdj.y-pt.y);
            deltaCoordinate.ZRef()=(ptAdj.z-pt.z);
            
            if(fabs(static_cast<double>(deltaCoordinate.X()))>1){
               deltaCoordinate.XRef()=
               ( deltaCoordinate.X()>0 ? -(deltaCoordinate.X()+1) % (fieldDim.x-1) :-(deltaCoordinate.X()-1) % (fieldDim.x-1) );
               
            }

            if(fabs(static_cast<double>(deltaCoordinate.Y()))>1){
               deltaCoordinate.YRef()=
               ( deltaCoordinate.Y()>0 ? -(deltaCoordinate.Y()+1) % (fieldDim.y-1) :-(deltaCoordinate.Y()-1) % (fieldDim.y-1) );
            }
                        

            if(fabs(static_cast<double>(deltaCoordinate.Z()))>1){
               deltaCoordinate.ZRef()=
               ( deltaCoordinate.Z()>0 ? -(deltaCoordinate.Z()+1) % (fieldDim.z-1) :-(deltaCoordinate.Z()-1) % (fieldDim.z-1) );
            }
            
            deltaEnergyNew+=  deltaCoordinate.X()*newCell->lambdaVecX+
                              deltaCoordinate.Y()*newCell->lambdaVecY+
                              deltaCoordinate.Z()*newCell->lambdaVecZ;
            
         }


      }
       
   }

   
   return deltaEnergyNew-deltaEnergyOld;
   
} 



double MotilityPotentiallPlugin::changeEnergy(const Point3D &pt,  const CellG *newCell,
                                                const CellG *oldCell)
{

	return (this->*changeEnergyFcnPtr)(pt,newCell,oldCell);

}      



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



std::string MotilityPotentiallPlugin::toString(){
      return "MotilityPotentiall";
}


std::string MotilityPotentiallPlugin::steerableName(){
   
   return toString();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



};
