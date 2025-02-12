// Fill out your copyright notice in the Description page of Project Settings.


#include "Chessboard.h"

#include "CollisionBox.h"
#include "Pieces/Bishop.h"
#include "Pieces/King.h"
#include "Pieces/Knight.h"
#include "Pieces/PawnPiece.h"
#include "Pieces/Queen.h"
#include "Pieces/Rook.h"

// Sets default values
AChessboard::AChessboard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/********** DEFINITION OF BOX COMPONENT **********/
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));		// Define a box component
	RootComponent = BoxComponent;													// Set it as RootComponent
	BoxComponent->InitBoxExtent(FVector(0, 0, 0));						// Set size to box
	/********** DEFINITION OF BOX COMPONENT **********/

	/********** DEFINITION OF MATERIALS **********/
	// Set light material to Wood Pine
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Materials/M_Wood_Pine.M_Wood_Pine'"));
	if (Material.Object != NULL)
	{
		LightMaterial = (UMaterial*)Material.Object;
	}
	// Set dark material to Wood Walnut
	static ConstructorHelpers::FObjectFinder<UMaterial> Material2(TEXT("Material'/Game/Materials/M_Wood_Walnut.M_Wood_Walnut'"));
	if (Material2.Object != NULL)
	{
		DarkMaterial = (UMaterial*)Material2.Object;
	}
	// Set highlighted material to Wood Walnut
	static ConstructorHelpers::FObjectFinder<UMaterial> Material3(TEXT("Material'/Game/Materials/M_SelectableGridMaterial.M_SelectableGridMaterial'"));
	if (Material3.Object != NULL)
	{
		HighlightedMaterial = (UMaterial*)Material3.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> Material4(TEXT("Material'/Game/Materials/M_LastMoveMaterial.M_LastMoveMaterial'"));
	if (Material4.Object != NULL)
	{
		LastMoveMaterial = (UMaterial*)Material4.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> Material5(TEXT("Material'/Game/Materials/M_MouseHoverMaterial.M_MouseHoverMaterial'"));
	if(Material5.Object != NULL)
	{
		MouseHoverMaterial = (UMaterial*)Material5.Object;
	}
	
	/********** DEFINITION OF MATERIALS **********/

	/********** CREATION OF BOARD GRIDS **********/
	// Calculation of coordinates
	for (int32 index = 0; index < 64; ++index)
	{
		// Set Coordinates
		int32 XCoordinate = (index % 8) * 400;
		int32 YCoordinate = (index / 8) * 400;

		// Add static mesh to component 
		AllGrids.Add(CreateDefaultSubobject<UStaticMeshComponent>(*FString("Grid" + FString::FromInt(index))));
		AllGrids[index]->SetupAttachment(BoxComponent); // Attach mesh to root component
		
		// Retrieve floor asset 
		static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("StaticMesh'/Game/Meshes/Floor.Floor'"));
		// If floor asset is retrieved successfully 
		if (StaticMeshAsset.Succeeded())
		{
			// Set floor asset to static mesh
			AllGrids[index]->SetStaticMesh(StaticMeshAsset.Object);

			// Set relative location values to calculated values
			AllGrids[index]->SetRelativeLocation(FVector(XCoordinate, YCoordinate, 0));

			AllGrids[index]->OnBeginCursorOver.AddDynamic(this, &AChessboard::OnCursorOver);
			AllGrids[index]->OnEndCursorOver.AddDynamic(this, &AChessboard::EndCursorOver);
		}
	}
	/********** CREATION OF BOARD GRIDS **********/
}

// Called when the game starts or when spawned
void AChessboard::BeginPlay()
{
	Super::BeginPlay();

	/* Set materials for grids based on their location. First one that spawns is always white (right back grid) */
	for (int32 index = 0; index < 64; ++index)
	{
		// Calculations to set materials 
		bool First = ((index / 8) % 2) == 0 ? false : true;
		bool Second = ((index % 8) % 2) == 0 ? false : true;
		bool Result = XOR(First, Second);
		if (Result)
		{
			AllGrids[index]->SetMaterial(0, DarkMaterial);
		}
		else
		{
			AllGrids[index]->SetMaterial(0, LightMaterial);
		}

		AllMaterials.Add(Cast<UMaterial>(AllGrids[index]->GetMaterial(0)));
	}
}

