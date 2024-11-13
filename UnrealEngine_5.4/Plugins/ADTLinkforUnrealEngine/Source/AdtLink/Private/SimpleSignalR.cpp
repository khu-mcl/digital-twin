//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#include "SimpleSignalR.h"

USimpleSignalR* USimpleSignalR::CreateSignalRClient(FString NegotiateFunctionUrl) {

	USimpleSignalR* newClient = NewObject<USimpleSignalR>();

	FString StringPayload;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&StringPayload);
	//"https://<functionApp>.azurewebsites.net/api/negotiate";
	newClient->NegotiateFuctionUrl = "https://" + NegotiateFunctionUrl + "/negotiate";
	newClient->HttpRequest(newClient->NegotiateFuctionUrl, "POST", {}, StringPayload);

	return newClient;
}

void USimpleSignalR::HttpRequest(FString HttpUrl, FString Method, TMap<FString, FString> Headers, FString Payload) {

	if (this->LogMessages) {
		UE_LOG(LogClass, Log, TEXT("SignalR Http request\n URL: %s \n Method: %s \n Payload: %s"), *HttpUrl, *Method, *Payload);
	}
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &USimpleSignalR::HandleNegotiationResponse);

	HttpRequest->SetURL(HttpUrl);
	HttpRequest->SetVerb(Method);
	HttpRequest->SetContentAsString(Payload);

	for (auto& Elem : Headers) {
		HttpRequest->SetHeader(Elem.Key, Elem.Value);
	}

	HttpRequest->ProcessRequest();
}

void USimpleSignalR::HandleNegotiationResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {

	SignalRNegotiateSuccess = false;

	if (HttpResponse.IsValid()) {
		FString ResponseContent = HttpResponse->GetContentAsString();
		if (this->LogMessages) {
			UE_LOG(LogClass, Log, TEXT("SignalR HTTP Response :\n %s"), *ResponseContent);
		}

		//Deserialize string to JSON object
		TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(ResponseContent);
		TSharedPtr<FJsonObject> JSON;
		bool SuccessDeserialize = FJsonSerializer::Deserialize(Reader, JSON);

		SignalRNegotiateSuccess = SuccessDeserialize;
		if (SuccessDeserialize && JSON->HasField("url") && JSON->HasField("accessToken")) {
			SignalRUrl = JSON->GetStringField("url");
			SignalRAccessToken = JSON->GetStringField("accessToken");

			SignalRNegotiateSuccess = true;

			// Send new negotiate request with
			FString SplitUrl;
			FString Hub;
			SignalRUrl.Split("?", &SplitUrl, &Hub);

			FString newSignalRNegotiateUrl = SplitUrl + "negotiate" + "?" + Hub;

			TMap<FString, FString> Headers;
			FString Authorization = "Bearer " + SignalRAccessToken;
			Headers.Add("Authorization", Authorization);

			USimpleSignalR::HttpRequest(newSignalRNegotiateUrl, "POST", Headers, "");
		}
		else if (SuccessDeserialize && JSON->HasField("connectionId")) {

			SignalRConnetionId = JSON->GetStringField("connectionId");

			FString WssUrl = SignalRUrl + "&id=" + SignalRConnetionId;
			WssUrl = WssUrl.Replace(TEXT("https"), TEXT("wss"), ESearchCase::IgnoreCase);

			TMap<FString, FString> Headers;
			FString Authorization = "Bearer " + SignalRAccessToken;
			Headers.Add("Authorization", Authorization);

			UE_LOG(LogClass, Log, TEXT("Websocket trying to Connect"));

			//Call parent function ConnectWebsocket in Websocket Class
			ConnectWebsocket(WssUrl, "wws", Headers);
		}
	}
}

void USimpleSignalR::MessageHandle(const FString& Message) {

	OnMessageRecived.Broadcast(Message);

	if (this->LogMessages) {
		UE_LOG(LogClass, Log, TEXT("Websocket Message revived:\n%s"), *Message);
	}

	if (Message.IsEmpty()) {
		return;
	}

	//Create JSON object from message with removed end character
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(Message.Replace(TEXT("\x1e"), TEXT("")));
	TSharedPtr<FJsonObject> JSONObject;
	bool SuccessDeserialize = FJsonSerializer::Deserialize(Reader, JSONObject);

	//Handle message
	if (SuccessDeserialize && JSONObject->HasField("type")) {
		handleSignalRMessage(JSONObject, Message);
	}
	else if(!SuccessDeserialize)
	{
		UE_LOG(LogClass, Error, TEXT("Failed to Deserialize message"));
	}
}

