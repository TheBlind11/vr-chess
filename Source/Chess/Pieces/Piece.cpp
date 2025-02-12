// Fill out your copyright notice in the Description page of Project Settings.

#include "Piece.h"

// Sets default values
APiece::APiece()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create static mesh and assign it as root
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

	StaticMesh->SetCollisionProfileName(TEXT("Pawn"));	// Set Collision
	StaticMesh->SetSimulatePhysics(true);				// Set Physics

	// Set WhiteMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> GetWhiteMaterial(TEXT("Material'/Game/Materials/M_LightMaterial.M_LightMaterial'"));  //Get the material in the folder specified (white pieces)
	if (GetWhiteMaterial.Succeeded())  //If it found it
	{
		WhiteMaterial = GetWhiteMaterial.Object;  //Assign material object in the field
	}

	// Set BlackMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> GetBlackMaterial(TEXT("Material'/Game/Materials/M_DarkMaterial.M_DarkMaterial'"));  //Get the material in the folder specified (black pieces)
	if (GetBlackMaterial.Succeeded())  //If it found it
	{
		BlackMaterial = GetBlackMaterial.Object;  //Assign material object in the field
	}

	// Set SelectedMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> GetSelectedMaterial(TEXT("Material'/Game/Materials/M_SelectedPieceMaterial.M_SelectedPieceMaterial'"));  //Get the material in the folder specified
	if (GetSelectedMaterial.Succeeded())  //If it found it
	{
		SelectedMaterial = GetSelectedMaterial.Object;  //Assign material object in the field
	}
}

// Called when the game starts or when spawned
void APiece::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Call when actor is selected by mouse click. Changes actor's material to selected material to indicate it is selected.
void APiece::SetSelected()
{
	StaticMesh->SetMaterial(0, SelectedMaterial);
}

// Called when actor is deselected. Changes actor's material to it's original material.
void APiece::SetDeselected()
{
	/*	REMAINDER TO ME: IF I ADD ADDITIONAL CODES HERE
		CREATE ANOTHER FUNCTION CALLED SetDefaultMaterial WHICH ONLY INCLUDES THIS CODES.
	 */
	if (IsWhite)  //If the piece is white
	{
		StaticMesh->SetMaterial(0, WhiteMaterial);  //Set white material to the piece
	}
	else
	{
		StaticMesh->SetMaterial(0, BlackMaterial);  //Set black material to the piece
	}
}

// Called when actor is moved by the player. Gets one parameter which holds location to move.
void APiece::ChangeLocation(FVector MoveLocation)
{
	SetActorLocation(MoveLocation);
}

// Eliminates piece from scene.
void APiece::Eliminate()
{
	this->Destroy();
}

//Function called to set white or black color to a piece
void APiece::SetIsWhite(bool IsWhite)
{
	this->IsWhite = IsWhite;
}

//Function useful to understand if a piece is black or white
bool APiece::GetIsWhite()
{
	return this->IsWhite;
}
