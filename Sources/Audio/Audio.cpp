#include "Audio.hpp"

#ifdef FL_BUILD_WINDOWS
#include <Windows.h>
#endif
#ifdef FL_BUILD_MACOS
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include "Helpers/FileSystem.hpp"
#include "Scenes/Scenes.hpp"

namespace fl
{
	Audio::Audio() :
		IModule(),
		m_alDevice(nullptr),
		m_alContext(nullptr)
	{
		m_alDevice = alcOpenDevice(nullptr);
		m_alContext = alcCreateContext(m_alDevice, nullptr);
		alcMakeContextCurrent(m_alContext);
	}

	Audio::~Audio()
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(m_alContext);
		alcCloseDevice(m_alDevice);
	}

	void Audio::Update()
	{
		ICamera *camera = Scenes::Get()->GetCamera();

		if (camera != nullptr)
		{
			// Listener position.
			alListener3f(AL_POSITION, camera->GetPosition()->m_x, camera->GetPosition()->m_y, camera->GetPosition()->m_z);

			// Listener velocity.
			alListener3f(AL_VELOCITY, camera->GetVelocity()->m_x, camera->GetVelocity()->m_y, camera->GetVelocity()->m_z);

			// Listener orientation.
			Vector3 *currentRay = camera->GetViewRay()->m_currentRay;
			ALfloat orientation[6] = {currentRay->m_x, currentRay->m_y, currentRay->m_z, 0.0f, 1.0f, 0.0f};

			alListenerfv(AL_ORIENTATION, orientation);
			ErrorAl(alGetError());
		}
	}

	std::string Audio::StringifyResultAl(const int &result)
	{
		switch (result)
		{
		case AL_NO_ERROR:
			return "Success";
		case AL_INVALID_NAME:
			return "Invalid name";
		case AL_INVALID_ENUM:
			return "Invalid enum";
		case AL_INVALID_VALUE:
			return "Invalid value";
		case AL_INVALID_OPERATION:
			return "Invalid operation";
		case AL_OUT_OF_MEMORY:
			return "A memory allocation failed";
		default:
			return "ERROR: UNKNOWN AL ERROR";
		}
	}

	void Audio::ErrorAl(const int &result)
	{
		if (result == AL_NO_ERROR)
		{
			return;
		}

		std::string failure = StringifyResultAl(result);

		fprintf(stderr, "OpenAL error: %s, %i\n", failure.c_str(), result);
#ifdef FL_BUILD_WINDOWS
		MessageBox(nullptr, "" + result, "OpenAL Error", 0);
#endif
		throw std::runtime_error("OpenAL runtime error.");
	}
}