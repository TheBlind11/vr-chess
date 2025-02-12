// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessController.h"
#include "Pieces/Bishop.h"
#include "CollisionBox.h"
#include "Chessboard.h"
#include "ChessPlayer.h"
#include "Controllers/SceneController.h"
#include "Controllers/WidgetController.h"
#include "Kismet/GameplayStatics.h"
#include "Network/ChessNetworkActor.h"
#include "Utility/Utility.h"

// Constructor
AChessController::AChessController()
{
	bShowMouseCursor = false;						// Show mouse cursor
	//DefaultMouseCursor = EMouseCursor::Crosshairs;	// Change cursor appearance to crosshair
	bEnableMouseOverEvents = false;  //If this is not true we can't check mouse click
}

// Begin play
void AChessController::BeginPlay()
{
	Super::BeginPlay();

	//This may be commented, I don't know if this is useful or not
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AChessController::Countdown, 1.f, true, 3.0);

}

//Function useful to set up mouse action on left click
void AChessController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind actions for mouse events
	//InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &AChessController::OnLeftMouseClick);
	//InputComponent->BindAction("TriggerLeft", IE_Pressed, this, &AChessController::OnLeftMouseClick);
	InputComponent->BindAction("TriggerRight", IE_Pressed, this, &AChessController::OnLeftMouseClick);
}

/*	Fired when left mouse button pressed.
Detects clicked objects and if it is a RTSCharacter sets them to selected.
**/
void AChessController::OnLeftMouseClick()
{
	if(SecondClick) {  //If it's second click, so player has already selected a piece and can choose a grid to make a move
		SetMovement();  //Set movement of the piece (select a grid to make a move)
		return;
	}

	if(!SecondClick)  //If it's first click player hasn't selected a piece
		SelectPiece(); // Detect hit object. Then highlight selected piece and selectable grids.
}

/* Select piece from chess set by mouse hit */
void AChessController::SelectPiece()
{
	
	if(!MainPlayer->GetIsTurn())  //Check if it's main player's turn
	{
		DisplayErrorMessage(FText::FromString("It's not your turn!"));  //If it's not his/her turn display this error message opn screen
		return;
	}
	else
	{
		/* Detects clicked area */
		//FHitResult TraceResult(ForceInit); // Will hold hit result
		//GetHitResultUnderCursor(ECollisionChannel::ECC_WorldDynamic, false, TraceResult); // Get hit result and assign it to TraceResult

		FHitResult OutHitResult;
		this->MainPlayer->DoLineTraceFromController(OutHitResult);

		//FHitResult OutHitResult = this->MainPlayer->GetRightWidgetInteractionComponent()->GetLastHitResult();
		
		// If Hit Target was a piece.
		if (OutHitResult.GetActor() != nullptr)
		{
			if (OutHitResult.GetActor()->IsA(APiece::StaticClass()))  /* If selected actor is a Piece */
			{
				SelectedPiece = Cast<APiece>(OutHitResult.GetActor()); // Cast actor to piece class
				if (SelectedPiece != nullptr) // Check if cast was successful
				{
					if((SelectedPiece->GetIsWhite() == MainPlayer->GetIsWhite()))  //Check if 
					{
						SelectedPiece->SetSelected();	// Call selected function from SelectedPiece to change selected piece's material.
						IsAnythingSelected = true;		// Set Checker variable to True
						this->Chessboard->HighlightSelectableGrids(SelectedPiece, AvailableMoves);		// Highlights Selectable Grids
						SecondClick = true;  //Next click will be second click
						return;
					}
					else
					{
						DisplayErrorMessage(FText::FromString("Can't select opponent's piece!"));
						return;
					}
				}
			}
		}
	
		//hit a grid rather than a piece (Like chess.com)
		if (OutHitResult.GetComponent() != nullptr)	// Check if any component was hit.
		{
			SelectedGrid = Cast<UStaticMeshComponent>(OutHitResult.GetComponent());	// Cast hit component to StaticMeshComponent. If it is not a StaticMeshComponent this will be nullptr.
			if (SelectedGrid != nullptr)	// Check if cast was successful.
			{
				FVector Location = SelectedGrid->GetComponentLocation() + FVector(200, 200, 0);  //Get hit component's location. Useful to spawn collision boxes in center grid, because pieces are in center grids
				AActor* CollisionBox = GetWorld()->SpawnActor<ACollisionBox>(Location, FRotator(0, 0, 0)); // Spawn collision box.
				CollisionBox->GetOverlappingActors(OverlappedActors, TSubclassOf<AActor>()); // Get Overlapped Actors by CollisionBox: actors that are in that position (it could be Chessboard or piece)

				if (OverlappedActors.Num() > 1)
				{
					for(auto Actor: OverlappedActors)
					{
						if (Actor->IsA(APiece::StaticClass()))  //Check if actor is a piece
						{
							SelectedPiece = Cast<APiece>(Actor); // Cast piece to its class
							if (SelectedPiece != nullptr) // Check if cast was successful
							{
								if(SelectedPiece->GetIsWhite() == MainPlayer->GetIsWhite())  //If player has selected one of his/her pieces
								{
									SelectedPiece->SetSelected();	// Call selected function from SelectedPiece to change selected actor's material.
									IsAnythingSelected = true;		// Set Checker variable to True
									this->Chessboard->HighlightSelectableGrids(SelectedPiece, AvailableMoves);		// Highlights Selectable Grids
									SecondClick = true;  //Next click will be second click

									OverlappedActors.Empty();  //Reset array of actors found
									CollisionBox->Destroy();  //Destroy the collision box
									return;
								}
								
								DisplayErrorMessage(FText::FromString("Can't select opponent's piece!"));
							}
						}
					}
				}

				OverlappedActors.Empty();  //Reset array of actors found
				CollisionBox->Destroy();  //Destroy the collision box
			}
		}
	}

	// If no actor was hit set anything to false.
	IsAnythingSelected = false;  //Nothing has been selected
	SecondClick = false;  //Reset the click
}

