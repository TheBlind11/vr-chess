// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget.h"
#include "PromotionWidget.generated.h"

UCLASS()
class CHESS_API UPromotionWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:

	//These functions are called when player uses PromotionWidget buttons, check blueprint graph in PromotionWidget
	UFUNCTION(BlueprintCallable)
	FString OnQueenButtonClick();

	UFUNCTION(BlueprintCallable)
	FString OnKnightButtonClick();

	UFUNCTION(BlueprintCallable)
	FString OnRookButtonClick();
	
	UFUNCTION(BlueprintCallable)
	FString OnBishopButtonClick();
	
	void RemoveFormView();
};
