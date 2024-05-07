#include "Characters/Attack/KhanNotifyState.h"
#include "Characters/Khan.h"

void UKhanNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AKhan* pKhan = Cast<AKhan>(MeshComp->GetOwner());

	if (pKhan)
		pKhan->AttackEnable(true);
}

void UKhanNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AKhan* pKhan = Cast<AKhan>(MeshComp->GetOwner());

	if (pKhan)
		pKhan->AttackEnable(false);
}