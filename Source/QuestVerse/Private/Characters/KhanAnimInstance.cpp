#include "Characters/KhanAnimInstance.h"
#include "Characters/Khan.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"

void UKhanAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	KhanCharacter = Cast<AKhan>(TryGetPawnOwner());
	if (KhanCharacter)
	{
		KhanCharacterMovement = KhanCharacter->GetCharacterMovement();
	}
}

void UKhanAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (KhanCharacterMovement && KhanCharacter)
	{
		fGroundSpeed = UKismetMathLibrary::VSizeXY(KhanCharacterMovement->Velocity);
		fDirection = UKismetAnimationLibrary::CalculateDirection(KhanCharacter->GetVelocity(), KhanCharacter->GetActorRotation());
		SetIsinAir();
		SetIsAccelerating();
		CharacterState = KhanCharacter->GetCharacterState();

		// AimOffsets
		rBaseAimRotation = KhanCharacter->GetBaseAimRotation();
		FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(FRotationMatrix::MakeFromX(vVelocity).Rotator(), rBaseAimRotation);
		fYawOffset = GetYawFromRotator(DeltaRotator);

		// Turn In Place
		fDeltaTimeX = DeltaTime;
		TurnInPlace();
	}
}

void UKhanAnimInstance::SetIsAccelerating()
{
	if (KhanCharacterMovement->GetCurrentAcceleration().Length() > 0.f)
		bIsAccelerating = true;
	else
		bIsAccelerating = false;
}

void UKhanAnimInstance::SetIsinAir()
{
	if (KhanCharacterMovement->IsFalling())
		bIsInAir = true;
	else
		bIsInAir = false;
}

float UKhanAnimInstance::GetYawFromRotator(const FRotator& Rotator)
{
	float fYaw{};
	float fPitch{};
	float fRoll{};

	UKismetMathLibrary::BreakRotator(Rotator, fYaw, fPitch, fRoll);

	return fYaw;
}

void UKhanAnimInstance::TurnInPlace()
{
	if ((fGroundSpeed > 0.f) || bIsInAir)
	{
		fRootYawOffset = UKismetMathLibrary::FInterpTo(fRootYawOffset, 0, fDeltaTimeX, 20);
		rMovingRotation = KhanCharacter->GetActorRotation();
		rLastMovingRotation = rMovingRotation;
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Do it"));
	}
	else
	{
		// Calculate Root Yaw Offset
		rLastMovingRotation = rMovingRotation;
		rMovingRotation = KhanCharacter->GetActorRotation();
		fRootYawOffset = fRootYawOffset - UKismetMathLibrary::NormalizedDeltaRotator(rMovingRotation, rLastMovingRotation).Yaw;

		FString YourVariableString = FString::Printf(TEXT("RootYawOffset: %f"), fRootYawOffset);
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, YourVariableString);

		if (GetCurveValue(FName(TEXT("Turning"))) > 0)
		{
			fLastDistanceCurve = fDistanceCurve;
			fDistanceCurve = GetCurveValue(FName(TEXT("DistanceCurve")));
			fDeltaDistanceCurve = fDistanceCurve - fLastDistanceCurve;
			if (fRootYawOffset > 0)
			{
				fRootYawOffset = fRootYawOffset - fDeltaDistanceCurve;
			}
			else
			{
				fRootYawOffset = fRootYawOffset + fDeltaDistanceCurve;
			}
			fAbsRootYawOffset = UKismetMathLibrary::Abs(fRootYawOffset);

			if (fAbsRootYawOffset > 90)
			{
				fYawExcess = fAbsRootYawOffset - 90;
			}
			if (fRootYawOffset > 0)
			{
				fRootYawOffset = fRootYawOffset - fYawExcess;
			}
			else
			{
				fRootYawOffset = fRootYawOffset + fYawExcess;
			}
		}
	}
}
