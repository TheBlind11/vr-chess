// Fill out your copyright notice in the Description page of Project Settings.


#include "CountdownWidget.h"

#include "ChessController.h"
#include "TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UCountdownWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

//Function useful to initialize this widget when it is spawned in scene
bool UCountdownWidget::Initialize()
{
	bool Success = Super::Initialize();

	if (!Success)
	{
		return false;
	}

	if (CountdownTextBlock)  //If CountdownTextBlock is not null
	{
		CountdownTextBlock->TextDelegate.BindUFunction(this, "UpdateTimer");  //Delegate UpdateTimer to modify countdown timer text
	}

	return true;
}

//Function useful to set countdown timer
void UCountdownWidget::SetCountdown(int seconds)
{
	PlayerController = Cast<AChessController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PlayerController->SetSeconds(seconds);
}

//Function useful to update every text block of CountdownWidget
FText UCountdownWidget::UpdateTimer()
{
	FString countdown = PlayerController->UpdateTimerDisplay();
	if (FCString::Atoi(*countdown) == 0)  //If timer has finished
	{
		BaseTextBlock->SetText(FText::FromString("Game is about to start..."));  //Display this text on BaseTextBlock
		CountdownTextBlock->SetVisibility(ESlateVisibility::Hidden);  //Hide the countdown timer text
	}
	
	return FText::FromString(countdown);
}
