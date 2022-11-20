// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_EMovementState.generated.h"

/**
 * 
 */
UENUM(BlueprintType)

enum EMovementState
{
	Walking UMETA(DisplayName = "WalkingState"),
	Crouching UMETA(DisplayName = "CrouchingState"),
	Sprinting UMETA(DisplayName = "SprintingState"),
	Sliding UMETA(DisplayName = "SlidingState")

};
