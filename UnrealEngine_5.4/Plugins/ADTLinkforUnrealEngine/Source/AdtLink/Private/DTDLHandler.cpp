//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#include "DTDLHandler.h"

//Create JSON from model Structs

void UDTDLHandler::GenerateJSONModel(FDTDLModel Model, FString& JSON, bool& Success) {

	TSharedPtr<FJsonObject> ModelObject = MakeShareable(new FJsonObject);

	FString Interface;
	TArray<FString> Properties;
	TArray<FString> Telemetrys;
	TArray<FString> Commands;
	TArray<FString> Components;
	TArray<FString> Relationships;

	//Parse Interface to JSON
	GenerateJSONInterface(Model._Interface, Interface, Success);
	if (!Success) {
		return;
	}

	//Parse all properties to JSON
	for (auto& PropertyObject : Model.Propertys) {
		FString PropertyJSON;
		GenerateJSONProperty(PropertyObject.Value, PropertyJSON, Success);
		if (!Success) {
			return;
		}
		Properties.Add(PropertyJSON);
	}

	//Parse all relationships to JSON
	for (auto& RelationshipObject : Model.Relationships) {
		FString RelationshipJSON;
		GenerateJSONRelationship(RelationshipObject.Value, RelationshipJSON, Success);
		if (!Success) {
			return;
		}
		Relationships.Add(RelationshipJSON);
	}


	AppendJSONContentToModel(Interface, Properties, Telemetrys, Commands, Components, Relationships, JSON, Success);

}

void UDTDLHandler::GenerateJSONInterface(FDTDLInterface Interface, FString& JSON, bool& Success) {

	TSharedPtr<FJsonObject> InterfaceObject = MakeShareable(new FJsonObject);

	if (Interface._Id.IsEmpty()) {
		UE_LOG(LogClass, Error, TEXT("Could not Generate JSON base model, Argument Id is required"));
		Success = false;
		return;
	}
	InterfaceObject->SetStringField("@id", Interface._Id);

	InterfaceObject->SetStringField("@type", "Interface");

	if (!Interface.Comment.IsEmpty()) {
		InterfaceObject->SetStringField("comment", Interface.Comment);
	}

	//contents
	TArray< TSharedPtr<FJsonValue> > ContentsArray;
	InterfaceObject->SetArrayField("contents", ContentsArray);

	if (Interface.Description.Num() != 0) {
		TSharedPtr<FJsonObject> DescObject = MakeShareable(new FJsonObject);
		for (auto& Desc : Interface.Description)
		{
			DescObject->SetStringField(Desc.Key, Desc.Value);
		}
		InterfaceObject->SetObjectField("description", DescObject);
	}

	if (Interface.DisplayName.Num() != 0) {
		TSharedPtr<FJsonObject> DisplayObject = MakeShareable(new FJsonObject);
		for (auto& Display : Interface.DisplayName)
		{
			DisplayObject->SetStringField(Display.Key, Display.Value);
		}
		InterfaceObject->SetObjectField("displayName", DisplayObject);
	}

	if (!Interface.Extends.IsEmpty()) {
		InterfaceObject->SetStringField("extends", Interface.Extends);
	}

	InterfaceObject->SetStringField("@context", "dtmi:dtdl:context;2");

	//Serialize
	FString JSONOutputInterface;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JSONOutputInterface);
	FJsonSerializer::Serialize(InterfaceObject.ToSharedRef(), Writer);
	JSON = JSONOutputInterface;

	Success = true;
}

