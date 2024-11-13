//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#include "WebSocket.h"

void UWebsocket::BeginDestroy() {
	Super::BeginDestroy();

	//Close connection if it is active when class is destroyed
	if (CheckWebsocketConnection()) {
		CloseConnection();
	}
}

UWebsocket* UWebsocket::CreateWebsocketClient(FString Url, FString Protocol, TMap<FString, FString> UpgradeHeaders) {

	//Create a new Websocket client and return it so it is accessible in blueprint
	UWebsocket* newConnection = NewObject<UWebsocket>();
	newConnection->ConnectWebsocket(Url, Protocol, UpgradeHeaders);
	return newConnection;
}

void UWebsocket::ConnectWebsocket(FString InputUrl, FString InputProtocol, TMap<FString, FString> InputUpgradeHeaders) {

	//Save arguments as variables
	this->Url = InputUrl;
	this->Protocol = InputProtocol;
	this->UpgradeHeaders = InputUpgradeHeaders;

	//Create websocket
	FModuleManager::Get().LoadModuleChecked(FName(TEXT("WebSockets")));
	TheWebSocket = FWebSocketsModule::Get().CreateWebSocket(InputUrl, InputProtocol, InputUpgradeHeaders);

	if (!TheWebSocket.IsValid()) {
		UE_LOG(LogClass, Error, TEXT("Websocket not valid"));
		return;
	}

	//Setup handlers
	TheWebSocket->OnConnected().AddUObject(this, &UWebsocket::ConnectHandler);
	TheWebSocket->OnConnectionError().AddUObject(this, &UWebsocket::ConnectionErrorHandle);
	TheWebSocket->OnMessage().AddUObject(this, &UWebsocket::MessageHandle);
	TheWebSocket->OnRawMessage().AddUObject(this, &UWebsocket::RawMessageHandle);
	TheWebSocket->OnMessageSent().AddUObject(this, &UWebsocket::MessageSentHandle);

	//Call Connect
	TheWebSocket->Connect();
}

void UWebsocket::ConnectHandler() {

	UE_LOG(LogClass, Log, TEXT("Websocket Connected"));
	OnConnectionChange.Broadcast(FWebsocketConnectChange(ConnectionType::Connect, ""));
}

void UWebsocket::ConnectionErrorHandle(const FString& ErrorMsg) {

	UE_LOG(LogClass, Error, TEXT("Websocket Connection failed %s"), *ErrorMsg);
	OnConnectionChange.Broadcast(FWebsocketConnectChange(ConnectionType::ConnectionError, ErrorMsg));
}

void UWebsocket::ClosedHandle(int32 StatusCode, const FString& Reason, bool bWasClean) {

	UE_LOG(LogClass, Log, TEXT("Websocket Session terminated, Reason %s"), *Reason);
	OnConnectionChange.Broadcast(FWebsocketConnectChange(ConnectionType::ClosedHandle, Reason));
}

void UWebsocket::MessageHandle(const FString& Message) {

	if (LogMessages) {
		UE_LOG(LogClass, Log, TEXT("Websocket Message received, %s"), *Message);
	}
	OnMessageRecived.Broadcast(Message);
}

void UWebsocket::RawMessageHandle(const void* Data, SIZE_T Size, SIZE_T BytesRemaining) {

	if (LogMessages) {
		//Assuming your Data is plain text 
		
		// Create a temporary array to store hexadecimal string representation
		TArray<TCHAR> HexString;
		// Each byte will be two characters + space
		HexString.Reserve(Size * 3); 

		const uint8* ByteData = reinterpret_cast<const uint8*>(Data);
		for (SIZE_T i = 0; i < Size; ++i) {
			HexString.Append(FString::Printf(TEXT("%02X "), ByteData[i]));
		}

		FString StringData = HexString.GetData();
		UE_LOG(LogClass, Log, TEXT("Websocket Binary Message received, %s"), *StringData);
	}
}

void UWebsocket::MessageSentHandle(const FString& MessageString) {

	if (LogMessages) {
		UE_LOG(LogClass, Log, TEXT("Websocket Message Sent, %s"), *MessageString);
	}
	OnMessageSent.Broadcast(MessageString);
}

void UWebsocket::SendWebsocketMessage(FString Message) {

	// Don't send if we're not connected.
	if (!TheWebSocket.IsValid() || !TheWebSocket->IsConnected())
	{
		UE_LOG(LogClass, Error, TEXT("Websocket Send failed, no connection established"));
		return;
	}

	TheWebSocket->Send(Message);
}

void UWebsocket::GetConnetionInfo(FString& UrlUsed, FString& ProtocolUsed, TMap<FString, FString>& UpgradeHeadersUsed) {

	UrlUsed = this->Url;
	ProtocolUsed = this->Protocol;
	UpgradeHeadersUsed = this->UpgradeHeaders;
}

bool UWebsocket::CheckWebsocketConnection() {

	bool isConnected = TheWebSocket.IsValid();
	UE_LOG(LogClass, Log, TEXT("Websocket isValied: %s"), TheWebSocket.IsValid() ? TEXT("True") : TEXT("False"));
	if (isConnected) {
		isConnected = TheWebSocket->IsConnected();
		UE_LOG(LogClass, Log, TEXT("Websocket connect: %s\n..."), TheWebSocket->IsConnected() ? TEXT("True") : TEXT("False"));
	}
	return isConnected;
}

void UWebsocket::CloseConnection() {

	UE_LOG(LogClass, Log, TEXT("Websocket connection closed"));
	TheWebSocket->Close(1000, "Manually closed connection");
}

void UWebsocket::SetLogMessages(bool WriteLogMessages) {

	this->LogMessages = WriteLogMessages;
}