#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "InputActionValue.h"
#include "Components/TimeLineComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Khan.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class AItem;
class AGreatSword;
class ABow;
class AArrow;
class UBowCrosshairWidget;

UCLASS()
class QUESTVERSE_API AKhan : public ACharacter
{
	GENERATED_BODY()

public:
	AKhan();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* CharacterMappingContext;

	//Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* IA_MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* IA_LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* IA_JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* IA_WalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* IA_InterAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* IA_AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* IA_GreatSwrodAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* IA_BowAction;

	// Character
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* ViewCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh")
	USplineComponent* ArrowPathSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh")
	UStaticMeshComponent* ArcEndSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh")
	UStaticMesh* SplineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh")
	TArray<USplineMeshComponent*> arSplineMesh;

	// WeaponSceneComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* SceneWeaponGreatSword;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* SceneWeaponBow;

	// WeaponActor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGreatSword> ActorToSpawnGreatSword;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABow> ActorToSpawnBow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AArrow> ActorToSpawnArrow;

	// AnimInstance
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> ABP_TravelMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> ABP_GreatSword;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> ABP_Bow;

	// AnimMontage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* m_AMGreatSwordAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* m_AMBowAttack;

	// Widget
	UPROPERTY(EditAnywhere, category = Widget)
	TSubclassOf<UUserWidget> m_BowCrosshairWidgetclass;

	UPROPERTY(VisibleInstanceOnly)
	UBowCrosshairWidget* m_BowCrosshairWidget;

	// Camera Socket Offset
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	UCurveFloat* InterpolationCurve;
	UPROPERTY()
	FTimeline CameraTimeline;
	FVector OriginalCameraLocation;
	FVector BowCameraLocation;
	bool bIsInterpolating;

	// InputAction
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Walk(const FInputActionValue& value);
	void Interact(const FInputActionValue& value);
	void Attack(const FInputActionValue& value);
	void ChangeBowWeapon(const FInputActionValue& value);
	void ChangeGreatSwordWeapon(const FInputActionValue& value);

	// Change Movement
	void Change8WayDirectionalMovement();
	void Change4WayDirectionalMovement();

	// SpawnWeapon
	void SpawnBow();
	void SpawnGreatSword();

	// DestroyWeapon
	void DestroyBow();
	void DestroyGreatSword();

	// Crosshair
	void CreateWidgetCrosshair();
	void RemoveWidgetCrosshair();

	/* --- Bow Trace and Arc Spline Function  ---*/
	// 1. Get Startand End for Trace
	void TraceForArrow();
	void GetStartAndEndForTrace();
	void GetArrowSpawnLocationAndRotation();
	void ProjectilePath();
	void ClearArc();

	/* --- Bow Trace and Arc Spline Variable  ---*/
	FVector vCrosshairWorldLocation;
	FVector vImpactPoint;
	FVector vArrowSpawnLocation;
	FVector vTargetArrowSpawnLocation;
	FVector vFinalArcLocation;

	FRotator rArrowSpawnRotation;
	FRotator rTargetArrowSpawnRotation;

	bool bCanBowFire = true;

	// Change Animation
	void ChangeAnimationBluprint(TSubclassOf<UAnimInstance> AnimationBluprint);

	int m_iComboAttackIndex = 0;
	AGreatSword* SpawnedGreatSword;
	ABow* SpawnedBow;

	UWorld* const World = GetWorld();
	
private:
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;


public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	void hitEvent();
	void AttackEnable(bool bEnable);

	UFUNCTION()
	void HandleOnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& pBranchingPayload);

	// Camera
	UFUNCTION()
	void InterpolateCameraLocation(float Value);
	UFUNCTION()
	void ResetCameraLocation();
	UFUNCTION()
	void ToggleInterpolation();
};