void UDTDLHandler::GenerateJSONProperty(FDTDLProperty Property, FString& JSON, bool& Success) {

	TSharedPtr<FJsonObject> PropertyObject = MakeShareable(new FJsonObject);
	PropertyObject->SetStringField("@type", "Property");

	if (Property._Name.IsEmpty()) {
		UE_LOG(LogClass, Error, TEXT("Could not Generate JSON property, Argument Name is required"));
		Success = false;
		return;
	}
	PropertyObject->SetStringField("name", Property._Name);


	FString SchemaDisplayName;
	if (UDTDLSchemasHelper::GetSchemaDisplayName(Property._Schema, SchemaDisplayName)) {
		PropertyObject->SetStringField("schema", SchemaDisplayName);
	}
	else {
		UE_LOG(LogClass, Error, TEXT("Could not Generate JSON property, DTDLSchemas schema type not found"));
		Success = false;
		return;
	}

	if (!Property.Id.IsEmpty()) {
		PropertyObject->SetStringField("@id", Property.Id);
	}

	if (!Property.Comment.IsEmpty()) {
		PropertyObject->SetStringField("comment", Property.Comment);
	}

	if (Property.Description.Num() != 0) {
		TSharedPtr<FJsonObject> DescObject = MakeShareable(new FJsonObject);
		for (auto& Desc : Property.Description)
		{
			DescObject->SetStringField(Desc.Key, Desc.Value);
		}
		PropertyObject->SetObjectField("description", DescObject);
	}

	if (Property.DisplayName.Num() != 0) {
		TSharedPtr<FJsonObject> DisplayObject = MakeShareable(new FJsonObject);
		for (auto& Display : Property.DisplayName)
		{
			DisplayObject->SetStringField(Display.Key, Display.Value);
		}
		PropertyObject->SetObjectField("displayName", DisplayObject);
	}

	//UNIT is not implemented at this stage

	PropertyObject->SetBoolField("writable", Property.Writable);

	//Serialize
	FString JSONOutputModel;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JSONOutputModel);
	FJsonSerializer::Serialize(PropertyObject.ToSharedRef(), Writer);
	JSON = JSONOutputModel;

	Success = true;
}

void UDTDLHandler::GenerateJSONRelationship(FDTDLRelationship Relationship, FString& JSON, bool& Success) {

	TSharedPtr<FJsonObject> RelationshipObject = MakeShareable(new FJsonObject);
	RelationshipObject->SetStringField("@type", "Relationship");

	if (Relationship._Name.IsEmpty()) {
		UE_LOG(LogClass, Error, TEXT("Could not Generate JSON property, Argument Name is required"));
		Success = false;
		return;
	}
	RelationshipObject->SetStringField("name", Relationship._Name);

	if (!Relationship.Id.IsEmpty()) {
		RelationshipObject->SetStringField("@id", Relationship.Id);
	}

	if (!Relationship.Comment.IsEmpty()) {
		RelationshipObject->SetStringField("comment", Relationship.Comment);
	}

	if (Relationship.Description.Num() != 0) {
		TSharedPtr<FJsonObject> DescObject = MakeShareable(new FJsonObject);
		for (auto& Desc : Relationship.Description)
		{
			DescObject->SetStringField(Desc.Key, Desc.Value);
		}
		RelationshipObject->SetObjectField("description", DescObject);
	}

	if (Relationship.DisplayName.Num() != 0) {
		TSharedPtr<FJsonObject> DisplayObject = MakeShareable(new FJsonObject);
		for (auto& Display : Relationship.DisplayName)
		{
			DisplayObject->SetStringField(Display.Key, Display.Value);
		}
		RelationshipObject->SetObjectField("displayName", DisplayObject);
	}

	if (Relationship.MaxMultiplicity > 0 && Relationship.MinMultiplicity > Relationship.MaxMultiplicity) {
		UE_LOG(LogClass, Error, TEXT("MinMultiplicity cant be greater value then MaxMultiplicity"));
		Success = false;
		return;
	}

	if (Relationship.MaxMultiplicity > 0) {
		RelationshipObject->SetNumberField("maxMultiplicity", Relationship.MaxMultiplicity);
	}

	if (Relationship.MinMultiplicity > 0) {
		RelationshipObject->SetNumberField("minMultiplicity", Relationship.MinMultiplicity);
	}

	if (!Relationship.Target.IsEmpty()) {
		RelationshipObject->SetStringField("target", Relationship.Target);
	}

	//Property and target is not implemented at this stage.

	RelationshipObject->SetBoolField("writable", Relationship.Writable);

	//Serialize
	FString JSONOutputModel;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JSONOutputModel);
	FJsonSerializer::Serialize(RelationshipObject.ToSharedRef(), Writer);
	JSON = JSONOutputModel;

	Success = true;
}

