// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight.h"

// Sets default values
AKnight::AKnight()
{
	// Set Static Mesh Component
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FoundMesh(TEXT("StaticMesh'/Game/Meshes/Knight.Knight'"));  //Get the mesh of the piece in the folder specified
	if (FoundMesh.Succeeded())  //If it found it
	{
		StaticMesh->SetStaticMesh(FoundMesh.Object);  //Set the mesh for this piece 
	}
}
