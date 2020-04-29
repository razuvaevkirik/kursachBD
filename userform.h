#ifndef USERFORM_H
#define USERFORM_H

#include <QWidget>
#include <QDebug>
#include <QDesktopWidget>
#include <QVector>
#include <QString>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QBuffer>
#include <QDate>
#include <QInputDialog>

namespace Ui {
class UserForm;
}

class UserForm : public QWidget
{
    Q_OBJECT

public:
    explicit UserForm(QWidget *parent = nullptr);
    ~UserForm();

public slots:
    void recieveData(QString str);

signals:
    void showFirst();

private slots:
    void on_exitButton_clicked();

    void on_citiesComboBox_currentIndexChanged(int index);

    void on_concertInfoButton_clicked();

    void on_bandBackButton_clicked();

    void on_concertBackButton_clicked();

    void on_buyTicketsButton_clicked();

    void on_cancelTicketsButton_clicked();

    void on_confirmBuyButton_clicked();

    void countPrice();

    void on_onMainButton_clicked();

private:
    Ui::UserForm *ui;

    void getConcertsInfo();

    void getCities();

    void getConcertInfo(int);

    void setConcertInfo(int, int);

    void setTicketsTable(QList<QString>);

    bool eventFilter(QObject *watched, QEvent *event);

    QString login;

    QSqlQueryModel  *concertsModel, *vipTicketsModel;

    QVariantList concertData, bandData, albumsData, musiciansData, hallData;
};

#endif // USERFORM_H
