#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTcpServer>
#include <QTcpSocket>
#include <QStandardItemModel>
#include <QSignalMapper>
#include <QNetworkInterface>
#include <QTime>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    bool Internet_Conn_Test();

    void on_server_pushButton_clicked();

    void newConnection();

    void fill_table();

    void startRead();

    void accept_pressed(int row);

    void clear_pressed(int row);

    void fill_cells();

    void on_tableView_clicked(const QModelIndex &index);

    void on_bus_number_lineEdit_textChanged();

    void on_Clear_all_pushButton_clicked();

    void on_Setup_pushButton_clicked();

    void on_Exit_pushButton_clicked();

    void on_Reset_pushButton_clicked();


private:

    Ui::MainWindow *ui;

    QTcpServer *server;
    QTcpSocket *socket;
    QStandardItemModel *model;

    QString Bus_stop_name;

};

#endif // MAINWINDOW_H
