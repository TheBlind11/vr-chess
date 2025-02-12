#include "Logger.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

void FLogger::Log(const FString& Message)
{
	// Scegliamo un file di destinazione dentro la cartella "Saved"
	static const FString LogFilePath = FPaths::ProjectSavedDir() / TEXT("MyShippingLog.txt");

	// Creiamo la stringa finale (con timestamp e newline)
	const FString FinalString = FString::Printf(
		TEXT("[%s] %s%s"),
		*FDateTime::Now().ToString(),
		*Message,
		LINE_TERMINATOR
	);

	// Salviamo in append
	FFileHelper::SaveStringToFile(
		FinalString,
		*LogFilePath,
		FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(),
		EFileWrite::FILEWRITE_Append
	);
}