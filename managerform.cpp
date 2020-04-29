#include "managerform.h"
#include "ui_managerform.h"

ManagerForm::ManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManagerForm)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    //this->resize(324, 399);
    this->setWindowTitle("Главная страница менеджера");
    moveToCenter();

    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));
}

ManagerForm::~ManagerForm()
{
    delete ui;
    delete model;
    delete bandInfoModel;
    delete bandAlbumsModel;
    delete bandConcertsModel;
    delete bandMusiciansModel;
    delete cityListModel;
    delete hallListModel;
    delete hallInfoModel;
    delete concertsModel;
    delete groupConcertsModel;
    delete orderInfoModel;
}

void ManagerForm::recieveData(QString str)
{
    login = str;
}

void ManagerForm::moveToCenter()
{
    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(desktop.primaryScreen()); // прямоугольник с размерами экрана
    QPoint center = rect.center(); //координаты центра экрана
    center.setX(center.x() - (this->width()/2));  // учитываем половину ширины окна
    center.setY(center.y() - (this->height()/2));  // .. половину высоты
    move(center);
}

void ManagerForm::on_bandsButton_clicked()
{
    url = "";
    ui->stackedWidget->setCurrentIndex(1);
    ui->tabWidget->setCurrentIndex(0);

    model = new QSqlTableModel(this);
    model->setTable("bands");
    model->setHeaderData(1, Qt::Horizontal, "Название группы");

    ui->bandsView->setModel(model);

    ui->bandsView->setColumnHidden(0, true);    // Скрываем колонку с id записей
    ui->bandsView->setColumnHidden(2, true);    // Скрываем колонку с страной
    ui->bandsView->setColumnHidden(3, true);    // Скрываем колонку с языком
    ui->bandsView->setColumnHidden(4, true);    // Скрываем колонку с информацией
    ui->bandsView->setColumnHidden(5, true);    // Скрываем колонку с годом
    ui->bandsView->setColumnHidden(6, true);    // Скрываем колонку с лого

    // Разрешаем выделение строк
    ui->bandsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->bandsView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->bandsView->resizeColumnsToContents();
    ui->bandsView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // Запрещаем редактирование
    ui->bandsView->horizontalHeader()->setStretchLastSection(true);

    model->select();

    connect(ui->bandsView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(getBandInfo(QModelIndex)));

    this->showMaximized();
    swapCheck = false;
}

void ManagerForm::on_bandsBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void ManagerForm::on_addBandButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->bandTitleEdit->clear();
    ui->bandCountryEdit->clear();
    ui->bandLanguageEdit->clear();
    ui->bandInfoTextEdit->clear();
    ui->picLabel->clear();
    this->showMaximized();
}

void ManagerForm::on_cancelAddBandButton_clicked()
{
    on_bandsButton_clicked();
}

void ManagerForm::on_confirmAddBandButton_clicked()
{
    if (url != "") {
        QPixmap inPixmap(url);
        QByteArray inByteArray;                             // Создаём объект QByteArray для сохранения изображения
        QBuffer inBuffer(&inByteArray);                   // Сохранение изображения производим через буффер
        inBuffer.open(QIODevice::WriteOnly);              // Открываем буффер
        inPixmap.save(&inBuffer, "JPG");
        QVariantList data;
        data.append(ui->bandTitleEdit->text());
        data.append(ui->bandCountryEdit->text());
        data.append(ui->bandLanguageEdit->text());
        data.append(ui->bandInfoTextEdit->toPlainText());
        data.append(ui->calendarWidget->selectedDate().toString("yyyy-MM-dd"));
        data.append(inByteArray);

        QSqlQuery query;
        /* В начале SQL запрос формируется с ключами,
         * которые потом связываются методом bindValue
         * для подстановки данных из QVariantList
         * */
        query.prepare("INSERT INTO bands (  band_title, "
                                            "band_country, "
                                            "band_language, "
                                            "band_info, "
                                            "band_found_year, "
                                            "band_logo )"
                      "VALUES (:Title, :Country, :Lang, :Info, :Year, :Logo)");
        query.bindValue(":Title",        data[0].toString());
        query.bindValue(":Country",      data[1].toString());
        query.bindValue(":Lang",         data[2].toString());
        query.bindValue(":Info",         data[3].toString());
        query.bindValue(":Year",         data[4].toString());
        query.bindValue(":Logo",         data[5].toByteArray());
        qDebug() << data[5];

        // После чего выполняется запросом методом exec()
        if(!query.exec()){
            qDebug() << "error insert into bands";
            qDebug() << query.lastError().text();

        } else {
            QMessageBox::warning(this, "Шик", "Группа успешно добавлена!");
        }

        on_bandsButton_clicked();
    }
    else {
        QMessageBox::warning(this, "Error!", "Заполните все поля и выберите картинку!");
    }
}

