#include "Characters/Khan.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/GreatSword.h"
#include "Items/Weapons/Bow.h"
#include "Items/Weapons/Arrow.h"
#include "Widget/BowCrosshairWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

AKhan::AKhan()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 800.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);
	ViewCamera->bUsePawnControlRotation = false;

	ArrowPathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("ArrowPathSpline"));
	ArrowPathSpline->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);

	ArcEndSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArcEndSphere"));
	ArcEndSphere->SetupAttachment(GetRootComponent());

	SceneWeaponGreatSword = CreateDefaultSubobject<USceneComponent>(TEXT("SceneWeaponGreatSword"));
	SceneWeaponGreatSword->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RightHandSocket"));

	SceneWeaponBow = CreateDefaultSubobject<USceneComponent>(TEXT("SceneWeaponBow"));
	SceneWeaponBow->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("LeftHandSocket"));
}

void AKhan::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CharacterMappingContext, 0);
		}
	}

	// Camera Location
	FOnTimelineFloat InterpFunction;
	InterpFunction.BindUFunction(this, FName("InterpolateCameraLocation"));
	CameraTimeline.AddInterpFloat(InterpolationCurve, InterpFunction);

	CameraTimeline.SetLooping(false);
	CameraTimeline.SetTimelineLength(TL_TimelineLength);
	bIsInterpolating = false;
}

void AKhan::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AKhan::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AKhan::Walk(const FInputActionValue& value)
{

}

void AKhan::Interact(const FInputActionValue& value)
{

}

void AKhan::Attack(const FInputActionValue& value)
{
	if (CharacterState == ECharacterState::ECS_EquippedGreatSword)
	{
		UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
		if (!(pAnimInst->Montage_IsPlaying(m_AMGreatSwordAttack)))
		{
			if (pAnimInst != nullptr && m_AMGreatSwordAttack != nullptr)
			{
				pAnimInst->Montage_Play(m_AMGreatSwordAttack);
			}
		}
		else
		{
			m_iComboAttackIndex = 1;
		}
	}
	if (CharacterState == ECharacterState::ECS_EquippedBow && bCanBowFire)
	{
		bCanBowFire = false;
		UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		World->SpawnActor<AArrow>(ActorToSpawnArrow, UKismetMathLibrary::MakeTransform(vArrowSpawnLocation, rArrowSpawnRotation), SpawnParams);
		pAnimInst->Montage_Play(m_AMBowAttack);
	}
}

void AKhan::ChangeBowWeapon(const FInputActionValue& value)
{
	if (CharacterState != ECharacterState::ECS_EquippedBow)
	{
		SpawnBow();
		CreateWidgetCrosshair();
		ToggleInterpolation();
	}
	else
	{
		DestroyBow();
		RemoveWidgetCrosshair();
		ToggleInterpolation();
		ClearArc();
	}
}

void AKhan::InterpolateCameraLocation(float Value)
{
	if (InterpolationCurve)
	{
		float CurveValue = InterpolationCurve->GetFloatValue(Value);

		CameraBoom->SocketOffset.X = UKismetMathLibrary::Lerp(0.f, 200.f, CurveValue); //120
		CameraBoom->SocketOffset.Y = UKismetMathLibrary::Lerp(0.f, 40.f, CurveValue); //120
		CameraBoom->SocketOffset.Z = UKismetMathLibrary::Lerp(0.f, 50.f, CurveValue); //75
		//CameraBoom->SocketOffset.X
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Value: %f"), CurveValue));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("InterpolateCamera"));
	}
}

void AKhan::ResetCameraLocation()
{
	CameraTimeline.Reverse();
}

void AKhan::ToggleInterpolation()
{
	if (bIsInterpolating)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("CameraOffsetEnd"));
		ResetCameraLocation();
		bIsInterpolating = false;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("CameraOffsetStart"));
		CameraTimeline.PlayFromStart();
		bIsInterpolating = true;
	}
}

