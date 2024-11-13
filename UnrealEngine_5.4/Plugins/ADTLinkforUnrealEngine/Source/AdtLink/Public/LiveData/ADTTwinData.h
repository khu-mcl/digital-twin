//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "ADTTwinData.generated.h"

/*
*	DTDL Property struct.
*	https://github.com/Azure/opendigitaltwins-DTDL/blob/master/DTDL/v2/DTDLv2.md#property
*
*	_ indicates required.
*
*/
USTRUCT(BlueprintType)
struct FAdtTwinData
{
	GENERATED_USTRUCT_BODY()

public:

	/*
	*	Id of the twin data belongs to
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString TwinId;

	/*
	*	Id of the model the twin is created from
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString ModelId;

	/*
	*	Current live data in string format.
	*	Can be parsed to specific DTDL Schema type.
	*	Key - Property id
	*	Value - Property String Value
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		TMap<FString, FString> StringValues;

	/*
	*	Current live data in bool format.
	*	Can be parsed to specific DTDL Schema type.
	*	Key - Property id
	*	Value - Property Bool Value
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		TMap<FString, bool> BoolValues;

	/*
	*	Current live data in float format.
	*	Can be parsed to specific DTDL Schema type.
	*	Key - Property id
	*	Value - Property Number Value
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		TMap<FString, float> NumberValues;

	/*
	*	Full message in JSON format
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString JSON;
};