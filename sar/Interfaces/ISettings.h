#pragma once

/// base for settings data...
interface ISettingsStream
{
};

/// base interface for settings managers...
interface ISettings
{
	virtual bool Serialize(ISettingsStream *pStream, DWORD & ObjectSize) = NULL;
	virtual bool Deserialize(DWORD & ObjectSize) = NULL;
};