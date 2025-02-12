// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetComponent.h"
#include "Widget/ChessUserWidget.h"
#include "GameFramework/HUD.h"
#include "Widget/CountdownWidget.h"
#include "Widget/GameFinishedWidget.h"
#include "Widget/PromotionWidget.h"
#include "WidgetController.generated.h"

UCLASS()
class CHESS_API AWidgetController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWidgetController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootComp;

	// Un Widget Component per la ChessUI
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* ChessUIComponent;

	// Un Widget Component per il Countdown
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* CountdownComponent;

	// Un Widget Component per la promozione (es. PromotionWidget)
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* PromotionComponent;

	// Un Widget Component per la fine partita
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* GameFinishedComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Function to get ChessUIWidget
	UChessUserWidget* GetChessUI();

	void SetChessUI(UChessUserWidget* Widget);

	//Function to get ChessUIWidgetClass
	TSubclassOf<class UUserWidget> GetChessUIWidgetClass();

	UWidgetComponent* GetChessUIComponent();

	void ShowChessUI(bool ToShow);

	//Function to show CanAcceptDrawButtons when opponent offers a draw
	void ShowCanAcceptDrawWidget();

	//Function to show CanAcceptRematchButtons when opponent offers a rematch
	void ShowCanAcceptRematchWidget();

	//Function to enable OfferDrawButtons at the next turn
	void EnableOfferDrawButton();

	UWidgetComponent* GetCountdownUIComponent();

	//Function to show CountdownWidget in scene
	void ShowCountdownUI(bool ToShow);

	void ShowCountdown(int Seconds);

	//Function to remove CountdownWidget from scene when countdown finishes
	void RemoveCountdownUI();

	//PromotionWidget field
	UPROPERTY(BlueprintReadOnly)
	UPromotionWidget *PromotionWidget;

	//Function to show PromotionWidget in scene
	void DisplayPromotionWidgetUI(bool ToShow);

	UGameFinishedWidget* GetGameFinishedWidget();
	
	UWidgetComponent* GetGameFinishedUIComponent();

	//Function to show GameFinishedWidget in scene
	void ShowGameFinishedUI(bool ToShow);
	
	void ShowGameFinishedWidget(bool YouLost, const bool IsDraw, const FString& Reason, const int NftId);

	//Function to remove GameFinishedWidget from scene
	void RemoveGameFinishedUI();

private:

	//ChessUIWidget field
	UPROPERTY()
	UChessUserWidget *ChessUI;

	//CountdownWidget field
	UPROPERTY()
	UCountdownWidget *CountdownWidget;

	//GameFinishedWidgetField
	UPROPERTY()
	UGameFinishedWidget *GameFinishedWidget;

	//ChessUIWidgetClass field
	UPROPERTY()
	TSubclassOf<class UUserWidget> ChessUIWidgetClass;

	//CountdownWidgetClass field
	UPROPERTY()
	TSubclassOf<class UUserWidget> CountdownWidgetClass;

	//GameFinishedWidgetClass field
	UPROPERTY()
	TSubclassOf<class UUserWidget> GameFinishedWidgetClass;

	//PromotionWidgetClass field
	UPROPERTY()
	TSubclassOf<class UUserWidget> PromotionWidgetClass;
	
};
