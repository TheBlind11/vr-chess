// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Pieces/Piece.h"
#include "Chessboard.generated.h"

UCLASS()
class CHESS_API AChessboard : public AActor
{
	GENERATED_BODY()

public:
	
	// Sets default values for this actor's properties
	AChessboard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UStaticMeshComponent* FindCurrentGrid(APiece* Piece);
	
	//Sets highlighted material to selectable grids
	void HighlightSelectableGrids(APiece* SelectedPiece, TArray<TSharedPtr<FJsonValue>> AvailableMoves);

	TArray<UStaticMeshComponent*> GetSelectableGrids();

	//Function useful to highlight a grid
	void SetMaterialToGrid(UStaticMeshComponent* Grid, UMaterial* Material);

	//Function useful to map indexes of chessboard between client and server
	int32 MappingIndex(int32 Index);

	//Function useful to spawn white pieces on chessboard
	void SpawnWhitePieces();

	//Function useful to spawn black pieces on chessboard
	void SpawnBlackPieces();

	//Function useful to display last move grids
	void DisplayLastMoveGrids(int32 Source, int32 Endpoint);

	//Returns highlighted grids' material to their default material
	void SetDefaultMaterialsToGrids(bool LastMove);

	//Function useful to get all grids mapped with indexes in an array
	TArray<UStaticMeshComponent*> GetAllGrids();

	//Function useful to get material of last move grids
	UMaterial* GetLastMoveMaterial();

	//Function useful to get material of grids that can be selected to make a move
	UMaterial* GetHighlightedMaterial();
	
	UMaterial* GetMouseHoverMaterial();

	UFUNCTION()
	void OnCursorOver(UPrimitiveComponent* Component);

	UFUNCTION()
	void EndCursorOver(UPrimitiveComponent* Component);
	
	void RemoveSelectableGrids();
	
	void RemoveDefaultMaterialsSelectableGrids();
	
	void RemoveLastMoveGrids();
	
	void RemoveDefaultMaterialsLastMoveGrids();

private:
	// Box component to use as Root Component
	UPROPERTY()
	class UBoxComponent* BoxComponent;

	// Pointer array to hold every grid's address.
	UPROPERTY()
	TArray<class  UStaticMeshComponent*> AllGrids;

	TArray<UMaterial*> AllMaterials;

	//Light colored grid material for the white chess grids.
	UPROPERTY()
	class UMaterial* LightMaterial;

	// Dark colored grid material for the black chess grids.
	UPROPERTY()
	class UMaterial* DarkMaterial;

	// Grid's material is changed to this when player has ability to move to this grid.
	UPROPERTY()
	class UMaterial* HighlightedMaterial;

	// This is null by default. Only used as temporary pointer when grid's original material is changed to highlighted or selected material.
	UPROPERTY()
	class UMaterial* CurrentMaterial;

	//LastMoveMaterial field
	UPROPERTY()
	UMaterial* LastMoveMaterial;

	//MouseHoverMaterial field. Material useful to player to understand which grid or piece is going to select
	UPROPERTY()
	UMaterial* MouseHoverMaterial;
	
	TArray<UStaticMeshComponent*> LastMoveGrids;

	TArray<UStaticMeshComponent*> HighlightedGrids;
	
	TArray<UMaterial*> DefaultMaterialsLastMoveGrids;
	
	TArray<UMaterial*> DefaultMaterialsHighlightedGrids;
};
