/*
 * AMQFrameProducer.cpp
 *
 *  Created on: Mar 22, 2009
 *      Author: Ian Zepp
 */

#include "AMQFrameProducer.h"
#include "AMQConnection.h"
#include "AMQConnectionFrame.h"

AMQFrameProducer::AMQFrameProducer(AMQConnection *parent) :
	QObject(parent)
{
	connection = parent;

	producerTimer = new QTimer(this);
	producerTimer->setInterval(3000);
	producerTimer->start();

	QObject::connect(producerTimer, SIGNAL(timeout()), this, SLOT(sendFrame()));
}

AMQFrameProducer::~AMQFrameProducer()
{
}

void AMQFrameProducer::sendFrame()
{
	AMQConnectionFrame frame(connection);
	frame.setCommandType(AMQConnectionFrame::Send);
	frame.setDestination(destination);
	frame.setPayload(payload.toUtf8());
	frame.send();
}
