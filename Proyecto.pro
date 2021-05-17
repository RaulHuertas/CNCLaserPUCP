TEMPLATE = subdirs
TARGET = Proyecto

SUBDIRS += gcodelib
SUBDIRS += TestGCodeInterpret


TestGCodeInterpret.depends = gcodelib
