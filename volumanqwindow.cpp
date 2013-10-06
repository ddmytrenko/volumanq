#include "volumanqwindow.h"
#include "aboutdialog.h"
#include "log.h"

#include <fcntl.h>
#include <alsa/asoundlib.h>

#include <QMenu>
#include <QCloseEvent>
#include <QApplication>

VolumeQWindow::VolumeQWindow()
{
  createActions();
  createTrayIcon();

  refreshState();

  trayIcon->show();
}

void VolumeQWindow::createActions()
{
//  // Adding and connecting `Settings' action
//  settings = new QAction(QIcon(":/resources/settings_128x128.png"), tr("&Settings"), this);
//  connect(settings, SIGNAL(triggered()), this, SLOT(show()));

  // Adding and connecting `Refresh' action
  refresh = new QAction(QIcon(":/resources/refresh_128x128.png"), tr("Re&fresh"), this);
  connect(refresh, SIGNAL(triggered()), this, SLOT(onRefreshMenuItemClick()));

  // Adding and connecting `About' action
  about = new QAction(QIcon(":/resources/star_128x128.png"), tr("&About"), this);
  connect(about, SIGNAL(triggered()), this, SLOT(onAboutMenuItemClick()));

  // Adding and connecting `Quit' action
  close = new QAction(QIcon(":/resources/quit_128x128.png"), tr("&Quit"), this);
  connect(close, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void VolumeQWindow::createTrayIcon()
{
  trayMenu = new QMenu(this);

//  trayMenu->addAction(settings);
//  trayMenu->addSeparator();
  trayMenu->addAction(refresh);
  trayMenu->addSeparator();
  trayMenu->addAction(about);
  trayMenu->addAction(close);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayMenu);

  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
      this, SLOT(trayIconClicked(QSystemTrayIcon::ActivationReason)));
}

void VolumeQWindow::setTrayIcon(long volumeLevel)
{
  if (currentVolume != volumeLevel)
  {
    STDLog().Get(logDEBUG) << "changing master volume from " << currentVolume << " to " << volumeLevel;

    /**
     * Current method set a different icon in order
     * to selected volume level.
     */

    if (volumeLevel == 0) {
      trayIcon->setIcon(QIcon(":/resources/mute_128x128.png"));
    } else if (volumeLevel <= 12) {
      trayIcon->setIcon(QIcon(":/resources/volume0_128x128.png"));
    } else if (volumeLevel <= 37) {
      trayIcon->setIcon(QIcon(":/resources/volume25_128x128.png"));
    } else if (volumeLevel <= 62) {
      trayIcon->setIcon(QIcon(":/resources/volume50_128x128.png"));
    } else if (volumeLevel <= 87) {
      trayIcon->setIcon(QIcon(":/resources/volume75_128x128.png"));
    } else if (volumeLevel <= 100) {
      trayIcon->setIcon(QIcon(":/resources/volume100_128x128.png"));
    }

    currentVolume = volumeLevel;

    /**
     * Set Master volume system wide via ALSA
     * Using libasound2-dev package.
     * LIBS += -lasound
     */
    audioVolume(AUDIO_VOLUME_SET, &currentVolume);

    /**
     * Set tray icon hint message
     */
    QString hint = "Volume: ";
    hint.append(QString("%1").arg(currentVolume));
    hint.append("%");
    trayIcon->setToolTip(hint);
  }
}

void VolumeQWindow::trayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::Trigger)
    {
      /**
       * Mute/Unmute on click
       */
      if (currentVolume > 0) {
        savedVolume = currentVolume;
        setTrayIcon(0);
      } else {
        setTrayIcon(savedVolume);
      }
    }
}

int VolumeQWindow::audioVolume(AudioVolumeAction action, long *volume)
{
  int ret = 0;

  snd_mixer_t* handle;
  snd_mixer_elem_t* elem;
  snd_mixer_selem_id_t* sid;

  static const char* mixName = "Master";
  static const char* soundCard = "default";
  static int mixIndex = 0;

  snd_mixer_selem_id_alloca(&sid);

  /**
   * Setting simple mixer index and name
   */
  snd_mixer_selem_id_set_index(sid, mixIndex);
  snd_mixer_selem_id_set_name(sid, mixName);

  if ((snd_mixer_open(&handle, 0)) < 0)
  {
    STDLog().Get(logERROR) << "could not open ALSA audio mixer (amixer)!";
    qApp->exit(-1);
  }

  if ((snd_mixer_attach(handle, soundCard)) < 0)
  {
    STDLog().Get(logERROR) << "could not attach mixer to default sound card!";
    snd_mixer_close(handle);
    qApp->exit(-2);
  }

  if ((snd_mixer_selem_register(handle, NULL, NULL)) < 0)
  {
    STDLog().Get(logERROR) << "could not register the mixer!";
    snd_mixer_close(handle);
    qApp->exit(-3);
  }

  ret = snd_mixer_load(handle);
  if (ret < 0)
  {
    STDLog().Get(logERROR) << "could not load mixer!";
    snd_mixer_close(handle);
    qApp->exit(-4);
  }

  elem = snd_mixer_find_selem(handle, sid);
  if (!elem)
  {
    STDLog().Get(logERROR) << "could not find the setting element!";
    snd_mixer_close(handle);
    qApp->exit(-5);
  }

  long minVol, maxVol;

  snd_mixer_selem_get_playback_volume_range (elem, &minVol, &maxVol);
  STDLog().Get(logDEBUG) << "volume range: [" << minVol << "," << maxVol << "]";

  if(action == AUDIO_VOLUME_GET)
  {
    if(snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_UNKNOWN, volume) < 0)
    {
      STDLog().Get(logERROR) << "could not get playback volume!";
      snd_mixer_close(handle);
      qApp->exit(-6);
    }

    STDLog().Get(logDEBUG) << "get volume " << *volume << " with status " << ret;
    /* make the value bound to 100 */
    *volume -= minVol;
    maxVol -= minVol;
    minVol = 0;

    /**
     * make the volume bound from 0 to 100
     */
    *volume = 100 * (*volume) / maxVol;
  }
  else if(action == AUDIO_VOLUME_SET)
  {
    if(*volume < 0 || *volume > 100)
    {
      /**
       * out of bounds
       */
      STDLog().Get(logERROR) << "volume value is out of bounds!";
      snd_mixer_close(handle);
      qApp->exit(-7);
    }

    long vol = (*volume * (maxVol - minVol) / (100-1)) + minVol;

    if(snd_mixer_selem_set_playback_volume(elem, SND_MIXER_SCHN_UNKNOWN, vol) < 0)
    {
      STDLog().Get(logERROR) << "could not set the playback volume!";
      snd_mixer_close(handle);
      qApp->exit(-8);
    }

    STDLog().Get(logDEBUG) << "set master volume " << *volume << " with status " << ret;
  }

  snd_mixer_close(handle);
  return 0;
}

void VolumeQWindow::refreshState()
{
  long systemVolume = 0;
  audioVolume(AUDIO_VOLUME_GET, &systemVolume);
  STDLog().Get(logDEBUG) << "system master volume level is " << systemVolume;
  setTrayIcon(systemVolume);
}

void VolumeQWindow::onAboutMenuItemClick()
{
  AboutDialog about;
  about.exec();
}

void VolumeQWindow::onRefreshMenuItemClick()
{
  refreshState();
}

VolumeQWindow::~VolumeQWindow()
{
  delete trayIcon;
  delete trayMenu;

//  delete settings;
  delete refresh;
  delete about;
  delete close;
}
