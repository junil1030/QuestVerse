// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Bow.generated.h"

/**
 * 
 */
UCLASS()
class QUESTVERSE_API ABow : public AItem
{
	GENERATED_BODY()
public:
	ABow();

public:
	UFUNCTION()
	void attackEnable(bool Enable);
	void BindOverlapEvent();

	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
