//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#include "HttpCommunication.h"

TMap<FString, FHttpADTRequest> UHttpCommunication::CallerList = TMap<FString, FHttpADTRequest>();

//----Client----

UHttpCommunication* UHttpCommunication::CreateADTHttpClient(FString TenantID, FString ClientID, FString ClientSecret, FString Host) {

	UHttpCommunication* Communicatior = NewObject<UHttpCommunication>();

	Communicatior->UpdatingToken = false;
	Communicatior->MyHost = Host;
	Communicatior->MyToken.TenantID = TenantID;
	Communicatior->MyToken.ClientID = ClientID;
	Communicatior->MyToken.ClientSecret = ClientSecret;

	return Communicatior;
}

UHttpCommunication* UHttpCommunication::CreateBaseHttpClient() {

	UHttpCommunication* Communicatior = NewObject<UHttpCommunication>();
	return Communicatior;
}

void UHttpCommunication::UpdateCredentials(FString TenantID, FString ClientID, FString ClientSecret, FString Host) {

	MyToken.ClientID = ClientID;
	MyToken.TenantID = TenantID;
	MyToken.ClientSecret = ClientSecret;
	MyHost = Host;
}

//----Token----

void UHttpCommunication::GenerateToken(bool& TokenRequestSent, FString& ErrorMessage) {

	if (MyToken.TenantID.IsEmpty() || MyToken.ClientID.IsEmpty() || MyToken.ClientSecret.IsEmpty()) {
		ErrorMessage = "Tenant ID, Client ID or Client Secret not set";
		UE_LOG(LogTemp, Error, TEXT("Generate Token Error: %s"), *ErrorMessage);
		TokenRequestSent = false;
		return;
	}

	//Microsoft login rest URL
	FString Url = "https://login.microsoftonline.com/" + MyToken.TenantID + "/oauth2/token";

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpCommunication::HandleGenerateTokenResponse);

	FString StringPayload = "grant_type=client_credentials";
	StringPayload.Append("&client_id=" + MyToken.ClientID);
	StringPayload.Append("&client_secret=" + MyToken.ClientSecret);
	StringPayload.Append("&resource=https://digitaltwins.azure.net");

	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetContentAsString(StringPayload);

	TMap<FString, FString> Headers;
	Headers.Add("Host", "login.microsoftonline.com");
	Headers.Add("Content-Type", "application/x-www-form-urlencoded");

	for (auto& Elem : Headers) {
		HttpRequest->SetHeader(Elem.Key, Elem.Value);
	}

	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Display, TEXT("Generate token request sent"));
	TokenRequestSent = true;
}

