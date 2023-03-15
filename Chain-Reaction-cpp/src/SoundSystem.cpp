#include "PrecompiledHeader.h"
#include "SoundSystem.h"

//********************** Initialization of SoundSystem static members ********************************

bool SoundSystem::_systemInitialized = false; // A boolean flag to indicate whether the audio system is sucessfully initialized.
ALCdevice* SoundSystem::_deviceHandle = nullptr; //Stores device handle for the default sound device. For multiple devices, a list is suitable.
std::string SoundSystem::_deviceName = ""; //Stores name of the currently used sound device.
ALCcontext* SoundSystem::_currentContext = nullptr; //Stores the current context. We only deal with a single context here. Each context has a uniquely assigned device. A device can have multiple contexts.
std::vector<ALuint> SoundSystem::_soundList = {}; //List of all audio bufferID that were created. A buffer data in the list is located on the device whose context was current at the time of upload. For a single device, a single list of ID suffices.
std::vector<SoundSource*> SoundSystem::_sourceList = {}; //List of all Sound Source objects that were created.

//*****************************************************************************************************

//************************************************ Source code of the member variables of SoundSystem class *****************

bool SoundSystem::initSystem() {
	std::string name(alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER)); //retrieves the name of the (default) device
	if (SoundSystem::_deviceHandle != nullptr) {
		if (SoundSystem::_currentContext != nullptr) {
			SoundSystem::_systemInitialized = true;
			return true;
		}
		else {
			ALCcontext* context = alcCreateContext(SoundSystem::_deviceHandle, NULL);
			if (context == NULL) {
				wxMessageBox(wxString::Format("Failed to create context on sound device '%s'", name), "Sound System Error!", wxOK | wxICON_ERROR);
				SoundSystem::_systemInitialized = false;
				return false;
			}
			alcMakeContextCurrent(context);
			SoundSystem::_currentContext = context;
			SoundSystem::_systemInitialized = true;
			return true;
		}
	}
	else {
		ALCdevice* device = alcOpenDevice(name.c_str());
		if (device) {
			ALCcontext* context = alcCreateContext(device, NULL);
			if (context == NULL) {
				wxMessageBox(wxString::Format("Failed to create context on sound device '%s'", name), "Sound System Error!", wxOK | wxICON_ERROR);
				SoundSystem::_systemInitialized = false;
				return false;
			}
			alcMakeContextCurrent(context);
			SoundSystem::_deviceHandle = device;
			SoundSystem::_deviceName.assign(name);
			SoundSystem::_currentContext = context;
			SoundSystem::_systemInitialized = true;
			return true;
		}
		else {
			wxMessageBox(wxString::Format("Failed to initialize default sound device '%s'.", name), "Sound System Error!", wxOK | wxICON_ERROR);
			SoundSystem::_systemInitialized = false;
			return false;
		}
	}
}
bool SoundSystem::shutDown(){
	//************ Delete all sources *************************************************
	auto sourceIterator = SoundSystem::_sourceList.begin();
	while (sourceIterator != SoundSystem::_sourceList.end()) {
		delete *sourceIterator;
		++sourceIterator;
	}
	SoundSystem::_sourceList.clear();
	//************ Delete all uploaded sound buffer before closing sound device **********
	auto bufferIterator = SoundSystem::_soundList.begin();
	while (bufferIterator != SoundSystem::_soundList.end()) {
		ALuint buffer = *bufferIterator;
		bool isBuffer = alIsBuffer(buffer);
		if (buffer && isBuffer) {
			alDeleteBuffers(1, &buffer);
			std::string errorString = checkError(alGetError());
			if (errorString != "AL_NO_ERROR") {
				wxMessageBox(wxString::Format("'%s' error occured during deletion of sound buffer.", errorString), "Sound system error.", wxOK | wxICON_ERROR);
				++bufferIterator;
			}
			else {
				bufferIterator = SoundSystem::_soundList.erase(bufferIterator);
			}
		}
		else ++bufferIterator;
	}
	//*************************************************************************************
	//************************* Destroy context *******************
	if (SoundSystem::_currentContext != nullptr) {
		alcMakeContextCurrent(NULL);
		alcDestroyContext(SoundSystem::_currentContext);
		SoundSystem::_currentContext = nullptr;
	}
	//************************************************************
	//************************* Close sound device ************************
	if (SoundSystem::_deviceHandle != nullptr) {
		ALCboolean isDeviceClosed = alcCloseDevice(_deviceHandle);
		if (isDeviceClosed == ALC_FALSE) return false;
		else {
			SoundSystem::_deviceHandle = nullptr;
			SoundSystem::_systemInitialized = false;
			return true;
		}
	}
	else {
		SoundSystem::_systemInitialized = false;
		return true;
	}
	//***********************************************************************
}
bool SoundSystem::setContextCurrent(ALCcontext* context) {
	if (context == nullptr) {
		ALboolean success = alcMakeContextCurrent(SoundSystem::_currentContext);
		if (success == AL_FALSE) {
			wxMessageBox("Could not set the current context.", "Sound System Error!", wxOK | wxICON_ERROR);
			return false;
		}
		return true;
	}
	else {
		ALboolean success = alcMakeContextCurrent(context);
		if (success == AL_FALSE) {
			wxMessageBox("Could not set the current context.", "Sound System Error!", wxOK | wxICON_ERROR);
			return false;
		}
		return true;
	}
}
ALuint SoundSystem::addAudioData(const char* fileName) {
	if (!SoundSystem::_systemInitialized) return NULL;
	uint8_t channels = 0;
	int32_t sampleRate = 0;
	uint8_t bitsPerSample = 0;
	ALsizei dataSize = 0;
	ALenum format;
	std::vector<uint8_t> dataContainer;
	bool loadedOk = AudioLoader::loadAudio(fileName, channels, sampleRate, bitsPerSample, dataContainer, dataSize);
	if (!loadedOk) {
		wxMessageBox(wxString::Format("Failed to load sound data from file '%s'", fileName), "Sound Loading Error!", wxOK | wxICON_ERROR);
		return NULL;
	}
	if (channels == 1 && bitsPerSample == 8)
		format = AL_FORMAT_MONO8;
	else if (channels == 1 && bitsPerSample == 16)
		format = AL_FORMAT_MONO16;
	else if (channels == 2 && bitsPerSample == 8)
		format = AL_FORMAT_STEREO8;
	else if (channels == 2 && bitsPerSample == 16)
		format = AL_FORMAT_STEREO16;
	else {
		wxMessageBox("Given file format is not supported by OpenAL.", "Sound Loading Error!", wxOK | wxICON_ERROR);
		return NULL;
	}
	ALuint buffer = SoundSystem::_uploadAudioData(&dataContainer[0], dataSize, format, sampleRate);
	dataContainer.clear();
	return buffer;
}
ALuint SoundSystem::addAudioDataFromMemory(const unsigned char* source, size_t size) {
	if (!SoundSystem::_systemInitialized) return NULL;
	uint8_t channels = 0;
	int32_t sampleRate = 0;
	uint8_t bitsPerSample = 0;
	ALsizei dataSize = 0;
	std::vector<uint8_t> dataContainer;
	bool loadedOk = AudioLoader::loadAudioFromMemory(source, size, channels, sampleRate, bitsPerSample, dataContainer, dataSize);
	if (!loadedOk) {
		wxMessageBox("Failed to load audio data from resource memory.", "Sound Loading Error!", wxOK | wxICON_ERROR);
		return NULL;
	}
	ALenum format;
	if (channels == 1 && bitsPerSample == 8)
		format = AL_FORMAT_MONO8;
	else if (channels == 1 && bitsPerSample == 16)
		format = AL_FORMAT_MONO16;
	else if (channels == 2 && bitsPerSample == 8)
		format = AL_FORMAT_STEREO8;
	else if (channels == 2 && bitsPerSample == 16)
		format = AL_FORMAT_STEREO16;
	else {
		wxMessageBox("Given file format is not supported by OpenAL.", "Sound Loading Error!", wxOK | wxICON_ERROR);
		return NULL;
	}
	ALuint buffer = SoundSystem::_uploadAudioData(&dataContainer[0], dataSize, format, sampleRate);
	dataContainer.clear();
	return buffer;
}
bool SoundSystem::removeAudioData(const ALuint& bufferID) {
	if (!SoundSystem::_systemInitialized) return NULL;
	auto iterator = SoundSystem::_soundList.begin();
	while (iterator != SoundSystem::_soundList.end()) {
		if (*iterator == bufferID) {
			bool isBuffer = alIsBuffer(bufferID);
			if (isBuffer) {
				alDeleteBuffers(1, &bufferID);
				std::string errorString = checkError(alGetError());
				if (errorString != "AL_NO_ERROR") {
					wxMessageBox(wxString::Format("'%s' error occured during deletion of sound buffer.", errorString), "Sound system error.", wxOK | wxICON_ERROR);
					return false;
				}
				else {
					iterator = SoundSystem::_soundList.erase(iterator);
					return true;
				}
			}
			else return false;
		}
		++iterator;
	}
	wxMessageBox(wxString::Format("Failed to locate audio buffer with ID %d", bufferID), "Audio buffer deletion error!", wxOK | wxICON_ERROR);
	return false;
}
ALuint SoundSystem::addAudioSource(ALfloat pitch, ALfloat gain, glm::vec3 position, glm::vec3 velocity, bool loopSound) {
	if (!SoundSystem::_systemInitialized) return NULL;
	SoundSource* source = new SoundSource(pitch, gain, position, velocity, loopSound);
	if (!source->_sourceID) delete source;
	SoundSystem::_sourceList.push_back(source);
	return source->_sourceID;
}
void SoundSystem::play(const ALuint& audioBufferID, const ALuint& sourceID) {
	if (!SoundSystem::_systemInitialized) return;
	bool isBuffer = alIsBuffer(audioBufferID);
	if (!isBuffer) {
		wxMessageBox(wxString::Format("Could not locate audio buffer with ID %d", audioBufferID), "Audio play error", wxOK | wxICON_ERROR);
		return;
	}
	auto sourceIterator = SoundSystem::_sourceList.begin();
	bool isSourceFound = false;
	while (sourceIterator != SoundSystem::_sourceList.end()) {
		SoundSource* source = *sourceIterator;
		if (source->_sourceID == sourceID) {
			if (source->_bufferID != audioBufferID) {
				alSourcei(source->_sourceID, AL_BUFFER, (ALint) audioBufferID);
				source->_bufferID = audioBufferID;
			}
			std::string errorString = checkError(alGetError());
			if (errorString != "AL_NO_ERROR") {
				wxMessageBox(wxString::Format("'%s' error occured during playing audio buffer with ID %d",errorString, audioBufferID), "Audio play error", wxOK | wxICON_ERROR);
				return;
			}
			isSourceFound = true;
			break;
		}
		++sourceIterator;
	}
	if (isSourceFound) {
		SoundSystem::setContextCurrent();
		alSourcePlay(sourceID);
		return;
	}
	else {
		wxMessageBox(wxString::Format("Could not locate audio source with ID %d", sourceID), "Audio source error", wxOK | wxICON_ERROR);
		return;
	}
}
ALuint SoundSystem::_uploadAudioData(const ALvoid* data, ALsizei dataSize, ALenum format, int32_t sampleRate) {
	ALuint buffer = 0;
	SoundSystem::setContextCurrent();
	alGenBuffers(1, &buffer);
	bool isBuffer = alIsBuffer(buffer);
	if (!(buffer && isBuffer)) {
		if (buffer) alDeleteBuffers(1, &buffer);
		wxMessageBox("Failed to generate audio buffer!", "Sound Loading Error!", wxOK | wxICON_ERROR);
		return NULL;
	}
	alBufferData(buffer, format, data, dataSize, sampleRate);
	isBuffer = alIsBuffer(buffer);
	if (!isBuffer) {
		if (buffer) alDeleteBuffers(1, &buffer);
		wxMessageBox("Failed to upload audio buffer!", "Sound Loading Error!", wxOK | wxICON_ERROR);
		return NULL;
	}
	ALenum error = alGetError();
	std::string errorString = checkError(error);
	if (errorString != "AL_NO_ERROR") {
		wxMessageBox(wxString::Format("'%s' error occured while uploading audio buffer.", errorString), "Sound system failure!", wxOK | wxICON_ERROR);
		if (buffer) alDeleteBuffers(1, &buffer);
		return NULL;
	}
	SoundSystem::_soundList.push_back(buffer);
	return buffer;
}
SoundSystem::SoundSystem(){
}
SoundSystem::~SoundSystem() {
	if (SoundSystem::_systemInitialized) {
		SoundSystem::shutDown();
		SoundSystem::_systemInitialized = false;
	}
}

