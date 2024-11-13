//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "DTDLProperty.h"
#include "DTDLRelationship.h"
#include "DTDLInterface.h"

#include "DTDLModel.generated.h"

/*
*	DTDL Model struct.
*
*	_ indicates required.
*/

USTRUCT(BlueprintType)
struct FDTDLModel
{
	GENERATED_USTRUCT_BODY()

public:

	/*
	*	Required
	*	Parsed Interface
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FDTDLInterface _Interface;


	/*
	*	Optional
	*	Parsed Content to find all property types
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		TMap<FString, FDTDLProperty> Propertys;

	/*
	*	Optional
	*	Parsed Content to find all relationships types
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		TMap<FString, FDTDLRelationship> Relationships;

	//TODO - Add map for DTDL Telemetry, Command, Component


	/*
	*	Ignore.
	*	Full JSON string of the model, generated on upload or download from ADT.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString JSON;
};