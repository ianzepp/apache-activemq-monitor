/*
 * AMQConnection.cpp
 *
 *  Created on: Mar 20, 2009
 *      Author: Ian Zepp
 */

#include "AMQConnection.h"
#include "AMQSubscription.h"

AMQConnection::AMQConnection(QObject *parent, QString connectionId) :
	QObject(parent), frameSize(0), id(connectionId)
{
	qDebug() << "AMQConnection::AMQConnection(QObject *, QString)";

	// Initialize socket
	socket = new QTcpSocket(this);

	QObject::connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
	QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
	QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(socketProcessBuffer()));

	// Initialize loopback
	QObject::connect(this, SIGNAL(receivedFrame(AMQConnectionFrame)), this, SLOT(receiveFrame(AMQConnectionFrame)));

	// Initialize receive timer
	receiveTimer = new QTimer(this);
	receiveTimer->setInterval(0);

	QObject::connect(receiveTimer, SIGNAL(timeout()), this, SLOT(receiveQueueTimeout()));

	// Initialize send timer
	sendTimer = new QTimer(this);
	sendTimer->setInterval(0);

	QObject::connect(sendTimer, SIGNAL(timeout()), this, SLOT(sendQueueTimeout()));

	// Startup offline
	emit stateChanged(Disconnected);
}

AMQConnection::~AMQConnection()
{
	qDebug() << "AMQConnection::~AMQConnection()";

	if (socket->isOpen())
		socket->disconnectFromHost();
	emit stateChanged(Disconnected);
}

void AMQConnection::loadSettings()
{
	qDebug() << "void AMQConnection::loadSettings()";
	qDebug() << "\t Id:" << getId();

	setName(getSetting("name").toString());
	setRemoteHost(getSetting("remoteHost").toString());
	setRemotePort(getSetting("remotePort").toUInt());

	qDebug() << "\t Name:" << getName();
	qDebug() << "\t Remote Host:" << getRemoteHost();
	qDebug() << "\t Remote Port:" << getRemotePort();
}

void AMQConnection::saveSettings() const
{
	qDebug() << "void AMQConnection::saveSettings()";
	qDebug() << "\t Id:" << getId();
	qDebug() << "\t Name:" << getName();
	qDebug() << "\t Remote Host:" << getRemoteHost();
	qDebug() << "\t Remote Port:" << getRemotePort();

	setSetting("name", getName());
	setSetting("remoteHost", getRemoteHost());
	setSetting("remotePort", getRemotePort());
}

void AMQConnection::removeSettings() const
{
	qDebug() << "void AMQConnection::removeSettings()";

	QSettings().remove(getSettingsGroup());
}

void AMQConnection::socketConnected()
{
	qDebug() << "void AMQConnection::socketConnected()";

	emit
	stateChanged(Connected);emit
	stateChanged(Authenticating);

	// Send the authorization frame
	AMQConnectionFrame frame(this);
	frame.setCommandType(AMQConnectionFrame::Connect);
	frame.setUser(QString::null);
	frame.setPassword(QString::null);
	sendFrame(frame);
}

void AMQConnection::socketDisconnected()
{
	emit stateChanged(Disconnected);
}

void AMQConnection::socketError(QTcpSocket::SocketError)
{
}

