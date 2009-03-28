/*
 * AMQSubscription.h
 *
 *  Created on: Mar 21, 2009
 *      Author: Ian Zepp
 */

#ifndef AMQSUBSCRIPTION_H_
#define AMQSUBSCRIPTION_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QUuid>

// Required for slots
#include "AMQConnectionFrame.h"
#include "AMQConnection.h"

class AMQSubscription: public QObject
{
Q_OBJECT

public:
	AMQSubscription(AMQConnection *parent, QString subscriptionId);
	virtual ~AMQSubscription();

public Q_SLOTS:
	void loadSettings();
	void saveSettings() const;
	void send(QString message, AMQConnectionFrame::Priority priority = AMQConnectionFrame::Normal);
	void setSubscribed(bool subscribed);

public:
	bool getAcknowledged() const
	{
		return acknowledged;
	}

	void setAcknowledged(bool acknowledged)
	{
		this->acknowledged = acknowledged;
	}

	bool getAutomatic() const
	{
		return automatic;
	}

	void setAutomatic(bool automatic)
	{
		this->automatic = automatic;
	}

	AMQConnection *getConnection() const
	{
		return connection;
	}

	QString getConnectionId() const
	{
		return getConnection()->getId();
	}

	QString getDestination() const
	{
		return destination;
	}

	void setDestination(QString destination)
	{
		this->destination = destination;
	}

	QString getId() const
	{
		return id;
	}

	void setId(QString id)
	{
		this->id = id;
	}

	void setGeneratedId()
	{
		this->id = QUuid::createUuid().toString();
	}

	QString getSelector() const
	{
		return selector;
	}

	void setSelector(QString selector)
	{
		this->selector = selector;
	}

	bool getSubscribed() const
	{
		return subscribed;
	}

	QString getSubscriptionReceipt() const
	{
		return subscriptionReceipt;
	}

	void setSubscriptionReceipt(QString subscriptionReceipt)
	{
		this->subscriptionReceipt = subscriptionReceipt;
	}

Q_SIGNALS:
	void subscriptionFrame(AMQConnectionFrame);

private Q_SLOTS:
	void stateChanged(AMQConnection::ConnectionState);
	void receivedFrame(AMQConnectionFrame);
	void receivedReceiptFrame(AMQConnectionFrame);
	void receivedMessageFrame(AMQConnectionFrame);
	void sentFrame(AMQConnectionFrame);

private:
	inline QString getSettingsGroup() const
	{
		return QString("connections/" + getConnectionId() + "/subscriptions/" + getId() + "/");
	}

	inline QVariant getSetting(QString key, QVariant defaultValue = QVariant()) const
	{
		return QSettings().value(getSettingsGroup() + key, defaultValue);
	}

	inline void setSetting(QString key, QVariant value) const
	{
		QSettings().setValue(getSettingsGroup() + key, value);
	}

private:
	AMQConnection *connection;
	QString id;
	QString destination;
	QString selector;
	QString subscriptionReceipt;
	bool automatic;
	bool subscribed;
	bool acknowledged;
};

#endif /* AMQSUBSCRIPTION_H_ */
