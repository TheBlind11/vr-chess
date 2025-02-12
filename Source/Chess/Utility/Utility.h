#pragma once

class Utility
{
public:
	//Utility to convert JasonObject to String
	static FString JsonToString(const class FJsonObject& iJsonObject);

	//Utility to find an object in the scene by its tag
	static AActor* FindByTag(UWorld* iWorld, FName iTag);
};
