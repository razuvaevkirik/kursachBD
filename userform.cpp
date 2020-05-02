#include "userform.h"
#include "ui_userform.h"

UserForm::UserForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserForm)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->bandTitleLabel->installEventFilter(this);

    getCities();

    connect(ui->danceSpinBox, SIGNAL(valueChanged(int)), this, SLOT(countPrice()));
}

UserForm::~UserForm()
{
    delete ui;
    delete concertsModel;
    delete vipTicketsModel;
}

void UserForm::recieveData(QString str)
{
    login = str;
}

void UserForm::on_exitButton_clicked()
{
    emit showFirst();
    this->close();
}

void UserForm::getCities()
{
    ui->citiesComboBox->clear();
    ui->citiesComboBox->addItem("Выберите город");

    QStringList citiesList;
    QSqlQuery query;
    query.prepare("SELECT DISTINCT hall_city FROM halls");
    query.exec();
    while (query.next()) {
        citiesList.append(query.value(0).toString());
    }
    ui->citiesComboBox->addItems(citiesList);
}

void UserForm::getConcertsInfo()
{
    concertsModel = new QSqlQueryModel(this);
    concertsModel->setQuery("SELECT * FROM concerts INNER JOIN halls on idhalls = halls_idhalls "
                            "INNER JOIN agency2.bands on idbands = bands_idbands "
                            "WHERE hall_city = '" + ui->citiesComboBox->currentText() + "'"
                            " AND concert_date >= '" + QDate::currentDate().toString("yyyy-MM-dd") + "'");
    concertsModel->setHeaderData(1, Qt::Horizontal, "Название концерта");
    concertsModel->setHeaderData(2, Qt::Horizontal, "Дата");
    concertsModel->setHeaderData(9, Qt::Horizontal, "Концертный зал");
    concertsModel->setHeaderData(17, Qt::Horizontal, "Название группы");

    ui->concertsView->setModel(concertsModel);

    //Скрываем ненужные колонки
    ui->concertsView->setColumnHidden(0, true);
    ui->concertsView->setColumnHidden(3, true);
    ui->concertsView->setColumnHidden(4, true);
    ui->concertsView->setColumnHidden(5, true);
    ui->concertsView->setColumnHidden(6, true);
    ui->concertsView->setColumnHidden(7, true);
    ui->concertsView->setColumnHidden(8, true);
    ui->concertsView->setColumnHidden(10, true);
    ui->concertsView->setColumnHidden(11, true);
    ui->concertsView->setColumnHidden(12, true);
    ui->concertsView->setColumnHidden(13, true);
    ui->concertsView->setColumnHidden(14, true);
    ui->concertsView->setColumnHidden(15, true);
    ui->concertsView->setColumnHidden(16, true);
    ui->concertsView->setColumnHidden(18, true);
    ui->concertsView->setColumnHidden(19, true);
    ui->concertsView->setColumnHidden(20, true);
    ui->concertsView->setColumnHidden(21, true);
    ui->concertsView->setColumnHidden(22, true);

    // Разрешаем выделение строк
    ui->concertsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->concertsView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->concertsView->resizeColumnsToContents();
    ui->concertsView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // Запрещаем редактирование
    ui->concertsView->horizontalHeader()->setStretchLastSection(true);
}

void UserForm::on_citiesComboBox_currentIndexChanged(int index)
{
    getConcertsInfo();
}

void UserForm::on_concertInfoButton_clicked()
{
    QItemSelectionModel *selectModel = ui->concertsView->selectionModel();

    if (selectModel->selectedRows().isEmpty()) {
        QMessageBox::information(this, "Концерт не выбран", "Пожалуйста, выберите концерт!");
    }
    else {
        ui->stackedWidget->setCurrentIndex(1);
        //this->showMaximized();
        getConcertInfo(concertsModel->data(concertsModel->index(selectModel->currentIndex().row(), 0)).toInt());
    }
}

