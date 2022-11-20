// Fill out your copyright notice in the Description page of Project Settings.


#include "StartEndLine.h"
#include "Kismet/GameplayStatics.h"
#include "BaseCharacter.h"

AStartEndLine::AStartEndLine()
{
    OnActorBeginOverlap.AddDynamic(this, &AStartEndLine::OnOverlapBegin);
    OnActorEndOverlap.AddDynamic(this, &AStartEndLine::OnOverlapEnd);
}

void AStartEndLine::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AStartEndLine::BeginPlay()
{
    Super::BeginPlay();
    BCReference = Cast<ABaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AStartEndLine::OnOverlapBegin(class AActor* OverlappedActor, class AActor* Player)
{
    if (BCReference && BCReference == Player)
    {
        
        BCReference->SetbTimerStarted(false);
        BCReference->SetTimeRankWidget();
    }
}

void AStartEndLine::OnOverlapEnd(class AActor* OverlappedActor, class AActor* Player)
{
    
    if (BCReference && BCReference == Player)
    {
        BCReference->SetStartTime(GetWorld()->GetTimeSeconds());
        BCReference->SetbTimerStarted(true);
    }
}

float AStartEndLine::GetStartTime()
{
    return fStartTime;
}

