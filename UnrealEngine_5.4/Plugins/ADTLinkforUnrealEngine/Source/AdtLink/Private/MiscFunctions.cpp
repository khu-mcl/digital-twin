//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#include "MiscFunctions.h"

UClass* UMiscFunctions::GetObjectParentClass(UObject* Object)
{
	return Object->GetClass()->GetSuperClass();
}

UClass* UMiscFunctions::GetParentClass(UClass* Class)
{
	return Class->GetSuperClass();
}


void UMiscFunctions::WriteToFile(FString FullPath, FString Text, bool AllowOverwrite, bool& Success) {


	if (!TestFilePath(FullPath, AllowOverwrite)) {
		Success = false;
		return;
	}
	Success = FFileHelper::SaveStringToFile(Text, *FullPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);

}

void UMiscFunctions::ReadFile(FString FullPath, FString& textResult, bool& Success) {

	if (!TestFilePath(FullPath, true)) {
		Success = false;
		return;
	}
	Success = FFileHelper::LoadFileToString(textResult, *FullPath);
}

void UMiscFunctions::ReadConfigFile(FString& HostUrl, FString& ClientId, FString& ClientSecret, FString& Tenant, FString& NegotiationUrl, FString& TsiPath, bool& Success) {

	//Read config file only available in editor
#if WITH_EDITOR
	//Get desktop platformModule to be able to open up dialog window
	void* ParentWindowPtr = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	if (DesktopPlatform)
	{
		//Oped dialog for user to choose location of config file.
		FString SaveDir = FPaths::ProjectDir();
		TArray<FString> OutFileName;
		uint32 SelectionFlag = 0; //0 represents single file, 1 represents multiple files
		DesktopPlatform->OpenFileDialog(ParentWindowPtr, "Read configuration file", SaveDir, FString(""), ".txt", SelectionFlag, OutFileName);

		//Read contents in file
		if (OutFileName.IsValidIndex(0)) {
			bool ReadFileSuccess;
			FString TextFile;
			ReadFile(OutFileName[0], TextFile, ReadFileSuccess);

			if (ReadFileSuccess) {

				TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(TextFile);
				TSharedPtr<FJsonObject> JSONObject;
				//Return if deserialize of config file contensts was not successful
				if (!FJsonSerializer::Deserialize(Reader, JSONObject)) {
					Success = false;
					UE_LOG(LogClass, Warning, TEXT("Could not deserialize JSON data in config file: %s"), *TextFile);
					return;
				}

				//Get config variables from file
				if (!JSONObject->TryGetStringField("adtHostName", HostUrl)) {
					Success = false;
					UE_LOG(LogClass, Warning, TEXT("Did not find ADT Host Name in config file: %s"), *TextFile);
					return;
				}
				if (!JSONObject->TryGetStringField("appId", ClientId)) {
					Success = false;
					UE_LOG(LogClass, Warning, TEXT("Did not find App Id/ Client Id Secret in config file: %s"), *TextFile);
					return;
				}
				if (!JSONObject->TryGetStringField("password", ClientSecret)) {
					Success = false;
					UE_LOG(LogClass, Warning, TEXT("Did not find Password/Client Secret in config file: %s"), *TextFile);
					return;
				}
				if (!JSONObject->TryGetStringField("tenant", Tenant)) {
					Success = false;
					UE_LOG(LogClass, Warning, TEXT("Did not find ADT Tenant in config file: %s"), *TextFile);
					return;
				}
				if (!JSONObject->TryGetStringField("signalRNegotiatePath", NegotiationUrl)) {
					Success = false;
					UE_LOG(LogClass, Warning, TEXT("Did not find SignalR Negotiation path in config file: %s"), *TextFile);
					return;
				}
				if (!JSONObject->TryGetStringField("tsiWebAppPath", TsiPath)) {
					Success = false;
					UE_LOG(LogClass, Warning, TEXT("Did not find TSI Path path in config file: %s"), *TextFile);
					return;
				}
			}
			else {
				UE_LOG(LogClass, Warning, TEXT("Could not Read file from path : %s"), *OutFileName[0]);
				Success = false;
				return;
			}
		}
		else {
			UE_LOG(LogClass, Warning, TEXT("No selected file in dialog window"));
			Success = false;
			return;
		}
	}
	else {
		UE_LOG(LogClass, Warning, TEXT("Failed to get FDesktopPlatformModule"));
		Success = false;
		return;
	}

	Success = true;
	return;
#endif
	UE_LOG(LogClass, Error, TEXT("Read config file only available in editor"));
	Success = false;
}


void UMiscFunctions::FindFilesInDirectory(FString Path, bool Recursive, TArray<FString> Extensions, TArray<FString>& Filenames, TArray<FString>& FilePaths) {

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	TArray<FString> Files;

	//If no extensions add an empty string to array
	if (Extensions.Num() == 0) {
		Extensions.Add("");
	}

	for (FString Extension : Extensions) {
		TArray<FString> TempFiles;
		if (Recursive) {

			PlatformFile.FindFilesRecursively(TempFiles, *Path, *Extension);
		}
		else {
			PlatformFile.FindFiles(TempFiles, *Path, *Extension);
		}
		Files.Append(TempFiles);
	}

	//Set values in "return" arrays
	for (FString pathName : Files) {
		Filenames.Add(FPaths::GetCleanFilename(pathName));
	}
	FilePaths.Append(Files);

}

bool UMiscFunctions::TestFilePath(FString Path, bool Overwrite) {

	if (!Overwrite && FPaths::FileExists(Path))
	{
		UE_LOG(LogClass, Error, TEXT("File to create already exists : %s"), *Path);
		return false;
	}
	FString Directory = FPaths::GetPath(Path);
	Directory.RemoveFromEnd("/");
	return TestDirectoryPath(Directory);

}

bool UMiscFunctions::TestDirectoryPath(FString Path) {

	if (!FPaths::DirectoryExists(Path))
	{
		UE_LOG(LogClass, Error, TEXT("Directory does not exist : %s"), *Path);
		return false;
	}
	return true;

}

void UMiscFunctions::CreateDirectory(FString FullPath, bool CreateTree, bool& Success) {

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!CreateTree) {
		Success = PlatformFile.CreateDirectory(*FullPath);
	}
	else {
		Success = PlatformFile.CreateDirectoryTree(*FullPath);
	}
}

void UMiscFunctions::GetModelToAssetsCSVFormat(TArray<FString> RowNames, TArray<FString> AssetNames, TArray<FString> AssetPaths, FString& CSVString, bool& Success) {

	int NrOfRows = RowNames.Num();
	if (AssetNames.Num() != NrOfRows || AssetPaths.Num() != NrOfRows) {
		Success = false;
		UE_LOG(LogTemp, Error, TEXT("Failed to get CSV String from ModelToAssets Arrays, all argument arrays need to be the same size"));
	}

	FString FullString = "---,AssetName,AssetPath\n";
	for (int i = 0; i < RowNames.Num(); i++)
	{

		FString RowString = "\"" + RowNames[i] + "\",";
		RowString += "\"" + AssetNames[i] + "\",";
		RowString += "\"" + AssetPaths[i] + "\"";

		FullString += RowString + "\n";
	}

	CSVString = FullString;
	Success = true;


}