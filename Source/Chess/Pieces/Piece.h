// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Piece.generated.h"

UCLASS()
class CHESS_API APiece : public AActor
{
	GENERATED_BODY()

/******** FUNCTIONS START *******/
public:	
	// Sets default values for this actor's properties
	APiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Call when actor is selected by mouse click.
	void SetSelected();

	// Called when actor is deselected.
	void SetDeselected();

	// Changes actors location. Gets one parameter which holds location to move.
	void ChangeLocation(FVector MoveLocation);

	// Eliminates piece from the game.
	void Eliminate();

	//Function useful to set a piece white or black
	void SetIsWhite(bool IsWhite);

	//Function useful to understand if a piece is black or white
	bool GetIsWhite();

protected:
	
	// Holds information about whether or not this piece is white. Use to check piece's color.
	bool IsWhite;

	// Static mesh for the class. Null by default overridden for every chess piece.
	UPROPERTY()
	class UStaticMeshComponent* StaticMesh;

	// White material for the piece.
	UPROPERTY()
	class UMaterial* WhiteMaterial;

	// Black material for the piece.
	UPROPERTY()
	class UMaterial* BlackMaterial;

	// Material to set when piece is selected.
	UPROPERTY()
	class UMaterial* SelectedMaterial;

};
