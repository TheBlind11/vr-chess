// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnPiece.h"

#include "Bishop.h"
#include "Knight.h"
#include "Queen.h"
#include "Rook.h"
#include "Controllers/WidgetController.h"
#include "Utility/Utility.h"


// Constructor. Gets and assigns mesh to class.
APawnPiece::APawnPiece()
{
	// Set Static Mesh Component
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("StaticMesh'/Game/Meshes/Pawn.Pawn'"));  //Get the mesh of the piece in the folder specified
	if (FoundMesh.Succeeded())  //If it found it
	{
		StaticMesh->SetStaticMesh(FoundMesh.Object);  //Set the mesh for this piece
	}
}

//Function useful to change the mesh of a pawn piece due to a promotion
void APawnPiece::ChangeMesh(FString PromoteTo)
{
	UE_LOG(LogTemp, Warning, TEXT("PawnPiece::MakingPromotion"));  //Log to check which function is running

	//AWidgetController* WidgetController = Cast<AWidgetController>(Utility::FindByTag(GetWorld(), "WidgetController"));  //Get the widget controller in scene by its tag

	bool IsWhite = this->GetIsWhite();  //Check if the pawn is white or black

	FVector Location = this->GetActorLocation();  //Get the location of the pawn
	FRotator Rotator;
	APiece* ActorToDestroy = this;  //We need to destroy this pawn from the scene, and create another one
	ActorToDestroy->Destroy();  //Destroy the pawn from the scene
	APiece* NewPiece = nullptr;

	if (IsWhite)  //If the pawn was white
	{
		Rotator = FRotator::ZeroRotator;
	}

	else  //The pawn was black
	{
		Rotator = FRotator(0, -180, 0);  //The rotation it's different between black and white pieces
	}
	
	if(PromoteTo == "Queen")  //If player chose to promote the pawn to a queen
	{
		NewPiece = GetWorld()->SpawnActor<AQueen>(Location, Rotator);  //Create a queen in that position
		UE_LOG(LogTemp, Warning, TEXT("PawnPiece::PawnPromotedToQueen"));
	}

	if(PromoteTo == "Knight")  //If player chose to promote the pawn to a knight
	{
		NewPiece = GetWorld()->SpawnActor<AKnight>(Location, Rotator);  //Create a knight in that position
		UE_LOG(LogTemp, Warning, TEXT("PawnPiece::PawnPromotedToKnight"));
	}
	
	if(PromoteTo == "Rook")  //If player chose to promote the pawn to a rook
	{
		NewPiece = GetWorld()->SpawnActor<ARook>(Location, Rotator);  //Create a rook in that position
		UE_LOG(LogTemp, Warning, TEXT("PawnPiece::PawnPromotedToRook"));
	}
	
	if(PromoteTo == "Bishop")  //If player chose to promote the pawn to a bishop
	{
		NewPiece = GetWorld()->SpawnActor<ABishop>(Location, Rotator);  //Create a bishop in that position
		UE_LOG(LogTemp, Warning, TEXT("PawnPiece::PawnPromotedToBishop"));
	}
	
	NewPiece->SetIsWhite(IsWhite);  //Set the color of the piece
	NewPiece->Tags.Add("Piece");
	NewPiece->SetDeselected();  //Deselect the piece in order to change its material to default
}