//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "DTDLProperty.h"
#include "DTDLRelationship.h"

#include "DTDLInterface.generated.h"

/*
*	DTDL Interface struct.
*	https://github.com/Azure/opendigitaltwins-DTDL/blob/master/DTDL/v2/DTDLv2.md#property
*
*	_ indicates required.
*
*/
USTRUCT(BlueprintType)
struct FDTDLInterface
{
	GENERATED_USTRUCT_BODY()

public:

	/*
	*	Required
	*	A digital twin model identifier for the interface
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DTDL")
		FString _Id;

	/*
	*	Ignore: This must be "Interface",
	*/
	//	FString _Type;

	/*
	*	Ignore: This must be "dtmi:dtdl:context;2"
	*	The context to use when processing this interface. For this version, it must be set to "dtmi:dtdl:context;2"
	*/
	//	FString _Context;

	/*
	*	Optional
	*	A comment for model authors
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DTDL")
		FString Comment;

	/*
	*   Not implemented
	*	A set of objects that define the contents (Telemetry, Properties, Commands, Relationships, and/or Components) of this interface
	*/
	//UPROPERTY(BlueprintReadWrite, Category = "ADT")
	//	FString Contents;

	/*
	*	Optional
	*	A localizable description for display
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DTDL")
		TMap<FString, FString> Description;

	/*
	*	Optional
	*	A localizable name for display
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DTDL")
		TMap<FString, FString> DisplayName;

	/*
	*	Optional - It added to blueprint with parent class with AdtTwinComponent, this will be set to parent Id.
	*	A set of DTMIs that refer to interfaces this interface inherits from. Interfaces can inherit from multiple interfaces.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DTDL")
		FString Extends;

	/*
	*   Not implemented
	*	A set of IRIs or objects that refer to the reusable schemas within this interface.
	*/
	//	FString Schemas;

	/*
	*	Ignore.
	*	Full JSON string of the interface, generated on upload or download from ADT.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DTDL")
		FString JSON;
};