/*	Fired when left mouse button pressed.
	Detects clicked area and make a move if selected grid is available to make a move.
**/
void AChessController::SetMovement()
{
	this->Chessboard->SetDefaultMaterialsToGrids(false); // Set default materials to highlighted grid.
	
	// Check if anything is selected. Execute block if it is selected. Otherwise do nothing.
	if (IsAnythingSelected)
	{
		//FHitResult TraceResult(ForceInit);	// FHitResult variable to keep track of mouse hit.
		//GetHitResultUnderCursor(ECollisionChannel::ECC_WorldDynamic, false, TraceResult);	// Get hit result and assign it to TraceResult.

		FHitResult OutHitResult;
		this->MainPlayer->DoLineTraceFromController(OutHitResult);

		//FHitResult OutHitResult = this->MainPlayer->GetRightWidgetInteractionComponent()->GetLastHitResult();
			
		if (OutHitResult.GetActor() != nullptr)	// Check if any actor was hit.
		{
			if(OutHitResult.GetActor()->IsA(AChessboard::StaticClass()))  //Check if the actor hit is Chessboard
			{
				SelectedGrid = Cast<UStaticMeshComponent>(OutHitResult.GetComponent());	// Cast hit component to StaticMeshComponent. If it is not a StaticMeshComponent this will be nullptr. We need StaticMeshComponent because every grid is a StaticMeshComponent
				if (SelectedGrid != nullptr)	// Check if cast was successful.
				{
					if(this->Chessboard->GetSelectableGrids().Contains(SelectedGrid)) // Check if selected grid in SelectableGrids.
					{
						GridLocation = SelectedGrid->GetComponentLocation() + FVector(200, 200, 0.5);	// Get hit component's location. Useful to spawn collision boxes in center grid, because pieces are in center grids

						int32 Endpoint = this->Chessboard->GetAllGrids().Find(SelectedGrid);  //Get the endpoint position index
					
						MakeMove();  //Move the piece
						SecondClick = false;  //Reset the click
						
						return;
					}

					//I have selected a grid where i can't move, let's check actors on it
					AActor* CollisionBox = GetWorld()->SpawnActor<ACollisionBox>(SelectedGrid->GetComponentLocation() + FVector(200, 200, 0.5), FRotator(0, 0, 0)); // Spawn collision box in the center of the grid
					CollisionBox->GetOverlappingActors(OverlappedActors, TSubclassOf<AActor>()); // Get Overlapped Actors by CollisionBox: actors that are in that position (it could be Chessboard or piece)
					for (auto Piece : OverlappedActors)  //Iterate on every actors found
					{
						if(Piece->IsA(APiece::StaticClass()))  //If the actor found is a piece
						{
							APiece* CastedPiece = Cast<APiece>(Piece);  //Cast it to its class
							if (CastedPiece->GetIsWhite() == MainPlayer->GetIsWhite())  //Check if it's main player's piece
							{
								SelectedPiece->SetDeselected();  //Deselect the piece
								SelectedPiece = nullptr;  //Reset the field of SelectedPiece
								SecondClick = true;  //Reset the click
								SelectedGrid = nullptr;  //Reset SelectedGrid because player has selected another piece
								SelectPiece();  //Select this new piece

								OverlappedActors.Empty();  //Reset array of actors found
								CollisionBox->Destroy(); //Destroy the collision box
								return;
							}
						}
					}

					OverlappedActors.Empty();  //Reset array of actors found
					CollisionBox->Destroy(); //Destroy the collision box

					DisplayErrorMessage(FText::FromString("Can't make a move there!"));  //Display error message because we have selected a grid that is not selectable
							
					SelectedPiece->SetDeselected();  //Deselect the piece
					SelectedPiece = nullptr;  //Reset the field of SelectedPiece
					OverlappedActors.Empty();  //Reset array of actors found
					CollisionBox->Destroy();  //Destroy the collision box
					SecondClick = false;  //Reset the click
					SelectedGrid = nullptr;  //Reset SelectedGrid because player has selected a grid that is not selectable
					return;
				}
			}

			else
			{
				if (OutHitResult.GetActor()->IsA(APiece::StaticClass()))  //If the actor hit is a piece
				{
					APiece* CastedPiece = Cast<APiece>(OutHitResult.GetActor());  //Cast it to its class
					if (CastedPiece->GetIsWhite() == MainPlayer->GetIsWhite())  //I have clicked on one of my pieces, let's select this instead 
					{
						SelectedPiece->SetDeselected();  //Deselect the piece
						SelectedGrid = nullptr;  //Reset SelectedGrid because player has selected another piece
						SecondClick = true;  //Reset the click
						SelectPiece();  //Select this new piece
						
						return;
					}

					//I have clicked on an opponent's piece, let's check the grid under it
					AActor* CollisionBox2 = this->GetWorld()->SpawnActor<ACollisionBox>(CastedPiece->GetActorLocation(), FRotator(0, 0, 0));  // Spawn collision box.
					CollisionBox2->GetOverlappingComponents(OverlappedComponents);  //Get the components found on this collision box
					for(auto Component : OverlappedComponents)  //Iterate on the components found
					{
						AActor* Board = Component->GetAttachmentRootActor();  //Returns the top-level actor that component is attached to
						if(Board->IsA(AChessboard::StaticClass()))  //Check if it's the Chessboard
						{
							SelectedGrid = Cast<UStaticMeshComponent>(Component);  //Cast it to a grid 
							if(this->Chessboard->GetSelectableGrids().Contains(SelectedGrid))  //Check if it can be selected
							{
								GridLocation = SelectedGrid->GetComponentLocation() + FVector(200, 200, 0.5);	// Get hit component's location. Useful to spawn collision boxes in center grid, because pieces are in center grids
					
								MakeMove();  //Move the piece
								SecondClick = false;  //Reset the click

								CollisionBox2->Destroy();  //Destroy the collision box
								OverlappedComponents.Empty();  //Reset array of components found
								return;
							}

							DisplayErrorMessage(FText::FromString("Can't make a move there!"));  //Display error message because we have selected a grid that is not selectable
							SecondClick = false;  //Reset the click
							SelectedPiece->SetDeselected();  //Deselect the piece
							SelectedPiece = nullptr;  //Reset the field of SelectedPiece
							CollisionBox2->Destroy();  //Destroy the collision box
							OverlappedComponents.Empty();  //Reset array of components found
							return;
						}
					}
				}
			}
		}
	}
	
	SecondClick = false;  //Reset the click
	SelectedPiece->SetDeselected();  //Deselect the piece
	SelectedPiece = nullptr;  //Reset the field of SelectedPiece
}

