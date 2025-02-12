// Fill out your copyright notice in the Description page of Project Settings.

#include "Queen.h"

// Constructor
AQueen::AQueen()
{
	// Set Static Mesh Component
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("StaticMesh'/Game/Meshes/Queen.Queen'"));  //Get the mesh of the piece in the folder specified
	if (FoundMesh.Succeeded())  //If it found it
	{
		StaticMesh->SetStaticMesh(FoundMesh.Object);  //Set the mesh for this piece 
	}
}