//****************************************** Source code of the member variables of SoundStructure class ********************* 

SoundSource::SoundSource(ALfloat pitch, ALfloat gain, glm::vec3 position, glm::vec3 velocity, bool loopSound) : _sourceID(0), _bufferID(0) {
	_pitch = pitch;
	_gain = gain;
	_position = position;
	_velocity = velocity;
	_loopsound = loopSound;
	alGenSources(1, &_sourceID);
	bool isSource = alIsSource(_sourceID);
	if (!(_sourceID && isSource)) {
		if (_sourceID) {
			alDeleteSources(1, &_sourceID);
			_sourceID = 0;
		}
		wxMessageBox("Failed to create audio source in current context!", "Audio Source Error!", wxOK | wxICON_ERROR);
		return;
	}
	alSourcef(_sourceID, AL_PITCH, _pitch);
	alSourcef(_sourceID, AL_GAIN, _gain);
	alSource3f(_sourceID, AL_POSITION, _position.x, _position.y, _position.z);
	alSource3f(_sourceID, AL_VELOCITY, _velocity.x, _velocity.y, _velocity.z);
	alSourcei(_sourceID, AL_LOOPING, _loopsound);
	ALenum error = alGetError();
	std::string errorString = SoundSystem::checkError(error);
	if (errorString != "AL_NO_ERROR") {
		wxMessageBox(wxString::Format("'%s' error occured while creating audio source.", errorString), "Audio Source Error!", wxOK | wxICON_ERROR);
		if (_sourceID) {
			alDeleteSources(1, &_sourceID);
			_sourceID = 0;
		}
		return;
	}
}
SoundSource::~SoundSource() {
	if (_sourceID) {
		alDeleteSources(1, &_sourceID);
		_sourceID = 0;
	}
}

//********************************************************************************************************
