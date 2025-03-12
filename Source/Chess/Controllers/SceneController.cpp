// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneController.h"

#include "Chessboard.h"
#include "ChessPlayer.h"
#include "CollisionBox.h"
#include "TextBlock.h"
#include "Widget/ChessUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Pieces/PawnPiece.h"
#include "Utility/Utility.h"

// Sets default values
ASceneController::ASceneController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ASceneController::BeginPlay()
{
	Super::BeginPlay();
}

//We use init to set up scene controller because we want to create object when we need it, not when the game starts
void ASceneController::Init()
{
	UWorld* World = GetWorld();  //Get the world of the scene
	UE_LOG(LogTemp, Display, TEXT("ASceneController::Init"));  //Log to understand what function is running
	
	if(World)  //if there is World and it is not null
	{
		if (WidgetController == nullptr )  //If there isn't a WidgetController we need to create it
		{
			
			WidgetController = static_cast<AWidgetController*>(World->SpawnActor(AWidgetController::StaticClass()));  //Spawn WidgetController in the scene
			AChessPlayer* Player = static_cast<AChessPlayer*>(GetWorld()->GetFirstPlayerController()->GetPawn());
			WidgetController->AttachToComponent(Player->GetCamera(), FAttachmentTransformRules::KeepRelativeTransform);
			WidgetController->SetActorRelativeLocation(FVector(900.f, 0.f, 0.f));
			//WidgetController->SetActorScale3D(FVector(0.2f));
			WidgetController->Tags.Add("WidgetController");  //Add a tag to the controller to find it when it's necessary
			UE_LOG(LogTemp, Display, TEXT("Spawned WidgetController."));
		}

		else
		{
			UE_LOG(LogTemp, Display, TEXT("WidgetController Not Spawned."));
		}

		if(ChessController == nullptr)  //If there isn't a ChessController we need to create it
		{
			ChessController = static_cast<AChessController*>(UGameplayStatics::GetPlayerController(World, 0)); //Spawn ChessController in the scene
			UE_LOG(LogTemp, Display, TEXT("PlayerController Exists"));
		}

		else
		{
			UE_LOG(LogTemp, Display, TEXT("WidgetController Not Exists."));
		}
	}
}

//This is called when a player connects to WebSocket
void ASceneController::ChessConnected(const FString& LocalParticipantId, const TSharedPtr<FJsonObject>& StateOfGame)
{
	UE_LOG(LogTemp, Display, TEXT("ASceneController::ChessConnected"));  //Log to understand what function is running
	//const FString TableState = StateOfGame.Get()->GetStringField("state");
	int32 NumPlayers = StateOfGame.Get()->GetArrayField("players").Num();  //Get the number of players in game
	//const FString GameId = StateOfGame.Get()->GetStringField("gameId");
	const TArray<TSharedPtr<FJsonValue>> PlayersArrayJson = StateOfGame.Get()->GetArrayField("players");  //Get all the players seated in game
	FString Nickname;
	if(NumPlayers != 0)  //If there is already a player waiting in game
	{
		for( auto p : PlayersArrayJson)
		{
			Nickname = p.Get()->AsObject().Get()->GetStringField("nickname");  //Get opponent player's nickname in order to display it on screen during the game
		}

		//Probably other players could be already connected so we need to spawn them in the scene
		//SpawnPlayers(LocalParticipantId, PlayerObjects);
	}

	AChessPlayer* Player = static_cast<AChessPlayer*>(GetWorld()->GetFirstPlayerController()->GetPawn());  //Get MainPlayer in scene
	UE_LOG(LogTemp, Display, TEXT("ASceneController::ChessConnected Main player spawned"));
	ChessController->SetMainPlayer(Player);  //Set up MainPlayer in ChessController
	ChessController->GetMainPlayer()->SetParticipantId(LocalParticipantId);  //Set ParticipantId of the MainPlayer who has just connected

	/* Load class from asset in Content Browser of the UserWidget we created (UGUserWidget) */
	TSubclassOf<class UUserWidget> Class = WidgetController->GetChessUIWidgetClass();  //Get the class of ChessUIWidget (we need it to create this widget)
	if(Class)  //Check if this class isn't null
	{
		/** Assign the class of the loaded asset to the WigetClass variable, which is a "subclass" of UUserWidget : Which our asset class is */
		//WidgetController->SetChessUI(Cast<UChessUserWidget>(ChessUIComponent->GetWidget()));  //Spawn ChessUIWidget and set it in WidgetController
		
		//UChessUserWidget* UI = WidgetController->GetChessUI();  //Get ChessUIWIdget 
		/* if(UI) //Check if it's not null
		{
			UI->AddToViewport();  //Add it to player viewport
			UE_LOG(LogTemp, Display, TEXT("ASceneController::ChessConnected UI added to viewport"));
		} */

		this->WidgetController->ShowChessUI(true);
		UE_LOG(LogTemp, Display, TEXT("ASceneController::ChessConnected UI added to viewport"));

		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();  //Get PlayerController 
		//PlayerController->SetInputMode(FInputModeUIOnly());  //Now Player can use only UI offered by ChessUIWidget

		this->WidgetController->GetChessUI()->GetOpponentNicknameTextBlock()->SetText(FText::FromString(Nickname));  //Set up opponent nickname
	}
}

