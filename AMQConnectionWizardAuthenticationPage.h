#ifndef AMQCONNECTIONWIZARDAUTHENTICATIONPAGE_H
#define AMQCONNECTIONWIZARDAUTHENTICATIONPAGE_H

#include <QtGui/QWizardPage>
#include "ui_AMQConnectionWizardAuthenticationPage.h"

class AMQConnectionWizardAuthenticationPage: public QWizardPage
{
Q_OBJECT

public:
	enum AuthenticationType
	{
		Anonymous, Simple
	};

public:
	AMQConnectionWizardAuthenticationPage(QWidget *parent = 0);
	~AMQConnectionWizardAuthenticationPage();

	virtual bool isComplete() const;

public Q_SLOTS:
	void changedAuthenticationType(int);
	void changedUser(QString);
	void changedPassword(QString);

private:
	Ui::AMQConnectionWizardAuthenticationPageClass ui;
};

#endif // AMQCONNECTIONWIZARDAUTHENTICATIONPAGE_H
