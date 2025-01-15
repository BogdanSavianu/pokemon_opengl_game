#ifndef AUDIO_MANAGER_HPP
#define AUDIO_MANAGER_HPP

#if defined(__APPLE__)
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#else
    #include <AL/al.h>
    #include <AL/alc.h>
#endif

#include <string>
#include <vector>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool initialize();
    bool loadSound(const std::string& filename, ALuint& buffer);
    void playSound(ALuint buffer, bool loop = false);
    void stopSound();
    void setVolume(float volume);
    void cleanup();

private:
    ALCdevice* device;
    ALCcontext* context;
    ALuint source;
    std::vector<ALuint> buffers;
    bool isInitialized;

    bool checkError(const std::string& msg);
};

#endif 