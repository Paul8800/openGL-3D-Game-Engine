#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>


// Declare functions
void checkError(const std::string& operation);
bool loadWAV(const std::string& filename, std::vector<char>& data, ALenum& format, ALsizei& freq);
int playAudio();

#endif // AUDIO_H
