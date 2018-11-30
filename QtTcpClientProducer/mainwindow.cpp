#include "mainwindow.h"
#include <ctime>
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QTimer>
#include <QRegularExpression>

#define TCP_PORT 1234

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    progressBarTimer = new QTimer(this);
    list = new QStringList();
    listModel = new QStringListModel(this);

    // timer para a barra de progresso
    connect(progressBarTimer, SIGNAL(timeout()), this, SLOT(updateProgressBar()));
    // botão de conexão
    connect(ui->btnConnect, SIGNAL(clicked(bool)), this, SLOT(tcpConnect()));
    // botão de desconexão
    connect(ui->btnDisconnect, SIGNAL(clicked(bool)), this, SLOT(tcpDisconnect()));
    // botão de começar a conexão
    connect(ui->btnStart, SIGNAL(clicked(bool)), this, SLOT(startSending()));
    // botão de parar a conexão
    connect(ui->btnStop, SIGNAL(clicked(bool)), this, SLOT(stopSending()));

    qsrand(time(NULL));

    ui->btnConnect->setEnabled(false);
}

/**
 * @brief MainWindow::setIpAddr
 * @param ip
 * @details Seta o endereço de ipv4 do servidor tcp
 */
void MainWindow::setIpAddr(QString ip)
{
    QString IP_REGEX = "^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$"; // expressão regular simples para validar ipv4
    QRegularExpression re(IP_REGEX);
    QRegularExpressionMatch match = re.match(ip);

    /*
        ip válido: cor de texto do input verde, habilita o botão de conexão
        ip inválido: cor de texto do input vermelha, desabilita o botão de conexão
    */
    if (match.hasMatch())
    {
        QPalette palette = ui->editIp->palette();
        palette.setColor(ui->editIp->foregroundRole(), Qt::darkGreen);
        ui->editIp->setPalette(palette);
        ui->btnConnect->setEnabled(true);
        this->ip = ip;
    }
    else
    {
        QPalette palette = ui->editIp->palette();
        palette.setColor(ui->editIp->foregroundRole(), Qt::red);
        ui->editIp->setPalette(palette);
        ui->btnConnect->setEnabled(false);
    }
}

/**
 * @brief MainWindow::setMin
 * @param min
 * @details Seta o valor mínimo do numero aleatorio a ser enviado
 */
void MainWindow::setMin(int min)
{
    this->min = min;
}

/**
 * @brief MainWindow::setMax
 * @param max
 * @details Seta o valor máximo do numero aleatorio a ser enviado
 */
void MainWindow::setMax(int max)
{
    this->max = max;
}

/**
 * @brief MainWindow::tcpConnect
 * @details Conecta ao servidor tcp
 */
void MainWindow::tcpConnect()
{
    socket->connectToHost(ip, TCP_PORT);

    if (socket->waitForConnected(3000))
    {
        ui->btnConnect->setEnabled(false);

        ui->btnDisconnect->setEnabled(true);

        ui->btnStart->setEnabled(true);

        ui->btnStop->setEnabled(true);

        QString success_msg = "Status: Conectado ao servidor.";
        statusBar()->showMessage(success_msg);
        qDebug() << success_msg;
    }
    else
    {
        QString error_msg = "Status: Não foi possível conectar ao servidor.";
        statusBar()->showMessage(error_msg);
        qDebug() << error_msg;
    }
}

/**
 * @brief MainWindow::tcpDisconnect
 * @details Deconecta do servidor tcp
 */
void MainWindow::tcpDisconnect()
{
    socket->disconnectFromHost();

    ui->btnConnect->setEnabled(true);

    ui->btnDisconnect->setEnabled(false);

    ui->btnStart->setEnabled(false);

    ui->btnStop->setEnabled(false);
    
    this->stopSending();
    
    QString msg = "Status: Desconectado do servidor.";
    statusBar()->showMessage(msg);
    qDebug() << msg;
}

/**
 * @brief MainWindow::postData
 * @details Envia os dados para o servidor
 */
void MainWindow::postData()
{
    QString command;
    qint64 milisdate;
    int min = ui->sliderMin->value();
    int max = ui->sliderMax->value();

    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        milisdate = QDateTime::currentDateTime().toMSecsSinceEpoch();
        // comando para envio segue o padrão: set datetime_em_utc numero_aleatorio
        command = "set " + QString::number(milisdate) + " " + QString::number(min + (qrand() % (max - min + 1))) + "\r\n";

        qDebug() << command;
        qDebug() << socket->write(command.toStdString().c_str()) << "bytes escritos";

        if (socket->waitForBytesWritten(3000))
        {
            qDebug() << "O comando foi escrito com sucesso";
        }

        // adiciona a lista
        list->append(command);

        listModel->setStringList(*list);

        ui->listView->setModel(listModel);

        // coloca o scroll do listview para baixo
        ui->listView->scrollTo(listModel->index(list->size() - 1));
    }
}

/**
 * @brief MainWindow::startSending
 * @details Envia os dados e seta o timer da progress bar
 */
void MainWindow::startSending()
{
    postData();

    interval = ui->sliderTiming->value();

    currentSeconds = 1;

    ui->progressBar->setMaximum(interval);

    progressBarTimer->setInterval(1000);

    progressBarTimer->start();
}

/**
 * @brief MainWindow::stopSending
 * @details Para de enviar os dados
 */
void MainWindow::stopSending()
{
    progressBarTimer->stop();
    ui->progressBar->setValue(1);
}

/**
 * @brief MainWindow::updateProgressBar
 * @details Atualiza a process bar
 */
void MainWindow::updateProgressBar()
{
    if (interval == currentSeconds)
    {
        currentSeconds = 1;
        postData();
    }
    else
    {
        currentSeconds++;
    }
    ui->progressBar->setValue(currentSeconds);
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete socket;
    delete progressBarTimer;
    delete list;
    delete listModel;
    delete ui;
}