void UserForm::getConcertInfo(int concertID)
{
    concertData.clear();
    bandData.clear();
    albumsData.clear();
    musiciansData.clear();
    hallData.clear();
    QSqlQuery query;
    int freeDance, freeVIP;

    //Получение записи концерта
    query.prepare("SELECT * FROM concerts WHERE idconcerts = " + QString::number(concertID));
    query.exec();
    while(query.next()) {
        concertData.append(query.value(0));
        concertData.append(query.value(1));
        concertData.append(query.value(2));
        concertData.append(query.value(3));
        concertData.append(query.value(4));
        concertData.append(query.value(5));
        concertData.append(query.value(6));
        concertData.append(query.value(7));
    }

    //Получение записи зала
    query.prepare("SELECT * FROM halls WHERE idhalls = " + concertData[7].toString());
    query.exec();
    while(query.next()) {
        hallData.append(query.value(0));
        hallData.append(query.value(1));
        hallData.append(query.value(2));
        hallData.append(query.value(3));
        hallData.append(query.value(4));
        hallData.append(query.value(5));
        hallData.append(query.value(6));
        hallData.append(query.value(7));
    }

    //Получение записи группы
    query.prepare("SELECT * FROM bands WHERE idbands = " + concertData[6].toString());
    query.exec();
    while(query.next()) {
        bandData.append(query.value(0));
        bandData.append(query.value(1));
        bandData.append(query.value(2));
        bandData.append(query.value(3));
        bandData.append(query.value(4));
        bandData.append(query.value(5));
        bandData.append(query.value(6));
    }

    query.prepare("SELECT * FROM albums WHERE bands_idbands = " + bandData[0].toString());
    query.exec();
    while(query.next()) {
        albumsData.append(query.value(0));
        albumsData.append(query.value(1));
        albumsData.append(query.value(2));
        albumsData.append(query.value(3));
        albumsData.append(query.value(4));
        albumsData.append(query.value(5));
        albumsData.append(query.value(6));
        albumsData.append(query.value(7));
    }

    query.prepare("SELECT * FROM musicians WHERE bands_idbands = " + bandData[0].toString());
    query.exec();
    while(query.next()) {
        musiciansData.append(query.value(0));
        musiciansData.append(query.value(1));
        musiciansData.append(query.value(2));
        musiciansData.append(query.value(3));
        musiciansData.append(query.value(4));
    }

    query.prepare("SELECT COUNT(*) FROM tickets WHERE ticket_is_free = 1 "
                  "AND ticket_zone = 'dance' AND concerts_idconcerts = "
                  + QString::number(concertID));
    query.exec();
    query.first();
    freeDance = query.value(0).toInt();

    query.prepare("SELECT COUNT(*) FROM tickets WHERE ticket_is_free = 1 "
                  "AND ticket_zone = 'VIP' AND concerts_idconcerts = "
                  + QString::number(concertID));
    query.exec();
    query.first();
    freeVIP = query.value(0).toInt();

    setConcertInfo(freeDance, freeVIP);
}

