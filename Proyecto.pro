TEMPLATE = subdirs
TARGET = Proyecto

SUBDIRS += gcodelib
SUBDIRS += TestGCodeInterpret
SUBDIRS += CNCLaserPUCP

TestGCodeInterpret.depends = gcodelib
CNCLaserPUCP.depends = gcodelib

