#include "volumeslider.h"
#include "aboutdialog.h"
#include "log.h"

#include <QMenu>
#include <QCloseEvent>
#include <QApplication>

#define WIDTH 20
#define HEIGHT 150

#define VOLUME_MUTE 0
#define VOLUME_MAX 100

#define SLIDER_SINGLE_STEP 5
#define SLIDER_PAGE_STEP 10

VolumeSlider::VolumeSlider(QWidget *parent) : QWidget(parent)
{
  alsaModule = new AlsaModule();

  setWindowFlags(Qt::Popup);
  resize(WIDTH, HEIGHT);

  createActions();
  createTrayIcon();

  refreshState();

  setUpQSlider();

  trayIcon->show();
}

void VolumeSlider::setUpQSlider()
{
  slider = new QSlider(Qt::Vertical, this);
  slider->setRange(VOLUME_MUTE, VOLUME_MAX);
  slider->setSingleStep(SLIDER_SINGLE_STEP);
  slider->setPageStep(SLIDER_PAGE_STEP);
  slider->setValue(currentVolume);
  slider->resize(WIDTH, HEIGHT);

  connect(slider, SIGNAL(valueChanged(int)), this, SLOT(changeSystemVolumeLevel(int)));
}

void VolumeSlider::createActions()
{
//  // Adding and connecting `Settings' action
//  settings = new QAction(QIcon(":/resources/settings_128x128.png"), tr("&Settings"), this);
//  connect(settings, SIGNAL(triggered()), this, SLOT(onSettingsMenuItemClick()));

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

void VolumeSlider::createTrayIcon()
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

void VolumeSlider::moveOnTopOfCursor()
{
  QPoint p = cursor().pos();
  p.setX(p.x() - WIDTH/2);
  p.setY(p.y() - HEIGHT);
  move(p);
}

void VolumeSlider::setTrayIcon(long volumeLevel)
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
    alsaModule->audioVolume(AUDIO_VOLUME_SET, &currentVolume);

    /**
     * Set tray icon hint message
     */
    QString hint = "Volume: ";
    hint.append(QString("%1").arg(currentVolume));
    hint.append("%");
    trayIcon->setToolTip(hint);
  }
}

void VolumeSlider::trayIconClicked(QSystemTrayIcon::ActivationReason reason)
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
  else if (reason == QSystemTrayIcon::MiddleClick)
    {
      moveOnTopOfCursor();
      show();
    }
}

void VolumeSlider::refreshState()
{
  long systemVolume = 0;
  alsaModule->audioVolume(AUDIO_VOLUME_GET, &systemVolume);
  STDLog().Get(logDEBUG) << "system master volume level is " << systemVolume;
  setTrayIcon(systemVolume);
}

void VolumeSlider::onAboutMenuItemClick()
{
  AboutDialog about;
  about.exec();
}

void VolumeSlider::onRefreshMenuItemClick()
{
  refreshState();
}

void VolumeSlider::changeSystemVolumeLevel(int val)
{
  setTrayIcon((long)val);
}

//void VolumeSlider::onSettingsMenuItemClick()
//{
//}

VolumeSlider::~VolumeSlider()
{
  delete trayIcon;
  delete trayMenu;
  delete slider;

  delete alsaModule;

//  delete settings;
  delete refresh;
  delete about;
  delete close;
}