// Called every frame
void AChessboard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UStaticMeshComponent* AChessboard::FindCurrentGrid(APiece* SelectedPiece)
{
	FVector CurrentGrid = SelectedPiece->GetActorLocation();  //Get piece location 
	AActor* CollisionBox = GetWorld()->SpawnActor<ACollisionBox>(CurrentGrid, FRotator(0, 0, 0)); // Spawn collision box in the location of the piece
	TArray<AActor*> OverlappedActors;
	TArray<UPrimitiveComponent*> OverlappedComponents;
	CollisionBox->GetOverlappingActors(OverlappedActors, TSubclassOf<AChessboard>()); // Get Overlapped Actors by CollisionBox
	CollisionBox->GetOverlappingComponents(OverlappedComponents); // Get Overlapped Components by CollisionBox

	//We need to find current position index in order to get selectable grids from that position
	if (OverlappedComponents.Num() != 0 && OverlappedActors.Num() != 0)
	{
		for (auto Actor : OverlappedActors)
		{
			// Code to perform if overlapped actor is a board grid.
			if (Actor->IsA(AChessboard::StaticClass()))
			{
				AChessboard* CastedPiece = Cast<AChessboard>(Actor); // Cast actor to Chessboard class
				// Check if cast was successful
				if (CastedPiece != nullptr)
				{
					// Foreach overlapped component run the loop
					for (auto Component : OverlappedComponents)
					{
						AActor *AttachmentActor = Component->GetAttachmentRootActor();  //Returns the top-level actor that component is attached to
						if (AttachmentActor->IsA(AChessboard::StaticClass()))  //Check if it is Chessboard
						{
							UStaticMeshComponent* CastedMesh = Cast<UStaticMeshComponent>(Component); // Cast component to StaticMeshComponent (every grid is a static mesh component)
							// Check if cast was successful
							if (CastedMesh != nullptr)
							{
								OverlappedActors.Empty();  //Reset array of actors found
								CollisionBox->Destroy();  //Destroy collision box
								
								return CastedMesh;  //Save current grid 
							}
						}
					}
				}
			}
		}
	}

	OverlappedActors.Empty();  //Reset array of actors found
	CollisionBox->Destroy();  //Destroy collision box

	return NULL;
}

//This function is called to highlight grid
void AChessboard::SetMaterialToGrid(UStaticMeshComponent* Grid, UMaterial* Material)
{
	Grid->SetMaterial(0, Material);  //Set material for the grid
}

//This function is called to map indexes of grids of the chessboard in order to communicate with server sending right source and endpoint when a move is done
		//This is client chessboard         //This is server chessboard
		/* 63 62 61 60 59 58 57 56			   56 57 58 59 60 61 62 63
		   55 54 53 52 51 50 49 48			   48 49 50 51 52 53 54 55
		   47 46 45 44 43 42 41 40             40 41 42 43 44 45 46 47
		   39 38 37 36 35 34 33 32             32 33 34 35 36 37 38 39
		   31 30 29 28 27 26 25 24             24 25 26 27 28 29 30 31
		   23 22 21 20 19 18 17 16			   16 17 18 19 20 21 22 23
		   15 14 13 12 11 10  9  8			    8  9 10 11 12 13 14 15
		    7  6  5  4  3  2  1  0              0  1  2  3  4  5  6  7 */
//As you can see every index in each column has the same rest in "index % 8", we want to map indexes between client chessboard and server chessboard, so knowing index position
//and its column we can deduce whether to add or subtract a certain number, you can do some tests if you don't trust me :)
int32 AChessboard::MappingIndex(int32 Index) {

	int32 Rest = Index % 8;
    if(Rest == 0)
        return Index + 7;
        
    if(Rest == 1)
    	return Index + 5;
    	
    if(Rest == 2)
    	return Index + 3;

	if(Rest == 3)
		return Index + 1;

	if(Rest == 4)
		return Index - 1;

	if(Rest == 5)
		return Index - 3;

	if(Rest == 6)
		return Index - 5;
	
	//if nothing is verified (Rest == 7)
	return Index - 7;
}