void UDTDLHandler::AppendJSONContentToModel(FString Interface, TArray<FString> Properties, TArray<FString> Telemetrys, TArray<FString> Commands, TArray<FString> Components, TArray<FString> Relationships, FString& JSON, bool& Success) {

	Success = true;

	//Deserialize string to JSON object
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Interface);
	TSharedPtr<FJsonObject> Object;
	bool SuccessDeserialize = FJsonSerializer::Deserialize(Reader, Object);
	//Return if not successful
	if (!SuccessDeserialize) {
		Success = false;
		UE_LOG(LogClass, Error, TEXT("Could not deserialize JSON base : %s"), *Interface);
		return;
	}

	TArray<TSharedPtr<FJsonValue>> ContentsArray;

	if (!AppendContent(ContentsArray, Properties, "Property")) {
		Success = false;
	}

	if (!AppendContent(ContentsArray, Telemetrys, "Telemetry")) {
		Success = false;
	}

	if (!AppendContent(ContentsArray, Commands, "Command")) {
		Success = false;
	}

	if (!AppendContent(ContentsArray, Components, "Component")) {
		Success = false;
	}

	if (!AppendContent(ContentsArray, Relationships, "Relationship")) {
		Success = false;
	}

	Object->SetArrayField("contents", ContentsArray);

	//Serialize
	FString JSONOutputModel;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JSONOutputModel);
	FJsonSerializer::Serialize(Object.ToSharedRef(), Writer);
	JSON = JSONOutputModel;
}

bool UDTDLHandler::AppendContent(TArray<TSharedPtr<FJsonValue>>& ReturningContent, TArray<FString> JSONContent, FString Type) {

	bool Success = true;

	for (auto ContentJson : JSONContent) {
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentJson);
		TSharedPtr<FJsonObject> ContentObject;
		bool SuccessDeserialize = FJsonSerializer::Deserialize(Reader, ContentObject);

		//Check if deserialization was successful
		if (SuccessDeserialize) {
			TSharedRef<FJsonValueObject> JSONValue = MakeShareable(new FJsonValueObject(ContentObject));
			ReturningContent.Add(JSONValue);
		}
		else {
			UE_LOG(LogClass, Warning, TEXT("Could not deserialize %s JSON : %s"), *Type, *ContentJson);
			Success = false;
		}
	}

	return Success;
}


//Create Model Structs from JSON


void UDTDLHandler::JSONModelToModelMap(FString JSONModel, bool& Success, TMap<FString, FDTDLModel>& ModelMap) {

	//Deserialize string to JSON object
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JSONModel);
	TSharedPtr<FJsonObject> Object;
	bool SuccessDeserialize = FJsonSerializer::Deserialize(Reader, Object);

	//Return if not successful
	if (!SuccessDeserialize) {
		Success = false;
		UE_LOG(LogClass, Warning, TEXT("Could not deserialize JSON in Model : %s"), *JSONModel);
		return;
	}

	//Find array values and iterate to find all models in JSON
	const TArray<TSharedPtr<FJsonValue>> Values = Object->GetArrayField(TEXT("value"));
	for (auto Value : Values) {


		//Create model struct and create model JSONObject
		TSharedPtr<FJsonObject> ModelObject = Value->AsObject()->GetObjectField(TEXT("model"));
		FString InterfaceId = ModelObject->GetStringField(TEXT("@id"));

		//Add full JSON to interface
		FString JSONOutputModel;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JSONOutputModel);
		FJsonSerializer::Serialize(ModelObject.ToSharedRef(), Writer);

		//Add variables to Interface struct from model JSON object
		FDTDLInterface Interface;
		setModelInterface(ModelObject, Interface);
		Interface.JSON = JSONOutputModel;

		TMap<FString, FDTDLProperty> Propertys;
		TMap<FString, FDTDLRelationship> Relationships;
		//TODO - Create map for DTDL Telemetry, Command, Component

		//Find array contents and iterate to find all properties, Relationships, Telemetry and more..
		const TArray<TSharedPtr<FJsonValue>> Contents = ModelObject->GetArrayField(TEXT("contents"));
		for (auto Content : Contents) {
			//make FJsonObject
			TSharedPtr<FJsonObject> ContentObject = Content->AsObject();

			//setProperty if current content is of type Property
			if (setModelProperty(ContentObject, Propertys)) {
				//continue loop if property found and added
				continue;
			}
			if (setModelRelationships(ContentObject, Relationships)) {
				//continue loop if Relatioship found and added
				continue;
			}
			//TODO - Set DTDL Telemetry, Command, Component
		}


		FDTDLModel Model;
		Model._Interface = Interface;
		Model.Propertys = Propertys;
		Model.Relationships = Relationships;
		//TODO - Add to interface:  DTDL Telemetry, Command, Component

		Model.JSON = JSONOutputModel;

		//Add Model to returning map
		ModelMap.Add(InterfaceId, Model);
	}

	Success = true;
}

