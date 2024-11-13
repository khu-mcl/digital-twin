//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "Runtime/Online/HTTP/Public/Http.h"
#include "Interfaces/IHttpRequest.h"

#include "Messages/WebsocketMessage.h"

#include "WebSocket.h"

#include "SimpleSignalR.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewSignalRValues, FWebsocketMessage, Response);

UCLASS(Blueprintable)
class ADTLINK_API USimpleSignalR : public UWebsocket
{
	GENERATED_BODY()

public:
	/*
	*	Create a new SignalR client
	*	@param NegotiateFunctionUrl - SignalR negotiation url
	*	@return The SignalR client established
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create SignalR Client", Keywords = "ADT websocket Azure Digital Twin"), Category = "ADT SignalR")
		static USimpleSignalR* CreateSignalRClient(FString NegotiateFunctionUrl);

	UPROPERTY(BlueprintAssignable)
		FNewSignalRValues OnNewSignalRValues;

private:

	FString NegotiateFuctionUrl;

	bool SignalRNegotiateSuccess;
	FString SignalRAccessToken;
	FString SignalRUrl;
	FString SignalRConnetionId;


	/*
	*	HTTP request used by SignalR to Authenticate connection
	*	@param Url - URL to send message to
	*	@param Method - Method to use (post, get, put...)
	*	@param Headers - Optional headers to include
	*	@param Payload - payload to send
	*/
	void HttpRequest(FString Url, FString Method, TMap<FString, FString> Headers, FString Payload);

	/*
	*	Negotiation call response trigger
	*	@param HttpRequest - Request data
	*	@param HttpResponse - Response data
	*	@param bSucceeded - Was the request successful
	*/
	void HandleNegotiationResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	/*
	*	New message received trigger
	*	@param Message - Message received
	*/
	virtual void MessageHandle(const FString& Message) override;

	/*
	*	New raw message received trigger
	*	@param Data - raw message data
	*	@param Size - size of message
	*	@param BytesRemaining -
	*/
	virtual void RawMessageHandle(const void* Data, SIZE_T Size, SIZE_T BytesRemaining) override;

	/*
	*	New message sent trigger
	*	@param MessageString - The message sent
	*/
	virtual void MessageSentHandle(const FString& MessageString) override;

	/*
	*	Connection error trigger
	*	@param ErrorMsg - Error message
	*/
	virtual void ConnectionErrorHandle(const FString& ErrorMsg) override;

	/*
	*	Clone connection trigger
	*	@param StatusCode - status code for closing websocket
	*	@param Reason - Reason in text why connection was closed
	*	@param bWasClean - Was it a clean closing of connection
	*/
	virtual void ClosedHandle(int32 StatusCode, const FString& Reason, bool bWasClean) override;

	/*
	*	Connection established trigger
	*/
	virtual void ConnectHandler() override;

	/*
	*	Send a Handshake request to establish connection
	*/
	void HandshakeRequest();

	/*
	*	Parse SignalR message to usable data and use delegate to update blueprint with the new data
	*	@param JSONObject - JSON data object to parse
	*	@param Message - Full message received
	*/
	void handleSignalRMessage(TSharedPtr<FJsonObject> JSONObject, FString Message);
};