//Spawn white pieces on the chessboard
void AChessboard::SpawnWhitePieces()
{
	TArray<class APiece*> SpawnedPieces; // Pointer to hold spawned pieces
	

	// Spawn pawns and add to SpawnedPieces array
	for (int i = 1; i <= 8; i++) {
		SpawnedPieces.Add(GetWorld()->SpawnActor<APawnPiece>(FVector((i*400.0f) - 200.0f, 600.0f, 0), FRotator::ZeroRotator));
	}

	// Spawn rooks and add to SpawnedPieces array
	SpawnedPieces.Add(GetWorld()->SpawnActor<ARook>(FVector(200, 200, 0), FRotator::ZeroRotator));
	SpawnedPieces.Add(GetWorld()->SpawnActor<ARook>(FVector(3000, 200, 0), FRotator::ZeroRotator));

	// Spawn bishops and add to SpawnedPieces array
	SpawnedPieces.Add(GetWorld()->SpawnActor<ABishop>(FVector(1000, 200, 0), FRotator::ZeroRotator));
	SpawnedPieces.Add(GetWorld()->SpawnActor<ABishop>(FVector(2200, 200, 0), FRotator::ZeroRotator));

	// Spawn knights and add to SpawnedPieces array
	SpawnedPieces.Add(GetWorld()->SpawnActor<AKnight>(FVector(600, 200, 0), FRotator::ZeroRotator));
	SpawnedPieces.Add(GetWorld()->SpawnActor<AKnight>(FVector(2600, 200, 0), FRotator::ZeroRotator));

	// Spawn queen and add to SpawnedPieces array
	SpawnedPieces.Add(GetWorld()->SpawnActor<AQueen>(FVector(1800, 200, 0), FRotator::ZeroRotator));

	// Spawn king and add to SpawnedPieces array
	SpawnedPieces.Add(GetWorld()->SpawnActor<AKing>(FVector(1400, 200, 0), FRotator::ZeroRotator));

	//Set color and material for pieces
	for (auto Piece : SpawnedPieces) {
		Piece->SetIsWhite(true);  //Set each piece white
		Piece->SetDeselected();  //Set each piece deselected with its material, in this case LightMaterial
		Piece->Tags.Add("Piece");
	}
}

//Spawn black pieces on the chessboard
void AChessboard::SpawnBlackPieces()
{
	TArray<class APiece*> SpawnedPieces; // Pointer to hold spawned pieces

	// Spawn pawns and add to SpawnedPieces array
	for (int i = 1; i <= 8; i++) {
		SpawnedPieces.Add(GetWorld()->SpawnActor<APawnPiece>(FVector((i*400.0f) - 200.0f, 2600.0f, 0), FRotator(0, -180, 0)));
	}
	
	// Spawn rooks and add to SpawnedPieces array
	SpawnedPieces.Add(GetWorld()->SpawnActor<ARook>(FVector(200, 3000, 0), FRotator(0, -180, 0)));
	SpawnedPieces.Add(GetWorld()->SpawnActor<ARook>(FVector(3000, 3000, 0), FRotator(0, -180, 0)));

	// Spawn bishops and add to SpawnedPieces array
	SpawnedPieces.Add(GetWorld()->SpawnActor<ABishop>(FVector(1000, 3000, 0), FRotator(0, -180, 0)));
	SpawnedPieces.Add(GetWorld()->SpawnActor<ABishop>(FVector(2200, 3000, 0), FRotator(0, -180, 0)));

	// Spawn knights and add to SpawnedPieces array
	SpawnedPieces.Add(GetWorld()->SpawnActor<AKnight>(FVector(600, 3000, 0), FRotator(0, -180, 0)));
	SpawnedPieces.Add(GetWorld()->SpawnActor<AKnight>(FVector(2600, 3000, 0), FRotator(0, -180, 0)));

	// Spawn queen and add to SpawnedPieces array
	SpawnedPieces.Add(GetWorld()->SpawnActor<AQueen>(FVector(1800, 3000, 0), FRotator(0, -180, 0)));

	// Spawn king and add to SpawnedPieces array
	SpawnedPieces.Add(GetWorld()->SpawnActor<AKing>(FVector(1400, 3000, 0), FRotator(0, -180, 0)));

	//Set color and material for pieces
	for(auto Piece : SpawnedPieces) {
		Piece->SetIsWhite(false);  //Set each piece black
		Piece->SetDeselected();  //Set each piece deselected with its material, in this case DarkMaterial
		Piece->Tags.Add("Piece");
	}
}

