#include "PrecompiledHeader.h"
#include "AudioLoader.h"

bool AudioLoader::loadAudio(const char* filePath, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample, std::vector<uint8_t>& storageLocation, ALsizei& audioBufferSize) {
	std::ifstream file(filePath, std::ios::binary);
	// check the file exists
	if (!file.good()) {
		wxMessageBox(wxString::Format("File doesn't exist or otherwise can't load file '%s'", filePath), "Audio file loading error!", wxOK | wxICON_ERROR);
		return false;
	}
	std::vector<uint8_t> fileData;
	file.unsetf(std::ios::skipws);
	file.seekg(0, std::ios::end);
	size_t length = file.tellg();
	file.seekg(0, std::ios::beg);
	// allocate size
	fileData.resize(length);
	file.read(reinterpret_cast<char*> (fileData.data()), length);
	file.close();
	if (file.gcount() != length) {
		wxMessageBox(wxString::Format("Couldn't read entire file '%s'", filePath), "Audio file loading error!", wxOK | wxICON_ERROR);
		return false;
	}
	std::string header(fileData.begin(), fileData.begin() + 4);
    if (header == "RIFF") {
        bool extractOk = extractWaveData(fileData, channels, sampleRate, bitsPerSample, storageLocation, audioBufferSize);
        return extractOk;
    }
    else if (header == "FORM") {
        bool extractOk = extractAiffData(fileData, channels, sampleRate, bitsPerSample, storageLocation, audioBufferSize);
        return extractOk;
    }
	else {
		wxMessageBox(wxString::Format("Audio file format not supported. File: '%s'", filePath), "Audio file loading error!", wxOK | wxICON_ERROR);
		return false;
	}
}
bool AudioLoader::loadAudioFromMemory(const BYTE* source, size_t rawDataSize, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample, std::vector<uint8_t>& storageLocation, ALsizei& audioBufferSize) {
    std::vector<uint8_t> fileData;
    fileData.resize(rawDataSize);
    fileData.assign(source, source + rawDataSize);
    std::string header(fileData.begin(), fileData.begin() + 4);
    if (header == "RIFF") {
        bool extractOk = extractWaveData(fileData, channels, sampleRate, bitsPerSample, storageLocation, audioBufferSize);
        return extractOk;
    }
    else if (header == "FORM") {
        bool extractOk = extractAiffData(fileData, channels, sampleRate, bitsPerSample, storageLocation, audioBufferSize);
        return extractOk;
    }
    else {
        wxMessageBox("File format stored in memory location is not recongnized.", "Audio file loading error!", wxOK | wxICON_ERROR);
        return false;
    }
    return true;
}
bool AudioLoader::extractWaveData(std::vector<uint8_t>& fileData, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample, std::vector<uint8_t>& storageLocation, ALsizei& audioBufferSize) {

    // -----------------------------------------------------------
    // HEADER CHUNK
    std::string headerChunkID(fileData.begin(), fileData.begin() + 4);
    //int32_t fileSizeInBytes = fourBytesToInt (fileData, 4) + 8;
    std::string format(fileData.begin() + 8, fileData.begin() + 12);

    // -----------------------------------------------------------
    // try and find the start points of key chunks
    int indexOfDataChunk = getIndexOfChunk(fileData, "data", 12);
    int indexOfFormatChunk = getIndexOfChunk(fileData, "fmt ", 12);
    int indexOfXMLChunk = getIndexOfChunk(fileData, "iXML", 12);

    // if we can't find the data or format chunks, or the IDs/formats don't seem to be as expected
    // then it is unlikely we'll able to read this file, so abort
    if (indexOfDataChunk == -1 || indexOfFormatChunk == -1 || headerChunkID != "RIFF" || format != "WAVE"){
        wxMessageBox("File data does not seem to be consistent .WAV format.", "Audio file loading error!", wxOK | wxICON_ERROR);
        return false;
    }

    // -----------------------------------------------------------
    // FORMAT CHUNK
    int f = indexOfFormatChunk;
    std::string formatChunkID(fileData.begin() + f, fileData.begin() + f + 4);
    //int32_t formatChunkSize = fourBytesToInt (fileData, f + 4);
    uint16_t audioFormat = twoBytesToInt(fileData, f + 8);
    uint16_t numChannels = twoBytesToInt(fileData, f + 10);
    uint32_t _sampleRate = (uint32_t)fourBytesToInt(fileData, f + 12);
    uint32_t numBytesPerSecond = fourBytesToInt(fileData, f + 16);
    uint16_t numBytesPerBlock = twoBytesToInt(fileData, f + 20);
    int bitDepth = (int)twoBytesToInt(fileData, f + 22);

    uint16_t numBytesPerSample = static_cast<uint16_t> (bitDepth) / 8;

    // check that the audio format is PCM or Float or extensible
    if (audioFormat != WavAudioFormat::PCM && audioFormat != WavAudioFormat::IEEEFloat && audioFormat != WavAudioFormat::Extensible){
        wxMessageBox("File data does not seem to be consistent .WAV format.", "Audio file loading error!", wxOK | wxICON_ERROR);
        return false;
    }

    // check the number of channels is mono or stereo
    if (numChannels < 1 || numChannels > 128)
    {
        wxMessageBox("This WAV file seems to contain invalid number of channels (or corrupted?)", "Audio file loading error!", wxOK | wxICON_ERROR);
        return false;
    }

    // check header data is consistent
    if (numBytesPerSecond != static_cast<uint32_t> ((numChannels * _sampleRate * bitDepth) / 8) || numBytesPerBlock != (numChannels * numBytesPerSample)){
        wxMessageBox("The header data in this WAV file seems to be inconsistent with .WAV format.", "Audio file loading error!", wxOK | wxICON_ERROR);
        return false;
    }

    // check bit depth is either 8, 16, 24 or 32 bit
    if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24 && bitDepth != 32){
        wxMessageBox("This file has a bit depth that is not 8, 16, 24 or 32 bits.", "Audio file loading error!", wxOK | wxICON_ERROR);
        return false;
    }
    // -----------------------------------------------------------
    // DATA CHUNK
    int d = indexOfDataChunk;
    std::string dataChunkID(fileData.begin() + d, fileData.begin() + d + 4);
    int32_t dataChunkSize = fourBytesToInt(fileData, d + 4);
    int numSamples = dataChunkSize / (numChannels * bitDepth / 8);
    int samplesStartIndex = indexOfDataChunk + 8;
    sampleRate = _sampleRate;
    channels = numChannels;
    bitsPerSample = bitDepth;
    audioBufferSize = dataChunkSize;
    storageLocation.assign(fileData.begin() + samplesStartIndex, fileData.end());
    return true;
}
bool AudioLoader::extractAiffData(std::vector<uint8_t>& fileData, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample, std::vector<uint8_t>& storageLocation, ALsizei& audioBufferSize) {
    // -----------------------------------------------------------
    // HEADER CHUNK
    std::string headerChunkID(fileData.begin(), fileData.begin() + 4);
    //int32_t fileSizeInBytes = fourBytesToInt (fileData, 4, Endianness::BigEndian) + 8;
    std::string format(fileData.begin() + 8, fileData.begin() + 12);

    int audioFormat = format == "AIFF" ? AIFFAudioFormat::Uncompressed : format == "AIFC" ? AIFFAudioFormat::Compressed : AIFFAudioFormat::Error;

    // -----------------------------------------------------------
    // try and find the start points of key chunks
    int indexOfCommChunk = getIndexOfChunk(fileData, "COMM", 12, Endianness::BigEndian);
    int indexOfSoundDataChunk = getIndexOfChunk(fileData, "SSND", 12, Endianness::BigEndian);
    int indexOfXMLChunk = getIndexOfChunk(fileData, "iXML", 12, Endianness::BigEndian);

    // if we can't find the data or format chunks, or the IDs/formats don't seem to be as expected
    // then it is unlikely we'll able to read this file, so abort
    if (indexOfSoundDataChunk == -1 || indexOfCommChunk == -1 || headerChunkID != "FORM" || audioFormat == AIFFAudioFormat::Error){
        wxMessageBox("The file data does not seem to be consistent AIFF format.", "Audio file loading error!", wxOK | wxICON_ERROR);
        return false;
    }

    // -----------------------------------------------------------
    // COMM CHUNK
    int p = indexOfCommChunk;
    std::string commChunkID(fileData.begin() + p, fileData.begin() + p + 4);
    //int32_t commChunkSize = fourBytesToInt (fileData, p + 4, Endianness::BigEndian);
    int16_t numChannels = twoBytesToInt(fileData, p + 8, Endianness::BigEndian);
    int32_t numSamplesPerChannel = fourBytesToInt(fileData, p + 10, Endianness::BigEndian);
    int bitDepth = (int)twoBytesToInt(fileData, p + 14, Endianness::BigEndian);
    uint32_t _sampleRate = getAiffSampleRate(fileData, p + 16);

    // check the sample rate was properly decoded
    if (_sampleRate == 0){
        wxMessageBox("The AIFF file data has an unsupported sample rate.", "Audio file loading error!", wxOK | wxICON_ERROR);
        return false;
    }

    // check the number of channels is mono or stereo
    if (numChannels < 1 || numChannels > 2){
        wxMessageBox("The AIFF file data seems to be neither mono nor stereo (perhaps multi-track, or corrupted?)", "Audio file loading error!", wxOK | wxICON_ERROR);
        return false;
    }

    // check bit depth is either 8, 16, 24 or 32-bit
    if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24 && bitDepth != 32){
        wxMessageBox("The file data has a bit depth that is not 8, 16, 24 or 32 bits.", "Audio file loading error!", wxOK | wxICON_ERROR);
        return false;
    }

    // -----------------------------------------------------------
    // SSND CHUNK
    int s = indexOfSoundDataChunk;
    std::string soundDataChunkID(fileData.begin() + s, fileData.begin() + s + 4);
    int32_t soundDataChunkSize = fourBytesToInt(fileData, s + 4, Endianness::BigEndian);
    int32_t offset = fourBytesToInt(fileData, s + 8, Endianness::BigEndian);

    int numBytesPerSample = bitDepth / 8;
    int numBytesPerFrame = numBytesPerSample * numChannels;
    int totalNumAudioSampleBytes = numSamplesPerChannel * numBytesPerFrame;
    int samplesStartIndex = s + 16 + (int)offset;

    // sanity check the data
    if ((soundDataChunkSize - 8) != totalNumAudioSampleBytes || totalNumAudioSampleBytes > static_cast<long>(fileData.size() - samplesStartIndex)){
        wxMessageBox("The metadata for this file data doesn't seem right.", "Audio file loading error!", wxOK | wxICON_ERROR);
        return false;
    }
    sampleRate = _sampleRate;
    channels = numChannels;
    bitsPerSample = bitDepth;
    audioBufferSize = soundDataChunkSize;
    storageLocation.assign(fileData.begin() + samplesStartIndex, fileData.end());
    return true;
}
AudioLoader::AudioLoader(){}
AudioLoader::~AudioLoader() {}