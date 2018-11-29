#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDebug>
#include <QTimer>
#include <QStringListModel>

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
    void setMin(int min);
    void setMax(int max);
    void postData();
    void updateProgressBar();
    void tcpConnect();
    void tcpDisconnect();
    void startSending();
    void stopSending();

  private:
    QString ip;
    int min, max, interval, currentSeconds;

    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QTimer *progressBarTimer;
    QStringList *list;
    QStringListModel *listModel;
};

#endif // MAINWINDOW_H
