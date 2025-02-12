// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessController.h"
#include "WidgetController.h"
#include "GameFramework/Actor.h"
#include "Pieces/PawnPiece.h"
#include "SceneController.generated.h"

UCLASS()
class CHESS_API ASceneController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASceneController();

	//Function to initialize the object
	void Init();

	//When connected to server it calls SpawnPlayers
	void ChessConnected(const FString& LocalParticipantId, const TSharedPtr<FJsonObject>& StateOfGame);

	//This function is called when player seats in game
	void PlayerSeated(const FString& PlayerId, const FString& Nickname, int SeatNumber);

	//When connected to server spawn other players if already connected
	//void SpawnPlayers(const FString& String, TArray<AChessPlayer*> Array);

	//Function to show CountdownWidget in scene
	void ShowCountdown(int Seconds);

	//This is called when game begins, it spawns everything in the scene
	void PlayerTurn(const FString& ParticipantId, const FString& TurnId, const bool IsWhite, const TArray<TSharedPtr<FJsonValue>>& AvailableMoves);

	//This function is called when PlayerHasMovedEvent is received
	void PlayerHasMoved(const FString& ParticipantId, const FString& TurnId, int ActionSeconds, int OpponentSource, int OpponentEndpoint,
						int SeatNumber, const FString& PromoteTo, const TArray<TSharedPtr<FJsonValue>>& AvailableMoves, const FString& Reason);

	//Function useful to display last move done on the other client, the one who has received the move from the opponent
	void DisplayLastMoveDone(int Source, int Endpoint, FString PromoteTo);

	//Function useful to make a pawn promotion and show PawnPromotionWidget
	void ShowPawnPromotionWidget();

	//Function useful to display opponent pawn promotion
	void DisplayOpponentPawnPromotion(const FString& PromoteTo, APawnPiece* Piece);

	//This function is called when GameFinishedEvent is received
	void ShowGameFinishedWidget(const FString& LastTurnPlayerId, const FString& WinnerId, const FString& LoserId, const FString& Reason, const bool
	                            IsDraw, int Source, int Endpoint, const FString& PromoteTo, const int NftId);

	//This function is called to display the box to accept draw offered by the opponent
	void ShowCanAcceptDrawUI();

	//This function is called to display the box to accept rematch offered by the opponent
	void ShowCanAcceptRematchUI();
	
	//Get WidgetController
	AWidgetController* GetWidgetController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//ChessController field
	UPROPERTY()
	class AChessController* ChessController;

	//WidgetController field
	UPROPERTY()
	class AWidgetController* WidgetController;
	
};
