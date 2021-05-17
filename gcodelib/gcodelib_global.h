#ifndef GCODELIB_GLOBAL_H
#define GCODELIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GCODELIB_LIBRARY)
#  define GCODELIB_EXPORT Q_DECL_EXPORT
#else
#  define GCODELIB_EXPORT Q_DECL_IMPORT
#endif

#endif // GCODELIB_GLOBAL_H
