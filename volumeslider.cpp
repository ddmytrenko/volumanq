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
    alsa_module = new AlsaModule();

    setWindowFlags(Qt::Popup);
    resize(WIDTH, HEIGHT);

    create_actions();
    create_tray_icon();

    refresh_state();

    setup_qslider();

    tray_icon->show();
}

void VolumeSlider::setup_qslider()
{
    slider = new QSlider(Qt::Vertical, this);
    slider->setRange(VOLUME_MUTE, VOLUME_MAX);
    slider->setSingleStep(SLIDER_SINGLE_STEP);
    slider->setPageStep(SLIDER_PAGE_STEP);
    slider->setValue(current_vol);
    slider->resize(WIDTH, HEIGHT);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(change_mixer_volume(int)));
}

void VolumeSlider::create_actions()
{
    //  // Adding and connecting `Settings' action
    //  settings = new QAction(QIcon(":/resources/settings_128x128.png"), tr("&Settings"), this);
    //  connect(settings, SIGNAL(triggered()), this, SLOT(onSettingsMenuItemClick()));

    // Adding and connecting `Refresh' action
    refresh = new QAction(QIcon(":/resources/refresh_128x128.png"), tr("Re&fresh"), this);
    connect(refresh, SIGNAL(triggered()), this, SLOT(on_refresh_menu_item_click()));

    // Adding and connecting `About' action
    about = new QAction(QIcon(":/resources/star_128x128.png"), tr("&About"), this);
    connect(about, SIGNAL(triggered()), this, SLOT(on_about_menu_item_click()));

    // Adding and connecting `Quit' action
    close = new QAction(QIcon(":/resources/quit_128x128.png"), tr("&Quit"), this);
    connect(close, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void VolumeSlider::create_tray_icon()
{
    tray_menu = new QMenu(this);

    //  trayMenu->addAction(settings);
    //  trayMenu->addSeparator();
    tray_menu->addAction(refresh);
    tray_menu->addSeparator();
    tray_menu->addAction(about);
    tray_menu->addAction(close);

    tray_icon = new QSystemTrayIcon(this);
    tray_icon->setContextMenu(tray_menu);

    connect(tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(on_tray_icon_activation(QSystemTrayIcon::ActivationReason)));
}

void VolumeSlider::move_on_top_of_cursor()
{
    QPoint p = cursor().pos();
    p.setX(p.x() - WIDTH/2);
    p.setY(p.y() - HEIGHT);
    move(p);
}

void VolumeSlider::set_tray_icon(long vol_level)
{
    if (current_vol != vol_level)
    {
        STDLog().Get(logDEBUG) << "changing master volume from " << current_vol << " to " << vol_level;

        /**
         * Current method set a different icon in order
         * to selected volume level.
         */

        if (vol_level == 0) {
            tray_icon->setIcon(QIcon(":/resources/mute_128x128.png"));
        } else if (vol_level <= 12) {
            tray_icon->setIcon(QIcon(":/resources/volume0_128x128.png"));
        } else if (vol_level <= 37) {
            tray_icon->setIcon(QIcon(":/resources/volume25_128x128.png"));
        } else if (vol_level <= 62) {
            tray_icon->setIcon(QIcon(":/resources/volume50_128x128.png"));
        } else if (vol_level <= 87) {
            tray_icon->setIcon(QIcon(":/resources/volume75_128x128.png"));
        } else if (vol_level <= 100) {
            tray_icon->setIcon(QIcon(":/resources/volume100_128x128.png"));
        }

        current_vol = vol_level;

        /**
         * Set Master volume system wide via ALSA
         * Using libasound2-dev package.
         * LIBS += -lasound
         */
        if (alsa_module->audio_volume(AUDIO_VOLUME_SET, &current_vol, "PCM") < 0) {
            alsa_module->audio_volume(AUDIO_VOLUME_SET, &current_vol, "Master");
        }

        /**
         * Set tray icon hint message
         */
        QString hint = "Volume: ";
        hint.append(QString("%1").arg(current_vol));
        hint.append("%");
        tray_icon->setToolTip(hint);
    }
}

void VolumeSlider::on_tray_icon_activation(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
    {
        /**
         * Mute/Unmute on click
         */
        if (current_vol > 0) {
            saved_vol = current_vol;
            set_tray_icon(0);
        } else {
            set_tray_icon(saved_vol);
        }
    }
    else if (reason == QSystemTrayIcon::MiddleClick)
    {
        move_on_top_of_cursor();
        show();
    }
}

void VolumeSlider::refresh_state()
{
    long system_volume = 0;
    if (alsa_module->audio_volume(AUDIO_VOLUME_GET, &system_volume, "PCM") < 0) {
        alsa_module->audio_volume(AUDIO_VOLUME_GET, &system_volume, "Master");
    }
    STDLog().Get(logDEBUG) << "system master volume level is " << system_volume;
    set_tray_icon(system_volume);
}

void VolumeSlider::on_about_menu_item_click()
{
    AboutDialog about;
    about.exec();
}

void VolumeSlider::on_refresh_menu_item_click()
{
    refresh_state();
}

void VolumeSlider::change_mixer_volume(int val)
{
    set_tray_icon((long)val);
}

//void VolumeSlider::onSettingsMenuItemClick()
//{
//}

VolumeSlider::~VolumeSlider()
{
    delete tray_icon;
    delete tray_menu;
    delete slider;

    delete alsa_module;

    //  delete settings;
    delete refresh;
    delete about;
    delete close;
}
