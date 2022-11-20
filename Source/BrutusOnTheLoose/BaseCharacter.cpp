// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameEngine.h"
#include "StartEndLine.h"
#define pass (void)0





// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

float ABaseCharacter::myWidgetTimeUpdate() const
{
	return fTimeSinceStartLine;
}
// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	//There may be other ways to implement the same thing to tack them out of tick which is costly
	Super::Tick(DeltaTime);
	if(bTimerStarted)
	{
		fTimeSinceStartLine = GetWorld()->GetTimeSeconds()-fStartTime;
	}

	CrouchTimelineHandle.TickTimeline(DeltaTime);
	SlideCamTiltTimelineHandle.TickTimeline(DeltaTime);

	if(MovementState == EMovementState::Crouching)
	{
		if(CanStand())
		{
			ResolveMovementState();
		}
	}
	//Constantly updates slide speed based off of changing angle of floor
	else if(MovementState == EMovementState::Sliding)
	{
		FVector FloorNormal = GetCharacterMovement()->CurrentFloor.HitResult.Normal;
		GetCharacterMovement()->AddForce(CalculateFloorInfluence(FloorNormal)); 
		
		if(GetCharacterMovement()->IsFalling())
		{
			EndCameraTilt();
			
		}	
	}
}


// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//MOVEMENT
	PlayerInputComponent->BindAxis(TEXT("WalkForwardBackward"), this, &ABaseCharacter::MoveForwardBackwards); //Write my own functions
	PlayerInputComponent->BindAxis(TEXT("WalkLeftRight"), this, &ABaseCharacter::StrafeLeftRight);			//Write my own functions 
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump); //  CHANGE TO MY OWN IN FUTURE 
//Blueprint was sprinting during slide becuase C++ code was enableing sprinting

	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &ABaseCharacter::SprintPressed);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &ABaseCharacter::SprintReleased);


	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ABaseCharacter::CrouchPressed);                  
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &ABaseCharacter::CrouchReleased); 
	//CAMERA
	PlayerInputComponent->BindAxis(TEXT("CameraVertical"), this, &ABaseCharacter::CameraVertical); //Going to have to make my own functions by copying the ideas of the reference path of these and creating a player controller
	PlayerInputComponent->BindAxis(TEXT("CameraHorizontal"), this, &APawn::AddControllerYawInput);
}
void ABaseCharacter::CameraVertical(float AxisValue)
{
	FRotator RelitveRotation = Camera->GetRelativeRotation();
	float NewPitch = RelitveRotation.Pitch + (-AxisValue * PitchSens);
	if (NewPitch <= 90 && NewPitch>= -90)
	{
		Camera->SetRelativeRotation(FRotator(NewPitch , RelitveRotation.Yaw, RelitveRotation.Roll));
	}
}

EMovementState ABaseCharacter::GetMovementState()
{
	return MovementState;
}

