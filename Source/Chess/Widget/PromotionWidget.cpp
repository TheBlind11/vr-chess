// Fill out your copyright notice in the Description page of Project Settings.


#include "PromotionWidget.h"

#include "WidgetComponent.h"
#include "Controllers/WidgetController.h"
#include "Utility/Utility.h"

void UPromotionWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

//Function called when player clicks on "Queen Button" in PromotionWidget
FString UPromotionWidget::OnQueenButtonClick()
{
	/* if(this->IsVisible())  //If this widget is visible
		this->RemoveFromViewport();  //Remove it from the viewport */

	this->RemoveFormView();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();  //Get player controller to modify his/her controls
	//PlayerController->SetInputMode(FInputModeGameAndUI());  //Now player can still make moves and use ChessUI buttons

	return "Queen";
}

//Function called when player clicks on "Knight Button" in PromotionWidget
FString UPromotionWidget::OnKnightButtonClick()
{
	/* if(this->IsVisible())  //If this widget is visible
		this->RemoveFromViewport();  //Remove it from the viewport */

	this->RemoveFormView();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();  //Get player controller to modify his/her controls
	//PlayerController->SetInputMode(FInputModeGameAndUI());  //Now player can still make moves and use ChessUI buttons

	return "Knight";
}

//Function called when player clicks on "Rook Button" in PromotionWidget
FString UPromotionWidget::OnRookButtonClick()
{
	/* if(this->IsVisible())  //If this widget is visible
		this->RemoveFromViewport();  //Remove it from the viewport */

	this->RemoveFormView();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();  //Get player controller to modify his/her controls
	//PlayerController->SetInputMode(FInputModeGameAndUI());  //Now player can still make moves and use ChessUI buttons

	return "Rook";
}

//Function called when player clicks on "Bishop Button" in PromotionWidget
FString UPromotionWidget::OnBishopButtonClick()
{
	/* if(this->IsVisible())  //If this widget is visible
		this->RemoveFromViewport();  //Remove it from the viewport */

	this->RemoveFormView();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();  //Get player controller to modify his/her controls
	//PlayerController->SetInputMode(FInputModeGameAndUI());  //Now player can still make moves and use ChessUI buttons

	return "Bishop";
}

void UPromotionWidget::RemoveFormView()
{
	AWidgetController* WidgetController = Cast<AWidgetController>(Utility::FindByTag(GetWorld(), "WidgetController"));  //Get the widget controller in scene by its tag
	WidgetController->DisplayPromotionWidgetUI(false);
	WidgetController->ShowChessUI(true);  //Reset the viewport with ChessUIWidget
}
