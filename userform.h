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
#include <QTextDocument>
#include <QFileDialog>
#include <QPrinter>

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

    void on_savePdfButton_clicked();

    void setBandInfo(QModelIndex);

private:
    Ui::UserForm *ui;

    void getConcertsInfo();

    void getCities();

    void getConcertInfo(int);

    void setConcertInfo(int, int);

    void setTicketsTable(QList<QString>);

    void getTicketData();

    QString login;

    QList<QString> ticketsID, bandsTitles;

    QSqlQueryModel  *concertsModel, *vipTicketsModel, *bandsListModel;

    QVariantList    concertData, bandData, albumsData,
                    musiciansData, hallData, ticketData;
};

#endif // USERFORM_H
