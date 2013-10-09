#include "volumeslider.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setQuitOnLastWindowClosed(false);

  VolumeSlider slider;
  slider.hide();

  return a.exec();
}
