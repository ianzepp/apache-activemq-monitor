/*
 * AMQSubscriptionModel.h
 *
 *  Created on: Mar 21, 2009
 *      Author: Ian Zepp
 */

#ifndef AMQSUBSCRIPTIONMODEL_H_
#define AMQSUBSCRIPTIONMODEL_H_

#include <QtCore/QAbstractTableModel>
#include <QtCore/QVariant>
#include <QtCore/QVector>

#include "AMQConnectionFrame.h"
#include "AMQSubscription.h"

class AMQSubscriptionModel: public QAbstractTableModel
{
Q_OBJECT

public:
	static const int MAX_ROWS = 1000;

	enum Column
	{
		MessageId, Priority, Payload, TotalColumns
	};

public:
	AMQSubscriptionModel(AMQSubscription *parent);
	virtual ~AMQSubscriptionModel();

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
	{
		return AMQSubscriptionModel::TotalColumns;
	}

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
	{
		return frames.size();
	}

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

private Q_SLOTS:
	void subscriptionFrame(AMQConnectionFrame);

private:
	QVector<AMQConnectionFrame> frames;
};

#endif /* AMQSUBSCRIPTIONMODEL_H_ */
