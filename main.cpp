#include "volumanqwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  // settings QDialog has to be non-visible at the beginning
  VolumeQWindow settings;
  settings.setVisible(false);
  settings.hide();

  return a.exec();
}
