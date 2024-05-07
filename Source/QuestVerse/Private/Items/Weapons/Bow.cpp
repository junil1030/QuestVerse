// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Bow.h"

ABow::ABow()
{
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BindOverlapEvent();
}

void ABow::attackEnable(bool Enable)
{

}

void ABow::BindOverlapEvent()
{

}

void ABow::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}
