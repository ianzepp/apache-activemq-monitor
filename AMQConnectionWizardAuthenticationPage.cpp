#include "AMQConnectionWizardAuthenticationPage.h"

AMQConnectionWizardAuthenticationPage::AMQConnectionWizardAuthenticationPage(QWidget *parent) :
	QWizardPage(parent)
{
	ui.setupUi(this);

	// Register option fields
	registerField("authenticationType*", ui.authenticationType);
	registerField("user", ui.user, "currentText");
	registerField("password", ui.password);
	registerField("storePassword", ui.storePassword);

	// Connect signals for overriden isComplete method.
	QObject::connect(ui.user, SIGNAL(currentIndexChanged(int)), this, SIGNAL(completeChanged()));
	QObject::connect(ui.user, SIGNAL(editTextChanged(QString)), this, SIGNAL(completeChanged()));
	QObject::connect(ui.password, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));

	// Set title and subtitle
	setTitle(trUtf8("Authentication Settings"));

}

AMQConnectionWizardAuthenticationPage::~AMQConnectionWizardAuthenticationPage()
{
}

bool AMQConnectionWizardAuthenticationPage::isComplete() const
{
	switch (ui.authenticationType->currentIndex())
	{
	case Anonymous:
		return true;

	case Simple:
		if (ui.user->currentText().isEmpty())
			return false;
		if (ui.password->text().isEmpty() && ui.storePassword->isChecked())
			return false;
		return true;

	default:
		return true;
	}
}

void AMQConnectionWizardAuthenticationPage::changedAuthenticationType(int authenticationType)
{
	switch (ui.authenticationType->currentIndex())
	{
	case Anonymous:
		ui.user->setEnabled(false);
		ui.password->setEnabled(false);
		ui.storePassword->setEnabled(false);
		ui.storePassword->setChecked(false);
		break;

	case Simple:
		ui.user->setEnabled(true);
		ui.password->setEnabled(ui.storePassword->isChecked());
		ui.storePassword->setEnabled(true);
		break;
	}
}

void AMQConnectionWizardAuthenticationPage::changedUser(QString user)
{
}

void AMQConnectionWizardAuthenticationPage::changedPassword(QString password)
{
}