// //When connected on server spawn other players if already connected
// void ASceneController::SpawnPlayers(const FString& LocalParticipant, const TArray<AChessPlayer*> PlayersObjects)
// {
// 	for (auto PlayerObject : PlayersObjects)
// 	{
// 		//const FString& participantId = PlayerObject->GetParticipantId();
// 		//if (participantId != LocalParticipant)
// 		{
// 			//const int playerPosition = PlayerObject->GetSeatNumber(); //Just for now we use seat number to choose player's color
//
// 			//if(playerPosition == 1) //Other player in game is black
// 			{
// 				//We need condition from server that tells us if we are white or black at the start of the game, in order to set up the camera position
// 				PlayerObject = GetWorld()->SpawnActor<AChessPlayer>(FVector(1600.0, 3700.0, 1500), FRotator(20.0f, -90.0f, 0.0f));
// 			}
//
// 			//else
// 			{
// 				PlayerObject = GetWorld()->SpawnActor<AChessPlayer>(FVector(1600.0, -500.0, 1500), FRotator(20.0f, 90.0f, 0.0f));
// 			}
// 		}
// 	}
// }

//This function is called when player seats in game, message is broadcast so check ParticipantID
void ASceneController::PlayerSeated(const FString& PlayerId, const FString& Nickname, int SeatNumber)
{
	if(SeatNumber == 0  && PlayerId == this->ChessController->GetMainPlayer()->GetParticipantId())  //I'm the first who seats in game in order to play
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, "WAIT FOR ANOTHER PLAYER");  //Log on screen to display that we are the only one in game, we need another player to start the game
	}

	if(PlayerId == this->ChessController->GetMainPlayer()->GetParticipantId())  //I'm the second player who decides to play, we need to save nickname to display it on screen during the game
	{
		this->WidgetController->GetChessUI()->GetNicknameTextBlock()->SetText(FText::FromString(Nickname));  //Set up my nickname
		this->ChessController->GetMainPlayer()->SetNickname(Nickname);
	}

	else
	{
		this->WidgetController->GetChessUI()->GetOpponentNicknameTextBlock()->SetText(FText::FromString(Nickname));  //Another player has joined the game, save his/her nickname :)
	}

}

//Function to show CountdownWidget in scene
void ASceneController::ShowCountdown(int Seconds)
{

	if(this->WidgetController->GetGameFinishedUIComponent()->IsVisible())
	{
		this->WidgetController->RemoveGameFinishedUI();
		this->WidgetController->PromotionWidget = nullptr;
	}

	AChessboard* Chessboard = Cast<AChessboard>(Utility::FindByTag(GetWorld(), "Chessboard"));  //If Chessboard already exists, players are going to have a rematch
	if(Chessboard)
	{
		TArray<AActor*> AllPieces;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Piece", AllPieces);
		UE_LOG(LogTemp, Warning, TEXT("%d"), AllPieces.Num());
		for(auto Piece: AllPieces)
		{
			Piece->Destroy();
		}

		UE_LOG(LogTemp, Warning, TEXT("%d"), AllPieces.Num());
		Chessboard->Destroy();
	}
	
	WidgetController->ShowCountdown(Seconds);
	
}

