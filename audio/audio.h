#ifndef SOUND_H
#define SOUND_H

#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstring>
#include <chrono>
#include <thread>

struct sound {
    ALuint source;
};

//Sound is temp name, change to audio class
class Audio {
public:
    bool loadFile(const std::string& filename, ALuint& source);
    void play(ALuint source);

    Audio() {
        device = alcOpenDevice(nullptr);
        if (!device) {
            std::cerr << "Failed to open OpenAL device." << std::endl;
            exit(EXIT_FAILURE);
        }
        context = alcCreateContext(device, nullptr);
        if (!context || alcMakeContextCurrent(context) == ALC_FALSE) {
            std::cerr << "Failed to create or make current OpenAL context." << std::endl;
            if (context) alcDestroyContext(context);
                alcCloseDevice(device); exit(EXIT_FAILURE);
        }
        checkError("initialization");
        //sources.resize(poolSize);
        //alGenSources(poolSize, sources.data());
        //for (ALuint source : sources) {
            //buffers[source] = 0;
        //}
    }


    
    ALuint loadFile(const char* path) {
        std::vector<char> data;
        ALenum format;
        ALsizei freq;

        if (!loadWAV(path, data, format, freq)) {
            std::cout << "Error loading wav file" << std::endl;
        }


        ALuint buffer;

        alGenBuffers(1, &buffer);
        checkError("buffer creation");

        alBufferData(buffer, format, data.data(), data.size(), freq);
        checkError("buffer data");

        return buffer;
    }

    ALuint createSpeaker() {
        ALuint source;

        alGenSources(1, &source);
        checkError("source creation");

        return source;
    }

    bool play(ALuint source, ALuint buffer) {
        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state == AL_PLAYING) {
            return false;
        }

        alSourcei(source, AL_BUFFER, buffer);
        alSourcePlay(source);
        std::cout << "playing" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
        alSourceStop(source);

        return true;
    }

    ALuint genBuffer(const char* path) {
        ALuint buffer = loadFile(path);
        return buffer;
    }

    ~Audio() {
        for (ALuint source : sources) {
            alDeleteSources(1, &source);
        }
        alDeleteBuffers(1, &buffer);
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);
    }


private:
    ALCdevice* device;
    ALCcontext* context;
    ALuint buffer;
    std::vector<ALuint> buffers;
    std::vector<ALuint> sources;




    bool loadWAV(const std::string& filename, std::vector<char>& data, ALenum& format, ALsizei& freq) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }

    char buffer[4];
    file.read(buffer, 4); // "RIFF"
    file.ignore(4);       // file size
    file.read(buffer, 4); // "WAVE"

    // Check WAV header
    if (std::strncmp(buffer, "WAVE", 4) != 0) {
        std::cerr << "Invalid WAV file: " << filename << std::endl;
        return false;
    }

    // Read format chunk
    file.read(buffer, 4); // "fmt "
    file.ignore(4);       // chunk size
    file.ignore(2);       // audio format
    short channels;
    file.read(reinterpret_cast<char*>(&channels), 2);
    int sampleRate;
    file.read(reinterpret_cast<char*>(&sampleRate), 4);
    file.ignore(6); // byte rate and block align
    short bitsPerSample;
    file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

    // Determine OpenAL format
    if (channels == 1) {
        format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    } else {
        format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }

    freq = sampleRate;

    // Read data chunk
    file.read(buffer, 4); // "data"
    int dataSize;
    file.read(reinterpret_cast<char*>(&dataSize), 4);

    data.resize(dataSize);
    file.read(data.data(), dataSize);

    return true;
} 

void checkError(const std::string& operation) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error during " << operation << ": " << alGetString(error) << std::endl;
        exit(EXIT_FAILURE);
    }
}

};

#endif // SOUND_H
