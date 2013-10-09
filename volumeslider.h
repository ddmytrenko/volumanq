#ifndef VOLUMESLIDER_H
#define VOLUMESLIDER_H

#include <QWidget>
#include <QSlider>
#include <QSystemTrayIcon>

#include "alsamodule.h"

class QMenu;

class VolumeSlider : public QWidget
{
  Q_OBJECT

public:
  explicit VolumeSlider(QWidget *parent = 0);
  ~VolumeSlider();

private:
  void setUpQSlider();
  void createActions();
  void createTrayIcon();
  void setTrayIcon(long volumeLevel);
  void moveOnTopOfCursor();

  QSlider *slider;

  AlsaModule *alsaModule;

  QSystemTrayIcon *trayIcon;
  QMenu *trayMenu;

//  QAction *settings;
  QAction *refresh;
  QAction *about;
  QAction *close;

  long currentVolume;   // the value of current master volume
  long savedVolume;     // saved value of current master volume (using in mute/unmute option)

  void refreshState();

private slots:
  void trayIconClicked(QSystemTrayIcon::ActivationReason);
  void changeSystemVolumeLevel(int);
  void onAboutMenuItemClick();
  void onRefreshMenuItemClick();
//  void onSettingsMenuItemClick();
};

#endif // VOLUMESLIDER_H