//Function that is called when PlayerTurnEvent is received
void ASceneController::PlayerTurn(const FString& ParticipantId, const FString& TurnId, const bool IsWhite, const TArray<TSharedPtr<FJsonValue>>& AvailableMoves)
{
	if(!this->WidgetController->GetChessUIComponent()->IsVisible())
	{
		this->WidgetController->ShowChessUI(true);
		this->WidgetController->GetChessUI()->EnableOfferDrawButton();
		this->WidgetController->GetChessUI()->RemoveClaimDrawButton();
		this->ChessController->HasOfferedDraw = false;
	}
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();  //Get PlayerController
	//PlayerController->SetInputMode(FInputModeGameAndUI());  //Now MainPlayer can use UI offered by ChessUIWidget and make move with pieces
	
	//Spawn chessboard
	AChessboard* Chessboard = GetWorld()->SpawnActor<AChessboard>(FVector(0, 0, 0), FRotator(0, 0, 0));
	Chessboard->Tags.Add("Chessboard");

	//Spawn black pieces on the board
	Chessboard->SpawnBlackPieces();

	//Spawn white pieces on the board
	Chessboard->SpawnWhitePieces();

	ChessController->SetChessboard(Chessboard);  //Set Chessboard spawned in ChessController
	
	if(IsWhite)  //If this message is for white player 
	{
		//Main Player is white
		//ChessController->GetMainPlayer()->SetActorLocationAndRotation(FVector(1600.0, 0.0, 1450), FRotator(2.0f, 90.0f, 0.0f));  //Set up player position in scene
		ChessController->GetMainPlayer()->SetLocationAndRotation(FVector(1600.0, -500.0, 900), FRotator(2.0f, 90.0f, 0.0f));  //Set up player position in scene
		ChessController->GetMainPlayer()->SetIsWhite(true);  //This MainPlayer is white
		ChessController->GetMainPlayer()->SetIsTurn(true);  //And it's his/her turn :)
		ChessController->SetAvailableMoves(AvailableMoves);  //Set AvailableMoves received in ChessController
		//UE_LOG(LogTemp, Log, TEXT("Bool value is: %s"), IsWhite ? TEXT("true") : TEXT("false") );
		//GetWorld()->SpawnActor<AChessPlayer>(FVector(1600.0, 3700.0, 1500), FRotator(20.0f, -90.0f, 0.0f));
	}

	else
	{
		//Main Player is black
		//ChessController->GetMainPlayer()->SetActorLocationAndRotation(FVector(1600.0, 3200.0, 1450), FRotator(2.0f, -90.0f, 0.0f));
		ChessController->GetMainPlayer()->SetLocationAndRotation(FVector(1600.0, 3800, 900), FRotator(2.0f, -90.0f, 0.0f));
		ChessController->GetMainPlayer()->SetIsTurn(false);
		ChessController->GetMainPlayer()->SetIsWhite(false);
		//GetWorld()->SpawnActor<AChessPlayer>(FVector(1600.0, -500.0, 1500), FRotator(20.0f, 90.0f, 0.0f));
	}

	ChessController->SetTurnId(TurnId);  //Not very important for now
	WidgetController->RemoveCountdownUI();  //Now game is started so we don't need CountdownWidget anymore
	WidgetController->GetChessUI()->DisplayOfferDrawResignButtons();  //Display OfferDraw and Resign buttons in scene, so player can use them now
	WidgetController->GetChessUI()->DisplayNicknameBoxes();  //Display Nicknames in scene ("Player vs Player")
	
}

//This function is called when PlayerHasMovedEvent is received, this message is broadcast so check ParticipantId
void ASceneController::PlayerHasMoved(const FString& ParticipantId, const FString& TurnId, int ActionSeconds, int OpponentSource, int OpponentEndpoint, int SeatNumber, const FString& PromoteTo, const TArray<TSharedPtr<FJsonValue>>& AvailableMoves, const FString& Reason)
{
	if(ParticipantId != this->ChessController->GetMainPlayer()->GetParticipantId())  //MainPlayer didn't make last move
	{
		this->DisplayLastMoveDone(OpponentSource, OpponentEndpoint, PromoteTo);  //Display opponent last move
		this->ChessController->GetMainPlayer()->SetIsTurn(true);  //Now this MainPlayer can make a move 
		this->ChessController->SetAvailableMoves(AvailableMoves);  //Set available moves

		if(Reason != "NOT_DRAWABLE" && Reason != "")  //Check if there is a CanClaimDrawReason 
		{
			this->WidgetController->GetChessUI()->DisplayClaimDrawButton();  //Display CanClaimDrawButton
			this->ChessController->SetClaimDrawReason(Reason);
		}

		else
			this->WidgetController->GetChessUI()->RemoveClaimDrawButton();  //Remove CanClaimDrawButton because there is no reason
	}

	this->ChessController->GetChessboard()->SetDefaultMaterialsToGrids(false);
	this->ChessController->GetChessboard()->DisplayLastMoveGrids(OpponentSource, OpponentEndpoint);  //Display last move grids for all players
	this->ChessController->SetHasOfferedDraw(false);
	this->ChessController->SetTurnId(TurnId);  //Change TurnId for all players
	this->WidgetController->EnableOfferDrawButton();  //Enable OfferDrawButton (It could be not enabled because player could have offered draw in previous turn)
}

