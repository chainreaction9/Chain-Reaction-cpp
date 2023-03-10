#pragma once
#ifndef AUDIOLOADER_H
#define AUDIOLOADER_H

#include "AudioFile.h"

class AudioLoader: AudioFile<float>{
	public:
		static bool loadAudio(const char* filePath, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample, std::vector<uint8_t>& data, ALsizei& size);
		static bool loadAudioFromMemory(const BYTE* source, size_t rawDataSize, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample, std::vector<uint8_t>& storageLocation, ALsizei& dataChunkSize);
		~AudioLoader();
	private:
		AudioLoader();
		static bool extractWaveData(std::vector<uint8_t>& fileData, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample, std::vector<uint8_t>& storageLocation, ALsizei& audioBufferSize);
		static bool extractAiffData(std::vector<uint8_t>& fileData, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample, std::vector<uint8_t>& storageLocation, ALsizei& audioBufferSize);
};
#endif //!AUDIOLOADER_H