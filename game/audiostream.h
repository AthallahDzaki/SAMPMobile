#pragma once

#define UNKNOW_SIZE 256

class CAudioStream
{
public:
	// Fields
	char filter_CAudioStream[UNKNOW_SIZE];
	
public:
	CAudioStream();
	
	void Initialize();
	void Play(char const *, float, float, float, float, bool);
	void Stop(bool);
	void Process();
};