//Function useful to display last move done on the other client, the one who has received the move from the opponent
void ASceneController::DisplayLastMoveDone(int Source, int Endpoint, FString PromoteTo)
{
	TArray<UStaticMeshComponent*> Grids = this->ChessController->GetChessboard()->GetAllGrids();  //Get all the grids of the Chessboard mapped with indexes
	
	if(Source == Endpoint)  //If game finishes with an agreement or a claim draw by a player, source and endpoint will be the same 0 and 0
		return;

	//Map the indexes received by server
	int32 SourceGrid = SourceGrid = this->ChessController->GetChessboard()->MappingIndex(Source);
	int32 EndpointGrid = EndpointGrid = this->ChessController->GetChessboard()->MappingIndex(Endpoint);

	//Get the vectors of last move grids 
	FVector SourceVector = Grids[SourceGrid]->GetComponentLocation();
	FVector EndpointVector = Grids[EndpointGrid]->GetComponentLocation();
	FVector EndpointCenterGridVector = EndpointVector + FVector(200, 200, 0.5);  //Useful to check actors in the center of the grid with collision box

	bool SelectedGridIsEmpty = true;  //Boolean useful to check if there could be an "EnPassant" move

	TArray<AActor*> OverlappedActors;
	AActor* CollisionBox = GetWorld()->SpawnActor<ACollisionBox>(EndpointCenterGridVector, FRotator(0, 0, 0)); // Spawn collision box on Endpoint grid
	CollisionBox->GetOverlappingActors(OverlappedActors, TSubclassOf<AActor>()); // Get Overlapped Actors by CollisionBox
	if (OverlappedActors.Num() > 1)
	{
		for (auto Piece : OverlappedActors)
		{
			if (Piece->IsA(APiece::StaticClass()))  //If the actor is a piece
			{
				APiece* CastedPiece = Cast<APiece>(Piece);  //Cast it to its class
				if (CastedPiece && (CastedPiece->GetIsWhite() == this->ChessController->GetMainPlayer()->GetIsWhite()))  //If opponent eats one of my pieces, i have to display it
				{
					CastedPiece->Eliminate();  //Destroy the piece from the scene
					SelectedGridIsEmpty = false;  //"EnPassant" move not found
					UE_LOG(LogTemp, Error, TEXT("ow yeah"));
				}
			}
		}
	}

	OverlappedActors.Empty();  //Reset array of actors found
	CollisionBox->Destroy();  //Destroy collision box
	
	AActor* CollisionBox2 = GetWorld()->SpawnActor<ACollisionBox>(SourceVector + FVector(200, 200, 0.5), FRotator(0, 0, 0)); // Spawn collision box on Source grid
	CollisionBox2->GetOverlappingActors(OverlappedActors, TSubclassOf<AActor>()); // Get Overlapped Actors by CollisionBox
	if (OverlappedActors.Num() > 1)
	{
		for (auto Piece : OverlappedActors)
		{
			if (Piece->IsA(APiece::StaticClass()))  //Check if actor found is a piece
			{
				APiece* CastedPiece = Cast<APiece>(Piece);  //Cast it to its class
				if (CastedPiece && (CastedPiece->GetIsWhite() != this->ChessController->GetMainPlayer()->GetIsWhite()))  //I have to display opponent's piece movement
				{
					CastedPiece->ChangeLocation(FVector(EndpointCenterGridVector.X, EndpointCenterGridVector.Y, CastedPiece->GetActorLocation().Z));  //Change location of opponent's piece
					if(PromoteTo != "")  //If there has been a pawn promotion by the opponent
					{
						DisplayOpponentPawnPromotion(PromoteTo, Cast<APawnPiece>(CastedPiece));

						OverlappedActors.Empty();  //Reset array of actors found
						CollisionBox2->Destroy();  //Destroy collision box
								
						return;
					}
					
					if(CastedPiece->GetClass()->GetName() == "King")  //Check if opponent moved a king
					{
						if(this->ChessController->CheckCastling(SourceGrid, EndpointGrid))  //Check if there is a castling
						{
							OverlappedActors.Empty();  //Reset array of actors found
							CollisionBox2->Destroy();  //Destroy collision box
								
							return;
						}
					}

					if(CastedPiece->GetClass()->GetName() == "PawnPiece")  //Check if opponent moved a pawn
					{
						if(SelectedGridIsEmpty)  //There could be an "EnPassant" move
						{
							if(this->ChessController->CheckEnPassant(SourceGrid, EndpointGrid, CastedPiece))  //Check EnPassant
							{
								OverlappedActors.Empty();  //Reset array of actors found
								CollisionBox2->Destroy();  //Destroy collision box
								
								return;
							}
						}
					}
				}
			}
		}
	}

	OverlappedActors.Empty();  //Reset array of actors found
	CollisionBox2->Destroy();  //Destroy collision box

	UE_LOG(LogTemp, Warning, TEXT("LAST MOVE DISPLAYED!"));
}

