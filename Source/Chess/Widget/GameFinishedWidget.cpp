// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFinishedWidget.h"

#include "Image.h"
#include "TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UGameFinishedWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

//Function useful to initialize this widget when it is spawned in scene
bool UGameFinishedWidget::Initialize()
{
	bool Success = Super::Initialize();

	if (!Success)
	{
		return false;
	}
	
	if (ReasonText && BaseTextBlock)  //If ReasonText and BaseTextBlock are not null
	{
		BaseTextBlock->TextDelegate.BindUFunction(this, "UpdateReason");  //Delegate UpdateReason to modify base text block
		ReasonText->TextDelegate.BindUFunction(this, "UpdateReason");  //Delegate UpdateReason to modify reason text block 
	}

	return true;
}

//Function useful to set the reason that caused the end of the game
void UGameFinishedWidget::SetReason(const FString& Reason)
{
	this->Reason = Reason;
}

//Function useful to to update every text block of GameFinishedWidget
void UGameFinishedWidget::UpdateReason(bool ILost, bool IsDraw)
{
	if(IsDraw)  //If it's a draw
		this->BaseTextBlock->SetText(FText::FromString("IT'S A DRAW!"));  //Display this text on BaseTextBlock
	
	if(!ILost && !IsDraw)  //If main player has won
		this->BaseTextBlock->SetText(FText::FromString("YOU WON!"));  //Display this text on BaseTextBlock

	if(ILost && !IsDraw)  //If main player has lost
		this->BaseTextBlock->SetText(FText::FromString("YOU LOST!"));  //Display this text on BaseTextBlock

	this->ReasonText->SetText(FText::FromString(this->Reason));  //Display this text on ReasonText
}

//Function useful to get the reason that caused the end of the game
FString UGameFinishedWidget::GetReason()
{
	return this->Reason;
}

//Function useful to set up GameFinishedWidget
void UGameFinishedWidget::SetUp(bool ILost, const FString& Reason, const bool IsDraw, const int NftId)
{
	this->SetReason(Reason);
	this->UpdateReason(ILost, IsDraw);
	if(!ILost)
	{
		this->UpdateNftImage(NftId);
		this->DisplayPrizeButton();
	}
}