void ManagerForm::on_bandLogoButton_clicked()
{
    url = QFileDialog::getOpenFileName(this, tr("Open File"),
                                               "/home",
                                               tr("Images (*.png *.xpm *.jpg)"));
    QPixmap inPixmap(url);
    ui->picLabel->setPixmap(inPixmap.scaled(200, 200));
}

void ManagerForm::getBandInfo(QModelIndex index)
{
    bandID = model->data(model->index(index.row(), 0)).toInt();
    QPixmap outPixmap = QPixmap(); // Создаём QPixmap, который будет помещаться в picLabel
        /* Забираем данные об изображении из таблицы в качестве QByteArray
         * и помещаем их в QPixmap
         * */
    outPixmap.loadFromData(model->data(model->index(index.row(), 6)).toByteArray());
    // Устанавливаем изображение в picLabel
    ui->bandLogoPic->setPixmap(outPixmap.scaled(this->width()/2, this->height()/2));

    //Модель для view с информацией о группе
    bandInfoModel = new QSqlTableModel(this);
    bandInfoModel->setTable("bands");
    bandInfoModel->setFilter("idbands = " + model->data(model->index(index.row(), 0)).toString());
    bandInfoModel->setHeaderData(1, Qt::Horizontal, "Название группы");
    bandInfoModel->setHeaderData(2, Qt::Horizontal, "Страна");
    bandInfoModel->setHeaderData(3, Qt::Horizontal, "Язык");
    bandInfoModel->setHeaderData(4, Qt::Horizontal, "Информация");
    bandInfoModel->setHeaderData(5, Qt::Horizontal, "Дата основания");

    ui->bandInfoView->setModel(bandInfoModel);

    ui->bandInfoView->setColumnHidden(0, true);    // Скрываем колонку с id записей
    ui->bandInfoView->setColumnHidden(6, true);    // Скрываем колонку с изображением

    // Разрешаем выделение строк
    ui->bandInfoView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->bandInfoView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->bandInfoView->resizeColumnsToContents();

    QHeaderView *rowHeader = ui->bandInfoView->verticalHeader();
    rowHeader->setMinimumSectionSize(1);
    rowHeader->setDefaultSectionSize(rowHeader->fontMetrics().height() * 10);

    ui->bandInfoView->horizontalHeader()->setStretchLastSection(true);
    if (swapCheck == false) {
        ui->bandInfoView->horizontalHeader()->swapSections(4, 5);
        swapCheck = true;
    }

    bandInfoModel->select();

    getAlbumsInfo(index);
    getMusiciansInfo(index);
    getConcertsInfo(index);
}

