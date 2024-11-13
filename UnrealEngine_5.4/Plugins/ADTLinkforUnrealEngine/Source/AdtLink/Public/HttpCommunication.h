//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Misc/DateTime.h"

#include "Util/AccessToken.h"
#include "Util/UpdateTwinBody.h"

#include "Messages/HttpBaseRequest.h"
#include "Messages/HttpAdtRequest.h"
#include "Messages/HttpAdtResponse.h"
#include "Messages/HttpAdtTokenResponse.h"

#include "HttpCommunication.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHttpDelegate, FHttpAdtResponse, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTokenDelegate, FHttpTokenResponse, Response);

UCLASS(Blueprintable)
class ADTLINK_API UHttpCommunication : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
		FHttpDelegate OnHttpResponse;

	UPROPERTY(BlueprintAssignable)
		FTokenDelegate OnTokenResponse;

private:

	/*
	* Handle response from an ADT REST call
	* @param HttpRequest - HTTP request data
	* @param HttpResponse - HTTP response data
	* @param bSucceeded - Was the response successful
	*/
	void HandleResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	/*
	* Broadcast to delegate depending on callType argument
	* @param Type - Type of ADT Rest request that the response belong to
	* @param Response - Response data
	*/
	void SendResponseByCallType(CallType Type, FHttpAdtResponse Response);

	/*
	* Handle generic response from HTTP call that was not necessary to ADT
	* @param HttpRequest - HTTP request data
	* @param HttpResponse - HTTP response data
	* @param bSucceeded - Was the response successful
	*/
	void HandleBaseResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	/*
	* Handle response from a generate Token REST call to Azure
	* @param HttpRequest - HTTP request data
	* @param HttpResponse - HTTP response data
	* @param bSucceeded - Was the response successful
	*/
	void HandleGenerateTokenResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	/*
	* Set MyToken from JSON Response
	* @param JSON - Authentication JSON string
	*/
	bool getTokenFromResponse(FString JSON);

	/*
	* Redo all request that came in while token was invalid
	*/
	void RedoRequests();

	/*
	*	Process HTTP call
	*	@param Request - http request struct including:
	*		URL - URL to the ADT
	*		Host - host to connect to
	*		Method - HTTP method, GET, POST, PUT...
	*		AuthorizationToken - Bearer Authorization Token
	*		Headers - Additional headers to include
	*		Payload - JSON payload to include
	*		Type - ADT call type
	*/
	void AdtHttpCall(FHttpADTRequest Request);

	FAccessToken MyToken;
	bool UpdatingToken;
	TMap<FString, FHttpADTRequest> RedoAdtRequestList;

	FString MyHost;

	FString ApiVersion = "2020-10-31";

	static TMap<FString, FHttpADTRequest> CallerList;

