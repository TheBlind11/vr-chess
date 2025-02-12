// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPlayer.h"
#include "Chessboard.h"
#include "ChessController.generated.h"


UCLASS()
class CHESS_API AChessController : public APlayerController
{
	GENERATED_BODY()

public:
	AChessController();

	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;
	
	// When left mouse button is clicked
	void OnLeftMouseClick();

	//Detects clicked area and make a move if selected grid is available to make a move
	void SetMovement();

	//Switches turn between players
	void SwitchPlayer();

	//Select piece from chess set by mouse hit
	void SelectPiece();

	//Function to get AvailableMoves sent by server
	TArray<TSharedPtr<FJsonValue>> GetAvailableMoves();

	//Function to set AvailableMoves of main player at every turn sent by server 
	void SetAvailableMoves(TArray<TSharedPtr<FJsonValue>> Moves);

	//Function useful to make "Castling" move
	void Castling(bool ToRight, int32 Source);

	// Detects clicked grid. Then moves previously selected actor to grid's location.
	UFUNCTION(BlueprintCallable)
	void MakeMove();

	//Function useful to set up the countdown
	void Countdown();

	//Function useful to set seconds of the countdown
	void SetSeconds(int Seconds);

	//Function useful to update timer display
	FString UpdateTimerDisplay();

	//Function useful to set TurnID
	void SetTurnId(FString Turn);

	//Function useful to get TurnID
	FString GetTurnId();

	//Function useful to display error message on screen (look at ChessUIWidget blueprint graph)
	void DisplayErrorMessage(FText Message);

	//Function useful to check Castling of the king
	bool CheckCastling(int32 INT32, int32 EndpointGrid);

	//Function useful to check "EnPassant" move
	bool CheckEnPassant(int32 SourceGrid, int32 EndpointGrid, APiece* Pawn);

	//Function useful to check a pawn promotion
	bool CheckPawnPromotion(int32 Endpoint);

	//Function useful to get source and endpoint after a PawnPromotion
	UFUNCTION(BlueprintImplementableEvent)
	void GetSourceAndEndpoint(int Source, int Endpoint);

	//Function to set MainPlayer
	void SetMainPlayer(AChessPlayer* Player);

	//Function to get MainPlayer
	AChessPlayer* GetMainPlayer();

	//Function to set Chessboard
	void SetChessboard(AChessboard* Board);

	//Function useful to set boolean value HasOfferedDraw
	void SetHasOfferedDraw(bool HasOffered);

	void SetClaimDrawReason(const FString& Reason);
	
	AChessboard* GetChessboard();

	APiece* GetSelectedPiece();

	// Holds selected piece
	UPROPERTY(BlueprintReadOnly)
	APiece* SelectedPiece;

	//Field of TurnID
	UPROPERTY(BlueprintReadWrite)
	FString TurnId;

	//Reason field to claim draw
	UPROPERTY(BlueprintReadWrite)
	FString ClaimDrawReason;

	//ErrorMessage field to save the error that we need to display on screen
	UPROPERTY(BlueprintReadOnly)
	FText ErrorMessage = FText::FromString("No error.");

	//Boolean useful to check if player has already offered draw in his/her turn :)
	UPROPERTY(BlueprintReadWrite)
	bool HasOfferedDraw = false;

private:

	//AvailableMoves field
	TArray<TSharedPtr<FJsonValue>> AvailableMoves;

	//Array field of grids that display last move done
	TArray<class UStaticMeshComponent*> LastMoveGrids;

	//Array field of actors found with collision box
	TArray<class AActor*> OverlappedActors;

	//Array field of components found with collision box
	TArray<class UPrimitiveComponent*> OverlappedComponents;

	//Array field of materials of highlighted grids 
	TArray<class UMaterial* > DefaultMaterials;

	//Array field of materials of last move grids
	TArray<UMaterial*> DefaultMaterialsLastMove;

	// Checker if anything is selected. False by default.
	bool IsAnythingSelected = false;

	// Holds selected grid
	UStaticMeshComponent* SelectedGrid;

	//Boolean field to check if it's first or second click
	bool SecondClick = false;

	//FVector field useful to get the center of a selected grid
	UPROPERTY()
	FVector GridLocation;

	// Pointer to chess board
	AChessboard* Chessboard;

	/* Pointer to main player */
	UPROPERTY()
	AChessPlayer* MainPlayer; // Pointer to main player

	//Field of countdown seconds
	int Seconds;
	
};