void UserForm::setConcertInfo(int freeDance, int freeVIP)
{
    QPixmap outPixmap = QPixmap();
    outPixmap.loadFromData(bandData[6].toByteArray());

    //Отображение информации на странице концерта
    ui->bandPicLabel->setPixmap(outPixmap.scaled(this->width()/2, this->height()/2));
    ui->concertTitleLabel->setText(concertData[1].toString());
    ui->bandTitleLabel->setText(bandData[1].toString());
    ui->dateLabel->setText("Дата: " + QDate::fromString(concertData[2].toString(), "yyyy-MM-dd").toString("dd MMMM yyyy"));
    ui->timeLabel->setText("Время: " + concertData[3].toString());
    ui->ageLabel->setText("Возраст: " + concertData[5].toString() + "+");
    ui->hallTitleLabel->setText("Клуб: " + hallData[1].toString());
    ui->hallAddressLabel->setText("Адрес: " + hallData[3].toString());
    ui->danceLabel->setText(QString::number(freeDance) + " билетов в танц-партер");
    ui->vipLabel->setText(QString::number(freeVIP) + " билетов в VIP");

    ui->albumsText->clear();
    //Отображение информации на странице группы
    ui->bandText->setPlainText( "Название группы: " + bandData[1].toString()
                                + "\r\n\r\nСтрана: " + bandData[2].toString()
                                + "\r\n\r\nЯзык: " + bandData[3].toString()
                                + "\r\n\r\nДата основания: " + QDate::fromString(bandData[5].toString(), "yyyy-MM-dd").toString("dd MMMM yyyy")
                                + "\r\n\r\nИнформация: " + bandData[4].toString());

    for (int i = 0; i < albumsData.size() / 8; i++) {
        ui->albumsText->insertPlainText(QString::number(i + 1) + ". ");
        for (int j = 0; j < 8; j++) {
            switch (j) {
            case 0:
                break;
            case 1:
                ui->albumsText->insertPlainText("Название: " + albumsData[j + (i * 8)].toString() + "\r\n");
                break;
            case 2:
                ui->albumsText->insertPlainText("Жанр: " + albumsData[j + (i * 8)].toString() + "\r\n");
                break;
            case 3:
                ui->albumsText->insertPlainText("Дата релиза: " +
                                                QDate::fromString(albumsData[j + (i * 8)].toString(), "yyyy-MM-dd").toString("dd MMMM yyyy")
                                                + "\r\n");
                break;
            case 4:
                ui->albumsText->insertPlainText("Продолжительность: " + albumsData[j + (i * 8)].toString() + "\r\n");
                break;
            case 5:
                ui->albumsText->insertPlainText("Рейтинг: " + albumsData[j + (i * 8)].toString() + " / 10\r\n");
                break;
            case 6:
                ui->albumsText->insertPlainText("Информация: " + albumsData[j + (i * 8)].toString() + "\r\n");
                break;
            case 7:
                break;
            }
        }
        if (i != (albumsData.size() / 8) - 1) {
            ui->albumsText->insertPlainText("\r\n");
        }
    }

    for (int i = 0; i < musiciansData.size() / 5; i++) {
        ui->musiciansText->insertPlainText(QString::number(i + 1) + ". ");
        for (int j = 0; j < 5; j++) {
            switch (j) {
            case 0:
                break;
            case 1:
                ui->musiciansText->insertPlainText("Имя: " + musiciansData[j + (i * 5)].toString() + "\r\n");
                break;
            case 2:
                ui->musiciansText->insertPlainText("Фамилия: " + musiciansData[j + (i * 5)].toString() + "\r\n");
                break;
            case 3:
                ui->musiciansText->insertPlainText("Инструмент: " + musiciansData[j + (i * 5)].toString() + "\r\n");
                break;
            case 4:
                break;
            }
        }
        if (i != (albumsData.size() / 5) - 1) {
            ui->musiciansText->insertPlainText("\r\n");
        }
    }

    ui->bandText->moveCursor(QTextCursor::Start);
    ui->albumsText->moveCursor(QTextCursor::Start);
    ui->musiciansText->moveCursor(QTextCursor::Start);
}

bool UserForm::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->bandTitleLabel)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            ui->stackedWidget->setCurrentIndex(2);
        }
    }
    return false;
}

void UserForm::on_bandBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void UserForm::on_concertBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void UserForm::on_buyTicketsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->danceSpinBox->setValue(0);
    ui->priceLabel->setText("0");

    //Установка картинки схемы зала
    QPixmap outPixmap = QPixmap();
    outPixmap.loadFromData(hallData[7].toByteArray());
    ui->hallPicLabel->setPixmap(outPixmap.scaled(this->width()/2, this->height()/2));

    vipTicketsModel = new QSqlQueryModel(this);
    vipTicketsModel->setQuery("SELECT * FROM tickets WHERE concerts_idconcerts = "
                              + QString::number(concertData[0].toInt())
                              + " AND ticket_zone = 'VIP' AND ticket_is_free = 1");
    vipTicketsModel->setHeaderData(2, Qt::Horizontal, "Цена");
    vipTicketsModel->setHeaderData(4, Qt::Horizontal, "Место");

    ui->vipTicketsView->setModel(vipTicketsModel);

    //Скрываем ненужные колонки
    ui->vipTicketsView->setColumnHidden(0, true);
    ui->vipTicketsView->setColumnHidden(1, true);
    ui->vipTicketsView->setColumnHidden(3, true);
    ui->vipTicketsView->setColumnHidden(5, true);
    ui->vipTicketsView->setColumnHidden(6, true);
    ui->vipTicketsView->setColumnHidden(7, true);

    // Разрешаем выделение строк
    ui->vipTicketsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->vipTicketsView->setSelectionMode(QAbstractItemView::MultiSelection);
    // Устанавливаем размер колонок по содержимому
    ui->vipTicketsView->resizeColumnsToContents();
    ui->vipTicketsView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // Запрещаем редактирование
    ui->vipTicketsView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->vipTicketsView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(countPrice()));
}

