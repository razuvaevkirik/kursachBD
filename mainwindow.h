#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <managerform.h>
#include <userform.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void sendData(QString str);

private slots:
    void on_mainRegButton_clicked();

    void on_mainEnterButton_clicked();

    void on_enterGuestButton_clicked();

    void on_enterBackButton_clicked();

    void on_regBackButton_clicked();

    void on_regButton_clicked();

    void on_enterButton_clicked();

private:
    Ui::MainWindow *ui;

    void moveToCenter();

    void connectToDatabase();

    void connections();

    ManagerForm *managerForm;

    UserForm *userForm;

    QSqlDatabase db;

    QString login, password, firstName, lastName, email, passwordFromDB;

    bool isManager;

};

#endif // MAINWINDOW_H