void UDTDLHandler::setModelInterface(TSharedPtr<FJsonObject> Content, FDTDLInterface& Interface) {

	//Set DTDL Interface parameters if available
	if (Content->HasField(TEXT("@id"))) {
		Interface._Id = Content->GetStringField(TEXT("@id"));
	}
	//Interface._Context = "dtmi:dtdl:context;2";
	//if (Model->HasField("@type")) {
	//	Interface._Type = Model->GetStringField("@type");
	//}
	if (Content->HasField(TEXT("comment"))) {
		Interface.Comment = Content->GetStringField(TEXT("comment"));
	}
	//if (Model->HasField("contents")) {
	//	Interface.Contents = Model->GetStringField("contents");
	//}
	if (Content->HasField(TEXT("description"))) {
		TSharedPtr<FJsonObject> DescObject = Content->GetObjectField(TEXT("description"));
		for (auto DescValues : DescObject->Values)
		{
			Interface.Description.Add(DescValues.Key, DescValues.Value->AsString());
		}
	}
	if (Content->HasField(TEXT("displayName"))) {
		TSharedPtr<FJsonObject> DisplayObject = Content->GetObjectField(TEXT("displayName"));
		for (auto DisplayValues : DisplayObject->Values)
		{
			Interface.DisplayName.Add(DisplayValues.Key, DisplayValues.Value->AsString());
		}
	}
	if (Content->HasField(TEXT("extends"))) {
		Interface.Extends = Content->GetStringField(TEXT("extends"));
	}
	//if (Content->HasField("schemas")) {
	//	Interface.Schemas = Content->GetStringField("schemas");
	//}

	//Add full JSON model to Interface as string
	FString JSONOutputInterface;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JSONOutputInterface);
	FJsonSerializer::Serialize(Content.ToSharedRef(), Writer);
	Interface.JSON = JSONOutputInterface;
}

