#include "AudioManager.hpp"
#include <iostream>
#include <vector>
#include <sndfile.h>

AudioManager::AudioManager() : device(nullptr), context(nullptr), isInitialized(false) {
}

AudioManager::~AudioManager() {
    cleanup();
}

bool AudioManager::initialize() {
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open audio device" << std::endl;
        return false;
    }

    context = alcCreateContext(device, nullptr);
    if (!context) {
        std::cerr << "Failed to create audio context" << std::endl;
        return false;
    }

    if (!alcMakeContextCurrent(context)) {
        std::cerr << "Failed to make context current" << std::endl;
        return false;
    }

    alGenSources(1, &source);
    if (!checkError("Source generation")) {
        return false;
    }

    isInitialized = true;
    return true;
}

bool AudioManager::loadSound(const std::string& filename, ALuint& buffer) {
    SF_INFO fileInfo;
    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &fileInfo);
    if (!file) {
        std::cerr << "Failed to open sound file: " << filename << std::endl;
        return false;
    }

    std::vector<short> samples(fileInfo.frames * fileInfo.channels);
    sf_read_short(file, samples.data(), samples.size());
    sf_close(file);

    alGenBuffers(1, &buffer);
    buffers.push_back(buffer);

    ALenum format = (fileInfo.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    alBufferData(buffer, format, samples.data(), 
                samples.size() * sizeof(short), fileInfo.samplerate);

    return checkError("Buffer loading");
}

void AudioManager::playSound(ALuint buffer, bool loop) {
    if (!isInitialized) return;

    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcePlay(source);
}

void AudioManager::stopSound() {
    if (!isInitialized) return;
    alSourceStop(source);
}

void AudioManager::setVolume(float volume) {
    if (!isInitialized) return;
    alSourcef(source, AL_GAIN, volume);
}

void AudioManager::cleanup() {
    if (isInitialized) {
        alSourceStop(source);
        alDeleteSources(1, &source);
        
        for (ALuint buffer : buffers) {
            alDeleteBuffers(1, &buffer);
        }
        buffers.clear();

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);
    }
}

bool AudioManager::checkError(const std::string& msg) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error (" << msg << "): " << error << std::endl;
        return false;
    }
    return true;
} 