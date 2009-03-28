/*
 * AMQSubscriptionModel.cpp
 *
 *  Created on: Mar 21, 2009
 *      Author: Ian Zepp
 */

#include "AMQSubscriptionModel.h"

AMQSubscriptionModel::AMQSubscriptionModel(AMQSubscription *parent) :
	QAbstractTableModel(parent)
{
	QObject::connect(parent, SIGNAL(subscriptionFrame(AMQConnectionFrame)), this, SLOT(subscriptionFrame(AMQConnectionFrame)));
}

AMQSubscriptionModel::~AMQSubscriptionModel()
{
}

void AMQSubscriptionModel::subscriptionFrame(AMQConnectionFrame frame)
{
	qDebug() << "void AMQSubscriptionModel::subscriptionFrame(AMQConnectionFrame)";
	qDebug() << "\t Stored Frames:" << frames.size();

	beginInsertRows(QModelIndex(), frames.size(), frames.size());
	frames.append(frame);
	endInsertRows();
}

QVariant AMQSubscriptionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	if (orientation != Qt::Horizontal)
		return QVariant();

	switch (section)
	{
	case MessageId:
		return QVariant("Message Id");
	case Priority:
		return QVariant("Priority");
	case Payload:
		return QVariant("Payload");
	default:
		return QVariant();
	}
}

QVariant AMQSubscriptionModel::data(const QModelIndex &index, int role) const
{
	AMQConnectionFrame frame = frames.at(index.row());

	if (role == Qt::ToolTipRole)
		return QVariant(frame.getPayloadDocument().toByteArray());
	if (role != Qt::DisplayRole)
		return QVariant();

	switch (index.column())
	{
	case MessageId:
		return QVariant(frame.getMessageId());
	case Priority:
		return QVariant(QString::number(frame.getPriority()));
	case Payload:
		return QVariant(frame.getPayload());
	default:
		return QVariant();
	}
}

