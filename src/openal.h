#ifndef __OPENAL_H
#define __OPENAL_H

#include "utlvector.h"
#include "openal_sample.h"
#include "AL/al.h"
#include "AL/alc.h"


#define METERS_TO_VALVEUNITS(u) (valveUnitsPerMeter*(u)) // Converts meters to valve units
#define VALVEUNITS_TO_METERS(u) (0.01905f*(u)) // Converts valve units to meters

// @see http://developer.valvesoftware.com/wiki/Dimensions
const float baseRolloffFactor = 1.0f;
const float valveUnitsPerMeter = 1 / 0.01905;
const float valveSpeedOfSound = METERS_TO_VALVEUNITS(340.29);

#ifndef CLIENT_DLL
#error You don't need OpenAL on the server. Only compile it for the client.
#endif

typedef struct
{
	char* name; // The full-text name of this group
	CUtlVectorMT<CUtlVector<IOpenALSample*>> samples;
} openal_groupdata_t;

/***
 * An implementation of OpenAL in the SDK environment, since Source doesn't provide access
 * to a lot of the internal audio resources required for this project.
 *
 * @TODO Abstract Samples into "Sources" that represent the physical object in the engine
 *       that actually emits these sounds. This would allow multiple sounds from one source
 *       as the OpenAL supports, and would allow for more sounds to be possibly occuring
 *       simultaneously.
 ***/
class COpenALGameSystem : public CAutoGameSystemPerFrame
{
public:
	DECLARE_CLASS_GAMEROOT(COpenALGameSystem, CBaseGameSystemPerFrame);

	COpenALGameSystem();
	~COpenALGameSystem();

	bool Init();
	void Shutdown();
	void Update(float frametime);
	inline void UpdateListener(const float frametime);
	void UpdateSamples(const float updateTime);

	bool Add(IOpenALSample* sample);

	IOpenALSample* GetSample(char* filename);

	const char *Name() { return "OpenALGameSystem"; }

	void GetSoundPath(const char* relativePath, char* buffer, size_t bufferSize);

	/***
	 * Methods for working with groups.
	 ***/
	openal_groupdata_t* FindGroup(char* name);
	void RemoveSampleGroup(char* name);
	void AddSampleToGroup(char* groupName, IOpenALSample *sample);
	void RemoveSampleFromGroup(char* groupName, IOpenALSample *sample);

private:
	ALCcontext *m_alContext;
	ALCdevice  *m_alDevice;
	bool m_bInitialized;

	CUtlVectorMT<CUtlVector<IOpenALSample*>> m_vSamples;
	CUtlLinkedList<openal_groupdata_t*> m_AudioGroups;
	openal_groupdata_t* m_grpGlobal;
};

/***
 * Allows for OpenAL to play music during loading and any other time that Source might think
 * that it's too cool for school.
 ***/
class COpenALUpdateThread : public CWorkerThread
{
public:
	enum { CALL_FUNC, EXIT, };

	COpenALUpdateThread();
	~COpenALUpdateThread();

	int Run();

	bool Init();
	void OnExit();
};

extern COpenALGameSystem      g_OpenALGameSystem;

#endif
