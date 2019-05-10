#ifndef MOTILITYPOTENTIAL_EXPORT_H
#define MOTILITYPOTENTIAL_EXPORT_H

    #if defined(_WIN32)
      #ifdef ExternalPotentialShared_EXPORTS
          #define MOTILITYPOTENTIAL_EXPORT __declspec(dllexport)
          #define MOTILITYPOTENTIAL_EXPIMP_TEMPLATE
      #else
          #define MOTILITYPOTENTIAL_EXPORT __declspec(dllimport)
          #define MOTILITYPOTENTIAL_EXPIMP_TEMPLATE extern
      #endif
    #else
         #define MOTILITYPOTENTIAL_EXPORT
         #define MOTILITYPOTENTIAL_EXPIMP_TEMPLATE
    #endif

#endif
