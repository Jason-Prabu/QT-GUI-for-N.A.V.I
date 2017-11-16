#include "mainwindow.h"
#include "ui_mainwindow.h"

#define PORT  1234

int row_count = 0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    model = new QStandardItemModel(1,4,this);               //2 Rows and 3 Columns
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("BUS STOP"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("TIME"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("ACCEPT"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("CLEAR"));


//    QPushButton *accept = new QPushButton("ACCEPT");
//    ui->tableView->setIndexWidget(model->index(0,2), accept);

//    QPushButton *clear = new QPushButton("clear");
//    ui->tableView->setIndexWidget(model->index(0,3), clear);

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::Internet_Conn_Test()
{
    QTcpSocket socket;
    socket.connectToHost("8.8.8.8", 53);          // google DNS to check for internet connectivity
    if (socket.waitForConnected(100))
    {
        qDebug() << socket.localAddress();
        socket.disconnect();
        socket.deleteLater();
        socket.close();
        return 1;
    }
    else
    {
        qWarning() << socket.errorString();
        QMessageBox messageBox;
        messageBox.critical(0,"Internet Connectivity Error","Please connect to an active internet connection and try again!");
        messageBox.setFixedSize(500,200);
        return 0;
    }
}

void MainWindow::on_bus_number_lineEdit_textChanged()
{
     ui->BUS_NO_label->setText(ui->bus_number_lineEdit->text());
}

void MainWindow::on_server_pushButton_clicked()
{

    if( ui->Server_Condition_label->text() == "SERVER DOWN")
        {

        server = new QTcpServer(this);
      //  connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

        if(!server->listen(QHostAddress::Any , PORT))
           {
                 qDebug() << "Server could not start!";
                 ui->server_pushButton->setText("START SERVER");
                 ui->Server_Condition_label->setText("SERVER DOWN");
        }

        else
           {

            qDebug() << server->serverAddress();
            qDebug() << server->serverPort();
            qDebug() << "Server started!";
            ui->server_pushButton->setText("SHUT DOWN SERVER");
            ui->Server_Condition_label->setText("SERVER RUNNNING ON PORT: ");
            ui->port_label->setNum(server->serverPort());
            connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));


        }
    }

    else if( ui->Server_Condition_label->text() == "SERVER RUNNNING ON PORT: " )
    {
        qDebug() << "SERVER SHUT DOWN";
        server->close();
        ui->server_pushButton->setText("START SERVER");
        ui->Server_Condition_label->setText("SERVER DOWN");
        ui->port_label->setText("");
    }

}


void MainWindow::newConnection()
{
    // grab the socket
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()),this, SLOT(startRead()));
}


void MainWindow::startRead()
{
    connect(socket, SIGNAL(destroyed(QObject*)),this, SLOT(fill_table()));

    socket->waitForBytesWritten(3000);
    socket->waitForReadyRead(3000);
    Bus_stop_name = socket->readAll();
    socket->disconnectFromHost();
    socket->deleteLater();
    socket->close();
}


void MainWindow::fill_table()
{
    //  qDebug() << Bus_stop_name;
    int recurrence = 0;

    if(row_count == 0)
    {
      fill_cells();
    }
    else
    {
        for(int i=0;i<model->rowCount();i++)
        {

        QModelIndex index = model->index(i, 0, QModelIndex());
        if(( model->data(index).toString() == Bus_stop_name) )
            {
            recurrence = 1;
            }
         }

        if(!recurrence)
            {
             fill_cells();
             recurrence = 0;
            }
     }

}

void MainWindow::fill_cells()
{
    QStandardItem *new_cell = new QStandardItem(Bus_stop_name);
    model->setItem(row_count,0,new_cell);

    QTime current_time = QTime::currentTime();
    QString s_time = current_time.toString("HH:mm:ss");

    QStandardItem *time_cell = new QStandardItem(s_time);
    model->setItem(row_count,1,time_cell);

    QStandardItem *accept_cell = new QStandardItem(QString("ACCEPT"));
    model->setItem(row_count,2,accept_cell);

    QStandardItem *clear_cell = new QStandardItem(QString("CLEAR"));
    model->setItem(row_count,3,clear_cell);
    row_count++;
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    if (index.column()==2)
    {
        accept_pressed(index.row());
    }
    else if (index.column()==3)
    {
        clear_pressed(index.row());
    }

}

void MainWindow::accept_pressed(int row)
{
 model->removeRow(row);
 row_count--;
}

void MainWindow::clear_pressed(int row)
{
 model->removeRow(row);
 row_count--;
}

void MainWindow::on_Clear_all_pushButton_clicked()
{
    model->removeRows(0,model->rowCount());
    row_count = 0;
}

void MainWindow::on_Setup_pushButton_clicked()
{
    bool Internet_available = Internet_Conn_Test();
    if(Internet_available)
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
}

void MainWindow::on_Exit_pushButton_clicked()
{
    close();
    qApp->exit();
}

void MainWindow::on_Reset_pushButton_clicked()
{
    ui->BUS_NO_label->setText("");
    ui->bus_number_lineEdit->setText("");

    if( ui->Server_Condition_label->text() == "SERVER RUNNNING ON PORT: " )
    {
        qDebug() << "SERVER SHUT DOWN";
        server->close();
        ui->server_pushButton->setText("START SERVER");
        ui->Server_Condition_label->setText("SERVER DOWN");
        ui->port_label->setText("");
    }

    ui->stackedWidget->setCurrentIndex(0);
}