// Detects clicked grid. Then moves previously selected actor to grid's location. 
void AChessController::MakeMove()
{
	bool SelectedGridIsEmpty = true;  //This boolean is useful to check "EnPassant" movement
	this->Chessboard->SetDefaultMaterialsToGrids(false);
	this->Chessboard->RemoveSelectableGrids();
	this->Chessboard->RemoveDefaultMaterialsSelectableGrids();
	AActor* CollisionBox = GetWorld()->SpawnActor<ACollisionBox>(GridLocation, FRotator(0, 0, 0)); // Spawn collision box in center of the selected grid
	CollisionBox->GetOverlappingActors(OverlappedActors, TSubclassOf<AActor>()); // Get Overlapped Actors by CollisionBox: actors that are in that position (it could be Chessboard or piece)
	if (OverlappedActors.Num() > 1)
	{
		for (auto Piece : OverlappedActors)
		{
			if (Piece->IsA(APiece::StaticClass()))  //Check if actor is a piece
			{
				APiece* CastedPiece = Cast<APiece>(Piece);  //Cast piece to its class, this is an opponent piece for sure, otherwise it would have not been oon a selectable grid
				if (CastedPiece)
				{
					CastedPiece->Eliminate();  //Player eats opponent's piece, let's destroy it
					SelectedGridIsEmpty = false;  //So let's change the flag, now "EnPassant" movement is not possible
					UE_LOG(LogTemp, Error, TEXT("ow yeah"));
				}
			}
		}
	}
	
	OverlappedActors.Empty();  //Reset array of actors found
	CollisionBox->Destroy();  //Destroy the collision box

	//We need the mesh of the grid where the selected piece is moving from
	UStaticMeshComponent* CurrentMesh = this->Chessboard->FindCurrentGrid(SelectedPiece);
	
	SelectedPiece->ChangeLocation(FVector(GridLocation.X, GridLocation.Y, SelectedPiece->GetActorLocation().Z));	// Change actor's location to selected grid's location.
	SelectedPiece->SetDeselected();  //Deselect the piece
	IsAnythingSelected = false;  //Reset boolean because now player has made a move
	SwitchPlayer(); // Switch player
	TArray<UStaticMeshComponent*> Grids = this->Chessboard->GetAllGrids();  //Get all the grids of the Chessboard
	AChessNetworkActor* NetActor = Cast<AChessNetworkActor>(Utility::FindByTag(GetWorld(), "NetworkActor"));  //We need to send the message of the move done to server, let's get ChessNetworkActor in scene
	int32 Endpoint = Grids.Find(SelectedGrid);  //Find index of source grid of the move
	int32 Source = Grids.Find(CurrentMesh);  //Find index of endpoint grid of the move

	UE_LOG(LogTemp, Warning, TEXT("ChessController::MakingMove"));  //Log to check every step

	if(SelectedPiece->GetClass()->GetName() == "King")  //If the piece moved is a king
	{
		CheckCastling(Source, Endpoint);  //Check if the move of the king caused a "Castling"
	}

	if(SelectedPiece->GetClass()->GetName() == "PawnPiece")  //If the piece moved is a pawn
	{
		if(SelectedGridIsEmpty)  //If the grid diagonally is empty
		{
			CheckEnPassant(Source, Endpoint, SelectedPiece);  //Check if the move of the pawn caused an "EnPassant"
		}
		
		if(CheckPawnPromotion(Endpoint))  //Let's check if there is a PawnPromotion
		{
			this->GetSourceAndEndpoint(Chessboard->MappingIndex(Source), Chessboard->MappingIndex(Endpoint));
			return;
		}
	}
	
	//Send the move done to server
	NetActor->SendMove(this->TurnId,
		Chessboard->MappingIndex(Source), Chessboard->MappingIndex(Endpoint), "");

	AWidgetController* CastedWidgetController = Cast<AWidgetController>(Utility::FindByTag(GetWorld(), "WidgetController"));  //Get WidgetController from the scene
	if(CastedWidgetController)
	{
		CastedWidgetController->GetChessUI()->RemoveCanAcceptDrawButtons();  //Remove CanAcceptDraw buttons of ChessUIWidget
		CastedWidgetController->GetChessUI()->RemoveClaimDrawButton();  //Remove ClaimDraw button of ChessUIWidget
	}
	
	UE_LOG(LogTemp, Warning, TEXT("MOVE: %d -> %d"), Chessboard->MappingIndex(Source), Chessboard->MappingIndex(Endpoint));
}