void AKhan::ChangeGreatSwordWeapon(const FInputActionValue& value)
{
	if (bCanBowFire)
	{
		if (CharacterState != ECharacterState::ECS_EquippedGreatSword)
		{
			SpawnGreatSword();
		}
		else
		{
			DestroyGreatSword();
		}
	}
}

void AKhan::Change8WayDirectionalMovement()
{
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void AKhan::Change4WayDirectionalMovement()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

void AKhan::SpawnBow()
{
	if (CharacterState == ECharacterState::ECS_EquippedGreatSword)
		DestroyGreatSword();

	Change8WayDirectionalMovement();
	//UWorld* const World = GetWorld();
	if (World)
	{
		if (SceneWeaponBow)
		{
			FVector SpawnLocation = SceneWeaponBow->GetComponentLocation();
			FRotator SpawnRotation = SceneWeaponBow->GetComponentRotation();

			SpawnedBow = World->SpawnActor<ABow>(ActorToSpawnBow, SpawnLocation, SpawnRotation);
			SpawnedBow->AttachToComponent(SceneWeaponBow, FAttachmentTransformRules::SnapToTargetIncludingScale);

			CharacterState = ECharacterState::ECS_EquippedBow;
			ChangeAnimationBluprint(ABP_Bow);

			UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
			if (pAnimInst != nullptr)
			{
				pAnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &AKhan::HandleOnMontageNotifyBegin);
			}
		}
	}
}

void AKhan::SpawnGreatSword()
{
	if (CharacterState == ECharacterState::ECS_EquippedBow)
	{
		DestroyBow();
		RemoveWidgetCrosshair();
	}

	Change8WayDirectionalMovement();
	//UWorld* const World = GetWorld();
	if (World)
	{
		if (SceneWeaponGreatSword)
		{
			FVector SpawnLocation = SceneWeaponGreatSword->GetComponentLocation();
			FRotator SpawnRotation = SceneWeaponGreatSword->GetComponentRotation();

			SpawnedGreatSword = World->SpawnActor<AGreatSword>(ActorToSpawnGreatSword, SpawnLocation, SpawnRotation);
			SpawnedGreatSword->AttachToComponent(SceneWeaponGreatSword, FAttachmentTransformRules::SnapToTargetIncludingScale);

			CharacterState = ECharacterState::ECS_EquippedGreatSword;
			ChangeAnimationBluprint(ABP_GreatSword);

			UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
			if (pAnimInst != nullptr)
			{
				pAnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &AKhan::HandleOnMontageNotifyBegin);
			}
		}
	}
}

void AKhan::DestroyBow()
{
	//UWorld* const World = GetWorld();
	CharacterState = ECharacterState::ECS_Unequipped;
	Change4WayDirectionalMovement();
	ChangeAnimationBluprint(ABP_TravelMode);
	if (SpawnedBow)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Destroy"));
		SpawnedBow->Destroy();
		SpawnedBow = nullptr;
	}
}

void AKhan::DestroyGreatSword()
{
	//UWorld* const World = GetWorld();
	CharacterState = ECharacterState::ECS_Unequipped;
	Change4WayDirectionalMovement();
	ChangeAnimationBluprint(ABP_TravelMode);
	if (SpawnedGreatSword)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Destroy"));
		SpawnedGreatSword->Destroy();
		SpawnedGreatSword = nullptr;
	}
}

void AKhan::CreateWidgetCrosshair()
{
	if (m_BowCrosshairWidgetclass)
	{
		m_BowCrosshairWidget = Cast<UBowCrosshairWidget>(CreateWidget(GetWorld(), m_BowCrosshairWidgetclass));
		if (m_BowCrosshairWidget)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Create UI"));
			m_BowCrosshairWidget->AddToViewport();
		}
	}
}

void AKhan::RemoveWidgetCrosshair()
{
	if (m_BowCrosshairWidgetclass)
	{
		//m_BowCrosshairWidget = Cast<UBowCrosshairWidget>(CreateWidget(GetWorld(), m_BowCrosshairWidgetclass));
		if (m_BowCrosshairWidget)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Remove UI"));
			m_BowCrosshairWidget->RemoveFromParent();
		}
	}
}

