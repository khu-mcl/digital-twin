//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"

#include "Serialization/JsonSerializer.h"

#include "DTDL/DTDLModel.h"
#include "DTDL/DTDLInterface.h"
#include "DTDL/DTDLRelationship.h"
#include "DTDL/DTDLProperty.h"
#include "DTDL/DTDLSchemasEnum.h"

#include "LiveData/ADTTwinData.h"
#include "LiveData/ADTTwinRelationship.h"

#include "DTDLHandler.generated.h"

/**
 *
 */
UCLASS()
class ADTLINK_API UDTDLHandler : public UObject
{
	GENERATED_BODY()

public:

	//Create JSON from model Structs

	/*
	*	Generate a JSON string from DTDL Model struct
	*	@param Model - The model struct
	*	@param JSON - JSON string generated
	*	@param Success - True if successful
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Generate JSON Model (Editor only)", Keywords = "ADT DTDL Model Interface Azure Digital Twin", AutoCreateRefTerm = "Description,DisplayName"), Category = "ADT JSON Generate")
		static void GenerateJSONModel(FDTDLModel Model, FString& JSON, bool& Success);

	/*
	*	Generate a JSON string from DTDL Interface struct
	*	@param Interface - The Interface struct
	*	@param JSON - JSON string generated
	*	@param Success - True if successful
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Generate JSON Interface", Keywords = "ADT DTDL Model Interface Azure Digital Twin", AutoCreateRefTerm = "Description,DisplayName"), Category = "ADT JSON Generate")
		static void GenerateJSONInterface(FDTDLInterface Interface,  FString& JSON, bool& Success);

	/*
	*	Generate a JSON string from DTDL Property struct
	*	@param Property - The Property struct
	*	@param JSON - JSON string generated
	*	@param Success - True if successful
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Generate JSON base Property (Editor only)", Keywords = "ADT DTDL Property Azure Digital Twin", AutoCreateRefTerm = "Description,DisplayName"), Category = "ADT JSON Generate")
		static void GenerateJSONProperty(FDTDLProperty Property, FString& JSON, bool& Success);

	/*
	*	Generate a JSON string from DTDL Relationship struct
	*	@param Relationship - The Relationship struct
	*	@param JSON - JSON string generated
	*	@param Success - True if successful
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Generate JSON base Relationship", Keywords = "ADT DTDL Property Azure Digital Twin", AutoCreateRefTerm = "Description,DisplayName"), Category = "ADT JSON Generate")
		static void GenerateJSONRelationship(FDTDLRelationship Relationship, FString& JSON, bool& Success);

	/*
	*	Generate a JSON string of DTDL Model type from other JSON components
	*	@param Interface - Interface in JSON format
	*	@param Properties - Properties in JSON format
	*	@param Telemetrys -  (not implemented)
	*	@param Commands - (not implemented)
	*	@param Components - (not implemented)
	*	@param Relationships - Relationships in JSON format
	*	@param JSON - JSON string generated
	*	@param Success - True if successful
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Create DTDL from JSON base content", Keywords = "ADT DTDL Property Model Azure Digital Twin", AutoCreateRefTerm = "Properties,Telemetrys,Commands,Components,Relationships"), Category = "ADT JSON Generate")
		static void AppendJSONContentToModel(FString Interface, TArray<FString> Properties, TArray<FString> Telemetrys, TArray<FString> Commands, TArray<FString> Components, TArray<FString> Relationships, FString& JSON, bool& Success);

	//Create Model Structss from JSON

	/*
	*	Create Interface model map from incoming JSON model data
	*	@param JSONModel - JSON String of Model structure
	*	@param Success - Was the operation successful
	*	@param Interfaces - Map with Interface name as key and data as value
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "JSON To Model Map", Keywords = "ADT DTDL Model Interface Azure Digital Twin"), Category = "ADT JSON Parse")
		static void JSONModelToModelMap(FString JSONModel, bool& Success, TMap<FString, FDTDLModel>& ModelMap);

	/*
	*	Create live twin data map from incoming JSON data
	*	@param JSONData - JSON String of data structure
	*	@param Interfaces - Current Model Interface structure
	*	@param Success - Was the operation successful
	*	@param TwinData - Map with Twin data name as key and data as value
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "JSON To Twin Data", Keywords = "ADT DTDL Model Interface Azure Digital Twin"), Category = "ADT JSON Parse")
		static void JSONToTwinDataMap(FString JSONData, bool& Success, TMap<FString, FAdtTwinData>& TwinData);

	/*
	*	Create live twin data map from incoming JSON data
	*	@param OwnerId - String of identifier relationship belongs to
	*	@param JSONRelationships - JSON String of Relationship structure
	*	@param Interfaces - Model Interface structure for relationship sensor
	*	@param Success - Was the operation successful
	*	@param TwinRelatinship - Map with Twin Relationship target as key and data as value
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "JSON Relationships to TwinRelatinship Map", Keywords = "ADT DTDL Model Interface Azure Digital Twin"), Category = "ADT JSON Parse")
		static void JSONToTwinRelationshipMap(FString OwnerId, FString JSONRelationships, FDTDLInterface Interfaces, bool& Success, TMap<FString, FADTTwinRelationship>& TwinRelatinship);


	//Handle live Response data

	/*
	*	Parse "Add Twin" response JSON data
	*	@param JSONData - JSON response
	*	@param TwinIdResponse - Twin Id found
	*	@param ModelIdResponse - Model Id found
	*	@param Success - True if parsing was successful
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "JSON To Add Twin Response", Keywords = "ADT DTDL Add Interface Azure Digital Twin"), Category = "ADT JSON Responses")
		static void JSONToAddTwinResponse(FString JSONData, FString& TwinIdResponse, FString& ModelIdResponse, bool& Success);

	/*
	*	Parse "Add Model" response JSON data
	*	@param JSONData - JSON response
	*	@param ModelIdResponse - Model Id found
	*	@param Success - True if parsing was successful
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "JSON To Add Model Response", Keywords = "ADT DTDL Add Interface Azure Digital Twin"), Category = "ADT JSON Responses")
		static void JSONToAddModelResponse(FString JSONData, FString& ModelIdResponse, bool& Success);

	/*
	*	Parse "Add Relationship" response JSON data
	*	@param JSONData - JSON response
	*	@param RelationshipId - Relationship Id found
	*	@param RelationshipName - Relationship Name found
	*	@param SourceId - Twin Id of source Twin
	*	@param TargetId - Twin Id of target Twin
	*	@param Success - True if parsing was successful
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "JSON To Add Relationship Response", Keywords = "ADT DTDL Add Interface Azure Digital Twin"), Category = "ADT JSON Responses")
		static void JSONToAddRelationshipResponse(FString JSONData, FString& RelationshipId, FString& RelationshipName, FString& SourceId, FString& TargetId, bool& Success);

private:

	//Create JSON from model Structs

	/*
	*	Apply JSON String content to JSON value array
	*	@param ReturningContent - Json value array to append new content to
	*	@param JSONContent - Array of content to append
	*	@param Type - What type of conetent that is added, only used when logging errors
	*	@return True if appending was successful
	*/
	static bool AppendContent(TArray<TSharedPtr<FJsonValue>>& ReturningContent, TArray<FString> JSONContent, FString Type);


	//Create Model Structs from JSON

	/*
	*	Set data on interface struct from Content JSON Object
	*	@param Content - JSON Object to get data from
	*	@param Interface - Struct to append data to
	*/
	static void setModelInterface(TSharedPtr<FJsonObject> Content, FDTDLInterface& Interface);

	/*
	*	Set data on Property struct from Content JSON Object
	*	@param Content - JSON Object to get data from
	*	@param Properties - Struct to append data to
	*/
	static bool setModelProperty(TSharedPtr<FJsonObject> Content, TMap<FString, FDTDLProperty>& Propertys);

	/*
	*	Set data on Relationship struct from Content JSON Object
	*	@param Content - JSON Object to get data from
	*	@param Relationships - Struct to append data to
	*/
	static bool setModelRelationships(TSharedPtr<FJsonObject> Content, TMap<FString, FDTDLRelationship>& Relationships);

	/*
	*	Set data on live twin data struct from JSON Object
	*	@param Object - JSON Object to get data from
	*	@param twinData - Struct to append data to
	*/
	static bool GetLiveDataFromJsonObject(TSharedPtr<FJsonObject> Object, FAdtTwinData& twinData);
};
