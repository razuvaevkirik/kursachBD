#ifndef MANAGERFORM_H
#define MANAGERFORM_H

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
#include <QFileDialog>
#include <QBuffer>

namespace Ui {
class ManagerForm;
}

class ManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ManagerForm(QWidget *parent = nullptr);
    ~ManagerForm();

public slots:
    void recieveData(QString str);

signals:
    void showFirst();

private slots:
    void on_bandsButton_clicked();

    void on_bandsBackButton_clicked();

    void on_addBandButton_clicked();

    void on_cancelAddBandButton_clicked();

    void on_confirmAddBandButton_clicked();

    void on_bandLogoButton_clicked();

    void getBandInfo(QModelIndex);

    void getHallList(QModelIndex);

    void getHallInfo(QModelIndex);

    void getConcertDetails(QModelIndex);

    void on_addAlbumButton_clicked();

    void on_addMusicianButton_clicked();

    void on_hallsBackButton_clicked();

    void on_addHallButton_clicked();

    void on_hallsButton_clicked();

    void on_hallAddCancelButton_clicked();

    void on_hallAddImageButton_clicked();

    void on_confirmHall_clicked();

    void on_concertsButton_clicked();

    void on_concertsBackButton_clicked();

    void on_addConcertButton_clicked();

    void on_addConcertCancelButton_clicked();

    void on_confirmAddConcertButton_clicked();

    void on_exitButton_clicked();

    void on_ordersBackButton_clicked();

    void on_ordersButton_clicked();

    void on_chooseConcertLogoButton_clicked();

private:
    Ui::ManagerForm *ui;

    void moveToCenter();

    void getAlbumsInfo(QModelIndex index);

    void getMusiciansInfo(QModelIndex index);

    void getConcertsInfo(QModelIndex index);

    void createTickets(int, int, QString);

    QString ticketRand();

    bool swapCheck;

    QString login, url;

    int bandID;

    QSqlTableModel  *bandInfoModel, *model, *bandAlbumsModel, *bandMusiciansModel, *bandConcertsModel,
                    *hallInfoModel, *concertsModel, *orderInfoModel, *bandsListModel;

    QSqlQueryModel  *hallListModel, *cityListModel, *groupConcertsModel, *bandsConcertListModel;
};

#endif // MANAGERFORM_H