//Function useful to check Castling of the king
bool AChessController::CheckCastling(int32 Source, int32 Endpoint)
{
	if(Source - Endpoint == 2)  //Difference between Source and Endpoint, if it's 2 it's a "Right Castling"
	{
		bool ToRight = true;
		Castling(ToRight, Source);
								
		return true;
	}

	if(Source - Endpoint == -2)  //Difference between Source and Endpoint, if it's -2 it's a "Left Castling"
	{
		bool ToRight = false;
		Castling(ToRight, Source);
								
		return true;
	}

	return false;
}

//Function useful to make "Castling" move
void AChessController::Castling(bool ToRight, int32 Source)
{
	TArray<UStaticMeshComponent*> Grids = this->Chessboard->GetAllGrids();  //Get all the grids of the board mapped with indexes in the array
	
	if(ToRight)  //If it is a right castling
	{
		UStaticMeshComponent* RookGrid = Grids[Source - 3];  //Get the rook's grid on the right. Rook it's 3 positions on the right of the king (look at client Chessboard)

		AActor* CollisionBox = GetWorld()->SpawnActor<ACollisionBox>(RookGrid->GetComponentLocation() + FVector(200, 200, 0.5), FRotator(0, 0, 0)); // Spawn collision box on rook's grid
		CollisionBox->GetOverlappingActors(OverlappedActors, TSubclassOf<AActor>()); // Get Overlapped Actors by CollisionBox (we need to get rook piece in order to move it)
		if (OverlappedActors.Num() > 1)
		{
			for (auto Piece : OverlappedActors)
			{
				if (Piece->IsA(APiece::StaticClass()))  //Check if the actor is a piece
				{
					APiece* CastedPiece = Cast<APiece>(Piece);  //Cast it to its class
					if(CastedPiece)  //If the piece is not null
					{
						CastedPiece->ChangeLocation(Grids[Source - 1]->GetComponentLocation() + FVector(200, 200, 0.5));  //Change the position of the rook to make castling. Rook will be moved on the left of the king of one position
					}
				}
			}
		}

		OverlappedActors.Empty();  //Reset array of actors found
		CollisionBox->Destroy();  //Destroy the collision box
	}

	else  //If it is a left castling
	{
		UStaticMeshComponent* RookGrid = Grids[Source + 4];  //Get the rook's grid on the left. Rook it's 4 positions on the left of the king (look at client Chessboard)

		AActor* CollisionBox = GetWorld()->SpawnActor<ACollisionBox>(RookGrid->GetComponentLocation() + FVector(200, 200, 0.5), FRotator(0, 0, 0)); // Spawn collision box on rook's grid
		CollisionBox->GetOverlappingActors(OverlappedActors, TSubclassOf<AActor>()); // Get Overlapped Actors by CollisionBox
		if (OverlappedActors.Num() > 1)
		{
			for (auto Piece : OverlappedActors)
			{
				if (Piece->IsA(APiece::StaticClass()))  //Check if the actor is a piece
				{
					APiece* CastedPiece = Cast<APiece>(Piece);  //Cast it to its class
					if(CastedPiece)
					{
						CastedPiece->ChangeLocation(Grids[Source + 1]->GetComponentLocation() + FVector(200, 200, 0.5));  //Change the position of the rook to make castling. Rook will be moved on the right of the king of one position
					}
				}
			}
		}

		OverlappedActors.Empty();  //Reset array of actors found
		CollisionBox->Destroy();  //Destroy the collision box
	}
}