void AMQConnection::socketProcessBuffer()
{
	qDebug() << "void AMQConnection::socketProcessBuffer()";
	qDebug() << "\t Buffer Size Before Read:" << buffer.size();

	// Pull in available data
	buffer += socket->readAll();

	qDebug() << "\t Buffer Size Now:" << buffer.size();

	// Setup the content length regex
	QRegExp matcher("content-length:\\s*(\\d+)\n");

	// Loop through the frames
	do
	{
		// Have we already checked for the required minimum frame length?
		if (frameSize && buffer.size() < frameSize)
		{
			qDebug() << "\t Waiting for minimum buffer size";
			qDebug() << "\t Expected Frame Size:" << frameSize;
			return;
		}

		// If the buffer is simply empty, return without an error message
		if (buffer.isEmpty())
			return;

		// Is there at least one null byte?
		if (false == buffer.contains('\0'))
		{
			qDebug() << "\t Buffer does not contain at least one NULL byte";
			return;
		}

		// Are at least all of the headers present?
		if (false == buffer.contains("\n\n"))
		{
			qDebug() << "\t Buffer does not contain a header/payload breakpoint";
			return;
		}

		// Determine the frame size
		if (matcher.indexIn(QString(buffer)) == -1)
			frameSize = buffer.indexOf('\0');
		else
			frameSize = buffer.indexOf("\n\n") + 2 + matcher.cap(1).toInt();

		qDebug() << "\t Required Frame Size:" << frameSize << "+ 2";

		// Do we have that much data available?
		if (frameSize > buffer.size())
		{
			qDebug() << "\t Buffer is not large enough for full read";
			return;
		}

		// Update the state
		emit
		stateChanged(ReceivingFrame);

		// Initialize the frame
		QByteArray frameBuffer = buffer.left(frameSize);

		// Trim the buffer data
		buffer.remove(0, frameSize + 2);

		qDebug() << "\t Buffer Size After Frame Read:" << buffer.size();

		// Reset the frame size
		frameSize = 0;

		// Push onto the queue
		receiveQueue.append(AMQConnectionFrame(this, frameBuffer));
		receiveTimer->start();

		// Update the state
		emit stateChanged(ReceivedFrame);

		// Done with this frame
	} while (true);
}

void AMQConnection::connectToHost()
{
	qDebug() << "void AMQConnection::connectToHost()";
	qDebug() << "\t Remote Host:" << getRemoteHost();
	qDebug() << "\t Remote Port:" << getRemotePort();

	emit
	stateChanged(Connecting);
	socket->connectToHost(getRemoteHost(), getRemotePort());
}

void AMQConnection::sendFrame(AMQConnectionFrame frame)
{
	qDebug() << "void AMQConnection::sendFrame(AMQConnectionFrame)";

	sendQueue.append(frame);
	sendTimer->start();
}

void AMQConnection::sendQueueFrame(AMQConnectionFrame frame)
{
	qDebug() << "void AMQConnection::sendQueueFrame(AMQConnectionFrame)";
	qDebug() << "\t Command:" << frame.getCommand();

	QHashIterator<QString, QString> it(frame.getHeaders());

	while (it.hasNext())
	{
		it.next();
		qDebug() << "\t Header:" << it.key() << "=>" << it.value();
	}

	// Show payload
	qDebug() << "\t Payload:" << QString(frame.getPayload());

	// Start send
	emit
	stateChanged(SendingFrame);

	// Do Send
	socket->write(frame.toFrame());

	// End send, emit frame
	emit
	stateChanged(SentFrame);
	emit sentFrame(frame);
}

void AMQConnection::receiveFrame(AMQConnectionFrame frame)
{
	qDebug() << "void AMQConnection::receiveFrame(AMQConnectionFrame)";
	qDebug() << "\t Command:" << frame.getCommand();

	QHashIterator<QString, QString> it(frame.getHeaders());

	// Show headers
	while (it.hasNext())
	{
		it.next();
		qDebug() << "\t Header:" << it.key() << "=>" << it.value();
	}

	// Show payload
	qDebug() << "\t Payload:" << QString(frame.getPayload());

	// Update state if connected
	if (frame.getCommandType() == AMQConnectionFrame::Connected)
		emit stateChanged(Authenticated);
}

void AMQConnection::receiveQueueTimeout()
{
	if (receiveQueue.isEmpty())
	{
		receiveTimer->stop();
	}
	else
	{
		emit receivedFrame(receiveQueue.first());
		receiveQueue.removeFirst();
	}
}

void AMQConnection::sendQueueTimeout()
{
	if (sendQueue.isEmpty())
	{
		sendTimer->stop();
	}
	else
	{
		sendQueueFrame(sendQueue.first());
		sendQueue.removeFirst();
	}
}