//Function useful to display last move grids
void AChessboard::DisplayLastMoveGrids(int32 Source, int32 Endpoint)
{
	if(Source == Endpoint)  //If game finishes with an agreement or a claim draw by a player, source and endpoint will be the same 0 and 0
		return;
	
	SetDefaultMaterialsToGrids(true);  //Set default materials to old last move
	this->RemoveLastMoveGrids();
	this->RemoveDefaultMaterialsLastMoveGrids();

	this->LastMoveGrids.Empty();
	this->DefaultMaterialsLastMoveGrids.Empty();

	//Map the indexes received by server
	int32 SourceGrid = SourceGrid = this->MappingIndex(Source);
	int32 EndpointGrid = EndpointGrid = this->MappingIndex(Endpoint);

	//Let's get the grids source and endpoint
	UStaticMeshComponent* SourceMesh = AllGrids[SourceGrid];
	UStaticMeshComponent* EndpointMesh = AllGrids[EndpointGrid];

	//Save them in the array of last move grids
	LastMoveGrids.Add(EndpointMesh);
	LastMoveGrids.Add(SourceMesh);

	UMaterial* CastedMaterial = Cast<UMaterial>(SourceMesh->GetMaterial(0));  //Get the default material
	UMaterial* CastedMaterial2 = Cast<UMaterial>(EndpointMesh->GetMaterial(0));  //Get the default material
	if (CastedMaterial && CastedMaterial2)
	{
		DefaultMaterialsLastMoveGrids.Add(CastedMaterial2); // Add Grid's material to this array.
		DefaultMaterialsLastMoveGrids.Add(CastedMaterial);  // Add Grid's material to this array.
	}
	
	this->SetMaterialToGrid(SourceMesh, LastMoveMaterial); // Change grid's material to last move material
	this->SetMaterialToGrid(EndpointMesh, LastMoveMaterial);  // Change grid's material to last move material
}

/* Sets highlighted material to selectable grids */
void AChessboard::HighlightSelectableGrids(APiece* SelectedPiece, TArray<TSharedPtr<FJsonValue>> AvailableMoves)
{
	HighlightedGrids.Empty(); // Empty previous records.
	DefaultMaterialsHighlightedGrids.Empty();

	UStaticMeshComponent* CurrentMesh = FindCurrentGrid(SelectedPiece);
	
	int32 CurrentIndex = AllGrids.Find(CurrentMesh);  //Get current grid index
	for( TSharedPtr<FJsonValue> JsonValue : AvailableMoves)
	{
		TSharedPtr<FJsonObject> Object = JsonValue->AsObject();  //Every element in this array has two field source and endpoints, the second one is an array of indexes (look at sequence diagram on trello)
		int32 Source = Object->GetIntegerField("source");  //Get source field
		if(this->MappingIndex(Source) == CurrentIndex)  //Check if the source is the same of selected piece position
		{
			TArray<FString> Endpoints;
			Object->TryGetStringArrayField("endpoints", Endpoints);  //Get the second field: array of possible positions where to move
			for(FString String : Endpoints)
			{
				int32 GridMesh = FCString::Atoi(*String);  //Convert element from string to int
				int32 GridMeshMapped = MappingIndex(GridMesh);  //Map the index received from server (look at Board.cpp and client/server Chessboard)
				UStaticMeshComponent* SelectableGrid = AllGrids[GridMeshMapped];  //Get the grid that is selectable
				UMaterial* DefaultMaterial = Cast<UMaterial>(SelectableGrid->GetMaterial(0));
				this->DefaultMaterialsHighlightedGrids.Add(DefaultMaterial);
				this->SetMaterialToGrid(SelectableGrid, HighlightedMaterial);  //Highlight this grid, it will be a grid selectable by the player who would like to make a move
				this->HighlightedGrids.Add(SelectableGrid);  //Add this grid to selectable grids
			}
			
			return;
		}
	}

}

