#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDebug>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

  public slots:
    void setIpAddr(QString ip);

    void setSource(QString ip);

    void setInterval(int interval);

    void getIps();

    void tcpConnect();

    void tcpDisconnect();

    void startDataRead();

    void stopDataRead();

    void getData();

  private:
    QString ip, source;
    int interval;

    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QTimer *timer;
};

#endif // MAINWINDOW_H
