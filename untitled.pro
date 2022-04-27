QT       += core gui
QT += network xlsx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
#CONFIG += resources_big

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addproductdialog.cpp \
    addproductitemdialog.cpp \
    adjustpricedialog.cpp \
    buttondelegate.cpp \
    buttonitemdelegate.cpp \
    configtool.cpp \
    editpasswdialog.cpp \
    editproductdialog.cpp \
    editproductitemdialog.cpp \
    formatexception.cpp \
    imageshowdialog.cpp \
    imgpathdelegate.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    managewindow.cpp \
    productitem.cpp \
    products.cpp \
    producttable.cpp \
    qexceltool.cpp \
    qhttptool.cpp \
    salesman.cpp \
    salesmantable.cpp \
    waitingdialog.cpp

HEADERS += \
    OperationNumber.h \
    addproductdialog.h \
    addproductitemdialog.h \
    adjustpricedialog.h \
    buttondelegate.h \
    buttonitemdelegate.h \
    configtool.h \
    editpasswdialog.h \
    editproductdialog.h \
    editproductitemdialog.h \
    formatexception.h \
    imageshowdialog.h \
    imgpathdelegate.h \
    logindialog.h \
    mainwindow.h \
    managewindow.h \
    productitem.h \
    products.h \
    producttable.h \
    qexceltool.h \
    qhttptool.h \
    salesman.h \
    salesmantable.h \
    waitingdialog.h

FORMS += \
    addproductdialog.ui \
    addproductitemdialog.ui \
    adjustpricedialog.ui \
    editpasswdialog.ui \
    editproductdialog.ui \
    editproductitemdialog.ui \
    imageshowdialog.ui \
    logindialog.ui \
    mainwindow.ui \
    managewindow.ui \
    waitingdialog.ui

TRANSLATIONS += \
    untitled_zh_CN.ts
TARGET=QuotationSystem
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