bool UDTDLHandler::setModelProperty(TSharedPtr<FJsonObject> Content, TMap<FString, FDTDLProperty>& Propertys) {
	//return false if type does not exist
	if (!Content->HasField(TEXT("@type"))) {
		return false;
	}
	//Return false if type is not property
	if (!Content->GetStringField(TEXT("@type")).Equals(TEXT("Property"))) {
		return false;
	}

	//Create variables for keeping the new property and name identifier
	FDTDLProperty NewProperty;
	FString Name;

	//Set DTDL Property parameters if available
	if (Content->HasField(TEXT("name"))) {
		NewProperty._Name = Content->GetStringField(TEXT("name"));
		Name = NewProperty._Name;
	}
	//if (Content->HasField("@type")) {
	//	NewProperty._Type = Content->GetStringField("@type");
	//}

	DTDLSchemas Schema;
	if (UDTDLSchemasHelper::StringToDtdlSchema(Content->GetStringField(TEXT("schema")), Schema)) {
		NewProperty._Schema = Schema;
	}

	if (Content->HasField(TEXT("@id"))) {
		NewProperty.Id = Content->GetStringField(TEXT("@id"));
	}
	if (Content->HasField(TEXT("comment"))) {
		NewProperty.Comment = Content->GetStringField(TEXT("comment"));
	}
	if (Content->HasField(TEXT("description"))) {
		TSharedPtr<FJsonObject> DescObject = Content->GetObjectField(TEXT("description"));
		for (auto DescValues : DescObject->Values)
		{
			NewProperty.Description.Add(DescValues.Key, DescValues.Value->AsString());
		}
	}
	if (Content->HasField(TEXT("displayName"))) {
		TSharedPtr<FJsonObject> DisplayObject = Content->GetObjectField(TEXT("displayName"));
		for (auto DisplayValues : DisplayObject->Values)
		{
			NewProperty.DisplayName.Add(DisplayValues.Key, DisplayValues.Value->AsString());
		}
	}
	//if (Content->HasField("unit")) {
	//	NewProperty.Unit = Model->GetStringField("unit");
	//}
	if (Content->HasField(TEXT("writable"))) {
		NewProperty.Writable = Content->GetBoolField(TEXT("writable"));
	}

	//Add full JSON content as string to Property
	FString JSONOutputProperty;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JSONOutputProperty);
	FJsonSerializer::Serialize(Content.ToSharedRef(), Writer);
	NewProperty.JSON = JSONOutputProperty;

	//Append new property to property map
	Propertys.Add(Name, NewProperty);

	//Return successful
	return true;
}

bool UDTDLHandler::setModelRelationships(TSharedPtr<FJsonObject> Content, TMap<FString, FDTDLRelationship>& Relationships) {
	//return false if type does not exist
	if (!Content->HasField(TEXT("@type"))) {
		return false;
	}
	//Return false if type is not property
	if (!Content->GetStringField(TEXT("@type")).Equals(TEXT("Relationship"))) {
		return false;
	}

	//Create variables for keeping the new Relationship and name identifier
	FDTDLRelationship NewRelationship;
	FString Name;

	//Set DTDL Relationship parameters if available
	if (Content->HasField(TEXT("name"))) {
		NewRelationship._Name = Content->GetStringField(TEXT("name"));
		Name = NewRelationship._Name;
	}
	//if (Content->HasField("@type")) {
	//	NewRelationship._Type = Content->GetStringField("@type");
	//}
	if (Content->HasField(TEXT("@id"))) {
		NewRelationship.Id = Content->GetStringField(TEXT("@id="));
	}
	if (Content->HasField(TEXT("comment"))) {
		NewRelationship.Comment = Content->GetStringField(TEXT("comment"));
	}
	if (Content->HasField(TEXT("descrition"))) {
		TSharedPtr<FJsonObject> DescObject = Content->GetObjectField(TEXT("description"));
		for (auto DescValues : DescObject->Values)
		{
			NewRelationship.Description.Add(DescValues.Key, DescValues.Value->AsString());
		}
	}
	if (Content->HasField(TEXT("displayName"))) {
		TSharedPtr<FJsonObject> DescObject = Content->GetObjectField(TEXT("displayName"));
		for (auto DescValues : DescObject->Values)
		{
			NewRelationship.Description.Add(DescValues.Key, DescValues.Value->AsString());
		}
	}
	if (Content->HasField(TEXT("minMultiplicity"))) {
		NewRelationship.MinMultiplicity = Content->GetNumberField(TEXT("minMultiplicity"));
	}
	else
	{
		NewRelationship.MinMultiplicity = 0;
	}
	if (Content->HasField(TEXT("maxMultiplicity"))) {
		NewRelationship.MaxMultiplicity = Content->GetNumberField(TEXT("maxMultiplicity"));
	}
	else
	{
		NewRelationship.MaxMultiplicity = -1;
	}
	if (Content->HasField(TEXT("target"))) {
		NewRelationship.Target = Content->GetStringField(TEXT("target"));
	}
	if (Content->HasField(TEXT("writable"))) {
		NewRelationship.Writable = Content->GetBoolField(TEXT("writable"));
	}

	//Not implemented properties on relationships
	//Create and set DTDL Property parameters if available in Relationship JSON content
	if (Content->HasField(TEXT("properties"))) {
		TMap<FString, FDTDLProperty> Propertys;

		//Find array contents and iterate to find all properties
		const TArray<TSharedPtr<FJsonValue>> PropertiesArray = Content->GetArrayField(TEXT("properties"));
		for (auto Prop : PropertiesArray) {
			//Make into FJsonObject and set Property
			TSharedPtr<FJsonObject> PropObject = Prop->AsObject();
			setModelProperty(PropObject, Propertys);
		}

		//Add Property's to new relationship struct
		//NewRelationship.Property = Propertys;
	}

	//Add full JSON content as string to Relationship
	FString JSONOutputRelationship;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JSONOutputRelationship);
	FJsonSerializer::Serialize(Content.ToSharedRef(), Writer);
	NewRelationship.JSON = JSONOutputRelationship;

	//Append new property to Relationship map
	Relationships.Add(Name, NewRelationship);

	//Return successful
	return true;
}