void USimpleSignalR::handleSignalRMessage(TSharedPtr<FJsonObject> JSONObject, FString Message) {

	int type = JSONObject->GetIntegerField("type");
	switch (type) {
	/*
	*   Invocation is the only type that is interesting because it triggers when a value is changed.
	*
	*	Type: Invocation includes
	*	invocationId - An optional String encoding the Invocation ID for a message.
	*	target - A String encoding the Target name, as expected by the Callee's Binder
	*	arguments - An Array containing arguments to apply to the method referred to in Target. This is a sequence of JSON Tokens, encoded as indicated below in the "JSON Payload Encoding" section
	*	streamIds - An optional Array of strings representing unique ids for streams coming from the Caller to the Callee and being consumed by the method referred to in Target.
	*/
	case Invocation: {
		FWebsocketMessage BreadcastMessage;
		BreadcastMessage.FullMessage = Message;

		bool TwinIdFound = false;
		bool ModelIdFound = false;

		for (auto Argument : JSONObject->GetArrayField("arguments")) {
			TSharedPtr < FJsonObject > ArgumentObject = Argument->AsObject();
			for (auto JSONValue = ArgumentObject->Values.CreateConstIterator(); JSONValue; ++JSONValue) {
				FString Name = (*JSONValue).Key;

				//Property is sent from Azure as a path, ex /Temperature. We are only interested in the name.
				if (Name.StartsWith("/")) {
					FString Left;
					FString Right;
					Name.Split("/", &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
					Name = Right;
				}
				if (Name.Equals("TwinId", ESearchCase::IgnoreCase)) {
					TwinIdFound = (*JSONValue).Value->TryGetString(BreadcastMessage.Id);
				}
				else if (Name.Equals("modelId", ESearchCase::IgnoreCase)) {
					ModelIdFound = (*JSONValue).Value->TryGetString(BreadcastMessage.Model);
				}
				else if ((*JSONValue).Value->Type == EJson::Number) {
					float NumberValue;
					(*JSONValue).Value->TryGetNumber(NumberValue);
					BreadcastMessage.NumberValues.Add(Name, NumberValue);
				}
				else if ((*JSONValue).Value->Type == EJson::Boolean) {
					bool BoolValue;
					(*JSONValue).Value->TryGetBool(BoolValue);
					BreadcastMessage.BoolValues.Add(Name, BoolValue);
				}
				else if ((*JSONValue).Value->Type == EJson::String) {
					FString StringValue;
					(*JSONValue).Value->TryGetString(StringValue);
					BreadcastMessage.StringValues.Add(Name, StringValue);
				}
			}
		}

		if (TwinIdFound && ModelIdFound)
		{
			BreadcastMessage.Success = true;
		}
		else
		{
			if (!TwinIdFound) {
				UE_LOG(LogClass, Error, TEXT("SignalR error, no twin Id found in message"));
			}
			if (!ModelIdFound) {
				UE_LOG(LogClass, Error, TEXT("SignalR error, no Model Id found in message"));
			}
			BreadcastMessage.Success = false;
		}

		OnNewSignalRValues.Broadcast(BreadcastMessage);
		break;
	}
	/*
	*	Type: StreamItem includes
	*	invocationId - A String encoding the Invocation ID for a message.
	*	item - A Token encoding the stream item (see "JSON Payload Encoding" for details).
	*/
	case StreamItem: {
		break;
	}
	/*
	*	Type: Completion includes
	*	invocationId - A String encoding the Invocation ID for a message.
	*	result - A Token encoding the result value (see "JSON Payload Encoding" for details). This field is ignored if error is present.
	*	error - A String encoding the error message.
	*/
	case Completion: {
		break;
	}
	/*
	*	Type: StreamInvocation includes
	*	invocationId - A String encoding the Invocation ID for a message.
	*	target - A String encoding the Target name, as expected by the Callee's Binder.
	*	arguments - An Array containing arguments to apply to the method referred to in Target. This is a sequence of JSON Tokens, encoded as indicated below in the "JSON Payload Encoding" section.
	*	streamIds - An optional Array of strings representing unique ids for streams coming from the Caller to the Callee and being consumed by the method referred to in Target.
	*/
	case StreamInvocation: {
		break;
	}
	/*
	*	Type: CancelInvocation includes
	*	invocationId - A String encoding the Invocation ID for a message.
	*/
	case CancelInvocation: {
		break;
	}
	/*
	*	Type: Ping includes
	*/
	case Ping: {
		break;
	}
	/*
	*	Type: Close includes
	*	error - An optional String encoding the error message.
	*	allowReconnect - An optional Boolean indicating to clients with automatic reconnects enabled that they should attempt to reconnect after receiving the message.
	*/
	case Close: {
		break;
	}
	}
}

void USimpleSignalR::RawMessageHandle(const void* Data, SIZE_T Size, SIZE_T BytesRemaining) {

}

void USimpleSignalR::MessageSentHandle(const FString& MessageString) {

	if (LogMessages) {
		UE_LOG(LogClass, Log, TEXT("Websocket message sent:\n%s"), *MessageString);
	}
}

void USimpleSignalR::ConnectionErrorHandle(const FString& ErrorMsg) {

	UE_LOG(LogClass, Log, TEXT("Websocket Connection Error:\n%s"), *ErrorMsg);
	OnConnectionChange.Broadcast(FWebsocketConnectChange(ConnectionType::ConnectionError, ErrorMsg));
}

void USimpleSignalR::ClosedHandle(int32 StatusCode, const FString& Reason, bool bWasClean) {

	UE_LOG(LogClass, Log, TEXT("Websocket Closed:\n%s"), *Reason);
	OnConnectionChange.Broadcast(FWebsocketConnectChange(ConnectionType::ClosedHandle, Reason));
}

void USimpleSignalR::ConnectHandler() {

	UE_LOG(LogClass, Log, TEXT("Websocket Connected"));
	OnConnectionChange.Broadcast(FWebsocketConnectChange(ConnectionType::Connect, ""));
	HandshakeRequest();
}

void USimpleSignalR::HandshakeRequest() {

	TSharedPtr<FJsonObject> JSONObject = MakeShareable(new FJsonObject());;
	JSONObject->SetStringField("protocol", "json");
	JSONObject->SetNumberField("version", 1);

	FString JSONMessage;
	FJsonSerializer::Serialize(JSONObject.ToSharedRef(), TJsonWriterFactory<>::Create(&JSONMessage));

	// '\u001e' is an important end of message indicator for SignalR.
	FString EndOfMessage = "\u001e";
	JSONMessage += EndOfMessage;
	FTCHARToUTF8 Converted(*JSONMessage);
	uint8* RawMessage = (uint8*)(Converted.Get());

	UE_LOG(LogClass, Log, TEXT("SignalR websocket handshake :\n %s"), *JSONMessage);
	TheWebSocket->Send(RawMessage, Converted.Length(), false);
}