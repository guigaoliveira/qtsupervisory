#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <cmath>
#include "plotter.h"

#define MAX_ITEMS 30.0

/**
 * @brief Plotter::Plotter
 * @param parent
 */
Plotter::Plotter(QWidget *parent) : QWidget(parent)
{
    times = new vector<float>();
    values = new vector<float>();
}

/**
 * @brief Plotter::~Plotter
 */
Plotter::~Plotter()
{
    delete times;
    delete values;
}

/**
 * @brief Plotter::paintEvent
 * @param event
 * @details Evento para atualizar o gráfico
 */
void Plotter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QBrush brush;
    QPen pen;

    painter.setRenderHint(QPainter::Antialiasing);

    brush.setColor(Qt::white);
    brush.setStyle(Qt::SolidPattern);

    painter.setBrush(brush);

    pen.setColor(QColor(33, 150, 243));
    pen.setWidth(1);

    painter.setPen(pen);

    painter.drawRect(0, 0, this->width(), this->height());

    pen.setColor(QColor(210, 210, 210));
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    int gap = 30;

    for (int i = width() / gap; i < width(); i = i + width() / gap)
    {
        painter.drawLine(i, 0, i, height());
    }

    for (int i = height() / gap; i < height(); i = i + height() / gap)
    {
        painter.drawLine(0, i, width(), i);
    }

    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(33, 150, 243));
    painter.setPen(pen);

    int data_total = this->times->size();

    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = height();

    int count = 0;

    double max_value = 0.0;

    if (this->values->size() > 0)
    {
        max_value = *std::max_element(values->begin(), values->end());
    }

    for (vector<float>::iterator i = values->begin(); i != values->end(); i++)
    {
        x1 = x2;
        y1 = y2;

        x2 = (count++ / (float)data_total) * width();
        y2 = height() - ((*i / max_value) * height());

        painter.drawLine(x1, y1, x2, y2);
    }
}

/**
 * @brief Plotter::setPlot
 * @param time
 * @param values
 * @details Plota o valores no gráfico
 */
void Plotter::setPlot(vector<float> time, vector<float> values)
{
    if (time.size() != values.size())
    {
        return;
    }

    this->times->clear();
    this->values->clear();

    for (vector<float>::iterator i = time.begin(); i != time.end(); i++)
    {
        this->times->push_back(*i);
    }
    for (vector<float>::iterator i = values.begin(); i != values.end(); i++)
    {
        this->values->push_back(*i);
    }

    this->repaint();
}

/**
 * @brief Plotter::clear
 * @details limpa os valores plotados no gráfico
 */
void Plotter::clear()
{
    this->times->clear();
    this->values->clear();

    this->repaint();
}
