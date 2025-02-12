// Fill out your copyright notice in the Description page of Project Settings.

#include "Bishop.h"

// Constructor
ABishop::ABishop()
{
	// Set Static Mesh Component
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("StaticMesh'/Game/Meshes/Bishop.Bishop'"));  //Get the mesh of the piece in the folder specified
	if (FoundMesh.Succeeded())  //If it found it
	{
		StaticMesh->SetStaticMesh(FoundMesh.Object);  //Set the mesh for this piece 
	}
}