//Function useful to check "EnPassant" move
bool AChessController::CheckEnPassant(int32 SourceGrid, int32 EndpointGrid, APiece* Pawn)
{
	TArray<UStaticMeshComponent*> Grids = this->Chessboard->GetAllGrids();  //Get all the grids mapped with indexes
	int32 AbsoluteValue = FMath::Abs(SourceGrid - EndpointGrid);  //Get absolute value of difference between source and endpoint
	TArray<AActor*> PawnActors;  //Array of pawn pieces useful to check if there is an actor on the grid near the source of the move
	AActor* CollisionBox;
	if(AbsoluteValue == 9)  //Check if absolute value is 9 (look at client Chessboard: pawn moves diagonally to left)
	{
		if(Pawn->GetIsWhite())  //if it's a white pawn, we need to check source + 1 grid
		{
			CollisionBox = GetWorld()->SpawnActor<ACollisionBox>(Grids[SourceGrid + 1]->GetComponentLocation() + FVector(200, 200, 0.5), FRotator(0, 0, 0)); // Spawn collision box.
		}
		else  //if it's a black pawn, we need to check source - 1 grid
		{
			CollisionBox = GetWorld()->SpawnActor<ACollisionBox>(Grids[SourceGrid - 1]->GetComponentLocation() + FVector(200, 200, 0.5), FRotator(0, 0, 0));
		}
								
		CollisionBox->GetOverlappingActors(PawnActors, TSubclassOf<AActor>()); // Get Overlapped Actors by CollisionBox
		if (PawnActors.Num() > 1)
		{
			for (auto Piece : PawnActors)
			{
				if (Piece->IsA(APiece::StaticClass()))  //If the actor is a piece
				{
					APiece* CastedPiece = Cast<APiece>(Piece);  //Cast it to its class
					if (CastedPiece)
					{
						CastedPiece->Eliminate();  //Destroy it from the scene ("eaten")
						UE_LOG(LogTemp, Error, TEXT("ow yeah"));

						PawnActors.Empty();  //Reset array of pawn found
						CollisionBox->Destroy();  //Destroy the collision box
						return true;
					}
				}
			}
		}

		PawnActors.Empty();  //Reset array of pawn found
		CollisionBox->Destroy();  //Destroy the collision box
	}

	if(AbsoluteValue == 7)  //Check if absolute value is 7 (look at client Chessboard: pawn moves diagonally to right)
	{
		if(Pawn->GetIsWhite())  //if it's a white pawn, we need to check source - 1 grid
			CollisionBox = GetWorld()->SpawnActor<ACollisionBox>(Grids[SourceGrid - 1]->GetComponentLocation() + FVector(200, 200, 0.5), FRotator(0, 0, 0)); // Spawn collision box.
		else  //if it's a black pawn, we need to check source + 1 grid
			CollisionBox = GetWorld()->SpawnActor<ACollisionBox>(Grids[SourceGrid + 1]->GetComponentLocation() + FVector(200, 200, 0.5), FRotator(0, 0, 0));

		CollisionBox->GetOverlappingActors(PawnActors, TSubclassOf<AActor>()); // Get Overlapped Actors by CollisionBox
		if (PawnActors.Num() > 1)
		{
			for (auto Piece : PawnActors)
			{
				if (Piece->IsA(APiece::StaticClass()))  //If the actor is a piece
				{
					APiece* CastedPiece = Cast<APiece>(Piece);  //Cast it to its class
					if (CastedPiece)
					{
						CastedPiece->Eliminate();  //Destroy it from the scene ("eaten")
						UE_LOG(LogTemp, Error, TEXT("ow yeah"));

						PawnActors.Empty();  //Reset array of pawn found
						CollisionBox->Destroy();  //Destroy the collision box
						return true;
					}
				}
			}
		}

		PawnActors.Empty();  //Reset array of pawn found
		CollisionBox->Destroy();  //Destroy the collision box
	}

	return false;
}

