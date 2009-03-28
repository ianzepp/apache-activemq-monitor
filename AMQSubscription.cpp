/*
 * AMQSubscription.cpp
 *
 *  Created on: Mar 21, 2009
 *      Author: Ian Zepp
 */

#include "AMQSubscription.h"

AMQSubscription::AMQSubscription(AMQConnection *parent, QString subscriptionId) :
	QObject(parent), connection(parent), id(subscriptionId)
{
	QObject::connect(parent, SIGNAL(stateChanged(AMQConnection::ConnectionState)), this,
			SLOT(stateChanged(AMQConnection::ConnectionState)));
	QObject::connect(parent, SIGNAL(receivedFrame(AMQConnectionFrame)), this, SLOT(receivedFrame(AMQConnectionFrame)));
	QObject::connect(parent, SIGNAL(sentFrame(AMQConnectionFrame)), this, SLOT(sentFrame(AMQConnectionFrame)));

	// Load from settings
	loadSettings();
}

AMQSubscription::~AMQSubscription()
{
}

void AMQSubscription::loadSettings()
{
	setAcknowledged(getSetting("acknowledged", true).toBool());
	setAutomatic(getSetting("automatic", false).toBool());
	setDestination(getSetting("destination").toString());
	setSelector(getSetting("selector").toString());
}

void AMQSubscription::saveSettings() const
{
	setSetting("acknowledged", getAcknowledged());
	setSetting("automatic", getAutomatic());
	setSetting("destination", getDestination());
	setSetting("selector", getSelector());
}

void AMQSubscription::setSubscribed(bool subscribed)
{
	qDebug() << "void AMQSubscription::setSubscribed(bool)";
	qDebug() << "\t Id:" << getId();
	qDebug() << "\t Destination:" << getDestination();
	qDebug() << "\t Selector:" << getSelector();
	qDebug() << "\t Acknowledged:" << getAcknowledged();
	qDebug() << "\t Subscribed:" << subscribed;

	if (this->subscribed == subscribed)
		return;

	AMQConnectionFrame frame(getConnection());

	if (subscribed)
		frame.setCommandType(AMQConnectionFrame::Subscribe);
	else
		frame.setCommandType(AMQConnectionFrame::Unsubscribe);

	if (!getId().isEmpty())
		frame.setId(getId());
	if (!getSelector().isEmpty())
		frame.setSelector(getSelector());

	frame.setDestination(getDestination());
	frame.setReceiptRequired(true);
	frame.setAcknowledged(getAcknowledged());

	// Save the receipt token
	setSubscriptionReceipt(frame.getReceipt());

	// Send it.
	frame.send();
}

void AMQSubscription::send(QString message, AMQConnectionFrame::Priority priority)
{
	AMQConnectionFrame frame(getConnection());
	frame.setCommandType(AMQConnectionFrame::Send);
	frame.setDestination(getDestination());
	frame.setPayload(message.toUtf8());
	frame.setPriority(priority);
	frame.send();
}

void AMQSubscription::stateChanged(AMQConnection::ConnectionState state)
{
	if (state == AMQConnection::Authenticated && getAutomatic())
		setSubscribed(true);
}

void AMQSubscription::receivedFrame(AMQConnectionFrame frame)
{
	// TODO message processing
	switch (frame.getCommandType())
	{
	case AMQConnectionFrame::Receipt:
		receivedReceiptFrame(frame);
		break;

	case AMQConnectionFrame::Message:
		receivedMessageFrame(frame);
		break;

	default:
		break;
	}
}

void AMQSubscription::sentFrame(AMQConnectionFrame frame)
{
	if (frame.getCommandType() == AMQConnectionFrame::Disconnect)
		subscribed = false;
	if (frame.getCommandType() != AMQConnectionFrame::Unsubscribe)
		return;
	if (frame.getDestination() != getDestination())
		return;
	if (frame.getSelector() != getSelector())
		return;

	// Mark as unsubscribed
	subscribed = false;
}

void AMQSubscription::receivedReceiptFrame(AMQConnectionFrame frame)
{
	if (frame.getReceipt() != getSubscriptionReceipt())
		return;

	qDebug() << "void AMQSubscription::receivedReceiptFrame(AMQConnectionFrame)";
	qDebug() << "\t Receipt:" << frame.getReceipt();
	qDebug() << "\t Expected Receipt:" << getSubscriptionReceipt();
	qDebug() << "\t Subscribed To:" << getDestination();

	// Only mark subscribed once the subscription receipt comes back
	subscribed = true;
}

void AMQSubscription::receivedMessageFrame(AMQConnectionFrame frame)
{
	if (!getId().isEmpty() && getId() != frame.getSubscriptionId())
		return;
	else if (frame.getDestination() != getDestination())
		return;

	qDebug() << "void AMQSubscription::receivedMessageFrame(AMQConnectionFrame)";
	qDebug() << "\t Subscribed To:" << frame.getDestination();
	qDebug() << "\t Selector:" << frame.getSelector();
	qDebug() << "\t Payload:" << QString(frame.getPayload());

	emit
	subscriptionFrame(frame);

	if (getAcknowledged())
		frame.acknowledge();
}

