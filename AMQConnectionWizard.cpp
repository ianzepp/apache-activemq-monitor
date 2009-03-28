/*
 * AMQConnectionWizard.cpp
 *
 *  Created on: Mar 23, 2009
 *      Author: Ian Zepp
 */

#include "AMQConnectionWizard.h"
#include "AMQConnectionWizardAuthenticationPage.h"
#include "AMQConnectionWizardNetworkPage.h"

AMQConnectionWizard::AMQConnectionWizard(QWidget *parent) :
	QWizard(parent)
{
	// Add intro page
	QWizardPage *page = new QWizardPage;
	page->setTitle(trUtf8("Create a new connection"));
	page->setSubTitle(trUtf8("The next few pages will help you to set up a remote connection"
		" to an ActiveMQ messaging server."));
	addPage(page);

	// Add content pages
	addPage(new AMQConnectionWizardNetworkPage(this));
	addPage(new AMQConnectionWizardAuthenticationPage(this));
	setWindowTitle(trUtf8("Create a New Connection"));
}

void AMQConnectionWizard::accept()
{
	qDebug() << "void AMQConnectionWizard::accept()";

	QSettings settings;
	QString connectionId = QUuid::createUuid().toString();

	qDebug() << "\t Connection Id:" << connectionId;
	qDebug() << "\t\t Name:" << field("name");
	qDebug() << "\t\t Remote Host:" << field("remoteHost");
	qDebug() << "\t\t Remote Port:" << field("remotePort");
	qDebug() << "\t\t Auth Type:" << field("authenticationType");
	qDebug() << "\t\t Auth User:" << field("user");
	qDebug() << "\t\t Auth Password:" << field("password");
	qDebug() << "\t\t Password Is Saved?" << field("storePassword");

	// Create the connection
	settings.beginGroup("connections/" + connectionId);
	settings.setValue("name", field("name"));
	settings.setValue("remoteHost", field("remoteHost"));
	settings.setValue("remotePort", field("remotePort"));

	// Connection authentication
	settings.setValue("auth/type", field("authenticationType"));
	settings.setValue("auth/user", field("user"));

	if (field("storePassword").toBool())
		settings.setValue("auth/password", field("password"));
	else
		settings.setValue("auth/password", "");

	// Update window
	emit
	createdConnection(connectionId);

	// Call the super
	QWizard::accept();
}
