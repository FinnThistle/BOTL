// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/TriggerVolume.h"
#include "StartEndLine.generated.h"

UCLASS()
class BRUTUSONTHELOOSE_API AStartEndLine : public ATriggerVolume
{
	GENERATED_BODY()
public:
	AStartEndLine();
	virtual void Tick(float DeltaTime) override;
	float fStartTime = 0;
	bool bTimerStarted = false;
	float GetStartTime();
	class ABaseCharacter* BCReference;
protected:
	virtual void BeginPlay() override;
	
private:
	float RealTimeSeconds = 0;
	
	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* Player);

	UFUNCTION()
	void OnOverlapEnd(class AActor* OverlappedActor, class AActor* Player);

	
};