void ManagerForm::getAlbumsInfo(QModelIndex index)
{
    bandAlbumsModel = new QSqlTableModel(this);
    bandAlbumsModel->setTable("albums");
    bandAlbumsModel->setFilter("bands_idbands = " + model->data(model->index(index.row(), 0)).toString());
    bandAlbumsModel->setHeaderData(1, Qt::Horizontal, "Название альбома");
    bandAlbumsModel->setHeaderData(2, Qt::Horizontal, "Жанр");
    bandAlbumsModel->setHeaderData(3, Qt::Horizontal, "Дата выпуска");
    bandAlbumsModel->setHeaderData(4, Qt::Horizontal, "Продолжительность");
    bandAlbumsModel->setHeaderData(5, Qt::Horizontal, "Рейтинг");
    bandAlbumsModel->setHeaderData(6, Qt::Horizontal, "Информация");

    ui->albumsView->setModel(bandAlbumsModel);

    ui->albumsView->setColumnHidden(0, true);    // Скрываем колонку с id записей
    ui->albumsView->setColumnHidden(7, true);    // Скрываем колонку с id группы

    // Разрешаем выделение строк
    ui->albumsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->albumsView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->albumsView->resizeColumnsToContents();

    QHeaderView *rowHeader = ui->albumsView->verticalHeader();
    rowHeader->setMinimumSectionSize(1);
    rowHeader->setDefaultSectionSize(rowHeader->fontMetrics().height() * 10);

    ui->albumsView->horizontalHeader()->setStretchLastSection(true);

    bandAlbumsModel->select();
}

void ManagerForm::on_addAlbumButton_clicked()
{
    int lastRow = bandAlbumsModel->rowCount();
    bandAlbumsModel->insertRow(lastRow);
    ui->albumsView->selectRow(lastRow);
    ui->albumsView->setFocus();
    bandAlbumsModel->setData(bandAlbumsModel->index(lastRow, 7), bandID);
    bandAlbumsModel->submitAll();
}

void ManagerForm::getMusiciansInfo(QModelIndex index)
{
    bandMusiciansModel = new QSqlTableModel(this);
    bandMusiciansModel->setTable("musicians");
    bandMusiciansModel->setFilter("bands_idbands = " + model->data(model->index(index.row(), 0)).toString());
    bandMusiciansModel->setHeaderData(1, Qt::Horizontal, "Имя");
    bandMusiciansModel->setHeaderData(2, Qt::Horizontal, "Фамилия");
    bandMusiciansModel->setHeaderData(3, Qt::Horizontal, "Инструмент");

    ui->musiciansView->setModel(bandMusiciansModel);

    ui->musiciansView->setColumnHidden(0, true);    // Скрываем колонку с id записей
    ui->musiciansView->setColumnHidden(4, true);    // Скрываем колонку с id группы

    // Разрешаем выделение строк
    ui->musiciansView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->musiciansView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->musiciansView->resizeColumnsToContents();

    ui->musiciansView->horizontalHeader()->setStretchLastSection(true);

    bandMusiciansModel->select();
}

void ManagerForm::on_addMusicianButton_clicked()
{
    int lastRow = bandMusiciansModel->rowCount();
    bandMusiciansModel->insertRow(lastRow);
    ui->musiciansView->selectRow(lastRow);
    ui->musiciansView->setFocus();
    bandMusiciansModel->setData(bandMusiciansModel->index(lastRow, 4), bandID);
    bandMusiciansModel->submitAll();
}


void ManagerForm::getConcertsInfo(QModelIndex index)
{
    groupConcertsModel = new QSqlQueryModel(this);
    groupConcertsModel->setQuery("SELECT concert_title, concert_date, concert_time, concert_duration "
                                "FROM concerts WHERE bands_idbands = "
                                + model->data(model->index(index.row(), 0)).toString());
    groupConcertsModel->setHeaderData(0, Qt::Horizontal, "Название");
    groupConcertsModel->setHeaderData(1, Qt::Horizontal, "Дата");
    groupConcertsModel->setHeaderData(2, Qt::Horizontal, "Время");
    groupConcertsModel->setHeaderData(3, Qt::Horizontal, "Продолжительность");

    ui->concertsView->setModel(groupConcertsModel);

    // Разрешаем выделение строк
    ui->concertsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->concertsView->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->concertsView->horizontalHeader()->setStretchLastSection(true);
}

