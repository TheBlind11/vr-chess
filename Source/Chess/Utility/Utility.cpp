#include "Utility.h"

#include "Kismet/GameplayStatics.h"

//Utility to convert JasonObject to String
FString Utility::JsonToString(const FJsonObject& iJsonObject)
{
	FString jsonString;
	using FCondensedJsonWriter = TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>;
	TSharedRef<FCondensedJsonWriter> jsonStringWriter = TJsonWriterFactory<
		TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&jsonString);
	FJsonSerializer::Serialize(MakeShareable(new FJsonObject(iJsonObject)), jsonStringWriter);
	return jsonString;
}

//Utility to find an object in the scene by its tag
AActor* Utility::FindByTag(UWorld* iWorld, FName iTag)
{
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsWithTag(iWorld, iTag, AllActors);
	if (AllActors.Num() > 0)
	{
		if (AllActors[0])
		{
			return (AllActors[0]);
		}
	}

	return nullptr;
}
