// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetController.h"

#include "Button.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/ChessUserWidget.h"
#include "Widget/PromotionWidget.h"

// Sets default values
AWidgetController::AWidgetController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<UChessUserWidget> WidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/Chess_UI'"));  //Get the class in the folder specified in order to spawn ChessUIWidget
	if (WidgetAsset.Succeeded())  //If it found it
	{
		UE_LOG(LogTemp, Warning, TEXT("CHESS_UI ASSET FOUND!"));
		// Assign the class of the loaded asset to the WigetClass variable, which is a "subclass" of UUserWidget : Which our asset class is #1#
		ChessUIWidgetClass = WidgetAsset.Class;
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO LOAD CHESS_UI ASSET"));
	}

	static ConstructorHelpers::FClassFinder<UCountdownWidget> CountdownAsset(TEXT("WidgetBlueprint'/Game/Blueprints/CountdownWidget_UI'"));  //Get the class in the folder specified in order to spawn CountdownWidget
	if (CountdownAsset.Succeeded())  //If it found it
	{
		UE_LOG(LogTemp, Warning, TEXT("COUNTDOWN_WIDGET_UI ASSET FOUND!"));
		//Assign the class of the loaded asset to the WigetClass variable, which is a "subclass" of UUserWidget : Which our asset class is #1#
		CountdownWidgetClass = CountdownAsset.Class;
	}
	
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO LOAD COUNTDOWN_WIDGET_UI ASSET"));
	}

	static ConstructorHelpers::FClassFinder<UGameFinishedWidget> GameFinishedAsset(TEXT("WidgetBlueprint'/Game/Blueprints/GameFinishedWidget_UI'"));  //Get the class in the folder specified in order to spawn GameFinishedWidget
	if (GameFinishedAsset.Succeeded())  //If it found it
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME_FINISHED_WIDGET_UI ASSET FOUND!"));
		//Assign the class of the loaded asset to the WigetClass variable, which is a "subclass" of UUserWidget : Which our asset class is #1#
		GameFinishedWidgetClass = GameFinishedAsset.Class;
	}
	
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO LOAD GAME_FINISHED_WIDGET_UI ASSET"));
	}

	static ConstructorHelpers::FClassFinder<UPromotionWidget> PromotionWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/PromotionWidget_UI'"));  //Get the class in the folder specified in order to spawn PromotionWidget
	if (PromotionWidgetAsset.Succeeded())  //If it found it
	{
		UE_LOG(LogTemp, Warning, TEXT("PROMOTION_WIDGET_UI ASSET FOUND!"));
		//Assign the class of the loaded asset to the WigetClass variable, which is a "subclass" of UUserWidget : Which our asset class is #1#
		PromotionWidgetClass = PromotionWidgetAsset.Class;
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO LOAD PROMOTION_WIDGET_UI ASSET"));
	}

	// Root scene (per poter posizionare i widget component in 3D)
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	// Creiamo i vari WidgetComponent per i tuoi widget
	ChessUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ChessUIComponent"));
	ChessUIComponent->SetupAttachment(RootComp);

	CountdownComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CountdownComponent"));
	CountdownComponent->SetupAttachment(RootComp);

	PromotionComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PromotionComponent"));
	PromotionComponent->SetupAttachment(RootComp);

	GameFinishedComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("GameFinishedComponent"));
	GameFinishedComponent->SetupAttachment(RootComp);

	/** Impostazioni di default:
	 *  - World Space (UI resa in 3D)
	 *  - Dimensioni e posizione provvisorie
	 */
	ChessUIComponent->SetWidgetSpace(EWidgetSpace::World);
	ChessUIComponent->SetDrawSize(FVector2D(1920, 1080));
	ChessUIComponent->SetPivot(FVector2D(0.5f, 0.5f));
	ChessUIComponent->SetRelativeRotation(FRotator(0, 180.f, 0.f));

	CountdownComponent->SetWidgetSpace(EWidgetSpace::World);
	CountdownComponent->SetDrawSize(FVector2D(1920, 1080));
	CountdownComponent->SetPivot(FVector2D(0.5f, 0.5f));
	CountdownComponent->SetRelativeRotation(FRotator(0, 180.f, 0.f));

	// Idem PromotionComponent e GameFinishedComponent
	PromotionComponent->SetWidgetSpace(EWidgetSpace::World);
	PromotionComponent->SetDrawSize(FVector2D(1920, 1080));
	PromotionComponent->SetPivot(FVector2D(0.5f, 0.5f));
	PromotionComponent->SetRelativeRotation(FRotator(0, 180.f, 0.f));

	GameFinishedComponent->SetWidgetSpace(EWidgetSpace::World);
	GameFinishedComponent->SetDrawSize(FVector2D(1920, 1080));
	GameFinishedComponent->SetPivot(FVector2D(0.5f, 0.5f));
	GameFinishedComponent->SetRelativeRotation(FRotator(0, 180.f, 0.f));
	
}

