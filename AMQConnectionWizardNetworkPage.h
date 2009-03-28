#ifndef AMQCONNECTIONWIZARDNETWORKPAGE_H
#define AMQCONNECTIONWIZARDNETWORKPAGE_H

#include <QtCore/QDebug>
#include <QtCore/QSet>
#include <QtCore/QSettings>
#include <QtGui/QWizardPage>
#include "ui_AMQConnectionWizardNetworkPage.h"

class AMQConnectionWizardNetworkPage: public QWizardPage
{
Q_OBJECT

public:
	AMQConnectionWizardNetworkPage(QWidget *parent = 0);
	~AMQConnectionWizardNetworkPage();

	virtual bool isComplete() const;

private:
	Ui::AMQConnectionWizardNetworkPageClass ui;
};

#endif // AMQCONNECTIONWIZARDNETWORKPAGE_H