TArray<UStaticMeshComponent*> AChessboard::GetSelectableGrids()
{
	return this->HighlightedGrids;
}

/* Returns highlighted grids' material to their default material */
void AChessboard::SetDefaultMaterialsToGrids(bool LastMove)
{

	if(LastMove)  //Check if we need to change material to last move grids or not
	{
		for(int32 index = 0; index < LastMoveGrids.Num(); index++)
		{
			LastMoveGrids[index]->SetMaterial(0, DefaultMaterialsLastMoveGrids[index]);  //Set default material to every last move grids 
		}
	}
	else  //We don't need to change material to last move grids but only to previous selectable grids
	{
		for (int32 index = 0; index < HighlightedGrids.Num(); index++)
		{
			HighlightedGrids[index]->SetMaterial(0,  DefaultMaterialsHighlightedGrids[index]);  //Set default material to every selectable grids
		}
	}
}

//Get all grids of the chessboard, indexes are important
TArray<UStaticMeshComponent*> AChessboard::GetAllGrids()
{
	return this->AllGrids;
}

//Get LastMoveMaterial to display last move grids
UMaterial* AChessboard::GetLastMoveMaterial()
{
	return this->LastMoveMaterial;
}

//Get HighlightedMaterial to display grids that can be selected to do a move when a piece is selected
UMaterial* AChessboard::GetHighlightedMaterial()
{
	return this->HighlightedMaterial;
}

//Get MouseHoverMaterial to display a grid when mouse cursor is on it
UMaterial* AChessboard::GetMouseHoverMaterial()
{
	return this->MouseHoverMaterial;
}


void AChessboard::OnCursorOver(UPrimitiveComponent* Component)
{
	UMaterial* Material = Cast<UMaterial>(Component->GetMaterial(0));
	this->CurrentMaterial = Material;
	Component->SetMaterial(0, this->MouseHoverMaterial);
}


void AChessboard::EndCursorOver(UPrimitiveComponent* Component)
{
	if(CurrentMaterial == HighlightedMaterial && HighlightedGrids.Contains(Component))
	{
		Component->SetMaterial(0, HighlightedMaterial);
		CurrentMaterial = nullptr;
		return;
	}
	
	if(CurrentMaterial == HighlightedMaterial && LastMoveGrids.Contains(Component))
	{
		Component->SetMaterial(0, LastMoveMaterial);
		CurrentMaterial = nullptr;
		return;
	}

	if(CurrentMaterial == HighlightedMaterial && HighlightedGrids.IsEmpty())
	{
		Component->SetMaterial(0, AllMaterials[AllGrids.Find(Cast<UStaticMeshComponent>(Component))]);
		CurrentMaterial = nullptr;
		return;
	}

	Component->SetMaterial(0, CurrentMaterial);
	CurrentMaterial = nullptr;
}

void AChessboard::RemoveSelectableGrids()
{
	this->HighlightedGrids.Empty();
}

void AChessboard::RemoveDefaultMaterialsSelectableGrids()
{
	this->DefaultMaterialsHighlightedGrids.Empty();
}

void AChessboard::RemoveLastMoveGrids()
{
	this->LastMoveGrids.Empty();
}

void AChessboard::RemoveDefaultMaterialsLastMoveGrids()
{
	this->DefaultMaterialsLastMoveGrids.Empty();
}

