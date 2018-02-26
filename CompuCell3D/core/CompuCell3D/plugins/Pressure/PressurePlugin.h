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

#ifndef PRESSUREPLUGIN_H
#define PRESSUREPLUGIN_H

#include <CompuCell3D/CC3D.h>

#include "PressureDLLSpecifier.h"

class CC3DXMLElement;

namespace CompuCell3D {
	class Potts3D;
	class CellG;


	class PRESSURE_EXPORT PressureEnergyParam{
	public:
		PressureEnergyParam():targetPressure(0.0),lambdaPressure(0.0){}
		//double targetPressure;
		double lambdaPressure;
		std::string typeName;
	};

	class PRESSURE_EXPORT PressurePlugin : public Plugin , public EnergyFunction {
		Potts3D *potts;
		CC3DXMLElement *xmlData;
		ParallelUtilsOpenMP *pUtils;
		ExpressionEvaluatorDepot eed;
		bool energyExpressionDefined;


		std::string pluginName;

		//double targetPressure;
		double lambdaPressure;
		enum FunctionType {GLOBAL=0,BYCELLTYPE=1,BYCELLID=2};
		FunctionType functionType;
		std::vector<PressureEnergyParam> pressureEnergyParamVector;


		typedef double (PressurePlugin::*changeEnergy_t)(const Point3D &pt, const CellG *newCell,const CellG *oldCell);

		PressurePlugin::changeEnergy_t changeEnergyFcnPtr;
		double changeEnergyGlobal(const Point3D &pt, const CellG *newCell,const CellG *oldCell);
		double changeEnergyByCellType(const Point3D &pt, const CellG *newCell,const CellG *oldCell);
		double changeEnergyByCellId(const Point3D &pt, const CellG *newCell,const CellG *oldCell);
		double customExpressionFunction(double _lambdaPressure/*,double _targetPressure*/, double _pressureBefore,double _pressureAfter);

	public:
		PressurePlugin():potts(0),energyExpressionDefined(false),pUtils(0),pluginName("Pressure"){};
		virtual ~PressurePlugin();

		// SimObject interface
		virtual void extraInit(Simulator *simulator);
		virtual void init(Simulator *simulator, CC3DXMLElement *_xmlData);
		virtual void update(CC3DXMLElement *_xmlData, bool _fullInitFlag=false);
		virtual void handleEvent(CC3DEvent & _event);

		//EnergyFunction interface
		virtual double changeEnergy(const Point3D &pt, const CellG *newCell,const CellG *oldCell);
		virtual std::string steerableName();
		virtual std::string toString();
	};
};
#endif
