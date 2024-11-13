//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "WebsocketMessage.generated.h"

//Websocket Connection type
UENUM(BlueprintType)
enum ConnectionType {
	Connect = 0,
	ConnectionError = 1,
	ClosedHandle = 2
};

//Type of message received
UENUM(BlueprintType)
enum MessageType {
	Default = 0,
	Invocation = 1,
	StreamItem = 2,
	Completion = 3,
	StreamInvocation = 4,
	CancelInvocation = 5,
	Ping = 6,
	Close = 7
};

//
USTRUCT(BlueprintType)
struct FWebsocketConnectChange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Websocket")
		TEnumAsByte < ConnectionType > Type;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Websocket")
		FString Message;

	FWebsocketConnectChange()
	{

	}

	FWebsocketConnectChange(ConnectionType TheType, FString TheMessage) {
		Type = TheType;
		Message = TheMessage;
	}

};

//Returning websocket message, specifically for ADT
USTRUCT(BlueprintType)
struct FWebsocketMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Websocket")
		FString FullMessage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Websocket")
		FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Websocket")
		FString Model;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Websocket")
		TMap<FString, float> NumberValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Websocket")
		TMap<FString, bool> BoolValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Websocket")
		TMap<FString, FString> StringValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Websocket")
		bool Success;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Websocket")
		TEnumAsByte < MessageType > MessageType;
};