void AKhan::TraceForArrow()
{
	double dDeltaSecond = UGameplayStatics::GetWorldDeltaSeconds(World);
	GetStartAndEndForTrace();
	GetArrowSpawnLocationAndRotation();

	vArrowSpawnLocation = UKismetMathLibrary::VInterpTo(vArrowSpawnLocation, vTargetArrowSpawnLocation, dDeltaSecond, 30.f);
	rArrowSpawnRotation = UKismetMathLibrary::RInterpTo(rArrowSpawnRotation, rTargetArrowSpawnRotation, dDeltaSecond, 30.f);
}

void AKhan::GetStartAndEndForTrace()
{
	FVector vCameraLocation = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
	FVector vActorForwardVector = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetActorForwardVector();

	FVector vSacleForwardVector = UKismetMathLibrary::Multiply_VectorVector(vActorForwardVector, FVector(15000, 15000, 15000));

	vCrosshairWorldLocation = vCameraLocation;
	vImpactPoint = UKismetMathLibrary::Add_VectorVector(vSacleForwardVector, vCameraLocation);
}

void AKhan::GetArrowSpawnLocationAndRotation()
{
	TArray<AActor*> arActorsToIgnore;
	bool bTraceComplex = false;
	bool bIgnoreSelf = true;
	FHitResult fOutHit;
	FRotator rRotation;
	FVector vScale;

	UKismetSystemLibrary::LineTraceSingle(
		this, vCrosshairWorldLocation, vImpactPoint, 
		ETraceTypeQuery::TraceTypeQuery1, bTraceComplex, 
		arActorsToIgnore, EDrawDebugTrace::None, fOutHit, bIgnoreSelf
	);

	if (fOutHit.bBlockingHit)
	{
		vImpactPoint = fOutHit.ImpactPoint;
		UKismetMathLibrary::BreakTransform(GetMesh()->GetSocketTransform(TEXT("arrow_socket")), vTargetArrowSpawnLocation, rRotation, vScale);
	}
	else
	{
		UKismetMathLibrary::BreakTransform(GetMesh()->GetSocketTransform(TEXT("arrow_socket")), vTargetArrowSpawnLocation, rRotation, vScale);
	}

	FVector vVectorFromArrowSpawnLocationToImpactPoint = UKismetMathLibrary::Subtract_VectorVector(vImpactPoint, vTargetArrowSpawnLocation);
	rTargetArrowSpawnRotation = UKismetMathLibrary::MakeRotFromX(vVectorFromArrowSpawnLocationToImpactPoint);
}

void AKhan::ProjectilePath()
{
	TArray<AActor*> arActorsToIgnore;
	FVector vArrowRotationToVector = UKismetMathLibrary::GetForwardVector(rArrowSpawnRotation);
	FVector vLaunchVelocity = UKismetMathLibrary::Multiply_VectorVector(vArrowRotationToVector, FVector(5000.f, 5000.f, 5000.f));
	FPredictProjectilePathParams PathParams;
	PathParams.StartLocation = vArrowSpawnLocation;
	PathParams.LaunchVelocity = vLaunchVelocity;
	PathParams.bTraceWithCollision = true;
	PathParams.bTraceWithChannel = true;
	PathParams.ProjectileRadius = 5.f;
	PathParams.MaxSimTime = 5.f;
	PathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
	PathParams.ActorsToIgnore = arActorsToIgnore;
	PathParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;

	// Update Arc Spline
	FPredictProjectilePathResult m_ArrowPathResult;
	UGameplayStatics::PredictProjectilePath(World, PathParams, m_ArrowPathResult);

	for (FPredictProjectilePathPointData element : m_ArrowPathResult.PathData)
	{
		ArrowPathSpline->AddSplinePoint(element.Location, ESplineCoordinateSpace::Local, true);
		ArcEndSphere->SetWorldLocation(vFinalArcLocation);
	}
	int32 iNum{};
	m_ArrowPathResult.PathData.Last(iNum);
	ArrowPathSpline->SetSplinePointType(iNum, ESplinePointType::CurveClamped);
	FPredictProjectilePathPointData element = m_ArrowPathResult.PathData.Last(iNum);
	vFinalArcLocation = element.Location;

	int32 iIndex = ArrowPathSpline->GetNumberOfSplinePoints() - 2;
	for (int i = 0; i < iIndex; i++)
	{
		USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>(this);
		NewSplineMesh->SetStaticMesh(SplineMesh);

		int32 StartIndex = i;
		int32 EndIndex = i + 1;

		FVector StartLocation = ArrowPathSpline->GetLocationAtSplinePoint(StartIndex, ESplineCoordinateSpace::Local);
		FVector StartTangent = ArrowPathSpline->GetTangentAtSplinePoint(StartIndex, ESplineCoordinateSpace::Local);
		FVector EndLocation = ArrowPathSpline->GetLocationAtSplinePoint(EndIndex + 1, ESplineCoordinateSpace::Local);
		FVector EndTangent = ArrowPathSpline->GetTangentAtSplinePoint(EndIndex + 1, ESplineCoordinateSpace::Local);

		NewSplineMesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);

		NewSplineMesh->AttachToComponent(ArrowPathSpline, FAttachmentTransformRules::SnapToTargetIncludingScale);

		NewSplineMesh->RegisterComponent();

		arSplineMesh.Add(NewSplineMesh);
	}
}