void ManagerForm::on_hallsBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void ManagerForm::on_addHallButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    ui->hallNameEdit->clear();
    ui->hallPicLabel->clear();
    ui->hallMetroEdit->clear();
    ui->hallSitSpinBox->setValue(0);
    ui->hallAddressEdit->clear();
    ui->hallDanceSpinBox->setValue(0);
    ui->hallCityEdit->clear();
    this->showMaximized();
}

void ManagerForm::on_hallsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    this->showMaximized();
    url = "";

    cityListModel = new QSqlQueryModel(this);
    cityListModel->setQuery("SELECT DISTINCT hall_city FROM halls");
    cityListModel->setHeaderData(0, Qt::Horizontal, "Город");

    ui->cityTableView->setModel(cityListModel);

    // Разрешаем выделение строк
    ui->cityTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->cityTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->cityTableView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->cityTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(getHallList(QModelIndex)));
}

void ManagerForm::getHallList(QModelIndex index)
{
    QString cityName = cityListModel->data(cityListModel->index(index.row(), 0)).toString();

    hallListModel = new QSqlQueryModel(this);
    hallListModel->setQuery("SELECT hall_title FROM halls WHERE hall_city = '" + cityName + "'");
    hallListModel->setHeaderData(0, Qt::Horizontal, "Название площадки");

    ui->hallNameTableView->setModel(hallListModel);

    // Разрешаем выделение строк
    ui->hallNameTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->hallNameTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    //ui->hallNameTableView->resizeColumnsToContents();

    ui->hallNameTableView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->hallNameTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(getHallInfo(QModelIndex)));
}

void ManagerForm::getHallInfo(QModelIndex index)
{
    hallInfoModel = new QSqlTableModel(this);
    hallInfoModel->setTable("halls");
    hallInfoModel->setFilter("hall_title = '" +
                             hallListModel->data(hallListModel->index(index.row(), 0)).toString() + "'");
    hallInfoModel->setHeaderData(3, Qt::Horizontal, "Адрес");
    hallInfoModel->setHeaderData(4, Qt::Horizontal, "Метро");
    hallInfoModel->setHeaderData(5, Qt::Horizontal, "Посадочные места");
    hallInfoModel->setHeaderData(6, Qt::Horizontal, "Места танц-партера");

    ui->hallInfoTableView->setModel(hallInfoModel);

    ui->hallInfoTableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
    ui->hallInfoTableView->setColumnHidden(1, true);    // Скрываем колонку с назваением
    ui->hallInfoTableView->setColumnHidden(2, true);    // Скрываем колонку с городом
    ui->hallInfoTableView->setColumnHidden(7, true);    // Скрываем колонку с изображением схемы

    // Разрешаем выделение строк
    ui->hallInfoTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->hallInfoTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->hallInfoTableView->resizeColumnsToContents();

    ui->hallInfoTableView->horizontalHeader()->setStretchLastSection(true);

    QHeaderView *rowHeader = ui->hallInfoTableView->verticalHeader();
    rowHeader->setMinimumSectionSize(1);
    rowHeader->setDefaultSectionSize(rowHeader->fontMetrics().height() * 10);

    hallInfoModel->select();

    QPixmap outPixmap = QPixmap(); // Создаём QPixmap, который будет помещаться в picLabel
        /* Забираем данные об изображении из таблицы в качестве QByteArray
         * и помещаем их в QPixmap
         * */
    outPixmap.loadFromData(hallInfoModel->data(hallInfoModel->index(0, 7)).toByteArray());
    // Устанавливаем изображение в picLabel
    ui->hallSchemePic->setPixmap(outPixmap.scaled(this->width()/2, this->height()/2));
}

void ManagerForm::on_hallAddCancelButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    this->showMaximized();
}

