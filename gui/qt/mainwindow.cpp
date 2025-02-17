#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../../src/shapes.hpp"

#include <QDebug>
#include <QThread>
#include <QIODevice>
#include <QTimer>
#include <QStatusBar>

#include <QMessageBox>

#define IP "10.170.15.1"

Model toModel(char** mat){

    Model res = Model(32, vector<bool>(32, false));
    for (int i = 0; i < 32; ++i){
        for (int j = 0; j < 32; ++j){
            res[i][j] = (mat[i][j] != 0);
        }
    }
    return res;
}

char** fromModel(Model mat){

    char** res = new char*[32];
    for (int i = 0; i < 32; ++i){
        res[i] = new char[32];
    }

    for (int i = 0; i < 32; ++i){
        for (int j = 0; j < 32; ++j){
            res[i][j] = (mat[i][j] != 0);
        }
    }
    return res;
}

QString toString(const Model model){
    QString s;
    for (int i = 0; i < 32; ++i){
        for (int j = 0; j < 32; ++j){
            s += ((model[i][j] != 0) ? "1" : "0");
        }
    }
    return s;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    ui->centralWidget->show();

    this->setWindowTitle("Asml DEMO");

    //m_refreshTimer = new QTimer(this);
    //m_refreshTimer->setInterval(2000);
    //m_refreshTimer->start();
    //connect(m_refreshTimer, &QTimer::timeout, this, &MainWindow::sendRequestPanels);


    ui->runButton->setEnabled(true);

    //selectPort(ui->portSelector->itemText(0));


    m_modelA = ui->canvas->getModel();
    m_modelB = ui->canvas->getModel();

    connect(ui->clearButton, &QPushButton::pressed, ui->canvas, &PaletteEditor::clear);
    connect(ui->editAbutton, &QPushButton::pressed, this, &MainWindow::selectA);
    connect(ui->editBbutton, &QPushButton::pressed, this, &MainWindow::selectB);
    connect(ui->runButton, &QPushButton::pressed, this, &MainWindow::sendRequest);

    ui->adjustPanels->addItem("4");
    ui->adjustPanels->addItem("3");
    ui->adjustPanels->addItem("2");
    ui->adjustPanels->addItem("1");

    connect(ui->adjustPanels, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, [this](int i){
        qDebug() << i << " selected";
        int n = 4 - i;

        switch(i){
        case 0:
            ui->canvas->resizePalette(32, 32);
            break;
        case 1:
            ui->canvas->resizePalette(32, 24);
            break;
        case 2:
            ui->canvas->resizePalette(32, 16);
            break;
        case 3:
            ui->canvas->resizePalette(32, 8);
            break;
        }

        Shapes::Shape obj;
        auto curr = ui->canvas->getBigModel();

        if (m_numPanels == 4){
            saveCurrent();
            ui->canvas->loadModel(*m_currentModel, true);
            curr = ui->canvas->getModel();
        }
        auto model = toModel(obj.resize(fromModel(curr), n));
        ui->canvas->loadModel(model, false);
        m_numPanels = n;
        update();
    });


    ui->selectShape->addItem("----");   
    ui->selectShape->addItem("Triangle");
    ui->selectShape->addItem("Square");
    ui->selectShape->addItem("Diamond");
    ui->selectShape->addItem("Circle");
    ui->selectShape->addItem("Large Disk");
    ui->selectShape->addItem("Small Disk");
    ui->selectShape->addItem("ASML");


    connect(ui->selectShape, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, [this](int i){
        qDebug() << i << " selected";
        Shapes::Shape obj;
        Shapes::Type t;
        if (!i)
            return;

        switch(i){
        case 1:
            t = Shapes::TRIANGLE;
            ui->canvas->loadModel(toModel(obj.get(t,  4 , 1)), true);
            break;
        case 2:
            t = Shapes::SQUARE;
            ui->canvas->loadModel(toModel(obj.get(t,  4 , 32)), true);
            break;
        case 3:
            t = Shapes::DIAMOND;
            ui->canvas->loadModel(toModel(obj.get(t,  4 , 1)), true);
            break;
        case 4:
            t = Shapes::ROUND;
            ui->canvas->loadModel(toModel(obj.get(t,  4 , 1)), true);
            break;
        case 5:
            t = Shapes::LARGE_OPAQUE;
            ui->canvas->loadModel(toModel(obj.get(t,  4 , 1)), true);
            break;
        case 6:
            t = Shapes::SMALL_OPAQUE;
            ui->canvas->loadModel(toModel(obj.get(t,  4 , 1)), true);
            break;
        case 7:
            t = Shapes::ASML;
            ui->canvas->loadModel(toModel(obj.get(t,  4 , 1)), true);
            break;

        }
        //ui->canvas->loadModel(toModel(obj.get(t,  4 , 1)));
        update();
    });

    m_currentModel = &m_modelA;
    selectA();
}


void MainWindow::saveCurrent(){
    *m_currentModel = ui->canvas->getModel();
}

Q_SLOT void MainWindow::selectA(){
    saveCurrent();
    m_currentModel = &m_modelA;
    ui->canvas->loadModel(m_modelA, true);
    ui->editAbutton->setEnabled(false);
    ui->editBbutton->setEnabled(true);
    update();
}

Q_SLOT void MainWindow::selectB(){
    saveCurrent();
    m_currentModel = &m_modelB;
    ui->canvas->loadModel(m_modelB, true);
    ui->editBbutton->setEnabled(false);
    ui->editAbutton->setEnabled(true);

    update();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::sendRequest()
{
    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onFinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));

    saveCurrent();
    QString img1 = toString(m_modelA);
    QString img2 = toString(m_modelB);
    QString request = QString("http://")+IP+"/?img1=" + img1 + "&img2=" + img2;

    mgr->get(QNetworkRequest(QUrl(request)));
    qDebug() << request;
}

void MainWindow::sendRequestPanels(){
    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onFinishPanels(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));

    QString request = QString("http://")+IP+"/?panels=";

    mgr->get(QNetworkRequest(QUrl(request)));
    qDebug() << request;
}

void MainWindow::onFinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    qDebug() << "received (images)" << str;
}

void MainWindow::onFinishPanels(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    int n = str.toInt();
    if (n <= 0)
        return;
    //ui->panels->display(n);
    qDebug() << "received" << str;
    update();
}
