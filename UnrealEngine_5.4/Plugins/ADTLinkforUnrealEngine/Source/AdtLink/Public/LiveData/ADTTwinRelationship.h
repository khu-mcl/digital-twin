//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "ADTTwinRelationship.generated.h"

/*
*	DTDL Property struct.
*	https://github.com/Azure/opendigitaltwins-DTDL/blob/master/DTDL/v2/DTDLv2.md#property
*
*	_ indicates required.
*
*/
USTRUCT(BlueprintType)
struct FADTTwinRelationship
{
	GENERATED_USTRUCT_BODY()

public:

	/*
	*	Type: both incoming and outgoing
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString _RelationshipId;


	/*
	*	Type: both incoming and outgoing
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString _RelationshipName;

	//
	/*
	*	Type: both incoming and outgoing
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString _SourceId;

	//
	/*
	*	Type: Only outgoing
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString TargetId;

	//
	/*
	*	Type: Only outgoing
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString Etag;

	//
	/*
	*	Type: Only outgoing
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString RelationshipProperty;

	//
	/*
	*	Type: Only incoming
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString RelationshipLink;

	/*
	*	Type: incoming or outgoing
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		bool _IsOutgoingType;

	/*
	* Full JSON Relationship
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADT")
		FString JSON;
};