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
    void setup_qslider();
    void create_actions();
    void create_tray_icon();
    void set_tray_icon(long volumeLevel);
    void move_on_top_of_cursor();

    QSlider *slider;

    AlsaModule *alsa_module;

    QSystemTrayIcon *tray_icon;
    QMenu *tray_menu;

    //  QAction *settings;
    QAction *refresh;
    QAction *about;
    QAction *close;

    long current_vol;   // the value of current master volume
    long saved_vol;     // saved value of current master volume (using in mute/unmute option)

    void refresh_state();

private slots:
    void on_tray_icon_activation(QSystemTrayIcon::ActivationReason);
    void change_mixer_volume(int);
    void on_about_menu_item_click();
    void on_refresh_menu_item_click();
    //  void onSettingsMenuItemClick();
};

#endif // VOLUMESLIDER_H