void UserForm::countPrice()
{
    int price;
    QSqlQuery query;

    query.prepare("SELECT ticket_price FROM tickets WHERE concerts_idconcerts = "
                  + QString::number(concertData[0].toInt())
                  + " AND ticket_zone = 'dance' AND ticket_is_free = 1");
    query.exec();
    query.first();

    price = ui->danceSpinBox->value() * query.value(0).toInt();

    QItemSelectionModel *select = ui->vipTicketsView->selectionModel();
    QList<QModelIndex> idxs = select->selectedRows();

    for (int i = 0; i < idxs.size(); i++) {
        price += vipTicketsModel->data(vipTicketsModel->index(idxs.at(i).row(), 2)).toInt();
    }

    ui->priceLabel->setText(QString::number(price));
}

void UserForm::on_cancelTicketsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void UserForm::on_confirmBuyButton_clicked()
{
    QSqlQuery query;
    QList<QString> ticketsIDs;
    int price = ui->priceLabel->text().toInt();
    QVariantList orderData;
    QItemSelectionModel *select = ui->vipTicketsView->selectionModel();
    QList<QModelIndex> idxs = select->selectedRows();

    if (ui->priceLabel->text() != "0") {
        if (login != "") {
            query.prepare("SELECT * FROM users WHERE user_login = '" + login + "'");
            query.exec();
            query.first();

            //Заполнение массива данных о для записи заказа
            orderData.append(query.value(3).toString());
            orderData.append(query.value(4).toString());
            orderData.append(query.value(5).toString());
            orderData.append(QDate::currentDate().toString("yyyy-MM-dd"));
            orderData.append(price);
        }
        else {
            bool bOk;
            orderData.append(QInputDialog::getText( 0,
                                                    "Input",
                                                    "Имя:",
                                                    QLineEdit::Normal,
                                                    "",
                                                    &bOk
                                                   ));
            orderData.append(QInputDialog::getText( 0,
                                                    "Input",
                                                    "Фамилия:",
                                                    QLineEdit::Normal,
                                                    "",
                                                    &bOk
                                                   ));
            orderData.append(QInputDialog::getText( 0,
                                                    "Input",
                                                    "e-mail:",
                                                    QLineEdit::Normal,
                                                    "",
                                                    &bOk
                                                   ));
            orderData.append(QDate::currentDate().toString("yyyy-MM-dd"));
            orderData.append(price);

        }

        query.prepare("INSERT INTO orders ( buyer_first_name, "
                                            "buyer_last_name, "
                                            "buyer_mail, "
                                            "order_date, "
                                            "order_amount )"
                      "VALUES (:First_Name, :Last_Name, :Mail, :Date, :Amount)");

        query.bindValue(":First_Name",          orderData[0]);
        query.bindValue(":Last_Name",           orderData[1]);
        query.bindValue(":Mail",                orderData[2]);
        query.bindValue(":Date",                orderData[3]);
        query.bindValue(":Amount",              orderData[4].toString());

        // После чего выполняется запросом методом exec()
        if(!query.exec()) {
            qDebug() << "error insert into orders";
            qDebug() << query.lastError().text();

        } else {
            //тут айдишники билетов получать по танцу из запроса, по випу из модели
            //ID VIP билетов
            for (int i = 0; i < idxs.size(); i++) {
                ticketsIDs.append(vipTicketsModel->data(vipTicketsModel->index(idxs.at(i).row(), 0)).toString());
            }

            query.prepare("SELECT idtickets FROM tickets WHERE concerts_idconcerts = "
                          + QString::number(concertData[0].toInt())
                          + " AND ticket_zone = 'dance' AND ticket_is_free = 1 LIMIT "
                          + QString::number(ui->danceSpinBox->value()));
            query.exec();
            while(query.next()) {
                ticketsIDs.append(query.value(0).toString());
            }
            qDebug() << ticketsIDs;

            for (int i = 0; i < ticketsIDs.size(); i++) {
                query.prepare("UPDATE tickets SET ticket_is_free = 0 WHERE idtickets = "
                              + ticketsIDs.at(i));
                query.exec();
            }

            ui->stackedWidget->setCurrentIndex(4);
            setTicketsTable(ticketsIDs);
        }
    }
    else {
        QMessageBox::warning(this, "Выберите билеты", "Сумма заказа равна нулю");
    }
}

