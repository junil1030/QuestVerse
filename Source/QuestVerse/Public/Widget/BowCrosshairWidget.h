// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BowCrosshairWidget.generated.h"

class UCanvasPanel;
class UImage;

UCLASS()
class QUESTVERSE_API UBowCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(Meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel = nullptr;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> CrosshairImage = nullptr;
};
