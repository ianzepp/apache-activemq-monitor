#ifndef AMQMONITOR_H
#define AMQMONITOR_H

#include <QtCore/QCoreApplication>
#include <QtCore/QHash>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QMainWindow>

#include "AMQConnection.h"
#include "AMQConnectionFrame.h"
#include "AMQSubscription.h"

// UI classes
#include "ui_AMQMonitor.h"

class AMQMonitor: public QMainWindow
{
Q_OBJECT

private:
	enum TreeColumn
	{
		Description, Id
	};

public:
	AMQMonitor(QWidget *parent = 0);
	~AMQMonitor();

public Q_SLOTS:
	void triggeredNewConnection();
	void triggeredNewSubscription();
	void triggeredDelete();

private Q_SLOTS:
	void stateChanged(AMQConnection::ConnectionState);
	void receivedFrame(AMQConnectionFrame);
	void sentFrame(AMQConnectionFrame);
	void loadConnection(QString connectionId);

	// Called by the menus
	void updateFileMenuContext();

private:
	bool isConnectionSelected()
	{
		return hasConnection(getItemId(getSelectedItem()));
	}

	bool isSubscriptionSelected()
	{
		return hasSubscription(getItemId(getSelectedItem()));
	}

	AMQConnection *getConnection(QString connectionId) const
	{
		if (connections.contains(connectionId))
			return connections.value(connectionId);
		else
			return NULL;
	}

	bool hasConnection(QString connectionId) const
	{
		return connections.contains(connectionId);
	}

	void insertConnection(QString connectionId, AMQConnection *connection)
	{
		connections.insert(connectionId, connection);
	}

	void removeConnection(QString connectionId)
	{
		connections.remove(connectionId);
	}

	QStringList getConnectionIds() const
	{
		QSettings settings;
		settings.beginGroup("connections");
		return settings.childGroups();
	}

	QTreeWidgetItem *getSelectedItem() const
	{
		QList<QTreeWidgetItem *> list = ui.treeWidget->selectedItems();
		return list.isEmpty() ? NULL : list.first();
	}

	AMQSubscription *getSubscription(QString subscriptionId) const
	{
		if (subscriptions.contains(subscriptionId))
			return subscriptions.value(subscriptionId);
		else
			return NULL;
	}

	bool hasSubscription(QString subscriptionId) const
	{
		return subscriptions.contains(subscriptionId);
	}

	void insertSubscription(QString subscriptionId, AMQSubscription *subscription)
	{
		subscriptions.insert(subscriptionId, subscription);
	}

	void removeSubscription(QString subscriptionId)
	{
		subscriptions.remove(subscriptionId);
	}

	QStringList getSubscriptionIds(QString connectionId) const
	{
		QSettings settings;
		settings.beginGroup("connections/" + connectionId + "/subscriptions");
		return settings.childKeys();
	}

	QString getItemId(QTreeWidgetItem *item)
	{
		return item ? item->text(Id) : QString::null;
	}

private:
	Ui::AMQMonitorClass ui;
	QHash<QString, AMQConnection *> connections;
	QHash<QString, AMQSubscription *> subscriptions;
};

#endif // AMQMONITOR_H
