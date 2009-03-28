/*
 * AMQFrameProducer.h
 *
 *  Created on: Mar 22, 2009
 *      Author: Ian Zepp
 */

#ifndef AMQFRAMEPRODUCER_H_
#define AMQFRAMEPRODUCER_H_

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QTimer>

class AMQConnection;

class AMQFrameProducer: public QObject
{
Q_OBJECT

public:
	AMQFrameProducer(AMQConnection *parent = 0);
	virtual ~AMQFrameProducer();

	void setDestination(QString destination)
	{
		this->destination = destination;
	}

	void setPayload(QString payload)
	{
		this->payload = payload;
	}

private Q_SLOTS:
	void sendFrame();

private:
	AMQConnection *connection;
	QString destination;
	QString payload;
	QTimer *producerTimer;
};

#endif /* AMQFRAMEPRODUCER_H_ */