public:

	//----Client----

	/*
	*	Create a HTTP Client with ADT credentials
	* @param TenantID - Tenent ID to Azure
	* @param ClientID - Client Id on ADT Instance
	* @param ClientSecret - Client Secret for ADT Instance
	* @param Host - ADT Host URL
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create ADT HTTP Client", Keywords = "ADT DTDL HTTP Azure Digital Twin Token"), Category = "ADT HTTP Client")
		static UHttpCommunication* CreateADTHttpClient(FString TenantID, FString ClientID, FString ClientSecret, FString Host);

	/*
	*	Create a HTTP Client with no added ADT credentials
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create base HTTP Client", Keywords = "ADT DTDL HTTP Azure Digital Twin Token"), Category = "ADT HTTP Client")
		static UHttpCommunication* CreateBaseHttpClient();

	/*
	*	Update the credentials used to connect to ADT
	* @param TenantID - Tenent ID to Azure
	* @param ClientID - Client Id on ADT Instance
	* @param ClientSecret - Client Secret for ADT Instance
	* @param Host - ADT Host URL
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update ADT Credentials", Keywords = "ADT DTDL HTTP Azure Digital Twin variables"), Category = "ADT HTTP Client")
		void UpdateCredentials(FString TenantID, FString ClientID, FString ClientSecret, FString Host);


	//----Token----

	/*
	*	Generate a new token request to ADT to use for authentication.
	* @param TokenRequestSent - Was the request successfully sent
	* @param ErrorMessage - Information text if there is request failes
	*
	*	(This will be called automatically if current token in use is invalid)
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Generate new token request", Keywords = "ADT DTDL HTTP Azure Digital Twin Token"), Category = "ADT HTTP Token")
		void GenerateToken(bool& TokenRequestSent, FString& ErrorMessage);

	//----Base----

	/*
	*	Create HTTP call to Anywhere
	*	@param Request - Including:
	*		URL - URL to the send request
	*		Method - HTTP method, GET, POST, PUT...
	*		Headers - Additional headers to include
	*		Payload - payload to include
	*
	*	Bind OnBaseHttpCallResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Base HTTP Call", Keywords = "ADT DTDL HTTP Azure Digital Twin", AutoCreateRefTerm = "Headers"), Category = "ADT HTTP Base")
		void BaseHttpCall(FHttpBaseRequest Request);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		FHttpDelegate OnBaseHttpCallResponse;

	//---- ADT Twin----

	/*
	*	Create HTTP call to Azure Digital Twin to get incoming relationships
	*	@param Id - Id of twin to get relationship from
	*
	*	Bind OnRelationshipIncomingResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Incoming Relationships", Keywords = "ADT DTDL HTTP Azure Digital Twin Relationship"), Category = "ADT HTTP Twin")
		void AdtHttpRelationshipIncoming(FString Id);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		FHttpDelegate OnRelationshipIncomingResponse;

	/*
	*	Create HTTP call to Azure Digital Twin to get outgoing relationships
	*	@param Id - Id of twin to get relationship from
	*
	*	Bind OnRelationshipOutgoingResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Outgoing Relationships", Keywords = "ADT DTDL HTTP Azure Digital Twin Relationship"), Category = "ADT HTTP Twin")
		void AdtHttpRelationshipOutgoing(FString Id);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		FHttpDelegate OnRelationshipOutgoingResponse;

	/*
	*	Create HTTP call to Azure Digital Twin to Add new Twin
	*	@param NewTwinId - Id for new Twin
	*	@param ModelId - Id for new Twin
	*
	*	Bind OnAddTwinResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Twin", Keywords = "ADT DTDL HTTP Azure Digital Twin Relationship"), Category = "ADT HTTP Twin")
		void AdtHttpAddTwin(FString NewTwinId, FString ModelId);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		FHttpDelegate OnAddTwinResponse;

	/*
	*	Create HTTP call to Azure Digital Twin to Delete new Twin
	*	@param TwinId - Id for Twin to delete
	*
	*	Bind OnDeleteTwinResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Delete Twin", Keywords = "ADT DTDL HTTP Azure Digital Twin Relationship"), Category = "ADT HTTP Twin")
		void AdtHttpDeleteTwin(FString TwinId);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		FHttpDelegate OnDeleteTwinResponse;

	/*
	*	Create HTTP call to Azure Digital Twin to Add new Relationship
	*	@param TwinSourceId - Id on source Twin to add relationship to
	*	@param TwinTargetId - Id on target Twin point relationship to
	*	@param RelationshipName - Name of on the type of relationship to create
	*	@param NewRelationshipId - Id for new Relationship
	*
	*	Bind OnAddRelationshipResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Relationship", Keywords = "ADT DTDL HTTP Azure Digital Twin Relationship"), Category = "ADT HTTP Twin")
		void AdtHttpAddRelationship(FString TwinSourceId, FString TwinTargetId, FString RelationshipName, FString NewRelationshipId);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		FHttpDelegate OnAddRelationshipResponse;


	/*
	*	Create HTTP call to Azure Digital Twin to Delete Relationship
	*	@param TwinId - Id for Twin to delete relationship on
	*	@param RelationshipId - Id for relationship to delete
	*
	*	Bind OnDeleteTwinResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Delete Relationship", Keywords = "ADT DTDL HTTP Azure Digital Twin Relationship"), Category = "ADT HTTP Twin")
		void AdtHttpDeleteRelationship(FString TwinId, FString RelationshipId);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		FHttpDelegate OnDeleteRelationshipResponse;

	/*
	*	Create HTTP call to Azure Digital Twin to get relationship by Id
	*	@param TwinId - Id for Twin to delete relationship on
	*	@param RelationshipId - Id for relationship to delete
	*
	*	Bind OnGetRelationshipByIdResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get ADT Relationship By Id", Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		void AdtHttpGetRelationshipById(FString TwinId, FString RelationshipId);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		FHttpDelegate OnGetRelationshipByIdResponse;

	/*
	*	Create HTTP call to Azure Digital Twin to get Twin by Id
	*	@param TwinId - Id for Twin to delete relationship on
	*
	*	Bind OnGetTwinByIdResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get ADT Twin By Id", Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		void AdtHttpGetTwinById(FString TwinId);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		FHttpDelegate OnGetTwinByIdResponse;

	/*
	*	Create HTTP call to Azure Digital Twin to Update a twins properties
	*	@param TwinId - Id for Twin to delete relationship on
	*
	*	Bind OnUpdateTwinIdResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Twin", Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		void AdtHttpUpdateTwin(FString TwinId, TArray<FUpdateTwinBody> TwinUpdate);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		FHttpDelegate OnUpdateTwinResponse;


	/*
	*	Telemetry, Component and UpdateRelationship not implemented
	*	https://docs.microsoft.com/en-us/rest/api/digital-twins/dataplane/twins
	*
	*	UpdateRelationship
	*	SendTelemetry
	*	GetComponent
	*	UpdateComponent
	*	SendComponentTelemetry
	*/


	//----ADT Query----

	/*
	*	Create HTTP call to Azure Digital Twin to get live twin data
	*	@param query - Specific query, empty to get all
	*
	*	Bind OnLiveTwinDataResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Query Live Twin Data", Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		void AdtHttpQueryTwins(FString query);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Twin")
		FHttpDelegate OnLiveTwinDataResponse;

	//----ADT Model----

	/*
	*	Create HTTP call to Azure Digital Twin to get Models
	*
	*	Bind OnGetADTModelsResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get ADT Models", Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Model")
		void AdtHttpListModels();

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Model")
		FHttpDelegate OnGetADTModelsResponse;

	/*
		Create HTTP call to Azure Digital Twin to Upload Model
		@param JSON - JSON DTDL model to be Add

		Bind OnAddADTModelResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add ADT Model", Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Model")
		void AdtHttpAddModel(FString JSON);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Model")
		FHttpDelegate OnAddADTModelResponse;

	//----Not tested ADT Model----

	/*
	*	Create HTTP call to Azure Digital Twin to Update Model
	*	@param JSON - JSON update
	*	@param ModelId - Model id of DTDL to update
	*
	*	Bind OnUpdateADTModelResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update ADT Model metadata", Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Model")
		void AdtHttpUpdateModelMetadata(FString JSON, FString ModelId);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Model")
		FHttpDelegate OnUpdateADTModelResponse;

	/*
	*	Create HTTP call to Azure Digital Twin to Delete Model
	*	@param ModelId - Model id of DTDL to delete
	*
	*	Bind OnDeleteADTModelResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Delete ADT Model", Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Model")
		void AdtHttpDeleteModel(FString ModelId);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Model")
		FHttpDelegate OnDeleteADTModelResponse;

	/*
	*	Create HTTP call to get Azure Digital Twin model by ID
	*	@param ModelId - Model id of DTDL to delete
	*	@param IncludeDefinition - Include model definition in Response otherwise only metadata
	*
	*	Bind OnGetADTModelByIdResponse for Response
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get ADT Model By Id", Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Model")
		void AdtHttpGetModelById(FString ModelId, bool IncludeDefinition);

	UPROPERTY(BlueprintAssignable, meta = (Keywords = "ADT DTDL HTTP Azure Digital Twin"), Category = "ADT HTTP Model")
		FHttpDelegate OnGetADTModelByIdResponse;
};