void UDTDLHandler::JSONToTwinDataMap(FString JSONData, bool& Success, TMap<FString, FAdtTwinData>& TwinData) {

	//Deserialize string to JSON object
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JSONData);
	TSharedPtr<FJsonObject> Object;
	bool SuccessDeserialize = FJsonSerializer::Deserialize(Reader, Object);

	//Return if not successful
	if (!SuccessDeserialize) {
		Success = false;
		UE_LOG(LogClass, Warning, TEXT("Could not deserialize JSON in twin data : %s"), *JSONData);
		return;
	}

	if (Object->HasField(TEXT("value"))) {

		//Find array values and iterate to find all Model
		const TArray<TSharedPtr<FJsonValue>> Values = Object->GetArrayField(TEXT("value"));
		for (auto Value : Values) {

			TSharedPtr<FJsonObject> ValueObject = Value->AsObject();
			FAdtTwinData MyTwinData;
			if (!GetLiveDataFromJsonObject(ValueObject, MyTwinData)) {
				Success = false;
			}

			//Append new twin data to twin data map
			TwinData.Add(MyTwinData.TwinId, MyTwinData);
		}

	}
	else if (Object->HasField(TEXT("$dtId")) && Object->HasField(TEXT("$metadata"))) {

		FAdtTwinData MyTwinData;
		if (!GetLiveDataFromJsonObject(Object, MyTwinData)) {
			Success = false;
		}

		//Append new twin data to twin data map
		TwinData.Add(MyTwinData.TwinId, MyTwinData);
	}
	else {
		Success = false;
		UE_LOG(LogClass, Warning, TEXT("$dtIda and $metadata or value field was not found in JSON data: %s"), *JSONData);
	}


	//Return successful
	Success = true;

}


bool UDTDLHandler::GetLiveDataFromJsonObject(TSharedPtr<FJsonObject> Object, FAdtTwinData& twinData) {

	bool Success = true;

	bool IdFound = false;
	bool ModelFound = false;
	for (auto& DataValue : Object->Values)
	{
		//Find id on model and twin
		if (DataValue.Key.StartsWith(TEXT("$")))
		{
			if (DataValue.Key == TEXT("$dtId"))
			{
				twinData.TwinId = DataValue.Value->AsString();
				IdFound = true;
			}
			else if (DataValue.Key == TEXT("$metadata"))
			{
				ModelFound = DataValue.Value->AsObject()->TryGetStringField(TEXT("$model"), twinData.ModelId);
				ModelFound = true;
			}
			continue;
		}

		//Find live data type and add to data maps
		if (DataValue.Value->Type == EJson::Number) {
			float NumberValue;
			DataValue.Value->TryGetNumber(NumberValue);
			twinData.NumberValues.Add(DataValue.Key, NumberValue);
		}
		else if (DataValue.Value->Type == EJson::Boolean) {
			bool BoolValue;
			DataValue.Value->TryGetBool(BoolValue);
			twinData.BoolValues.Add(DataValue.Key, BoolValue);
		}
		else if (DataValue.Value->Type == EJson::String) {
			FString StringValue;
			DataValue.Value->TryGetString(StringValue);
			twinData.StringValues.Add(DataValue.Key, StringValue);
		}
	}
	if (!IdFound) {

		UE_LOG(LogClass, Warning, TEXT("$dtId is a required value in JSON twin data that was not found"));
		Success = false;
	}
	if (!ModelFound) {
		UE_LOG(LogClass, Warning, TEXT("Metadata->$model is a required value in JSON twin data that was not found"));
		Success = false;
	}

	//Add full JSON twin data as string
	FString JSONTwinData;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JSONTwinData);
	FJsonSerializer::Serialize(Object.ToSharedRef(), Writer);
	twinData.JSON = JSONTwinData;

	return Success;
}

