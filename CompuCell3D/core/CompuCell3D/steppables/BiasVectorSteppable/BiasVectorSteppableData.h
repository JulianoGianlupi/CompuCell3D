
#ifndef BIASVECTORSTEPPABLEPATA_H
#define BIASVECTORSTEPPABLEPATA_H


#include <vector>
#include "BiasVectorSteppableDLLSpecifier.h"

namespace CompuCell3D {

   
   class BIASVECTORSTEPPABLE_EXPORT BiasVectorSteppableData{
      public:
         BiasVectorSteppableData(){};
         
         ~BiasVectorSteppableData(){};
         std::vector<float> array;
         int x;
         
         
   };
};
#endif