//Function useful to check a pawn promotion
bool AChessController::CheckPawnPromotion(int32 Endpoint)
{
	if((Endpoint >= 0 && Endpoint <= 7) || (Endpoint >= 56 && Endpoint <= 63))  //If a pawn is on last line, there could be a promotion
	{
		this->Chessboard->SetDefaultMaterialsToGrids(true);
		this->Chessboard->RemoveLastMoveGrids();
		this->Chessboard->RemoveDefaultMaterialsLastMoveGrids();
		this->SelectedPiece->SetDeselected();
			
		ASceneController* SceneController = Cast<ASceneController>(Utility::FindByTag(GetWorld(), "SceneController"));
		SceneController->ShowPawnPromotionWidget();  //Player can make a promotion
		SecondClick = false;  //Reset the click 
						
		return true;
	}

	return false;
}

//Function useful to set up the countdown
void AChessController::Countdown()
{
	if (Seconds != 0)
	{
		Seconds = Seconds - 1;  //Update seconds remaining
	}

}

//Function useful to set seconds of the countdown
void AChessController::SetSeconds(int SecondsPassed)
{
	Seconds = SecondsPassed;
}

//Function useful to update timer display
FString AChessController::UpdateTimerDisplay()
{
	return FString::FromInt(FMath::Max(Seconds, 0));
}

