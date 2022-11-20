
//Note: might need to specify Enum type "EMovementState::(themovementstate)" in parameter on SetMovementState()
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "CPP_EMovementState.h"


#include "BaseCharacter.generated.h"

UCLASS()
class BRUTUSONTHELOOSE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit);
	
private:

	//Misc functions and vars:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerComps", meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerComps", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* PlayerMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerComps", meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float StandingCapsuleHalfHeight = 96.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float CrouchingCapsuleHalfHeight = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float StandingCameraHeight = 64.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float CrouchingCameraHeight = 40.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float SprintSpeed = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))	
		float WalkSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))	
		float CrouchSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))	
		float DefaultGroundFriction = 8.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))	
		float SlidingGroundFriction = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))	
		float DefaultBrakingDeceleration = 2048.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))	
		float SlidingBrakingDeceleration = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))	
		float SlopeSlideInfluence = 800000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))	
		bool IsSliding = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float PitchSens = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing", meta = (AllowPrivateAccess = "true"))
		float BestTime = 37.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing", meta = (AllowPrivateAccess = "true"))
		float GoodTime = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing", meta = (AllowPrivateAccess = "true"))
		float OkTime = 90.f;
	UPROPERTY(EditAnywhere)
		USoundBase* LandingSound;
		float MaxSlideSpeed = 1000.f;
		float MinSlideSpeed = 0.f;
		bool bCrouchKeyDown = false;
		bool bSprintKeyDown = false;
		float fStartTime = 0;
		bool bTimerStarted = false;
		float fTimeSinceStartLine = 0.0f;
		void CameraVertical(float AxisValue);

	//Slide functions
	
	EMovementState MovementState; 
	UFUNCTION(BlueprintCallable)
	EMovementState GetMovementState();
	void SetMovemementState(EMovementState NewMovementState);
	void OnMovementStateChanged(EMovementState PreviousMovementState);
	void ResolveMovementState();
	FVector CalculateFloorInfluence(FVector FloorNormal);
	bool CanStand();
	bool CanSprint();
	void BeginSlide();
	void EndSlide();
	void BeginCrouch();
	void EndCrouch();
	void StrafeLeftRight(float AxisValue);
	void MoveForwardBackwards(float AxisValue);
	void SprintPressed();
	void CrouchPressed();
	void SprintReleased();
	void CrouchReleased();
	
	UFUNCTION(BlueprintCallable)
		void BeginCameraTilt();
	UFUNCTION(BlueprintCallable)
		void EndCameraTilt();

	
public:	
	ABaseCharacter();

	UFUNCTION(BlueprintPure)
		float myWidgetTimeUpdate() const;

	UFUNCTION(BlueprintPure)
		int32 GetTimeRankID();
	UPROPERTY(EditAnywhere, Category = "Timeline")
		class UCurveFloat* CurveFloat;
	UFUNCTION()
		void CrouchTimeline(float Value);
	UPROPERTY()
		FTimeline CrouchTimelineHandle;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		class UCurveFloat* SlideCamTiltCurveFloat;
	UFUNCTION()
		void SlideCamTiltTimeline(float Value);
	UPROPERTY()
		FTimeline SlideCamTiltTimelineHandle;
	void SetStartTime(float val);
	void SetbTimerStarted(bool val);
	void SetTimeRankWidget();
	int32 TimeRankID = 0;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
};
