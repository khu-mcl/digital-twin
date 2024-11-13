//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once
#include "HttpBaseRequest.h"

#include "HttpAdtRequest.generated.h"

//Type of message sent
UENUM()
enum CallType
{
	//internal type
	Redo = 0,

	//Twin - https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/twins
	RelationshipIncoming = 1,
	RelationshipOutgoing = 2,
	AddTwin = 3,
	UpdateTwin = 4,
	DeleteTwin = 5,
	AddRelationship = 6,
	DeleteRelationship = 7,
	//Sending UpdateRelationship messages not implemented
	UpdateRelationship = 8,
	GetRelationshipById = 9,
	GetById = 10,

	//Sending Telemetry and Component messages not implemented
	SendTelemetry = 11,
	GetComponent = 12,
	UpdateComponent = 13,
	SendComponentTelemetry = 14,

	//Query - https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/query
	QueryTwins = 100,

	//Models - https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/models
	ListModels = 101,
	AddModel = 102,
	UpdateModel = 103,
	DeleteModel = 104,
	GetModelById = 105,
};

//ADT standard message request
USTRUCT(BlueprintType)
struct FHttpADTRequest : public FHttpBaseRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		FString AuthorizationToken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		TEnumAsByte<CallType> Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
		FString Host;
};