//Function useful to get TurnID
FString AChessController::GetTurnId()
{
	return this->TurnId;
}

//Function useful to set TurnID
void AChessController::SetTurnId(FString Turn)
{
	this->TurnId = Turn;
}

//Function useful to display error message on screen (look at ChessUIWidget blueprint graph)
void AChessController::DisplayErrorMessage(FText Message)
{
	this->ErrorMessage = Message;  //Save ErrorMessage in a field to easily display it (look at ChessUIWidget blueprint graph)
	AWidgetController* CastedWidgetController = Cast<AWidgetController>(Utility::FindByTag(GetWorld(), "WidgetController"));  //Get WidgetController in scene by its tag
	if (CastedWidgetController != nullptr)
	{
		CastedWidgetController->GetChessUI()->DisplayErrorMessage(); // Display error message on the screen.
	}
}

//Function to set MainPlayer
void AChessController::SetMainPlayer(AChessPlayer* Player)
{
	this->MainPlayer = Player;
}

//Function to get MainPlayer
AChessPlayer* AChessController::GetMainPlayer()
{
	return this->MainPlayer;
}

//Function to set Chessboard
void AChessController::SetChessboard(AChessboard* Board)
{
	this->Chessboard = Board;
}

void AChessController::SetHasOfferedDraw(bool HasOffered)
{
	this->HasOfferedDraw = HasOffered;
}

void AChessController::SetClaimDrawReason(const FString& Reason)
{
	this->ClaimDrawReason = Reason;
}

AChessboard* AChessController::GetChessboard()
{
	return this->Chessboard;
}

APiece* AChessController::GetSelectedPiece()
{
	return this->SelectedPiece;
}

//Function to set AvailableMoves of main player at every turn sent by server 
void AChessController::SetAvailableMoves(TArray<TSharedPtr<FJsonValue>> Moves)
{
	this->AvailableMoves = Moves;
}

//Function to get AvailableMoves sent by server
TArray<TSharedPtr<FJsonValue>> AChessController::GetAvailableMoves()
{
	return this->AvailableMoves;
}

/* Switches between player */
void AChessController::SwitchPlayer()
{
	if(MainPlayer->GetIsTurn())  //If it has been main player's turn
		MainPlayer->SetIsTurn(false);  //Change the turn
	else
		MainPlayer->SetIsTurn(true);  //Change the turn
}