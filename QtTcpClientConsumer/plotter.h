#ifndef PLOTTER_H
#define PLOTTER_H

#include <vector>
#include <QMap>
#include <QWidget>

using namespace std;

class Plotter : public QWidget
{
    Q_OBJECT
  public:
    explicit Plotter(QWidget *parent = nullptr);
    ~Plotter();

    void paintEvent(QPaintEvent *event);

    void setPlot(vector<float> times, vector<float> values);

    void clear();

  signals:

  public slots:

  private:
    vector<float> *times;
    vector<float> *values;
};

#endif // PLOTTER_H