void UHttpCommunication::HandleGenerateTokenResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {

	FHttpTokenResponse Response;
	Response.RequestUrl = HttpRequest->GetURL();
	Response.Status = EHttpRequestStatus::ToString(HttpRequest->GetStatus());
	Response.Success = false;

	if (HttpResponse.IsValid()) {

		Response.Message = HttpResponse->GetContentAsString();
		Response.Code = HttpResponse->GetResponseCode();

		if (EHttpResponseCodes::IsOk(Response.Code)) {
			if (getTokenFromResponse(HttpResponse->GetContentAsString())) {
				UE_LOG(LogTemp, Display, TEXT("Generate token Response success"));
				Response.AccessToken = MyToken;
				Response.Success = true;
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Authentication failed: Deserialize JSON Response token failed"));
				Response.Message = "Deserialize JSON Response token failed:" + Response.Message;
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Authentication failed: Response not valid"));
			Response.Message = "Response code not valid:" + Response.Message;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Authentication failed: Generate token Response not valid"));
		Response.Message = "Response is null";
	}
	OnTokenResponse.Broadcast(Response);

	//Redo all the requests that arrived while fetching new token
	if (UpdatingToken && Response.Success) {
		UpdatingToken = false;
		RedoRequests();
	}
}

bool UHttpCommunication::getTokenFromResponse(FString JSON) {

	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JSON);
	TSharedPtr<FJsonObject> Object;
	bool SuccessDeserialize = FJsonSerializer::Deserialize(Reader, Object);

	if (!SuccessDeserialize && Object->HasField("access_token")) {
		UE_LOG(LogTemp, Warning, TEXT("Deserialize JSON failed"));
		return false;
	}

	int UnixTimeExpire = Object->GetNumberField("expires_on");
	MyToken.ExpiresOn = FDateTime::FromUnixTimestamp(UnixTimeExpire);
	MyToken.Token = Object->GetStringField("access_token");
	MyToken.TokenType = Object->GetStringField("token_type");
	MyToken.Inisiated = true;

	return true;
}

//----Twin----

void UHttpCommunication::AdtHttpRelationshipIncoming(FString Id) {

	//GET https://digitaltwins-hostname/digitaltwins/{id}/incomingrelationships?api-version=2020-10-31

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/digitaltwins/" + Id + "/incomingrelationships?api-version=" + ApiVersion;
	Request.Method = "GET";
	Request.Payload = "";
	Request.Type = CallType::RelationshipIncoming;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	AdtHttpCall(Request);
}

void UHttpCommunication::AdtHttpRelationshipOutgoing(FString Id) {

	//GET https ://digitaltwins-hostname/digitaltwins/{id}/relationships?api-version=2020-10-31

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/digitaltwins/" + Id + "/relationships?api-version=" + ApiVersion;
	Request.Method = "GET";
	Request.Payload = "";
	Request.Type = CallType::RelationshipOutgoing;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	AdtHttpCall(Request);
}

//----Twin Not Tested----

void UHttpCommunication::AdtHttpAddTwin(FString NewTwinId, FString ModelId) {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/twins/digitaltwins_add

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/digitaltwins/" + NewTwinId + "?api-version=" + ApiVersion;
	Request.Method = "PUT";

	TSharedPtr<FJsonObject> Metadata = MakeShareable(new FJsonObject);
	Metadata->SetStringField("$model", ModelId);

	TSharedPtr<FJsonObject> JSONPayload = MakeShareable(new FJsonObject);
	JSONPayload->SetObjectField("$metadata", Metadata);

	FString StringPayload;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&StringPayload);
	FJsonSerializer::Serialize(JSONPayload.ToSharedRef(), Writer);
	Request.Payload = StringPayload;

	Request.Type = CallType::AddTwin;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	TMap<FString, FString> Headers;
	Headers.Add("Content-Type", "application/JSON");
	Request.Headers = Headers;

	AdtHttpCall(Request);
}

void UHttpCommunication::AdtHttpDeleteTwin(FString TwinId) {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/twins/digitaltwins_delete

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/digitaltwins/" + TwinId + "?api-version=" + ApiVersion;
	Request.Method = "DELETE";

	Request.Type = CallType::DeleteTwin;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	AdtHttpCall(Request);

}

void UHttpCommunication::AdtHttpAddRelationship(FString TwinSourceId, FString TwinTargetId, FString RelationshipName, FString NewRelationshipId) {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/twins/digitaltwins_addrelationship

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/digitaltwins/" + TwinSourceId + "/relationships/" + NewRelationshipId + "?api-version=" + ApiVersion;
	Request.Method = "PUT";

	TSharedPtr<FJsonObject> JSONPayload = MakeShareable(new FJsonObject);
	JSONPayload->SetStringField("$targetId", TwinTargetId);
	JSONPayload->SetStringField("$relationshipName", RelationshipName);

	FString StringPayload;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&StringPayload);
	FJsonSerializer::Serialize(JSONPayload.ToSharedRef(), Writer);
	Request.Payload = StringPayload;

	Request.Type = CallType::AddRelationship;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	TMap<FString, FString> Headers;
	Headers.Add("Content-Type", "application/JSON");
	Request.Headers = Headers;

	AdtHttpCall(Request);
}

