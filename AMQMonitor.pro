TEMPLATE = app
TARGET = AMQMonitor
QT += core \
    gui \
    sql \
    xml \
    network \
    svg \
    opengl \
    webkit
HEADERS += AMQConnectionWizardAuthenticationPage.h \
    AMQConnectionWizardNetworkPage.h \
    AMQConnectionWizard.h \
    AMQFrameProducer.h \
    AMQSubscriptionModel.h \
    AMQSubscription.h \
    AMQConnectionFrame.h \
    AMQConnection.h \
    AMQMonitor.h
SOURCES += AMQConnectionWizardAuthenticationPage.cpp \
    AMQConnectionWizardNetworkPage.cpp \
    AMQConnectionWizard.cpp \
    AMQFrameProducer.cpp \
    AMQSubscriptionModel.cpp \
    AMQSubscription.cpp \
    AMQConnectionFrame.cpp \
    AMQConnection.cpp \
    main.cpp \
    AMQMonitor.cpp
FORMS += AMQConnectionWizardAuthenticationPage.ui \
    AMQConnectionWizardNetworkPage.ui \
    AMQMonitor.ui
RESOURCES += 
