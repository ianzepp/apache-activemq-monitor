/*
 * AMQConnectionFrame.cpp
 *
 *  Created on: Mar 20, 2009
 *      Author: Ian Zepp
 */

#include "AMQConnectionFrame.h"
#include "AMQConnection.h"

AMQConnectionFrame::AMQConnectionFrame(AMQConnection *connection)
{
	this->connection = connection;
}

AMQConnectionFrame::AMQConnectionFrame(AMQConnection *connection, QByteArray frameData)
{
	this->connection = connection;

	// Split apart the headers from the body data
	QRegExp messageRegex("(.+\\n)\\n(.*)");
	QString message(frameData);
	int breakpoint = -1;

	if (!messageRegex.exactMatch(message))
	{
		throw QString("Failed to exactly match the incoming frame data: " + message);
	}

	// Extract the command value
	QString frameHeaders = messageRegex.cap(1);
	breakpoint = frameHeaders.indexOf('\n');

	// Save the command
	setCommand(frameHeaders.left(breakpoint).trimmed());

	// Strip the command and the terminating newline from the headers
	frameHeaders.remove(0, breakpoint + 1);

	// Save the header property => value mappings
	while (frameHeaders.contains(QChar('\n')))
	{
		breakpoint = frameHeaders.indexOf('\n');
		QRegExp propertyRegex("([^:]+):(.+)");
		QString propertySet(frameHeaders.left(breakpoint).trimmed());
		frameHeaders.remove(0, breakpoint + 1);

		if (!propertyRegex.exactMatch(propertySet))
		{
			throw QString("Failed to match property item regex: " + propertySet);
		}

		QString property = propertyRegex.cap(1).trimmed();
		QString value = propertyRegex.cap(2).trimmed();

		// Save to the object headers hash
		setHeader(property, value);
	}

	// Save the body data
	setPayload(messageRegex.cap(2).trimmed().toUtf8());
}

AMQConnectionFrame::~AMQConnectionFrame()
{
}

QByteArray AMQConnectionFrame::toFrame()
{
	if (getCachedFrameSize() > 0)
		return getCachedFrame();

	QByteArray data;

	// Append command
	data.append(getCommand());
	data.append('\n');

	// Append headers
	QHashIterator<QString, QString> iterator(getHeaders());

	while (iterator.hasNext())
	{
		iterator.next();

		data.append(iterator.key());
		data.append(':');
		data.append(' ');
		data.append(iterator.value());
		data.append('\n');
	}

	// Append content length header for messages
	if (getCommandType() == Send)
	{
		data.append(QString("content-length"));
		data.append(':');
		data.append(' ');
		data.append(QString::number(getPayload().size()));
		data.append('\n');
	}

	// Linebreak between headers and payload
	data.append('\n');

	// Append payload
	data.append(getPayload());
	data.append('\0');
	//data.append('\n');

	// Save to cached frame
	setCachedFrame(data);

	// Done.
	return data;
}

void AMQConnectionFrame::acknowledge()
{
	if (getCommandType() != Message)
		return; // must be a message type to acknowledge

	AMQConnectionFrame frame(getConnection());
	frame.setCommandType(AMQConnectionFrame::Acknowledge);

	if (getTransactionId().isEmpty())
		frame.setMessageId(getMessageId());
	else
		frame.setTransactionId(getTransactionId());

	frame.send();
}

void AMQConnectionFrame::send()
{
	getConnection()->sendFrame(*this);
}

AMQConnectionFrame &AMQConnectionFrame::operator=(const AMQConnectionFrame &frame)
{
	cachedFrame = frame.cachedFrame;
	command = frame.command;
	connection = frame.connection;
	headers = frame.headers;
	payload = frame.payload;
	return *this;
}
