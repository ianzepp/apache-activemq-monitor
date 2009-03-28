#include "AMQConnectionWizardNetworkPage.h"

AMQConnectionWizardNetworkPage::AMQConnectionWizardNetworkPage(QWidget *parent) :
	QWizardPage(parent)
{
	qDebug() << "AMQConnectionWizardNetworkPage::AMQConnectionWizardNetworkPage(QWidget *)";

	ui.setupUi(this);

	// Register option fields
	registerField("name*", ui.name);
	registerField("remoteHost*", ui.remoteHost, "currentText");
	registerField("remotePort", ui.remotePort, "currentText");
	registerField("automatic", ui.automatic);

	// Connect signals for overriden isComplete method.
	QObject::connect(ui.name, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
	QObject::connect(ui.remoteHost, SIGNAL(currentIndexChanged(int)), this, SIGNAL(completeChanged()));
	QObject::connect(ui.remoteHost, SIGNAL(editTextChanged(QString)), this, SIGNAL(completeChanged()));

	// Set title and subtitle
	setTitle(trUtf8("Network Connection Settings"));

	// Preload similar hostnames
	qDebug() << "\t Preloading other connection remoteHost values";

	QSettings settings;
	settings.beginGroup("connections");
	QStringListIterator it(settings.childGroups());
	QSet<QString> remoteHosts;

	while (it.hasNext())
	{
		QString connectionId = it.next();
		QString remoteHost = settings.value(connectionId + "/remoteHost").toString();

		qDebug() << "\t Connection Id:" << connectionId;
		qDebug() << "\t\t Remote Host:" << remoteHost;

		remoteHosts.insert(remoteHost);
	}

	QSetIterator<QString> sit(remoteHosts);

	while (sit.hasNext())
	{
		QString remoteHost = sit.next();

		qDebug() << "\t Adding to ui.remoteHost:" << remoteHost;

		ui.remoteHost->addItem(remoteHost);
	}
}

AMQConnectionWizardNetworkPage::~AMQConnectionWizardNetworkPage()
{
}

bool AMQConnectionWizardNetworkPage::isComplete() const
{
	if (ui.name->text().isEmpty())
		return false;
	else if (ui.remoteHost->currentText().isEmpty())
		return false;
	else
		return true;
}
