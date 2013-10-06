#ifndef VOLUMANQWINDOW_H
#define VOLUMANQWINDOW_H

#include <QDialog>
#include <QSystemTrayIcon>

typedef enum {
  AUDIO_VOLUME_SET,
  AUDIO_VOLUME_GET
} AudioVolumeAction;

class QMenu;

class VolumeQWindow : public QDialog
{
  Q_OBJECT

public:
  VolumeQWindow();
  ~VolumeQWindow();

private:
  void createActions();
  void createTrayIcon();
  void setTrayIcon(long volumeLevel);

  QSystemTrayIcon *trayIcon;
  QMenu *trayMenu;

//  QAction *settings;
  QAction *refresh;
  QAction *about;
  QAction *close;

  long currentVolume;   // the value of current master volume
  long savedVolume;     // saved value of current master volume (using in mute/unmute option)

  /**
   * This method is using for getting and setting master volume
   * system-wide via ALSA (using libasound2-dev).
   *
   * @brief audioVolume
   * @param action
   * @param volume
   * @return
   */
  int audioVolume(AudioVolumeAction action, long* volume);

  void refreshState();

private slots:
  void trayIconClicked(QSystemTrayIcon::ActivationReason);
  void onAboutMenuItemClick();
  void onRefreshMenuItemClick();
};

#endif // VOLUMANQWINDOW_H
