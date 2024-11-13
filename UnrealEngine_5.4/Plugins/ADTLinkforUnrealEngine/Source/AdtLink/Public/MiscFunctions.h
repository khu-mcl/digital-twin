//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFilemanager.h"
#include "Framework/Application/SlateApplication.h"

#if WITH_EDITOR
#include "IDesktopPlatform.h"
#include "DesktopPlatformModule.h"
#endif

#include "MiscFunctions.generated.h"

/**
 *
 */
UCLASS()
class ADTLINK_API UMiscFunctions : public UObject
{
	GENERATED_BODY()

public:

	//Get expose get parent to blueprint

	/*
	*	Get parent class from object
	*	@param Object - Object to find parent from
	*	@return The parent class
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Parent Class from Object", Keywords = "ADT Misc parent class Azure Digital Twin"), Category = "ADT Misc")
		static UClass* GetObjectParentClass(UObject* Object);

	/*
	*	Get parent class from a class
	*	@param Class - Class to find parent from
	*	@return  parent class
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Parent Class from Class", Keywords = "ADT Misc parent object Azure Digital Twin"), Category = "ADT Misc")
		static UClass* GetParentClass(UClass* Class);

	//File manager

	/*
	*	Write a file to disk
	*	@param FullPath - Full path on disk where file should be create/written
	*	@param Text - Text to write
	*	@param AllowOverwrite - Allow overwrite if file already exist
	*	@param Success - True if text was written to file
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write file to disk", Keywords = "ADT Misc write file Azure Digital Twin"), Category = "ADT Misc")
		static void WriteToFile(FString FullPath, FString Text, bool AllowOverwrite, bool& Success);

	/*
	*	Read a file from disk
	*	@param FullPath - Full path on disk where file should be read
	*	@param textResult - text read from file
	*	@param Success - True if text was read from file
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read file from disk", Keywords = "ADT Misc read file Azure Digital Twin"), Category = "ADT Misc")
		static void ReadFile(FString FullPath, FString& textResult, bool& Success);

	/*
	*	Read an ADT configuration JSON file from disk
	*	@param HostUrl - Host URL found in file
	*	@param ClientId - Client Id found in file
	*	@param ClientSecret - Client Secret found in file
	*	@param Tenant - Tenant Id found in file
	*	@param NegotiationUrl - SignalR Negotiation URL found in file
	*	@param Success - True if text was read from file
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read Config File (Editor only)", Keywords = "ADT Misc read config file Azure Digital Twin"), Category = "ADT Misc")
		static void ReadConfigFile(FString& HostUrl, FString& ClientId, FString& ClientSecret, FString& Tenant, FString& NegotiationUrl, FString& TsiPath, bool& Success);


	/*
	*	Iterate in directory for file names
	*	@param Path - Path to directory
	*	@param Recursive - Use recursive search to find files in lower directories
	*	@param Extensions - Optional to find only specific file types
	*	@param Filenames - Filenames found
	*	@param FilePaths - Paths to files found
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get filenames from Directory", Keywords = "ADT Misc File filename directory Azure Digital Twin"), Category = "ADT Misc")
		static void FindFilesInDirectory(FString Path, bool Recursive, TArray<FString> Extensions, TArray<FString>& Filenames, TArray<FString>& FilePaths);


	//Specific CSV functions


	/*
	*	Parse data in "Model to Asset" data table to CSV Format that works with the "Fill Data Table from String" in engine function
	*	@param RowNames - RowNames column from "Model to Asset" data
	*	@param AssetNames - AssetNames column from "Model to Asset" data
	*	@param AssetPaths - AssetPaths column from "Model to Asset" data
	*	@param CSVString - String with "Model to Asset" data in CSV format
	*	@param Success - True if parsing was successful
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Model To Assets CSV Format", Keywords = "CSV model to assets data table Azure Digital Twin"), Category = "ADT Misc")
		static void GetModelToAssetsCSVFormat(TArray<FString> RowNames, TArray<FString> AssetNames, TArray<FString> AssetPaths, FString& CSVString, bool& Success);



private:

	//File manager


	/*
	*	Test File path if valid
	*	@param Path - path to test
	*	@param Overwrite - Is overwrite allowed
	*	@return True if file path is valid
	*/
	static bool TestFilePath(FString Path, bool Overwrite);

	/*
	*	Test directory path if valid
	*	@param Path - path to test
	*	@return True if directory path is valid
	*/
	static bool TestDirectoryPath(FString Path);

	/*
	*	Create a directory on disk
	*	@param FullPath - path to create directory in
	*	@param CreateTree - Create a directory tree from path
	*	@param Success - True if creation was successful
	*/
	static void CreateDirectory(FString FullPath, bool CreateTree, bool& Success);

};
