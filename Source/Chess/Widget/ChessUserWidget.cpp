// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessUserWidget.h"

//Function useful to get main player nickname text block in ChessUIWidget
UTextBlock* UChessUserWidget::GetNicknameTextBlock()
{
	return this->NicknameTextBlock;
}

//Function useful to get opponent nickname text block in ChessUIWidget
UTextBlock* UChessUserWidget::GetOpponentNicknameTextBlock()
{
	return this->OpponentNicknameTextBlock;
}