void UDTDLHandler::JSONToTwinRelationshipMap(FString OwnerId, FString JSONRelationships, FDTDLInterface Interfaces, bool& Success, TMap<FString, FADTTwinRelationship>& TwinRelatinship) {

	//Deserialize string to JSON object
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JSONRelationships);
	TSharedPtr<FJsonObject> Object;
	bool SuccessDeserialize = FJsonSerializer::Deserialize(Reader, Object);

	//Return if not successful
	if (!SuccessDeserialize) {
		Success = false;
		UE_LOG(LogClass, Warning, TEXT("Could not deserialize JSON in relationship data : %s"), *JSONRelationships);
		return;
	}

	//Find array values and iterate to find all interfaces
	const TArray<TSharedPtr<FJsonValue>> Values = Object->GetArrayField(TEXT("value"));
	for (auto Value : Values) {
		//make FJsonObject
		TSharedPtr<FJsonObject> ValueObject = Value->AsObject();

		//New relationship twin struct
		FADTTwinRelationship MyTwinRelationship;

		//Incoming and outgoing
		if (ValueObject->HasField(TEXT("$relationshipId"))) {
			MyTwinRelationship._RelationshipId = ValueObject->GetStringField(TEXT("$relationshipId"));
		}
		if (ValueObject->HasField(TEXT("$sourceId"))) {
			MyTwinRelationship._SourceId = ValueObject->GetStringField(TEXT("$sourceId"));
		}
		if (ValueObject->HasField(TEXT("$relationshipName"))) {
			MyTwinRelationship._RelationshipName = ValueObject->GetStringField(TEXT("$relationshipName"));
		}

		//outgoing
		if (ValueObject->HasField(TEXT("$targetId"))) {
			MyTwinRelationship.TargetId = ValueObject->GetStringField(TEXT("$targetId"));
			//set type to outgoing relationship
			MyTwinRelationship._IsOutgoingType = true;
		}
		else {
			MyTwinRelationship.TargetId = OwnerId;
			MyTwinRelationship._IsOutgoingType = false;
		}

		if (ValueObject->HasField(TEXT("$etag"))) {
			MyTwinRelationship.Etag = ValueObject->GetStringField(TEXT("$etag"));
		}
		if (ValueObject->HasField(TEXT("relationshipProperty"))) {
			MyTwinRelationship.RelationshipProperty = ValueObject->GetStringField(TEXT("relationshipProperty"));
		}

		//Incoming
		if (ValueObject->HasField(TEXT("$relationshipLink"))) {
			MyTwinRelationship.RelationshipLink = (*ValueObject->Values.Find(TEXT("$relationshipLink")))->AsString();
			MyTwinRelationship.RelationshipLink = ValueObject->GetStringField(TEXT("$relationshipLink"));
		}

		//Add full JSON metadata as string
		FString JSONOutputRelationship;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JSONOutputRelationship);
		FJsonSerializer::Serialize(ValueObject.ToSharedRef(), Writer);
		MyTwinRelationship.JSON = JSONOutputRelationship;

		//Set key to other sensor id (we already know our own id)
		FString Key;
		if (MyTwinRelationship._IsOutgoingType) {
			Key = MyTwinRelationship.TargetId;
		}
		else {
			Key = MyTwinRelationship._SourceId;
		}

		TwinRelatinship.Add(Key, MyTwinRelationship);
	}

	//Return successful
	Success = true;
}