//Function useful to make a pawn promotion and show PawnPromotionWidget
void ASceneController::ShowPawnPromotionWidget()
{
	if (WidgetController != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("WIDGETCONTROLLER EXISTS"));
		WidgetController->DisplayPromotionWidgetUI(true); // Display PromotionWidgetUI 
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();  //Get player controller
	//PlayerController->SetInputMode(FInputModeUIOnly());  //Player can only use UI buttons
}

//Function useful to display opponent pawn promotion
void ASceneController::DisplayOpponentPawnPromotion(const FString& PromoteTo, APawnPiece* Piece)
{
	UE_LOG(LogTemp, Warning, TEXT("SceneController::DisplayingOpponentPromotion"));

	Piece->ChangeMesh(PromoteTo);
}

//This function is called when GameFinishedEvent is received, this is broadcast so check ParticipantId
void ASceneController::ShowGameFinishedWidget(const FString& LastTurnPlayerId, const FString& WinnerId, const FString& LoserId, const FString& Reason, const bool IsDraw, int Source, int Endpoint, const FString& PromoteTo, const int NftId)
{
	bool YouLost = false;

	if(LastTurnPlayerId != this->ChessController->GetMainPlayer()->GetParticipantId())  //This condition is important when game finished with a draw, we need to know where to display last move done
	{
		UE_LOG(LogTemp, Warning, TEXT("SceneController::ShowGameFinishedWidget")); //Log to understand what function is running
		this->DisplayLastMoveDone(Source, Endpoint, PromoteTo); //MainPlayer is not the one who has made last move, so need to display last opponent move
	}
	
	if(WinnerId != this->ChessController->GetMainPlayer()->GetParticipantId())  //This condition is important when game finished without a draw, we need to know who won or lost
		YouLost = true;  //In this case this MainPlayer didn't win

	this->ChessController->GetChessboard()->DisplayLastMoveGrids(Source, Endpoint);  //Display last move grids on both clients
	this->ChessController->GetChessboard()->SetDefaultMaterialsToGrids(false);

	if(this->ChessController->GetSelectedPiece())
		this->ChessController->GetSelectedPiece()->SetDeselected();

	WidgetController->ShowGameFinishedWidget(YouLost, IsDraw, Reason, NftId);  //Display GameFinishedWidget on both clients
}

//This function is called to display the box to accept draw offered by the opponent
void ASceneController::ShowCanAcceptDrawUI()
{
	this->WidgetController->ShowCanAcceptDrawWidget();
}

void ASceneController::ShowCanAcceptRematchUI()
{
	this->WidgetController->ShowCanAcceptRematchWidget();
}

//This function gets WidgetController 
AWidgetController* ASceneController::GetWidgetController()
{
	return this->WidgetController;
}

// Called every frame
void ASceneController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