void ABaseCharacter::MoveForwardBackwards(float AxisValue)
{
	if((Controller != NULL) && (AxisValue!=0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(9, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
	
}

void ABaseCharacter::StrafeLeftRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void ABaseCharacter::CrouchTimeline(float Value)
{
	//PROBLEM: This function is not being called
	
	float CurrentCameraHeight = FMath::GetMappedRangeValueClamped(TRange<float>(0.f, 1.f), TRange<float>(CrouchingCameraHeight, StandingCameraHeight), Value);
	Camera->SetRelativeLocation(FVector(Camera->GetRelativeLocation().X, Camera->GetRelativeLocation().Y, CurrentCameraHeight));
	float CurrentCapHalfHeight = FMath::GetMappedRangeValueClamped(TRange<float>(0.f, 1.f), TRange<float>(CrouchingCapsuleHalfHeight, StandingCapsuleHalfHeight), Value);
	GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentCapHalfHeight, true);
}

void ABaseCharacter::SlideCamTiltTimeline(float Value)
{
	
	FRotator TiltRollPitchYaw = FRotator(GetController()->GetControlRotation().Pitch, GetController()->GetControlRotation().Yaw, Value);

	GetController()->SetControlRotation(TiltRollPitchYaw);
}
void ABaseCharacter::SprintPressed()
{
	bSprintKeyDown = true;
	
	if (MovementState == EMovementState::Walking)
	{
		ResolveMovementState();
	}
	if (MovementState == EMovementState::Crouching)
	{
		ResolveMovementState();
	}

}
void ABaseCharacter::SprintReleased()
{
	bSprintKeyDown = false;
	if (MovementState == EMovementState::Sprinting)
	{
		ResolveMovementState();
	}
}

void ABaseCharacter::CrouchPressed()
{
	bCrouchKeyDown = true;
	if (MovementState == EMovementState::Walking)
	{
		
		if(GetCharacterMovement()->IsFalling())
		{
			SetMovemementState(Sliding);
		}
		else
		{
			SetMovemementState(EMovementState::Crouching);
		}
	}
	else if (MovementState == EMovementState::Sprinting)
	{
		SetMovemementState(EMovementState::Sliding);
	}	
}

void ABaseCharacter::CrouchReleased()
{
	bCrouchKeyDown = false;
	if (MovementState == EMovementState::Sliding)
	{		
		SetMovemementState(EMovementState::Crouching);
	}
}

void ABaseCharacter::BeginCameraTilt()
{
		//Begin camera tilt
	if(SlideCamTiltCurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("SlideCamTiltTimeline"));
		SlideCamTiltTimelineHandle.AddInterpFloat(SlideCamTiltCurveFloat, TimelineProgress);
		SlideCamTiltTimelineHandle.SetLooping(false);
		SlideCamTiltTimelineHandle.PlayFromStart();
	}
	
}

void ABaseCharacter::EndCameraTilt()
{
	if(SlideCamTiltCurveFloat)
		{	
			FOnTimelineFloat TimelineProgress;
			TimelineProgress.BindUFunction(this, FName("SlideCamTiltTimeline"));
			SlideCamTiltTimelineHandle.AddInterpFloat(SlideCamTiltCurveFloat, TimelineProgress);
			SlideCamTiltTimelineHandle.SetLooping(false);
			SlideCamTiltTimelineHandle.Reverse();
		}
}


void ABaseCharacter::BeginCrouch()
{
	if(CurveFloat)
	{	
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("CrouchTimeline"));
		CrouchTimelineHandle.AddInterpFloat(CurveFloat, TimelineProgress);
		CrouchTimelineHandle.SetLooping(false);
		CrouchTimelineHandle.PlayFromStart();
	}
}

void ABaseCharacter::EndCrouch()
{
	if(CurveFloat)
	{
		
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("CrouchTimeline"));
		CrouchTimelineHandle.AddInterpFloat(CurveFloat, TimelineProgress);
		CrouchTimelineHandle.SetLooping(false);
		CrouchTimelineHandle.Reverse();

	}
}

void ABaseCharacter::BeginSlide()
{
	IsSliding = true;
	if(!GetCharacterMovement()->IsFalling())
	{
		BeginCameraTilt();	
	}
	if(GetVelocity().Size() > MaxSlideSpeed)
	{			
		GetCharacterMovement()->Velocity = MaxSlideSpeed * GetVelocity().GetSafeNormal(0.0001f);
	}
	if(GetVelocity().Size() < MinSlideSpeed)
	{
		ResolveMovementState();
	}
	
}


void ABaseCharacter::EndSlide()
{
	//Might be useful later
	IsSliding = false;
	EndCameraTilt();
}


