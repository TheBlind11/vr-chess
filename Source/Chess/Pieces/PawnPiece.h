// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Piece.h"
#include "PawnPiece.generated.h"

UCLASS()
class CHESS_API APawnPiece : public APiece
{
	GENERATED_BODY()

public:
	//	Constructor
	APawnPiece();

	//Function useful to change the mesh of a pawn piece due to a promotion
	UFUNCTION(BlueprintCallable)
	void ChangeMesh(UPARAM(ref) FString PromoteTo);
};