void UHttpCommunication::AdtHttpDeleteRelationship(FString TwinId, FString RelationshipId) {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/twins/digitaltwins_delete

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/digitaltwins/" + TwinId + "/relationships/" + RelationshipId + "?api-version=" + ApiVersion;
	Request.Method = "DELETE";

	Request.Type = CallType::DeleteRelationship;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	AdtHttpCall(Request);
}

void UHttpCommunication::AdtHttpGetRelationshipById(FString TwinId, FString RelationshipId) {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/twins/digitaltwins_getrelationshipbyid

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/digitaltwins/" + TwinId + "/relationships/" + RelationshipId + "?api-version=" + ApiVersion;
	Request.Method = "GET";

	Request.Type = CallType::GetRelationshipById;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	AdtHttpCall(Request);
}

void UHttpCommunication::AdtHttpGetTwinById(FString TwinId) {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/twins/digitaltwins_getbyid

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/digitaltwins/" + TwinId + "?api-version=" + ApiVersion;
	Request.Method = "GET";

	Request.Type = CallType::GetById;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	AdtHttpCall(Request);
}

//----Query----

void UHttpCommunication::AdtHttpQueryTwins(FString query) {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/query/querytwins

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/query?api-version=" + ApiVersion;
	Request.Method = "POST";
	Request.Headers.Add("Content-Type", "application/JSON");
	Request.AuthorizationToken = MyToken.Token;

	if (query == "") {
		query = "SELECT * FROM DIGITALTWINS";
	}

	TSharedPtr<FJsonObject> JSONPayload = MakeShareable(new FJsonObject);
	JSONPayload->SetStringField("query", query);

	FString StringPayload;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&StringPayload);
	FJsonSerializer::Serialize(JSONPayload.ToSharedRef(), Writer);
	Request.Payload = StringPayload;
	Request.Type = CallType::QueryTwins;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	AdtHttpCall(Request);
}


void UHttpCommunication::AdtHttpUpdateTwin(FString TwinId, TArray<FUpdateTwinBody> TwinUpdate) {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/twins/digitaltwins_update

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/digitaltwins/" + TwinId + "?api-version=" + ApiVersion;
	Request.Method = "PATCH";
	Request.Type = CallType::UpdateTwin;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	FString JSONPayload;
	for (auto& UpdateElem : TwinUpdate) {

		TSharedPtr<FJsonObject> ElemPayload = MakeShareable(new FJsonObject);

		//If the operation is to remove the property, no values is sent.
		bool hasValue = false;

		//Set operation on JSON property elem
		FString Operation;
		switch (UpdateElem.Operation)
		{
			case UpdateTwinOperation::Add: {
				Operation = "add";
				hasValue = true;
				break;
			}
			case UpdateTwinOperation::Replace: {
				Operation = "replace";
				hasValue = true;
				break;
			}
			case UpdateTwinOperation::Remove: {
				Operation = "remove";
				hasValue = false;
				break;;
			}
		}
		ElemPayload->SetStringField("op", Operation);

		//Set property name to JSON property elem
		ElemPayload->SetStringField("path", "/" + UpdateElem.PropertyName);

		//Add new property value to JSON property elem
		if (hasValue) {
			switch (UpdateElem.ValueType)
			{
				case JsonValueType::_Bool: {
					ElemPayload->SetBoolField("value", UpdateElem.BoolValue);
					break;
				}
				case JsonValueType::_String: {
					ElemPayload->SetStringField("value", UpdateElem.StringValue);
					break;
				}
				case JsonValueType::_Numeric: {
					ElemPayload->SetNumberField("value", UpdateElem.NumericValue);
					break;;
				}
			}
		}

		FString StringElemPayload;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&StringElemPayload);
		FJsonSerializer::Serialize(ElemPayload.ToSharedRef(), Writer);

		JSONPayload += StringElemPayload + ",";
	}

	//Append claps as to JSON payload to make it an array format.
	Request.Payload = "[" + JSONPayload + "]";

	TMap<FString, FString> Headers;
	Headers.Add("Content-Type", "application/JSON");
	Request.Headers = Headers;

	AdtHttpCall(Request);
}