void UDTDLHandler::JSONToAddTwinResponse(FString JSONData, FString& TwinIdResponse, FString& ModelIdResponse, bool& Success) {

	//Deserialize string to JSON object
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JSONData);
	TSharedPtr<FJsonObject> Object;
	bool SuccessDeserialize = FJsonSerializer::Deserialize(Reader, Object);

	//Return if not successful
	if (!SuccessDeserialize) {
		Success = false;
		UE_LOG(LogClass, Warning, TEXT("Could not deserialize JSON in twin data : %s"), *JSONData);
		return;
	}

	bool IdFound = false;
	bool ModelFound = false;
	for (auto& DataValue : Object->Values)
	{
		//Find id on model and twin
		if (DataValue.Key.StartsWith(TEXT("$")))
		{
			if (DataValue.Key == TEXT("$dtId"))
			{
				TwinIdResponse = DataValue.Value->AsString();
				IdFound = true;
			}
			else if (DataValue.Key == TEXT("$metadata"))
			{
				ModelFound = DataValue.Value->AsObject()->TryGetStringField(TEXT("$model"), ModelIdResponse);
			}
		}
	}
	if (!IdFound) {

		UE_LOG(LogClass, Warning, TEXT("$dtId is a required value in JSON twin data that was not found: %s"), *JSONData);
		Success = false;
	}
	if (!ModelFound) {
		UE_LOG(LogClass, Warning, TEXT("Metadata->$model is a required value in JSON twin data that was not found : %s"), *JSONData);
		Success = false;
	}


	Success = true;
}

void UDTDLHandler::JSONToAddModelResponse(FString JSONData, FString& ModelIdResponse, bool& Success) {

	//Only takes non array, remove array indicator if exist
	int index;
	bool CharFound;
	CharFound = JSONData.FindChar(TCHAR('['), index);
	if (CharFound && index == 0) {
		JSONData.RemoveAt(index);
	}
	CharFound = JSONData.FindLastChar(TCHAR(']'), index);
	if (CharFound && index == JSONData.Len()-1) {
		JSONData.RemoveAt(index);
	}

	//Deserialize string to JSON object
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JSONData);
	TSharedPtr<FJsonObject> Object;
	bool SuccessDeserialize = FJsonSerializer::Deserialize(Reader, Object);

	//Return if not successful
	if (!SuccessDeserialize) {
		Success = false;
		UE_LOG(LogClass, Warning, TEXT("Could not deserialize JSON in twin data : %s"), *JSONData);
		return;
	}

	Success = Object->TryGetStringField(TEXT("id"), ModelIdResponse);

}

void UDTDLHandler::JSONToAddRelationshipResponse(FString JSONData, FString& RelationshipId, FString& RelationshipName, FString& SourceId, FString& TargetId, bool& Success) {

	//Deserialize string to JSON object
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JSONData);
	TSharedPtr<FJsonObject> Object;
	bool SuccessDeserialize = FJsonSerializer::Deserialize(Reader, Object);

	//Return if not successful
	if (!SuccessDeserialize) {
		Success = false;
		UE_LOG(LogClass, Warning, TEXT("Could not deserialize JSON in twin data : %s"), *JSONData);
		return;
	}

	if (!Object->TryGetStringField(TEXT("$relationshipId"), RelationshipId)) {
		Success = false;
	}
	if (!Object->TryGetStringField(TEXT("$relationshipName"), RelationshipName)) {
		Success = false;
	}
	if (!Object->TryGetStringField(TEXT("$sourceId"), SourceId)) {
		Success = false;
	}
	if (!Object->TryGetStringField(TEXT("$targetId"), TargetId)) {
		Success = false;
	}

	Success = true;
}
