//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

#pragma once

#include "DTDLSchemasEnum.generated.h"

//Enum for schema types included in DTDL
UENUM(BlueprintType)
enum class DTDLSchemas : uint8 {
	_Boolean = 0    UMETA(DisplayName = "boolean"),
	_Date = 1  UMETA(DisplayName = "date"),
	_DateTime = 2  UMETA(DisplayName = "dateTime"),
	_Double = 3  UMETA(DisplayName = "double"),
	_Time = 4  UMETA(DisplayName = "time"),
	_Float = 5  UMETA(DisplayName = "float"),
	_Integer = 6  UMETA(DisplayName = "integer"),
	_Long = 7  UMETA(DisplayName = "long"),
	_String = 8  UMETA(DisplayName = "string"),
	//_Duration = 9  UMETA(DisplayName = "duration"), (not implemented)

	//Complex schemas, Not implemented
	/*
	SArray = 9  UMETA(DisplayName = "Array"),
	SEnum = 10  UMETA(DisplayName = "Enum"),
	SMap = 11  UMETA(DisplayName = "Map"),
	SObject = 12  UMETA(DisplayName = "Object")
	*/
};

//Utility Class for DTDL Schemas
UCLASS()
class ADTLINK_API UDTDLSchemasHelper : public UObject
{
	GENERATED_BODY()


public:

	/*
	*	Get the displace name from a schema
	*	@param Schema - type of schema
	*	@param DisplayName - the display name for the schema, can be used in creating models and the like
	*/
	static bool GetSchemaDisplayName(DTDLSchemas Schema, FString& DisplayName) {

		//Editor only
#if WITH_EDITOR
		UEnum* EnumPtr = FindFirstObjectSafe<UEnum>(TEXT("DTDLSchemas"));
		if (!EnumPtr) {
			return false;
		}

		DisplayName = EnumPtr->GetDisplayNameTextByValue((int64)Schema).ToString();
		if (DisplayName.IsEmpty()) {
			return false;
		}
		else {
			return true;
		}
#else
		// Non-editor
		return false;

#endif
	}

	/*
	*	From an input string get the DTDL Schema enum
	*	@param SchemaString - DTDL Schema as a string
	*	@param EnumFound - DTDL Schema as an enum
	*/
	static bool StringToDtdlSchema(FString SchemaString, DTDLSchemas& EnumFound) {

		if (SchemaString.Equals("boolean")) {
			EnumFound = DTDLSchemas::_Boolean;
		}
		else if (SchemaString.Equals("date")) {
			EnumFound = DTDLSchemas::_Date;
		}
		else if (SchemaString.Equals("dateTime")) {
			EnumFound = DTDLSchemas::_DateTime;
		}
		else if (SchemaString.Equals("double")) {
			EnumFound = DTDLSchemas::_Double;
		}
		//else if (SchemaString.Equals("duration")) {
		//	EnumFound = DTDLSchemas::_Duration;
		//}
		else if (SchemaString.Equals("float")) {
			EnumFound = DTDLSchemas::_Float;
		}
		else if (SchemaString.Equals("integer")) {
			EnumFound = DTDLSchemas::_Integer;
		}
		else if (SchemaString.Equals("long")) {
			EnumFound = DTDLSchemas::_Long;
		}
		else if (SchemaString.Equals("string")) {
			EnumFound = DTDLSchemas::_String;
		}
		else if (SchemaString.Equals("time")) {
			EnumFound = DTDLSchemas::_Time;
		}
		else {
			return false;
		}
		return true;
	}
};
