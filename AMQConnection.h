/*
 * AMQConnection.h
 *
 *  Created on: Mar 20, 2009
 *      Author: Ian Zepp
 */

#ifndef AMQCONNECTION_H_
#define AMQCONNECTION_H_

#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QRegExp>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QVariant>
#include <QtNetwork/QTcpSocket>

// Required for slots
#include "AMQConnectionFrame.h"

// Forward declarations
class AMQSubscription;

class AMQConnection: public QObject
{
Q_OBJECT

public:
	enum ConnectionState
	{
		Connecting,
		Connected,
		Authenticating,
		Authenticated,
		Listening,
		ReceivingFrame,
		ReceivedFrame,
		SendingFrame,
		SentFrame,
		Disconnecting,
		Disconnected
	};

	enum Port
	{
		Stomp = 61613, OpenWire = 61616
	};

public:
	AMQConnection(QObject *parent, QString connectionId);
	virtual ~AMQConnection();

public Q_SLOTS:
	void connectToHost();
	void sendFrame(AMQConnectionFrame);
	void receiveFrame(AMQConnectionFrame);

	void loadSettings();
	void saveSettings() const;
	void removeSettings() const;

public:
	inline QString getId() const
	{
		return id;
	}

	inline QString getName() const
	{
		return name;
	}

	inline void setName(QString name)
	{
		this->name = name;
	}

	inline QString getRemoteHost() const
	{
		return remoteHost;
	}

	inline void setRemoteHost(QString remoteHost)
	{
		this->remoteHost = remoteHost;
	}

	inline quint16 getRemotePort() const
	{
		return remotePort;
	}

	inline void setRemotePort(quint16 remotePort)
	{
		this->remotePort = remotePort;
	}

Q_SIGNALS:
	void stateChanged(AMQConnection::ConnectionState);
	void receivedFrame(AMQConnectionFrame);
	void sentFrame(AMQConnectionFrame);

private Q_SLOTS:
	void socketConnected();
	void socketDisconnected();
	void socketError(QTcpSocket::SocketError);
	void socketProcessBuffer();

	void receiveQueueTimeout();
	void sendQueueTimeout();
	void sendQueueFrame(AMQConnectionFrame);

private:
	inline QString getSettingsGroup() const
	{
		return QString("connections/" + getId() + "/");
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
	QString id;

	// Settings based
	QString name;
	QString remoteHost;
	quint16 remotePort;

	// Instance based
	QList<AMQSubscription *> subscriptions;
	QList<AMQConnectionFrame> receiveQueue;
	QList<AMQConnectionFrame> sendQueue;
	QTimer *receiveTimer;
	QTimer *sendTimer;
	QTcpSocket *socket;
	QByteArray buffer;
	qint32 frameSize;
};

#endif /* AMQCONNECTION_H_ */
