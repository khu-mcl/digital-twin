//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "UObject/ObjectMacros.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "Messages/WebsocketMessage.h"

#include "Websocket.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageDelegate, FString, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageSentDelegate, FString, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectionDelegate, FWebsocketConnectChange, Response);

UCLASS(Blueprintable)
class ADTLINK_API UWebsocket : public UObject
{
	GENERATED_BODY()

protected:
	TSharedPtr<IWebSocket> TheWebSocket;

	/*
	*	Create a new connection to websocket
	*	@param InputUrl - URL to connect to
	*	@param InputProtocol - Protocol to use
	*	@param InputUpgradeHeaders - Headers to include
	*/
	void ConnectWebsocket(FString InputUrl, FString InputProtocol, TMap<FString, FString> InputUpgradeHeaders);

	/*
	*	On class begin destroyed
	*/
	virtual void BeginDestroy();

	/*
	*	Connection established trigger
	*/
	virtual void ConnectHandler();

	/*
	*	Connection error trigger
	*	@param ErrorMsg - Error message
	*/
	virtual void ConnectionErrorHandle(const FString& ErrorMsg);

	/*
	*	Clone connection trigger
	*	@param StatusCode - status code for closing websocket
	*	@param Reason - Reason in text why connection was closed
	*	@param bWasClean - Was it a clean closing of connection
	*/
	virtual void ClosedHandle(int32 StatusCode, const FString& Reason, bool bWasClean);

	/*
	*	New message received trigger
	*	@param Message - Message received
	*/
	virtual void MessageHandle(const FString& Message);

	/*
	*	New raw message received trigger
	*	@param Data - raw message data
	*	@param Size - size of message
	*	@param BytesRemaining -
	*/
	virtual void RawMessageHandle(const void* Data, SIZE_T Size, SIZE_T BytesRemaining);

	/*
	*	New message sent trigger
	*	@param MessageString - The message sent
	*/
	virtual void MessageSentHandle(const FString& MessageString);

	FString Url;
	FString Protocol;
	TMap<FString, FString> UpgradeHeaders;
	bool LogMessages = false;

public:

	UPROPERTY(BlueprintAssignable)
		FMessageDelegate OnMessageRecived;

	UPROPERTY(BlueprintAssignable)
		FMessageSentDelegate OnMessageSent;

	UPROPERTY(BlueprintAssignable)
		FConnectionDelegate OnConnectionChange;

	/*
	*	Create a new websocket client
	*	@param Url - URL to connect to
	*	@param Protocol - Protocol to use
	*	@param UpgradeHeaders - Headers to include
	*	@return The websocket client established
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create Websocket Client", Keywords = "ADT websocket Azure Digital Twin", AutoCreateRefTerm = "UpgradeHeaders"), Category = "ADT Websocket")
		static UWebsocket* CreateWebsocketClient(FString Url, FString Protocol, TMap<FString, FString> UpgradeHeaders);

	/*
	*	Send a new websocket message
	*	@param Message - The message to send
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Websocket Send", Keywords = "ADT websocket Azure Digital Twin"), Category = "ADT Websocket")
		void SendWebsocketMessage(FString Message);

	/*
	*	Get information about the current connection
	*	@param UrlUsed - The URL websocket is connected to
	*	@param ProtocolUsed - Protocol used on connection
	*	@param UpgradeHeadersUsed - All headers used on connection
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Connection Information", Keywords = "ADT websocket Azure Digital Twin"), Category = "ADT Websocket")
		void GetConnetionInfo(FString& UrlUsed, FString& ProtocolUsed, TMap<FString, FString>& UpgradeHeadersUsed);

	/*
	*	Check if the connection to the websocket is active
	*	@return True if connected
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Check Websocket connection", Keywords = "ADT websocket Azure Digital Twin"), Category = "ADT Websocket")
		bool CheckWebsocketConnection();

	/*
	*	Close the connection to the websocket
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close connection", Keywords = "ADT websocket Azure Digital Twin"), Category = "ADT Websocket")
		void CloseConnection();

	/*
	*	Activate or disable writing messages to the log
	*	@param WriteLogMessages - Model id of DTDL to delete
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write messages to the log", Keywords = "ADT websocket Azure Digital Twin"), Category = "ADT Websocket")
		void SetLogMessages(bool WriteLogMessages);
};
