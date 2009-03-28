#include "AMQMonitor.h"
#include "AMQConnection.h"
#include "AMQConnectionWizard.h"
#include "AMQFrameProducer.h"
#include "AMQSubscription.h"
#include "AMQSubscriptionModel.h"

AMQMonitor::AMQMonitor(QWidget *parent) :
	QMainWindow(parent)
{
	ui.setupUi(this);

	// Initialize the settings
	QCoreApplication::setOrganizationName("Zepp Software");
	QCoreApplication::setOrganizationDomain("ianzepp.com");
	QCoreApplication::setApplicationName("AMQMonitor");

	// Initialize the action signals

	// Initialize connections from settings
	QStringListIterator it(getConnectionIds());

	while (it.hasNext())
	{
		loadConnection(it.next());
	}

}

AMQMonitor::~AMQMonitor()
{

}

void AMQMonitor::triggeredNewConnection()
{
	qDebug() << "void AMQMonitor::triggeredNewConnection()";

	AMQConnectionWizard *wizard = new AMQConnectionWizard(this);

	QObject::connect(wizard, SIGNAL(createdConnection(QString)), this, SLOT(loadConnection(QString)));

	wizard->show();
}

void AMQMonitor::triggeredNewSubscription()
{
	qDebug() << "void AMQMonitor::triggeredNewSubscription()";
}

void AMQMonitor::triggeredDelete()
{
	qDebug() << "void AMQMonitor::triggeredDelete()";

	// Get the current selected item
	QTreeWidgetItem *item = getSelectedItem();

	if (item == 0)
		return;

	QString itemId = getItemId(item);

	if (hasConnection(itemId))
	{
		AMQConnection *connection = getConnection(itemId);

		Q_CHECK_PTR(connection);
		qDebug() << "\t Removing Connection:";
		qDebug() << "\t\t Id:" << connection->getId();
		qDebug() << "\t\t Remote Host:" << connection->getRemoteHost();
		qDebug() << "\t\t Remote Port:" << connection->getRemotePort();

		connection->removeSettings();
		removeConnection(connection->getId());

		delete connection;
		delete item;
	}

}

void AMQMonitor::loadConnection(QString connectionId)
{
	AMQConnection *connection = new AMQConnection(this, connectionId);
	connection->loadSettings();

	// Initialize signals & slots
	QObject::connect(connection, SIGNAL(stateChanged(AMQConnection::ConnectionState)), this, SLOT(stateChanged(AMQConnection::ConnectionState)));
	QObject::connect(connection, SIGNAL(receivedFrame(AMQConnectionFrame)), this, SLOT(receivedFrame(AMQConnectionFrame)));
	QObject::connect(connection, SIGNAL(sentFrame(AMQConnectionFrame)), this, SLOT(sentFrame(AMQConnectionFrame)));

	// Create the tree item
	QTreeWidgetItem *item = new QTreeWidgetItem(ui.treeWidget);
	item->setText(0, connection->getName());
	item->setText(1, connection->getId());

	// Add to the mapping
	insertConnection(connectionId, connection);

	// Add two monitoring test items
	AMQSubscription *subscription = new AMQSubscription(connection, "test");
	subscription->setDestination("/topic/com.beatport.content.delivery.notifications.aspera");
	subscription->setAutomatic(true);

	// Connect
	connection->connectToHost();
}

void AMQMonitor::updateFileMenuContext()
{
}

void AMQMonitor::stateChanged(AMQConnection::ConnectionState)
{

}

void AMQMonitor::receivedFrame(AMQConnectionFrame)
{

}

void AMQMonitor::sentFrame(AMQConnectionFrame)
{

}

