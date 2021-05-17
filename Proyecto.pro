TEMPLATE = subdirs
TARGET = Proyecto

SUBDIRS += gcodelib \
    CNCLaserPUCP
SUBDIRS += TestGCodeInterpret


TestGCodeInterpret.depends = gcodelib
