#ifndef ALSAMODULE_H
#define ALSAMODULE_H

typedef enum {
    AUDIO_VOLUME_SET,
    AUDIO_VOLUME_GET
} AudioVolumeAction;

class AlsaModule
{

public:
    AlsaModule();

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

};

#endif // ALSAMODULE_H