// Called when the game starts or when spawned
void AWidgetController::BeginPlay()
{
	Super::BeginPlay();

	// Assegna le classi Blueprint ai component (se disponibili)
	if (ChessUIComponent && ChessUIWidgetClass)
	{
		ChessUI = CreateWidget<UChessUserWidget>(GetWorld(), ChessUIWidgetClass);
		if (ChessUI == nullptr)
			UE_LOG(LogTemp, Warning, TEXT("ChessUIWidget is NULL"));

		ChessUIComponent->SetWidget(ChessUI);
		ChessUIComponent->SetVisibility(false);
	}

	if (CountdownComponent && CountdownWidgetClass)
	{
		CountdownWidget = CreateWidget<UCountdownWidget>(GetWorld(), CountdownWidgetClass);
		if (CountdownWidget == nullptr)
			UE_LOG(LogTemp, Warning, TEXT("CountdownWidget is NULL"));

		CountdownComponent->SetWidget(CountdownWidget);
		CountdownComponent->SetVisibility(false);
	}

	if (PromotionComponent && PromotionWidgetClass)
	{
		PromotionWidget = CreateWidget<UPromotionWidget>(GetWorld(), PromotionWidgetClass);
		if (PromotionWidget == nullptr)
			UE_LOG(LogTemp, Warning, TEXT("PromotionIWidget is NULL"));
		
		PromotionComponent->SetWidget(PromotionWidget);
		PromotionComponent->SetVisibility(false);
	}

	if (GameFinishedComponent && GameFinishedWidgetClass)
	{
		GameFinishedWidget = CreateWidget<UGameFinishedWidget>(GetWorld(), GameFinishedWidgetClass);
		if (GameFinishedWidget == nullptr)
			UE_LOG(LogTemp, Warning, TEXT("GameFinishedWidget is NULL"));

		GameFinishedComponent->SetWidget(GameFinishedWidget);
		GameFinishedComponent->SetVisibility(false);
	}
	
}

void AWidgetController::ShowChessUI(bool ToShow)
{
	ChessUIComponent->SetVisibility(ToShow);
}

UWidgetComponent* AWidgetController::GetCountdownUIComponent()
{
	return this->CountdownComponent;
}

//Function to show CountdownWidget in scene
void AWidgetController::ShowCountdown(int Seconds)
{
	this->ShowCountdownUI(true);
	CountdownWidget->SetCountdown(Seconds);
}

//Function to show CountdownWidget in scene
void AWidgetController::ShowCountdownUI(bool ToShow)
{
	UE_LOG(LogTemp, Display, TEXT("AWidgetController::ShowCountdownUI"));  //Log to understand what function is running

	// Step 0 get player controller.
	const auto PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!ensure(PlayerController != nullptr))
	{
		return;
	}
	
	if (CountdownWidget == nullptr)  //If CountdownWidget is null we need to create it
	{
		CountdownWidget = CreateWidget<UCountdownWidget>(GetWorld(), CountdownWidgetClass);  //Spawn it
		CountdownComponent->SetWidget(CountdownWidget);
	}
	
	if (CountdownWidget && ToShow)  //If CountdownWidget is not null and we need to show it
	{
		//CountdownWidget->AddToViewport(0);  //Add it to viewport
		CountdownComponent->SetVisibility(ToShow);
		
		UE_LOG(LogTemp, Display, TEXT("AWidgetController::ShowCountdownUI -> CountdownWidget Added"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("AWidgetController::ShowCountdownUI -> CountdownWidget Missing"));
	}

	if (CountdownWidget && !ToShow && CountdownComponent->IsVisible()) //CountdownWidget->IsVisible())  //If CountdownWidget is not null, we don't need to show it and it's visible in scene
	{
		UE_LOG(LogTemp, Display, TEXT("AWidgetController::ShowCountdownUI -> CountdownWidget Removed"));
		
		//CountdownWidget->RemoveFromViewport();  //Remove it from viewport
		CountdownComponent->SetVisibility(ToShow);
		CountdownWidget = nullptr;
	}
}

//Function to remove CountdownWidget from scene when countdown finishes
void AWidgetController::RemoveCountdownUI()
{
	this->ShowCountdownUI(false);
}

//Function to show PromotionWidget in scene
void AWidgetController::DisplayPromotionWidgetUI(bool ToShow)
{
	UE_LOG(LogTemp, Warning, TEXT("WidgetController::DisplayPromotionWidgetUI"))  //Log to understand what function is running
	//this->ChessUI->RemoveFromViewport();  //Remove ChessUIWidget from viewport
	this->ChessUIComponent->SetVisibility(false);

	if (PromotionWidget == nullptr)  //If PromotionWidget is null we need to create it
	{
		UE_LOG(LogTemp, Warning, TEXT("PROMOTION_WIDGET DOESN'T EXISTS"));
		PromotionWidget = CreateWidget<UPromotionWidget>(GetWorld(), PromotionWidgetClass);  //Spawn it in the world
		PromotionComponent->SetWidget(PromotionWidget);
	}

	if(PromotionWidget != nullptr && ToShow)  //If PromotionWidget has already been created we just need to add it to viewport
	{
		//this->PromotionWidget->AddToViewport();  //Add to viewport
		this->PromotionComponent->SetVisibility(ToShow);
		UE_LOG(LogTemp, Warning, TEXT("PROMOTIONWIDGET EXISTS AND DISPLAYED"));
	}

	if (PromotionWidget && !ToShow && PromotionComponent->IsVisible())
	{
		UE_LOG(LogTemp, Display, TEXT("AWidgetController::DisplayPromotionWidgetUI -> PromotionWidgetUI Removed"));
		PromotionComponent->SetVisibility(ToShow);
		PromotionWidget = nullptr;
	}
}