void AKhan::ClearArc()
{
	for (USplineMeshComponent* cSplineMesh : arSplineMesh)
	{
		if (cSplineMesh)
		{
			cSplineMesh->DestroyComponent();
		}
	}
	arSplineMesh.Empty();

	ArrowPathSpline->ClearSplinePoints();
}

void AKhan::ChangeAnimationBluprint(TSubclassOf<UAnimInstance> AnimationBluprint)
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (CharacterMesh)
	{
		CharacterMesh->SetAnimInstanceClass(AnimationBluprint);
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void AKhan::hitEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("HIT!!!!!!!!!!!!"));
}

void AKhan::AttackEnable(bool bEnable)
{
	if (SpawnedGreatSword)
		SpawnedGreatSword->attackEnable(bEnable);
}

void AKhan::HandleOnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& pBranchingPayload)
{
	if (NotifyName == FName("PlayMontageNotify"))
	{
		m_iComboAttackIndex--;

		if (m_iComboAttackIndex < 0)
		{
			UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
			if (pAnimInst != nullptr)
			{
				pAnimInst->Montage_Stop(0.35f, m_AMGreatSwordAttack);
				m_iComboAttackIndex = 0;
			}
		}
	}

	if (NotifyName == FName("ResetBowAttack"))
	{
		bCanBowFire = true;
	}
}

void AKhan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraTimeline.TickTimeline(DeltaTime);
	if (CharacterState == ECharacterState::ECS_EquippedBow)
	{
		TraceForArrow();
		ClearArc();
		ProjectilePath();
	}
}

void AKhan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(IA_MoveAction, ETriggerEvent::Triggered, this, &AKhan::Move);

		//Looking
		EnhancedInputComponent->BindAction(IA_LookAction, ETriggerEvent::Triggered, this, &AKhan::Look);

		//Jumping
		EnhancedInputComponent->BindAction(IA_JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(IA_JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Walking
		EnhancedInputComponent->BindAction(IA_WalkAction, ETriggerEvent::Triggered, this, &AKhan::Walk);

		//Interaction
		EnhancedInputComponent->BindAction(IA_InterAction, ETriggerEvent::Triggered, this, &AKhan::Interact);

		//Attack
		EnhancedInputComponent->BindAction(IA_AttackAction, ETriggerEvent::Triggered, this, &AKhan::Attack);

		//WeaponMode
		EnhancedInputComponent->BindAction(IA_BowAction, ETriggerEvent::Triggered, this, &AKhan::ChangeBowWeapon);
		EnhancedInputComponent->BindAction(IA_GreatSwrodAction, ETriggerEvent::Triggered, this, &AKhan::ChangeGreatSwordWeapon);
	}
}