void ManagerForm::on_hallAddImageButton_clicked()
{
    url = QFileDialog::getOpenFileName(this, tr("Open File"),
                                               "/home",
                                               tr("Images (*.png *.xpm *.jpg)"));
    QPixmap inPixmap(url);
    ui->hallPicLabel->setPixmap(inPixmap.scaled(200, 200));
}

void ManagerForm::on_confirmHall_clicked()
{
    if (url != "") {
        QPixmap inPixmap(url);
        QByteArray inByteArray;                             // Создаём объект QByteArray для сохранения изображения
        QBuffer inBuffer(&inByteArray);                   // Сохранение изображения производим через буффер
        inBuffer.open(QIODevice::WriteOnly);              // Открываем буффер
        inPixmap.save(&inBuffer, "JPG");
        QVariantList data;
        data.append(ui->hallNameEdit->text());
        data.append(ui->hallCityEdit->text());
        data.append(ui->hallAddressEdit->text());
        data.append(ui->hallMetroEdit->text());
        data.append(ui->hallSitSpinBox->text());
        data.append(ui->hallDanceSpinBox->text());
        data.append(inByteArray);

        QSqlQuery query;
        /* В начале SQL запрос формируется с ключами,
         * которые потом связываются методом bindValue
         * для подстановки данных из QVariantList
         * */
        query.prepare("INSERT INTO halls (  hall_title, "
                                            "hall_city, "
                                            "hall_address, "
                                            "hall_metro, "
                                            "hall_sitting_places, "
                                            "hall_dance_places, "
                                            "hall_scheme )"
                      "VALUES (:Title, :City, :Address, :Metro, :Sit, :Dance, :Scheme)");
        query.bindValue(":Title",           data[0].toString());
        query.bindValue(":City",            data[1].toString());
        query.bindValue(":Address",         data[2].toString());
        query.bindValue(":Metro",           data[3].toString());
        query.bindValue(":Sit",             data[4].toInt());
        query.bindValue(":Dance",           data[5].toInt());
        query.bindValue(":Scheme",          data[6].toByteArray());

        // После чего выполняется запросом методом exec()
        if (!query.exec()) {
            qDebug() << "error insert into halls";
            qDebug() << query.lastError().text();
        }
        else {
            QMessageBox::warning(this, "Шик", "Зал успешно добавлен!");
        }

        on_hallsButton_clicked();
    }
    else {
        QMessageBox::warning(this, "Error!", "Заполните все поля и выберите картинку!");
    }
}

void ManagerForm::on_concertsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
    this->showMaximized();

    ui->concertsBandLabel->clear();
    ui->concertsHallLabel->clear();

    concertsModel = new QSqlTableModel(this);
    concertsModel->setTable("concerts");

    concertsModel->setHeaderData(1, Qt::Horizontal, "Название концерта");
    concertsModel->setHeaderData(2, Qt::Horizontal, "Дата");
    concertsModel->setHeaderData(3, Qt::Horizontal, "Время");
    concertsModel->setHeaderData(4, Qt::Horizontal, "Длительность");
    concertsModel->setHeaderData(5, Qt::Horizontal, "Возрастное ограничение");

    ui->concertsTableView->setModel(concertsModel);

    ui->concertsTableView->setColumnHidden(0, true);    // Скрываем колонку с id записей
    ui->concertsTableView->setColumnHidden(6, true);    // Скрываем колонку с id группы
    ui->concertsTableView->setColumnHidden(7, true);    // Скрываем колонку с id зала

    // Разрешаем выделение строк
    ui->concertsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->concertsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->concertsTableView->resizeColumnsToContents();

    ui->concertsTableView->horizontalHeader()->setStretchLastSection(true);

    QHeaderView *rowHeader = ui->concertsTableView->verticalHeader();
    rowHeader->setMinimumSectionSize(1);
    rowHeader->setDefaultSectionSize(rowHeader->fontMetrics().height() * 10);

    concertsModel->select();

    connect(ui->concertsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(getConcertDetails(QModelIndex)));
}

