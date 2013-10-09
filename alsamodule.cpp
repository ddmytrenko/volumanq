#include "alsamodule.h"
#include "log.h"

#include <fcntl.h>
#include <alsa/asoundlib.h>

AlsaModule::AlsaModule()
{
}

int AlsaModule::audioVolume(AudioVolumeAction action, long *volume)
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
    return -1;
  }

  if ((snd_mixer_attach(handle, soundCard)) < 0)
  {
    STDLog().Get(logERROR) << "could not attach mixer to default sound card!";
    snd_mixer_close(handle);
    return -2;
  }

  if ((snd_mixer_selem_register(handle, NULL, NULL)) < 0)
  {
    STDLog().Get(logERROR) << "could not register the mixer!";
    snd_mixer_close(handle);
    return -3;
  }

  ret = snd_mixer_load(handle);
  if (ret < 0)
  {
    STDLog().Get(logERROR) << "could not load mixer!";
    snd_mixer_close(handle);
    return -4;
  }

  elem = snd_mixer_find_selem(handle, sid);
  if (!elem)
  {
    STDLog().Get(logERROR) << "could not find the setting element!";
    snd_mixer_close(handle);
    return -5;
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
      return -6;
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
      return -7;
    }

    long vol = (*volume * (maxVol - minVol) / (100-1)) + minVol;

    if(snd_mixer_selem_set_playback_volume(elem, SND_MIXER_SCHN_UNKNOWN, vol) < 0)
    {
      STDLog().Get(logERROR) << "could not set the playback volume!";
      snd_mixer_close(handle);
      return -8;
    }

    STDLog().Get(logDEBUG) << "set master volume " << *volume << " with status " << ret;
  }

  snd_mixer_close(handle);
  return 0;
}