FVector ABaseCharacter::CalculateFloorInfluence(FVector FloorNormal)
{
	FVector VectorUp = FVector(0.f, 0.f, 1.f);
	if(FloorNormal == VectorUp)
	{
		return FVector(0.f, 0.f, 0.f);
	}
	FVector CP = FVector::CrossProduct(FVector::CrossProduct(VectorUp, FloorNormal), FloorNormal).GetSafeNormal(0.0001f);
	float DP = SlopeSlideInfluence * FMath::Clamp(1 - FVector::DotProduct(VectorUp, FloorNormal), 0.f, 1.f);

	return CP * DP;
}
void ABaseCharacter::SetMovemementState(EMovementState NewMovementState)
{
	EMovementState PreviousMovementState;

	if (NewMovementState != MovementState)
	{
		PreviousMovementState = MovementState;
		MovementState = NewMovementState;
		OnMovementStateChanged(PreviousMovementState);
		if (MovementState == EMovementState::Walking)
		{
			if(PreviousMovementState == EMovementState::Crouching)
			{
				EndCrouch();
			}
			
		}
		else if (MovementState == EMovementState::Sprinting)
		{
			if(PreviousMovementState == EMovementState::Crouching)
			{
				EndCrouch();
			}
		}
		else if (MovementState == EMovementState::Crouching)
		{
			BeginCrouch();
		}
		else if (MovementState == EMovementState::Sliding)
		{
			BeginCrouch();
			BeginSlide();
		}
	}
}

void ABaseCharacter::OnMovementStateChanged(EMovementState PreviousMovementState)
{

	//select
	if (MovementState == EMovementState::Walking)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else if (MovementState == EMovementState::Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else if (MovementState == EMovementState::Crouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	}
	else if (MovementState == EMovementState::Sliding)
	{
		GetCharacterMovement()->MaxWalkSpeed = 0;
	}

	if (PreviousMovementState == EMovementState::Sliding)
	{
		GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
		
		GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDeceleration;
		EndSlide();
	}

	if (MovementState == EMovementState::Sliding)
	{
		if(!GetCharacterMovement()->IsFalling())
		{
			GetCharacterMovement()->Velocity = GetActorForwardVector() * SprintSpeed;
		}

		GetCharacterMovement()->GroundFriction = SlidingGroundFriction;
		GetCharacterMovement()->BrakingDecelerationWalking = SlidingBrakingDeceleration;
	}

}

void ABaseCharacter::ResolveMovementState()
{
	if(CanSprint())
	{
		SetMovemementState(EMovementState::Sprinting);
		return;
	}
	else if(CanStand())
	{
		SetMovemementState(EMovementState::Walking);
		return;
	}
	else if(!CanStand())
	{
		SetMovemementState(EMovementState::Crouching);
		return;
	}
}

bool ABaseCharacter::CanStand()
{
	if (bCrouchKeyDown)
	{
		return false;
	}

	FVector Feet = GetActorLocation() - FVector(0.f, 0.f ,GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	FVector Head = FVector(0.f,0.f,StandingCapsuleHalfHeight*2) + Feet;
	FHitResult nothing;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	//Ignores character or AI
	Params.AddIgnoredActor(GetOwner());
	bool Result = GetWorld()->LineTraceSingleByChannel(nothing, Feet, Head, ECollisionChannel::ECC_Visibility, Params);
	return !(Result);
}

bool ABaseCharacter::CanSprint()
{
	if(bSprintKeyDown)
	{
		return (!GetCharacterMovement()->IsFalling() && CanStand());
	}	
	return false;	
}
void ABaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if(MovementState == EMovementState::Sliding)
	{
		FTimerHandle TiltTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TiltTimerHandle, this, &ABaseCharacter::BeginCameraTilt, 0.01f, false);
		
	}
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), LandingSound, GetActorLocation());
}

void ABaseCharacter::SetStartTime(float val)
{
	fStartTime = val;
}

void ABaseCharacter::SetbTimerStarted(bool val)
{
	bTimerStarted = val;
}

void ABaseCharacter::SetTimeRankWidget()
{
	if(fTimeSinceStartLine <= BestTime)
	{
		TimeRankID = 1;
	}
	else if(fTimeSinceStartLine <= GoodTime)
	{
		TimeRankID = 2;
	}
	else if(fTimeSinceStartLine <= OkTime)
	{
		TimeRankID = 3;
	}
	else
	{
		TimeRankID = 4;
	}
}

int32 ABaseCharacter::GetTimeRankID()
{
	return TimeRankID;
}

