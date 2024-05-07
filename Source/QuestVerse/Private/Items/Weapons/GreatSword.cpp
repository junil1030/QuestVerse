#include "Items/Weapons/GreatSword.h"
#include "Characters/Khan.h"

AGreatSword::AGreatSword()
{
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BindOverlapEvent();
}

void AGreatSword::attackEnable(bool Enable)
{
	if (Enable)
	{
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("On"));
	}
	else
	{
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Off"));
	}
}

void AGreatSword::BindOverlapEvent()
{
	if (ItemMesh != nullptr)
	{
		ItemMesh->OnComponentBeginOverlap.AddDynamic(this, &AGreatSword::OnOverlapBegin);
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Binding"));
	}
}

void AGreatSword::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKhan* pKhan = Cast<AKhan>(OtherActor);
	if (pKhan)
	{
		pKhan->hitEvent();
	}
}
