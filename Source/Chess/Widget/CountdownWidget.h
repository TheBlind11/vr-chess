// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget.h"
#include "CountdownWidget.generated.h"

UCLASS()
class CHESS_API UCountdownWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
public:
	
	//Function useful to update text block of CountdownWidget
	UFUNCTION()
	FText UpdateTimer();

	//Function useful to set countdown timer
	void SetCountdown(int seconds);

protected:
	
	//Function useful to initialize this widget when it is spawned in scene
	virtual bool Initialize() override;

	//Field of BaseTextBlock of CountdownWidget
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BaseTextBlock = nullptr;

	//Field of CountdownTextBlock of CountdownWidget
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountdownTextBlock = nullptr;

public:

	//Field of PlayerController 
	UPROPERTY()
	class AChessController* PlayerController = nullptr;
	
};
