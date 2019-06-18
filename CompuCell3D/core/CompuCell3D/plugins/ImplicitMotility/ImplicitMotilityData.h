
#ifndef IMPLICITMOTILITYPATA_H
#define IMPLICITMOTILITYPATA_H


#include <vector>
#include "ImplicitMotilityDLLSpecifier.h"

namespace CompuCell3D {

   
   class IMPLICITMOTILITY_EXPORT ImplicitMotilityData{
      public:
         ImplicitMotilityData(){};
         
         ~ImplicitMotilityData(){};
         std::vector<float> array;
         int x;
         
         
   };
};
#endif
