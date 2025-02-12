// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Button.h"
#include "UserWidget.h"
#include "GameFinishedWidget.generated.h"

UCLASS()
class CHESS_API UGameFinishedWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	//Function useful to initialize this widget when it is spawned in scene
	virtual bool Initialize() override;

public:

	//Function useful to to update every text block of GameFinishedWidget
	void UpdateReason(bool ILost, bool IsDraw);
	
	//Function useful to set up GameFinishedWidget
	void SetUp(bool ILost, const FString& Reason, const bool IsDraw, const int NftId);

	//Function useful to get the reason that caused the end of the game
	FString GetReason();

	//Function useful to set the reason that caused the end of the game
	void SetReason(const FString& Reason);

	//Function useful to to update NftImage of GameFinishedWidget if player wins
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateNftImage(int NftId); 

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayCanAcceptRematchButtons();

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayAndEnableRematchButton();

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayPrizeButton();

protected:

	//BaseTextBlock field
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BaseTextBlock = nullptr;

	//ReasonTextBlock field
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ReasonText = nullptr;

private:

	//Reason field
	FString Reason;
};