void UserForm::setTicketsTable(QList<QString> ticketsIDs) {
    QStringList headers = {"Номер билета", "Цена", "Зона", "Место"};
    QSqlQuery query;
    ui->ticketsTableWidget->clearContents();
    ui->ticketsTableWidget->clear();
    ui->ticketsTableWidget->setRowCount(0);

    ui->ticketsTableWidget->setColumnCount(4); // Указываем число колонок
    ui->ticketsTableWidget->setShowGrid(true); // Включаем сетку
    // Разрешаем выделение только одного элемента
    ui->ticketsTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    // Устанавливаем заголовки колонок
    ui->ticketsTableWidget->setHorizontalHeaderLabels(headers);
    // Растягиваем последнюю колонку на всё доступное пространство
    ui->ticketsTableWidget->horizontalHeader()->setStretchLastSection(true);

    for (int i = 0; i < ticketsIDs.size(); i++) {
        query.prepare("SELECT ticket_number, ticket_price, ticket_zone, ticket_seat "
                      "FROM tickets WHERE idtickets = " + ticketsIDs.at(i));
        query.exec();
        query.first();

        ui->ticketsTableWidget->insertRow(i);
        ui->ticketsTableWidget->setItem(i, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->ticketsTableWidget->setItem(i, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->ticketsTableWidget->setItem(i, 2, new QTableWidgetItem(query.value(2).toString()));
        ui->ticketsTableWidget->setItem(i, 3, new QTableWidgetItem(query.value(3).toString()));
    }

    ticketsID.clear();
    ticketsID = ticketsIDs;
    getTicketData();
}

void UserForm::on_onMainButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void UserForm::getTicketData()
{
    QSqlQuery query;
    int concertID, bandID, hallID;
    ticketData.clear();

    for (int i = 0; i < ticketsID.size(); i++) {
        query.prepare("SELECT ticket_number, ticket_zone, ticket_seat, concerts_idconcerts "
                      "FROM tickets "
                      "WHERE idtickets = " + ticketsID.at(i));
        query.exec();
        query.first();

        ticketData.append(query.value(0));
        ticketData.append(query.value(1));
        ticketData.append(query.value(2));

        concertID = query.value(3).toInt();

        query.prepare("SELECT concert_title, concert_date, concert_time, concert_age_limit, "
                      "bands_idbands, halls_idhalls "
                      "FROM concerts "
                      "WHERE idconcerts = " + QString::number(concertID));
        query.exec();
        query.first();

        ticketData.append(query.value(0));
        ticketData.append(query.value(1)); //Преобразование в дату
        ticketData.append(query.value(2));
        ticketData.append(query.value(3));

        bandID = query.value(4).toInt();
        hallID = query.value(5).toInt();

        query.prepare("SELECT band_title "
                      "FROM bands "
                      "WHERE idbands = " + QString::number(bandID));
        query.exec();
        query.first();

        ticketData.append(query.value(0));

        query.prepare("SELECT hall_title, hall_address "
                      "FROM halls "
                      "WHERE idhalls = " + QString::number(hallID));
        query.exec();
        query.first();

        ticketData.append(query.value(0));
        ticketData.append(query.value(1));
    }
}

void UserForm::on_savePdfButton_clicked()
{
    QString html;
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");
    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName.append(".pdf");
    }

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(fileName);

    for (int i = 0; i < ticketData.size() / 10; i++) {
        html.append("<h1>" + ticketData[i * 10 + 3].toString() + "</h1>\n"); //Название концерта
        html.append("<h2>Группа: " + ticketData[i * 10 + 7].toString() + "</h2>\n"); //Группа
        html.append("<h2>Номер билета: " + ticketData[i * 10].toString() + "</h2>"); //Номер билета
        html.append("<h2>Дата: " +
                    QDate::fromString(ticketData[i * 10 + 4].toString(), "yyyy-MM-dd").toString("dd MMMM yyyy")
                    + "</h2>"); //Дата
        html.append("<h2>Время: " + QTime::fromString(ticketData[i * 10 + 5].toString(), "hh:mm:ss").toString("hh:mm")
                    + "</h2>\n"); //Время
        html.append("<h2>Возрастное ограничение: " + ticketData[i * 10 + 6].toString() + " +</h2>\n"); //Возраст
        html.append("<h2>Зона: " + ticketData[i * 10 + 1].toString() + " </h2>"); //Зона
        html.append("<h2>Место: " + ticketData[i * 10 + 2].toString() + "</h2>\n"); //Место
        html.append("<h2>Концертный зал: " + ticketData[i * 10 + 8].toString() + "</h2>\n"); //Зал
        html.append("<h2>Адрес: " + ticketData[i * 10 + 9].toString() + "</h2>\n"); //Адрес
        if (i != ticketData.size() / 10 - 1) {
            html.append("<br style=\"page-break-after: always\">");
        }
    }

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);
}