void ManagerForm::getConcertDetails(QModelIndex index)
{
    QSqlQuery query;
    query.prepare("SELECT band_title FROM bands WHERE idbands = "
                  + concertsModel->data(concertsModel->index(index.row(), 6)).toString());
    query.exec();
    query.first();
    ui->concertsBandLabel->setText(query.value(0).toString());

    query.prepare("SELECT hall_title FROM halls WHERE idhalls = "
                  + concertsModel->data(concertsModel->index(index.row(), 7)).toString());
    query.exec();
    query.first();
    ui->concertsHallLabel->setText(query.value(0).toString());
}

void ManagerForm::on_concertsBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void ManagerForm::on_addConcertButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    this->showNormal();
    this->resize(200, 200);
    moveToCenter();

    ui->bandsComboBox->clear();
    ui->hallsComboBox->clear();
    ui->bandsComboBox->addItem("Выберите группу");
    ui->hallsComboBox->addItem("Выберите площадку");
    ui->concertTitleLineEdit->clear();
    ui->concertDateEdit->setDate(QDate::currentDate());
    QTime time(0, 0);
    ui->concertTimeEdit->setTime(time);
    ui->concertDurationEdit->setTime(time);
    ui->concertAgeEdit->setValue(0);
    ui->concertTitleLineEdit->setFocus();

    QSqlQuery query;
    query.prepare("SELECT band_title FROM bands");
    query.exec();
    while (query.next()) {
        ui->bandsComboBox->addItem(query.value(0).toString());
    }

    query.prepare("SELECT hall_title FROM halls");
    query.exec();
    while (query.next()) {
        ui->hallsComboBox->addItem(query.value(0).toString());
    }
}

void ManagerForm::on_addConcertCancelButton_clicked()
{
    on_concertsButton_clicked();
}

void ManagerForm::on_confirmAddConcertButton_clicked()
{
    if (ui->bandsComboBox->currentText() != "Выберите группу"
            && ui->hallsComboBox->currentText() != "Выберите площадку"
            && ui->concertTitleLineEdit->text() != ""
            && ui->concertDateEdit->date() >= QDate::currentDate()) {
        QVariantList data;
        data.append(ui->concertTitleLineEdit->text());
        data.append(ui->concertDateEdit->date());
        data.append(ui->concertTimeEdit->time());
        data.append(ui->concertDurationEdit->time());
        data.append(ui->concertAgeEdit->value());

        QSqlQuery query;
        query.prepare("SELECT idbands FROM bands WHERE band_title = '"
                      + ui->bandsComboBox->currentText() + "'");
        query.exec();
        while (query.next()) {
            data.append(query.value(0));
        }

        query.prepare("SELECT idhalls FROM halls WHERE hall_title = '"
                      + ui->hallsComboBox->currentText() + "'");
        query.exec();
        while (query.next()) {
            data.append(query.value(0));
        }

        query.prepare("INSERT INTO concerts (  concert_title, "
                                            "concert_date, "
                                            "concert_time, "
                                            "concert_duration, "
                                            "concert_age_limit, "
                                            "bands_idbands, "
                                            "halls_idhalls )"
                      "VALUES (:Title, :Date, :Time, :Dur, :Age, :BandID, :HallID)");
        query.bindValue(":Title",           data[0].toString());
        query.bindValue(":Date",            data[1].toString());
        query.bindValue(":Time",            data[2].toString());
        query.bindValue(":Dur",             data[3].toString());
        query.bindValue(":Age",             data[4].toInt());
        query.bindValue(":BandID",          data[5].toInt());
        query.bindValue(":HallID",          data[6].toInt());

        // После чего выполняется запросом методом exec()
        if (!query.exec()) {
            qDebug() << "error insert into halls";
            qDebug() << query.lastError().text();
        }
        else {
            QMessageBox::warning(this, "Шик", "Концерт успешно добавлен!");
            createTickets(ui->vipCostEdit->text().toInt(), ui->danceCostEdit->text().toInt());
            on_concertsButton_clicked();
        }
    }
    else {
        QMessageBox::warning(this, "Error!", "Корректно заполните все поля и "
                                             "повторите попытку!");
    }
}