//Function to show GameFinishedWidget in scene
void AWidgetController::ShowGameFinishedUI(bool ToShow)
{
	UE_LOG(LogTemp, Display, TEXT("AWidgetController::ShowGameFinishedUI"));  //Log to understand what function is running

	// Step 0 get player controller.
	const auto PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!ensure(PlayerController != nullptr))
	{
		return;
	}
	
	if (GameFinishedWidget == nullptr)  //If it's null we need to create it
	{
		GameFinishedWidget = CreateWidget<UGameFinishedWidget>(GetWorld(), GameFinishedWidgetClass);  //Creation of GameFinishedWidget
		GameFinishedComponent->SetWidget(GameFinishedWidget);
	}
	
	if (GameFinishedWidget && ToShow)  //If it's not null and we need to show it
	{
		//GameFinishedWidget->AddToViewport(0);  //Add it to viewport
		GameFinishedComponent->SetVisibility(ToShow);
		GameFinishedWidget->DisplayAndEnableRematchButton();
		UE_LOG(LogTemp, Display, TEXT("AWidgetController::ShowGameFinishedUI -> GameFinishedWidget Added"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("AWidgetController::ShowGameFinishedUI -> GameFinishedWidget Missing"));
	}

	if (GameFinishedWidget && !ToShow && GameFinishedComponent->IsVisible())  //If it's not null, we don't need to show it and it's visible in scene
	{
		UE_LOG(LogTemp, Display, TEXT("AWidgetController:ShowGameFinishedUI -> GameFinishedWidget Removed"));
		//GameFinishedWidget->RemoveFromViewport();  //Remove it from viewport
		GameFinishedComponent->SetVisibility(ToShow);
		GameFinishedWidget = nullptr;
	}
}

//Function to show GameFinishedWidget in scene
void AWidgetController::ShowGameFinishedWidget(bool YouLost, const bool IsDraw, const FString& Reason, const int NftId)
{
	UE_LOG(LogTemp, Warning, TEXT("WidgetController::ShowGameFinishedWidget"))  //Log to understand what function is running
	//this->ChessUI->RemoveFromViewport();  //Remove ChessUIWidget from viewport
	this->ChessUIComponent->SetVisibility(false);
	this->ShowGameFinishedUI(true);  //Show GameFinishedWidget
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();  //Get PlayerController in order to change player's controls
	//PlayerController->SetInputMode(FInputModeUIOnly());  //Player can only interact with Widget
	this->GameFinishedWidget->SetUp(YouLost, Reason, IsDraw, NftId);  //Set up of GameFinishedWidget
}

//Function to remove GameFinishedWidget from scene
void AWidgetController::RemoveGameFinishedUI()
{
	this->ShowGameFinishedUI(false);
}

//Function to show CanAcceptDrawButtons when opponent offers a draw
void AWidgetController::ShowCanAcceptDrawWidget()
{
	this->ChessUI->DisplayCanAcceptDrawButtons();
}

void AWidgetController::ShowCanAcceptRematchWidget()
{
	this->GameFinishedWidget->DisplayCanAcceptRematchButtons();
}

//Function to enable OfferDrawButtons at the next turn
void AWidgetController::EnableOfferDrawButton()
{
	UButton* OfferDrawButton = Cast<UButton>(this->ChessUI->GetWidgetFromName(TEXT("Offer_draw")));  //Get the button in the widget

	if(!OfferDrawButton->GetIsEnabled())  //If it's not enabled
		OfferDrawButton->SetIsEnabled(true);  //Enable it, player can now use the button and offer draw to the opponent
}

UGameFinishedWidget* AWidgetController::GetGameFinishedWidget()
{
	return this->GameFinishedWidget;
}

UWidgetComponent* AWidgetController::GetGameFinishedUIComponent()
{
	return this->GameFinishedComponent;
}

//This function sets ChessUIWidget in its field
void AWidgetController::SetChessUI(UChessUserWidget* Widget)
{
	this->ChessUI = Widget;
}

//Function to get ChessUIWidget
UChessUserWidget* AWidgetController::GetChessUI()
{
	return this->ChessUI;
}

UWidgetComponent* AWidgetController::GetChessUIComponent()
{
	return this->ChessUIComponent;
}

//Function to get ChessUIWidgetClass
TSubclassOf<UUserWidget> AWidgetController::GetChessUIWidgetClass()
{
	return this->ChessUIWidgetClass;
}

// Called every frame
void AWidgetController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

