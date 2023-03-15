#pragma once
#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H

#include "PrecompiledHeader.h"
#include "AudioLoader.h"

/**
* @brief A structure for storing configuration settings of an OpenAL audio source.
*/
typedef struct SoundSource {
	ALfloat _pitch, _gain;
	glm::vec3 _position, _velocity;
	bool _loopsound; //a boolean flag to tell the audio source if all audio buffer should be played in a loop or not.
	ALuint _sourceID; //stores the unique ID of the audio source.
	ALuint _bufferID; //stores the ID of most recently played audio buffer.
	/**
	* @brief Default constructor for the SoundSource structure.
	*/
	SoundSource(): _sourceID(0), _bufferID(0), _pitch(1.0f), _gain(1.0f), _position(0, 0, 0), _velocity(0, 0, 0), _loopsound(false) {};
	/**
	* @brief Default constructor for the SoundSource structure.
	*/
	SoundSource(ALfloat pitch = 1.0f, ALfloat gain = 1.0f, glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 velocity = glm::vec3(0, 0, 0), bool loopsound = false);
	/**
	* @brief Default destructor for the SoundSource structure. Handles deletion of the memories allocated while creating the OpenAL sound source.
	*/
	~SoundSource();
} SoundSource;
/**
* @brief A sound system class based on OpenAL api designed to work with a single audio device and a single OpenAL context.
*/
class SoundSystem{
public:
	/**
	* @brief A static method which initiates the default sound device of the system.
	* @returns true if audio system was successfully initialized, false otherwise.
	*/
	static bool initSystem();
	/*
	* @brief Returns the name of the currently used sound device.
	* @returns {std::string} name of the currently used sound device if the audio system is initialized, empty string otherwise.
	*/
	inline static std::string getDeviceName() { return SoundSystem::_deviceName; }
	/**
	* @brief A static method which shuts down (if already initialized) the default sound device of the system and free all audio resources (e.g., audio buffer etc.).
	* @returns true if audio system was successfully shut down and all memories allocated to audio resources were freed, false otherwise.
	*/
	static bool shutDown();
	static bool setContextCurrent(ALCcontext* context = nullptr);
	/**
	* @brief Uploads audio data from a file source to the device memory associated with the current OpenAL context.
	* @param {const char*} fileName: name of the file (supported audio format: .wav, .ogg, etc.) containing audio data. 
	* @returns {ALuint} location ID of the uploaded audio buffer which is stored in the audio device.
	*/
	static ALuint addAudioData(const char* fileName);
	/**
	* @brief Uploads audio buffer (supported audio format: .wav, .ogg, etc.) contained in `source` to the device memory associated with the current OpenAL context.
	* @param {const unsigned char*} source: pointer to the audio buffer that is to be uploaded.
	* @param {size_t} size: size of the buffer source (in byte length).
	* @returns {ALuint} location ID of the uploaded audio buffer which is stored in the audio device.
	*/
	static ALuint addAudioDataFromMemory(const unsigned char* source, size_t size);
	/**
	* @brief Deletes previously uploaded audio buffer from the audio device memory.
	* @param {const ALuint&} bufferID: location ID of the audio buffer that is to be deleted.
	* @returns {bool} true if the audio buffer at location `bufferID` was found and successfully deleted from memory, false otherwise.
	*/
	static bool removeAudioData(const ALuint& bufferID);
	/**
	* @brief Adds OpenAL audio source in the 3D world.
	* @param {ALfloat} pitch: pitch at which the audio source plays an audio buffer.
	* @param {ALfloat} gain pitch: the value by which the audio source amplifies an audio buffer before playing.
	* @param {glm::vec3} position: initial position of the audio source in 3D world.
	* @param {glm::vec3} velocity: velocity (in three directions) at which the audio source moves in the 3D world.
	* @param {bool} loopsound: a boolean flag to tell the audio source if all audio buffer should be played in a loop or not.
	* @returns {ALuint} location ID of the audio source in device memory.
	*/
	static ALuint addAudioSource(ALfloat pitch = 1.0f, ALfloat gain = 1.0f, glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 velocity = glm::vec3(0, 0, 0), bool loopsound = false);
	static inline std::string checkError(ALenum error) {
		std::string errorString = "AL_NO_ERROR";
		switch (error) {
		case AL_NO_ERROR: break;
		case AL_INVALID_ENUM:
			errorString.assign("AL_INVALID_ENUM");
			break;
		case AL_INVALID_OPERATION:
			errorString.assign("AL_INVALID_OPERATION");
			break;
		case AL_INVALID_VALUE:
			errorString.assign("AL_INVALID_VALUE");
			break;
		case AL_OUT_OF_MEMORY:
			errorString.assign("AL_OUT_OF_MEMORY");
			break;
		case ALC_INVALID_DEVICE:
			errorString.assign("ALC_INVALID_DEVICE");
			break;
		default:
			break;
		}
		return errorString;
	}
	/**
	* @brief Plays a previously uploaded audio buffer in an OpenAL audio source.
	* @param {const ALuint&} audioBufferID: location ID of the audio buffer that is to be played.
	* @param {const ALuint&} sourceID: {ALuint} location of the source which plays the audio buffer.
	*/
	static void play(const ALuint& audioBufferID, const ALuint& sourceID);
	~SoundSystem();
private:
	/**
	* @brief A sound system class constructor based on OpenAL api designed to work with a single audio device and a single OpenAL context.
	*/
	SoundSystem(); //The constructor is made private to prevent instantiating this class.
	static bool _systemInitialized; // A boolean flag to indicate whether the audio system is sucessfully initialized.
	static ALCdevice* _deviceHandle; //Stores device handle for the default sound device. For multiple devices, a list is suitable.
	static std::string _deviceName; //Stores name of the currently used sound device.
	static ALCcontext* _currentContext; //Stores the current context. We only deal with a single context here. Each context has a uniquely assigned device. A device can have multiple contexts.
	/**
	* @brief A method to upload audioBuffer in a sound device.
	* @param {const ALvoid*} data: pointer to the byte stream containing audio data.
	* @param {ALsizei} dataSize: size of the audio data in byte length.
	* @param {ALenum} format: audio format of the data.
	* @param {int32_t} sampleRate: sample rate of the audio data.
	* @returns {ALuint} location of the uploaded audio buffer in audio device.
	*/
	static ALuint _uploadAudioData(const ALvoid* data, ALsizei dataSize, ALenum format, int32_t sampleRate);
	/** @brief List of all sound sources that were created. The sources are attached to the context that was current at the time of creation.
	 * Therefore, for a single context, a single list is enough.
	*/
	static std::vector<SoundSource*> _sourceList;
	
	/** 
	 * @brief A vector which stores all location IDs of the audio buffer that were uploaded. The buffer data is typically located on the device whose context 
	 * was current at the time of uploading. Therefore, when working with a single device, a single list of ID suffices.
	 * @brief For multiple devices, a mapping between the device names and the list of uploaded audio buffer IDs is required.
	*/
	static std::vector<ALuint> _soundList;
};
#endif // !SOUND_SYSTEM_H