//----Model----

void UHttpCommunication::AdtHttpListModels() {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/models/digitaltwinmodels_list

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/models?&includeModelDefinition=True&api-version=" + ApiVersion;
	Request.Method = "GET";
	Request.Payload = "";
	Request.Type = CallType::ListModels;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	AdtHttpCall(Request);
}

void UHttpCommunication::AdtHttpAddModel(FString JSON) {

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/models?api-version=" + ApiVersion;
	Request.Method = "POST";
	//Payload needs to be in array format
	Request.Payload = "[" + JSON + "]";
	Request.Type = CallType::AddModel;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	TMap<FString, FString> Headers;
	Headers.Add("Content-Type", "application/JSON");
	Request.Headers = Headers;

	AdtHttpCall(Request);
}

void UHttpCommunication::AdtHttpUpdateModelMetadata(FString JSON, FString ModelId) {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/models/digitaltwinmodels_update

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/models/" + ModelId + "?api-version=" + ApiVersion;
	Request.Method = "PATCH";
	//Payload needs to be in array format
	Request.Payload = "[" + JSON + "]";
	Request.Type = CallType::UpdateModel;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	TMap<FString, FString> Headers;
	Headers.Add("Content-Type", "application/JSON");
	Request.Headers = Headers;

	AdtHttpCall(Request);
}

void UHttpCommunication::AdtHttpDeleteModel(FString ModelId) {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/models/digitaltwinmodels_delete

	FHttpADTRequest Request;
	Request.Url = "https://" + MyHost + "/models/" + ModelId + "?api-version=" + ApiVersion;
	Request.Method = "DELETE";
	Request.Type = CallType::DeleteModel;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	TMap<FString, FString> Headers;
	Headers.Add("Content-Type", "application/JSON");
	Request.Headers = Headers;

	AdtHttpCall(Request);
}

void UHttpCommunication::AdtHttpGetModelById(FString ModelId, bool IncludeDefinition) {

	//https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/models/digitaltwinmodels_getbyid

	FHttpADTRequest Request;

	if (IncludeDefinition) {
		Request.Url = "https://" + MyHost + "/models/" + ModelId + "?includeModelDefinition=True&api-version=" + ApiVersion;
	}
	else {
		Request.Url = "https://" + MyHost + "/models/" + ModelId + "?api-version=" + ApiVersion;
	}

	Request.Method = "GET";
	Request.Type = CallType::GetModelById;
	Request.AuthorizationToken = MyToken.Token;
	Request.Host = MyHost;

	TMap<FString, FString> Headers;
	Headers.Add("Content-Type", "application/JSON");
	Request.Headers = Headers;

	AdtHttpCall(Request);
}

//----Main----

void UHttpCommunication::RedoRequests() {

	for (auto Request : RedoAdtRequestList) {
		//Update the token in the request before resending
		Request.Value.AuthorizationToken = MyToken.Token;
		AdtHttpCall(Request.Value);
	}
}

