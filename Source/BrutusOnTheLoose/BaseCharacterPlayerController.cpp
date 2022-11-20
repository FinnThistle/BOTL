// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterPlayerController.h"
#include "Blueprint/UserWidget.h"

void ABaseCharacterPlayerController::BeginPlay()
{
    Super::BeginPlay();
    

        UUserWidget* PlayerHUD = CreateWidget(this, PlayerHUDClass);
        if (PlayerHUD != nullptr)
        {
            PlayerHUD->AddToViewport();
        }
    
}
