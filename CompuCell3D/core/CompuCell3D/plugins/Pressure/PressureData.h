/*
 * PressureData.h
 *
 *  Created on: Mar 28, 2018
 *      Author: jfgianlupi
 */

#include <vector>
#include "PressureDLLSpecifier.h"

namespace CompuCell3D {


   class PRESSURE_EXPORT PressureData{
      public:
         PressureData(){};

         ~PressureData(){};
         std::vector<float> array;
         int x;


   };
};
#endif
