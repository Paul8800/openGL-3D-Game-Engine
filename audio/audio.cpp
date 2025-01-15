//RC: g++ -o /home/paul/NeoVimProjects/temp/temp audio/audio.cpp -lopenal
#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <thread>

#include "audio.h"

// Helper function to check for errors
void checkError(const std::string& operation) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error during " << operation << ": " << alGetString(error) << std::endl;
        exit(EXIT_FAILURE);
    }
}
// Helper function to load a WAV file
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

int playAudio() {
    // Initialize OpenAL
    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open OpenAL device." << std::endl;
        return EXIT_FAILURE;
    }

    ALCcontext* context = alcCreateContext(device, nullptr);
    if (!context || alcMakeContextCurrent(context) == ALC_FALSE) {
        std::cerr << "Failed to create or make current OpenAL context." << std::endl;
        if (context) alcDestroyContext(context);
        alcCloseDevice(device);
        return EXIT_FAILURE;
    }

    checkError("initialization");

    // Load WAV file
    std::string filename = "audio/files/smallCrash.wav";
    std::vector<char> data;
    ALenum format;
    ALsizei freq;
    if (!loadWAV(filename, data, format, freq)) {
        alcDestroyContext(context);
        alcCloseDevice(device);
        return EXIT_FAILURE;
    }

    // Create buffer and source
    ALuint buffer, source;
    alGenBuffers(1, &buffer);
    checkError("buffer creation");

    alBufferData(buffer, format, data.data(), data.size(), freq);
    checkError("buffer data");

    alGenSources(1, &source);
    checkError("source creation");

    alSourcei(source, AL_BUFFER, buffer);
    checkError("source setup");

    // Play sound
    alSourcePlay(source);
    std::cout << "Playing sound..." << std::endl;

    // Wait for the sound to finish playing
    /*
    ALint state;
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);
    */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Stop playback after 1 second
    alSourceStop(source);

    // Clean up
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);

    std::cout << "Done." << std::endl;
    return EXIT_SUCCESS;
}
