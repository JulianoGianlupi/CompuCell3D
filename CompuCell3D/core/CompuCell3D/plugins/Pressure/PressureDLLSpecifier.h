#ifndef PRESSURE_EXPORT_H
#define PRESSURE_EXPORT_H

    #if defined(_WIN32)
      #ifdef PressureShared_EXPORTS
          #define PRESSURE_EXPORT __declspec(dllexport)
          #define PRESSURE_EXPIMP_TEMPLATE
      #else
          #define PRESSURE_EXPORT __declspec(dllimport)
          #define PRESSURE_EXPIMP_TEMPLATE extern
      #endif
    #else
         #define PRESSURE_EXPORT
         #define PRESSURE_EXPIMP_TEMPLATE
    #endif

#endif
