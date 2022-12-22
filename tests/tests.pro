TEMPLATE = app

QT += testlib

include(../src/src.pri)

TARGET = tests

SOURCES += \
  main.cpp \
  testqmlpath.cpp

HEADERS += \
  helper.h \
  testqmlpath.h

RESOURCES += \
  qml/qml.qrc

OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
UI_DIR = .ui
