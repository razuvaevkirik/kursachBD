#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    this->resize(324, 399);
    this->setWindowTitle("Главная страница");
    moveToCenter();

    connectToDatabase(); //Подключение к бд
    connections();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete managerForm;
    delete userForm;
}

void MainWindow::connections()
{
    managerForm = new ManagerForm;
    userForm = new UserForm;

    connect(managerForm, &ManagerForm::showFirst, this, &MainWindow::show);
    connect(this, SIGNAL(sendData(QString)), managerForm, SLOT(recieveData(QString)));

    connect(userForm, &UserForm::showFirst, this, &MainWindow::show);
    connect(this, SIGNAL(sendData(QString)), userForm, SLOT(recieveData(QString)));
}

void MainWindow::on_mainRegButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    this->resize(548, 250);
    this->setWindowTitle("Регистрация");
    moveToCenter();
    ui->regMailEdit->clear();
    ui->regNameEdit->clear();
    ui->regPassEdit->clear();
    ui->regLoginEdit->clear();
    ui->regSurnameEdit->clear();
    ui->regRepeatPassEdit->clear();
}

void MainWindow::on_mainEnterButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    this->resize(280, 282);
    this->setWindowTitle("Вход");
    moveToCenter();
}

void MainWindow::on_enterGuestButton_clicked()
{
    userForm->show();
    emit sendData("");
    this->hide();
}

void MainWindow::on_enterBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    this->resize(324, 399);
    this->setWindowTitle("Главная страница");
    moveToCenter();
}

void MainWindow::on_regBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    this->resize(324, 399);
    this->setWindowTitle("Главная страница");
    moveToCenter();
}

void MainWindow::moveToCenter()
{
    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(desktop.primaryScreen()); // прямоугольник с размерами экрана
    QPoint center = rect.center(); //координаты центра экрана
    center.setX(center.x() - (this->width()/2));  // учитываем половину ширины окна
    center.setY(center.y() - (this->height()/2));  // .. половину высоты
    move(center);
}

void MainWindow::connectToDatabase() //Подключение к бд
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setDatabaseName("agency3");
    db.setUserName("root");
    db.setPassword("root");

    if (!db.open()) //Статус подключения к бд
    {
        ui->statusBar->setStyleSheet("color: red;");
        ui->statusBar->showMessage("Ошибка соединения");
        qDebug() << QSqlError(db.lastError()).text();
        ui->mainRegButton->setDisabled(true);
        ui->mainEnterButton->setDisabled(true);
        ui->enterGuestButton->setDisabled(true);
    }
    else {
        ui->statusBar->setStyleSheet("color: green;");
        ui->statusBar->showMessage("Соединение установлено");
    }
}

void MainWindow::on_regButton_clicked()
{
    QSqlQuery query;
    int countExistingUser;
    login = ui->regLoginEdit->text();
    password = ui->regPassEdit->text();
    firstName = ui->regNameEdit->text();
    lastName = ui->regSurnameEdit->text();
    email = ui->regMailEdit->text();

    if (ui->regMailEdit->text() != "" && ui->regNameEdit->text() != "" && ui->regPassEdit->text() != "" &&
            ui->regLoginEdit->text() != "" && ui->regSurnameEdit->text() != "" &&
            ui->regRepeatPassEdit->text() != "") {
        if (ui->regPassEdit->text() == ui->regRepeatPassEdit->text()) {
            //Занесение записи в таблицу юзеров
            //Перенаправление на страницу входа
            query.prepare("SELECT COUNT(*) FROM users WHERE user_login = '" + login + "'");
            if (!query.exec()) {
                QMessageBox::warning(this, "asd", QSqlError(query.lastError()).text());
            }

            while (query.next()) {
                countExistingUser = query.value(0).toInt();
            }

            if (countExistingUser == 0) {
                if (!query.exec("INSERT INTO users (user_login, user_password, user_is_manager, "
                                "user_first_name, user_last_name, user_mail) "
                                "VALUES ('" + login + "', '" + password + "', '0', '" + firstName + "',"
                                "'" + lastName + "', '" + email + "')")) {
                    QMessageBox::warning(this, "asd", QSqlError(query.lastError()).text());
                }
                else {
                    QMessageBox::information(this, "Регистрация успешна", "Аккаунт успешно создан, перенаправление на "
                                                                          "страницу авторизации");
                    on_mainEnterButton_clicked();
                }
            }
            else {
                QMessageBox::warning(this, "Внимание","Пользователь с таким логином уже существует!");
                countExistingUser = 0;
            }
        }
        else {
            QMessageBox::warning(this, "Внимание", "Пароли не совпадают!");
        }
    }
    else {
        QMessageBox::warning(this, "Внимание", "Заполните все поля!");
    }
}

void MainWindow::on_enterButton_clicked()
{
    QSqlQuery query;
    int countExistingUser;
    login = ui->enterLoginEdit->text();
    password = ui->enterPassEdit->text();

    if (login == "admin") {
        query.exec("SELECT user_password FROM users WHERE user_login = 'admin'");
        while (query.next()) {
            if (password == query.value(0).toString()) {
                ui->enterLoginEdit->clear();
                ui->enterPassEdit->clear();
                //adminForm->show();
                QMessageBox::information(this, "Вход", "Выполнен вход админа");
                //this->hide();
            }
            else {
                QMessageBox::warning(this, "Внимание","Введен неверный пароль!");
            }
        }
    }
    else {
        query.exec("SELECT COUNT(*) FROM users WHERE user_login = '" + login + "'");
        while (query.next()) {
            countExistingUser = query.value(0).toInt();
        }

        if (countExistingUser == 1) {
            query.exec("SELECT user_password FROM users WHERE user_login = '" + login + "'");
            while (query.next()) {
                passwordFromDB = query.value(0).toString();
            }

            query.exec("SELECT user_is_manager FROM users WHERE user_login = '" + login + "'");
            while (query.next()) {
                isManager = query.value(0).toBool();
            }

            if (password == passwordFromDB && isManager == 1) {
                ui->enterLoginEdit->clear();
                ui->enterPassEdit->clear();
                managerForm->show();
                emit sendData(login);
                this->hide();
            }
            else if (password == passwordFromDB && isManager == 0) {
                ui->enterLoginEdit->clear();
                ui->enterPassEdit->clear();
                userForm->show();
                emit sendData(login);
                this->hide();
            }
            else {
                QMessageBox::warning(this, "Внимание","Введен неверный пароль!");
            }
        }
        else {
            QMessageBox::warning(this, "Внимание","Такого пользователя не существует!");
        }
    }
}
