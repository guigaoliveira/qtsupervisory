#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime> // windows
#include <vector>
#include <QTimer>
#include <QDateTime>
#include <QRegularExpression>

#define MAX_ITEMS = 30;

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    timer = new QTimer(this);
    tcpConnect();

    // temporizador
    connect(timer, SIGNAL(timeout()), this, SLOT(getData()));
    // botão de conexão
    connect(ui->btnConnect, SIGNAL(clicked(bool)), this, SLOT(tcpConnect()));
    // botão de disconexão
    connect(ui->btnDisconnect, SIGNAL(clicked(bool)), this, SLOT(tcpDisconnect()));
}

/**
 * @brief MainWindow::setIpAddr
 * @param ip
 * @details Seta o ip do servidor TCP
 */
void MainWindow::setIpAddr(QString ip)
{
    QString IP_REGEX = "^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$"; // expressão regular simples para validar ipv4
    QRegularExpression re(IP_REGEX);
    QRegularExpressionMatch match = re.match(ip);
    if (match.hasMatch())
    {
        QPalette palette = ui->editIpAddr->palette();
        palette.setColor(ui->editIpAddr->foregroundRole(), Qt::darkGreen);
        ui->editIpAddr->setPalette(palette);
        ui->btnConnect->setEnabled(true);
        this->ip = ip;
    }
    else
    {
        QPalette palette = ui->editIpAddr->palette();
        palette.setColor(ui->editIpAddr->foregroundRole(), Qt::red);
        ui->editIpAddr->setPalette(palette);
        ui->btnConnect->setEnabled(false);
    }
}

/**
 * @brief MainWindow::setSource
 * @param ip
 * @details Seta o ip do produtor
 */
void MainWindow::setSource(QString ip)
{

    ui->widget->clear();

    if (!ui->btnStart->isEnabled() && !ui->btnStop->isEnabled())
    {
        ui->btnStart->setEnabled(true);
        ui->btnStop->setEnabled(true);
    }

    if (ip.size() != 0)
    {
        this->source = ip;
        if (timer->isActive())
        {
            getData();
        }
    }
}

/**
 * @brief MainWindow::setInterval
 * @param interval
 * @details Seta o intervalo de atualização
 */
void MainWindow::setInterval(int interval)
{
    this->interval = interval;
}

/**
 * @brief MainWindow::tcpConnect
 * @details Conecta ao servidor TCP
 */
void MainWindow::tcpConnect()
{
    socket->connectToHost(ip, 1234);
    if (socket->waitForConnected(3000))
    {
        QString msg = "Conectado ao servidor TCP!";
        ui->statusBar->showMessage(msg);
        qDebug() << msg;

        ui->btnConnect->setEnabled(false);
        ui->btnUpdate->setEnabled(true);
        ui->sliderTiming->setEnabled(true);

        getIps();
    }
    else
    {
        qDebug() << "Desconectado do servidor TCP!";
    }
}

/**
 * @brief MainWindow::tcpConnect
 * @details Desconecta ao servidor TCP
 */
void MainWindow::tcpDisconnect()
{
    timer->stop();
    socket->disconnectFromHost();

    ui->btnConnect->setEnabled(true);
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(false);
    ui->btnUpdate->setEnabled(false);
    ui->sliderTiming->setEnabled(false);

    ui->listIps->clear();

    QString msg = "Desconectado do servidor TCP!";
    statusBar()->showMessage(msg);
    qDebug() << msg;
}

/**
 * @brief MainWindow::startDataRead
 * @details Inicia a leitura dos dados
 */
void MainWindow::startDataRead()
{
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);

    timer->setInterval(interval * 1000);
    timer->setSingleShot(false);
    timer->start();

    getData();
}

/**
 * @brief MainWindow::stopDataRead
 * @details Para a leitura dos dados
 */
void MainWindow::stopDataRead()
{
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);

    timer->stop();
}

/**
 * @brief MainWindow::getData
 * @details Ler os dados enviados de um produtor
 */
void MainWindow::getData()
{
    QString str;
    QStringList list;

    std::vector<float> time;
    std::vector<float> values;

    qDebug() << "Obtendo os dados...";

    if (socket->state() != QAbstractSocket::ConnectedState || !socket->isOpen())
    {
        return;
    }

    qDebug() << "Lendo...";
    QString command = "get " + source + " 30\r\n";
    qDebug() << command;

    socket->write(command.toStdString().c_str());
    socket->waitForBytesWritten();
    socket->waitForReadyRead();

    qDebug() << socket->bytesAvailable();

    while (socket->bytesAvailable())
    {
        str = socket->readLine().replace("\n", "").replace("\r", "");
        list = str.split(" ");
        if (list.size() == 2)
        {
            bool ok;

            str = list.at(0);
            time.push_back(str.toFloat(&ok));

            str = list.at(1);
            values.push_back(str.toFloat(&ok));

            qDebug() << list.at(0) << ": " << list.at(1);
        }
    }

    ui->widget->setPlot(time, values);
}

/**
 * @brief MainWindow::getIps
 * @details Lista os ips de produtores disponíveis para conexão
 */
void MainWindow::getIps()
{
    QString str;
    qDebug() << "Pegando a lista de IPs de produtores...";

    if (socket->state() != QAbstractSocket::ConnectedState || !socket->isOpen())
    {
        return;
    }

    socket->write("list\r\n");
    socket->waitForBytesWritten();
    socket->waitForReadyRead();
    qDebug() << socket->bytesAvailable();
    ui->listIps->clear();

    while (socket->bytesAvailable())
    {
        str = socket->readLine().replace("\n", "").replace("\r", "");
        ui->listIps->addItem(str);
    }
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete socket;
    delete timer;
    delete ui;
}