void ManagerForm::createTickets(int sitCost, int danceCost)
{
    QSqlQuery query;
    QString hallID, concertID, ticketNumber;
    int sitPlaces, dancePlaces;

    query.prepare("SELECT halls_idhalls, idconcerts FROM concerts WHERE idconcerts = LAST_INSERT_ID()");
    query.exec();
    query.first();
    hallID = query.value(0).toString();
    concertID = query.value(1).toString();
    query.prepare("SELECT hall_sitting_places, hall_dance_places FROM halls WHERE idhalls = " +
                  hallID);
    query.exec();
    query.first();
    sitPlaces = query.value(0).toInt();
    dancePlaces = query.value(1).toInt();


    //Генерация билетов и запись их в таблицу tickets
    //поменять в модели тип данных номеров билетов с инт на varchar
    for (int i = 0; i < sitPlaces; i++) {
        ticketNumber = ticketRand();
        //Запрос
        query.prepare("INSERT INTO tickets (ticket_number, ticket_price, ticket_zone, "
                      "ticket_seat, ticket_is_free, concerts_idconcerts) VALUES ("
                      "'" + ticketNumber + "', '" + QString::number(sitCost) + "', 'VIP', "
                      + QString::number(i + 1) + ", 1, "
                      + concertID + ")");
        if (!query.exec()) {
            qDebug() << query.lastError();
        }
    }

    for (int i = 0; i < dancePlaces; i++) {
        ticketNumber = ticketRand();
        query.prepare("INSERT INTO tickets (ticket_number, ticket_price, ticket_zone, "
                      "ticket_is_free, concerts_idconcerts) VALUES ("
                      "'" + ticketNumber + "', '" + QString::number(danceCost) + "', 'dance', "
                      "1, " + concertID + ")");
        if (!query.exec()) {
            qDebug() << query.lastError();
        }
    }
}

QString ManagerForm::ticketRand()
{
    QString tNumber = "";
    int number;

    for (int i = 0; i < 10; i++) {
        number = qrand() % 9;
        tNumber.append(QString::number(number));
    }

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM tickets WHERE ticket_number = '" + tNumber + "'");
    query.exec();
    query.first();
    if (query.value(0) > 0) {
        tNumber = "";
        tNumber = ticketRand();
    }

    return tNumber;
}

void ManagerForm::on_exitButton_clicked()
{
    emit showFirst();
    this->close();
}

void ManagerForm::on_ordersBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void ManagerForm::on_ordersButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);

    orderInfoModel = new QSqlTableModel(this);
    orderInfoModel->setTable("orders");

    orderInfoModel->setHeaderData(1, Qt::Horizontal, "Имя");
    orderInfoModel->setHeaderData(2, Qt::Horizontal, "Фамилия");
    orderInfoModel->setHeaderData(3, Qt::Horizontal, "e-mail");
    orderInfoModel->setHeaderData(4, Qt::Horizontal, "Дата заказа");
    orderInfoModel->setHeaderData(5, Qt::Horizontal, "Сумма заказа");

    ui->ordersTableView->setModel(orderInfoModel);

    ui->ordersTableView->setColumnHidden(0, true);    //Скрываем колонку с id записей

    // Разрешаем выделение строк
    ui->ordersTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одной строки в таблице
    ui->ordersTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->ordersTableView->resizeColumnsToContents();

    ui->ordersTableView->horizontalHeader()->setStretchLastSection(true);

    orderInfoModel->select();
}
