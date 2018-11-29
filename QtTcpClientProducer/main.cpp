#include "mainwindow.h"
#include <QApplication>

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  MainWindow window;
  window.show();

  return app.exec();
}
