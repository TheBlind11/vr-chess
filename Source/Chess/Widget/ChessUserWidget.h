// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChessUserWidget.generated.h"

UCLASS()
class CHESS_API UChessUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	//These are events implemented on blueprint, so have a look on blueprint graph in ChessUIWidget
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayClaimDrawButton();

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveClaimDrawButton();

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayErrorMessage();

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayOfferDrawResignButtons();

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayCanAcceptDrawButtons();

	UFUNCTION(BlueprintImplementableEvent)
	void EnableOfferDrawButton();

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveCanAcceptDrawButtons();

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayNicknameBoxes();

	//Function useful to get main player nickname text block in ChessUIWidget
	class UTextBlock* GetNicknameTextBlock();

	//Function useful to get opponent nickname text block in ChessUIWidget
	class UTextBlock* GetOpponentNicknameTextBlock();

private:

	//This is opponent nickname text block
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* OpponentNicknameTextBlock = nullptr;

	//This is main player nickname text block
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NicknameTextBlock = nullptr;
	
};
