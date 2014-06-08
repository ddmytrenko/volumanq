#ifndef ALSAMODULE_H
#define ALSAMODULE_H

typedef enum {
    AUDIO_VOLUME_SET,
    AUDIO_VOLUME_GET
} audio_volume_action;

class AlsaModule
{

public:

    AlsaModule();

    /**
     * This method is using for getting and setting master volume
     * system-wide via ALSA (using libasound2-dev).
     *
     * @brief audio_volume
     * @param action
     * @param volume
     * @param mixName
     * @return
     */
    int audio_volume(audio_volume_action action, long* volume, const char* mix_name);

};

#endif // ALSAMODULE_H
