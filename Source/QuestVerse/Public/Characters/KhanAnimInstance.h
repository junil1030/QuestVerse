#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "KhanAnimInstance.generated.h"

UCLASS()
class QUESTVERSE_API UKhanAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	void SetIsAccelerating();
	void SetIsinAir();
	float GetYawFromRotator(const FRotator& Rotator);
	void TurnInPlace();

	UPROPERTY(BlueprintReadOnly)
	class AKhan* KhanCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* KhanCharacterMovement;


	// Move
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float fGroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float fDirection;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FVector vVelocity;

	// Turn In Place
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FRotator rBaseAimRotation;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FRotator rMovingRotation;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FRotator rLastMovingRotation;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float fYawOffset;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float fRootYawOffset;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float fAbsRootYawOffset;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float fDeltaTimeX;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float fDistanceCurve;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float fLastDistanceCurve;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float fDeltaDistanceCurve;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float fYawExcess;

	ECharacterState CharacterState;
};