void UHttpCommunication::AdtHttpCall(FHttpADTRequest Request) {

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpCommunication::HandleResponse);
	HttpRequest->SetURL(Request.Url);
	HttpRequest->SetVerb(Request.Method);
	HttpRequest->SetContentAsString(Request.Payload);

	Request.Headers.Add("Host", Request.Host);
	FString Authorization = "Bearer " + Request.AuthorizationToken;
	Request.Headers.Add("Authorization", Authorization);
	for (auto& Elem : Request.Headers) {
		HttpRequest->SetHeader(Elem.Key, Elem.Value);
	}


	FDateTime TimeNow = FDateTime::UtcNow();
	bool InvalidToken = TimeNow >= MyToken.ExpiresOn;

	int ToketUnix = MyToken.ExpiresOn.ToUnixTimestamp();
	int NowUnix = TimeNow.ToUnixTimestamp();

	bool invalidUnix = NowUnix >= ToketUnix;

	FString ToketHTTP = MyToken.ExpiresOn.ToHttpDate();
	FString NowHTTP = TimeNow.ToHttpDate();


	//Check if authorization token does not exits or expired, then send a generate token request
	if (Request.AuthorizationToken.IsEmpty() || InvalidToken) {
		//Check if update token request has already been sent and waiting for response
		if (!UpdatingToken) {
			RedoAdtRequestList.Add(Request.Url, Request);
			UpdatingToken = true;
			bool TokenRequestSent = false;
			FString ErrorMessage;
			GenerateToken(TokenRequestSent, ErrorMessage);

			if (TokenRequestSent) {
				UE_LOG(LogClass, Error, TEXT("Authentication failed trying to generate new token. %s"), *ErrorMessage);
				return;
			}
		}
		//Add request to redo map if token currently is updating
		else if (UpdatingToken && !RedoAdtRequestList.Contains(Request.Url)) {
			RedoAdtRequestList.Add(Request.Url, Request);
			return;
		}
	}
	//Authentication is valid, process request
	else {
		HttpRequest->ProcessRequest();
	}

	CallerList.Add(Request.Url, Request);
}

void UHttpCommunication::HandleResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {

	//Return to blueprint onSuccess or onFail outputs
	FHttpAdtResponse Response;
	Response.RequestUrl = HttpRequest->GetURL();
	Response.Status = EHttpRequestStatus::ToString(HttpRequest->GetStatus());
	Response.Success = false;

	FHttpADTRequest Request = CallerList.FindRef(Response.RequestUrl);

	if (HttpResponse.IsValid()) {
		Response.Code = HttpResponse->GetResponseCode();

		//Successful response
		if (EHttpResponseCodes::IsOk(Response.Code)) {
			Response.Success = true;
			Response.Message = HttpResponse->GetContentAsString();
			UE_LOG(LogClass, Log, TEXT("HTTP Response : %s"), *Response.Message);
		}
		//In case of authentication error response create token request
		else if (Response.Code == 401) { //Authentication failure
			Response.Message = "Authentication Error. " + HttpResponse->GetContentAsString();

			if (!UpdatingToken) {
				RedoAdtRequestList.Add(Response.RequestUrl, Request);

				UpdatingToken = true;
				bool TokenRequestSent;
				FString ErrorMessage;
				GenerateToken(TokenRequestSent, ErrorMessage);

				if (TokenRequestSent) {
					UE_LOG(LogClass, Error, TEXT("Authentication failed trying to generate new token. : %s"), *ErrorMessage);
					return;
				}
			}
			//Add request to redo map if token currently is updating
			else if (UpdatingToken && !RedoAdtRequestList.Contains(Response.RequestUrl)) {
				RedoAdtRequestList.Add(Response.RequestUrl, Request);
				return;
			}
		}
		//None-authentication error
		else {
			Response.Message = "Request failed see logs for further information. " + HttpResponse->GetContentAsString();
			RedoAdtRequestList.Remove(Response.RequestUrl);
			UE_LOG(LogClass, Error, TEXT("HTTP Response error: %s"), *Response.Message);
		}
	}
	else {
		Response.Message = "Response is null";
		UE_LOG(LogClass, Error, TEXT("HTTP Response error: %s"), *Response.Message);
	}

	//Broadcast response to the right delegate that the user should be subscribed to
	SendResponseByCallType(Request.Type, Response);

	//Call Generic http response if user want to subscribe to all responses
	OnHttpResponse.Broadcast(Response);

	//Remove request from caller list
	CallerList.Remove(Response.RequestUrl);
}

void UHttpCommunication::SendResponseByCallType(CallType Type, FHttpAdtResponse Response) {

	switch (Type)
	{
	//Twin

	case CallType::RelationshipIncoming: {
		OnRelationshipIncomingResponse.Broadcast(Response);
		break;
	}
	case CallType::RelationshipOutgoing: {
		OnRelationshipOutgoingResponse.Broadcast(Response);
		break;
	}
	case CallType::AddTwin: {
		OnAddTwinResponse.Broadcast(Response);
		break;
	}
	case CallType::UpdateTwin: {
		OnUpdateTwinResponse.Broadcast(Response);
		break;
	}
	case CallType::DeleteTwin: {
		OnDeleteTwinResponse.Broadcast(Response);
		break;
	}
	case CallType::AddRelationship: {
		OnAddRelationshipResponse.Broadcast(Response);
		break;
	}
	case CallType::DeleteRelationship: {
		OnDeleteRelationshipResponse.Broadcast(Response);
		break;
	}
	case CallType::UpdateRelationship: {
		//(Update Relationship not implemented)
		//.Broadcast(Response);
		break;
	}
	case CallType::GetRelationshipById: {
		OnGetRelationshipByIdResponse.Broadcast(Response);
		break;
	}
	case CallType::GetById: {
		OnGetTwinByIdResponse.Broadcast(Response);
		break;
	}
	case CallType::SendTelemetry: {
		//(Telemetry not implemented)
		//.Broadcast(Response);
		break;
	}
	case CallType::GetComponent: {
		//(Component not implemented)
		//.Broadcast(Response);
		break;
	}
	case CallType::UpdateComponent: {
		//(Component not implemented)
		//.Broadcast(Response);
		break;
	}
	case CallType::SendComponentTelemetry: {
		//(Component Telemetry not implemented)
		//.Broadcast(Response);
		break;
	}
	//Query
	case CallType::QueryTwins: {
		OnLiveTwinDataResponse.Broadcast(Response);
		break;
	}
	//Model
	case CallType::ListModels: {
		OnGetADTModelsResponse.Broadcast(Response);
		break;
	}
	case CallType::AddModel: {
		OnAddADTModelResponse.Broadcast(Response);
		break;
	}
	case CallType::UpdateModel: {
		OnUpdateADTModelResponse.Broadcast(Response);
		break;
	}
	case CallType::DeleteModel: {
		OnDeleteADTModelResponse.Broadcast(Response);
		break;
	}
	case CallType::GetModelById: {
		OnGetADTModelByIdResponse.Broadcast(Response);
		break;
	}
	}
}

//----Base----

void UHttpCommunication::BaseHttpCall(FHttpBaseRequest Request) {

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpCommunication::HandleBaseResponse);
	HttpRequest->SetURL(Request.Url);
	HttpRequest->SetVerb(Request.Method);
	HttpRequest->SetContentAsString(Request.Payload);

	for (auto& Elem : Request.Headers) {
		HttpRequest->SetHeader(Elem.Key, Elem.Value);
	}

	HttpRequest->ProcessRequest();
}

void UHttpCommunication::HandleBaseResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {

	//Return to blueprint onSuccess or onFail outputs
	FHttpAdtResponse Response;
	Response.RequestUrl = HttpRequest->GetURL();
	Response.Status = EHttpRequestStatus::ToString(HttpRequest->GetStatus());
	Response.Success = false;

	FHttpADTRequest Request = CallerList.FindRef(Response.RequestUrl);

	if (HttpResponse.IsValid()) {
		Response.Code = HttpResponse->GetResponseCode();

		if (EHttpResponseCodes::IsOk(Response.Code)) {
			Response.Success = true;
			Response.Message = HttpResponse->GetContentAsString();
		}
	}

	//Call Base HTTP response that the user should be subscribed to
	OnBaseHttpCallResponse.Broadcast(Response);

	//Call Generic HTTP response if user want to subscribe to all responses
	OnHttpResponse.Broadcast(Response);
}