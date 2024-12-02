#include "GodsOfEnneadPlayerController.h"
#include "EngineUtils.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Public/CardActor.h"
#include "Blueprint/UserWidget.h"
#include "CardUserWidget.h"
#include "GameFramework/Character.h"

AGodsOfEnneadPlayerController::AGodsOfEnneadPlayerController()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClass(TEXT("/Game/BP/BP_Character.BP_Character"));
    if (PlayerPawnClass.Succeeded())
    {
        FVector ddSpawnLocation = FVector(0.0f, 0.0f, 300.0f); // Задайте начальную позицию
        FRotator ddSpawnRotation = FRotator(0.0f, 0.0f, 0.0f); // Задайте начальное вращение
        GetWorld()->SpawnActor<ACharacter>(PlayerPawnClass.Class, ddSpawnLocation, ddSpawnRotation);
    };

    static ConstructorHelpers::FObjectFinder<UDataTable> dataCardsObject(TEXT("DataTable'/Game/DataBase/DT_They.DT_They'"));
    if (dataCardsObject.Succeeded())
    {
        CardDataTable = dataCardsObject.Object;
    }

    // for (int i = 0; i < )
}
void AGodsOfEnneadPlayerController::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Error, TEXT("BeginPlay"));
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    PC->bShowMouseCursor = true;
    PC->bEnableClickEvents = true;
    PC->bEnableMouseOverEvents = true;
    
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
    
    if (APawn* ControlledPawn = GetPawn())
    {
        ControlledPawn->DisableInput(this);
    }

    const APlayerStart* PlayerStart = nullptr;
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        PlayerStart = *It;
        break;
    }

    if (PlayerStart)
    {
        const FVector InitialLocation = PlayerStart->GetActorLocation();
        GetPawn()->SetActorLocation(InitialLocation);
    }

    EndLocation = FVector(7157.0f, 7200.00f, 2380.00f);
    EndRotation = FRotator(-66.00f, 0.00f, 0.00f);
    MoveDuration = 3.0f;

    if (const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld()); CurrentLevelName == "Game")    
    {
        MovePlayerToTarget();
    }
}

void AGodsOfEnneadPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindAction("SelectCard", IE_Pressed, this, &AGodsOfEnneadPlayerController::TakeCard);
    }}

void AGodsOfEnneadPlayerController::TakeCard()
{
    if (CurrentTurnStatus == ETurnStatus::Waiting_Choose)
    {
       FHitResult HitResult;
        if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
        {
            if (ACardActor* ClickedCard = Cast<ACardActor>(HitResult.GetActor()))
            {
                if (!DeckCardsActors.Contains(ClickedCard) && !ShowDeckCardsActors.Contains(ClickedCard)) return;
               
                UE_LOG(LogTemp, Log, TEXT("Player: Card clicked: %s"), *ClickedCard->GetName());
                PlayersHands[1]->MoveToHand(ClickedCard);
                if (DeckCardsActors.Contains(ClickedCard))
                {
                    DeckCardsActors.Remove(ClickedCard);
                    FixDeck(DeckCardsActors);
                }
                else
                {
                    ShowDeckCardsActors.Remove(ClickedCard);
                    FixDeck(ShowDeckCardsActors);
                }
                CurrentTurnStatus = ETurnStatus::Player_Turn;
            }
        }
    }

    if (CurrentTurnStatus == ETurnStatus::Player_Turn)
    {
       FHitResult HitResult;
        if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
        {
            if (ACardActor* ClickedCard = Cast<ACardActor>(HitResult.GetActor()))
            {
                bool bCardFound = PlayersHands[1]->CardPositions.FindByPredicate([ClickedCard](const FCardPosition& CardPosition)
                {
                    return CardPosition.CardActor == ClickedCard;
                }) != nullptr;
                if (!bCardFound) return;
               
                UE_LOG(LogTemp, Log, TEXT("Player: Card clicked: %s"), *ClickedCard->GetName());
                PlayersHands[1]->MoveToDeck(ClickedCard, ShowDeckCardsActors.Num() ? ShowDeckCardsActors.Last()->GetActorLocation()
                                                                                    :  FVector(7840.0f, 7680.0f, 563.0f));
                ShowDeckCardsActors.Add(ClickedCard);
                CurrentTurnStatus = ETurnStatus::Waiting_Choose;
            }
        }
    }
}

void AGodsOfEnneadPlayerController::MovePlayerToTarget()
{
    if (!GetPawn() || MoveDuration <= 0.0f)
    {
        return;
    }

    StartLocation = GetPawn()->GetActorLocation();
    StartRotation = GetPawn()->GetActorRotation();
    ElapsedTime = 0.0f;
    bIsMoving = true;

    SetTickableWhenPaused(true);
    SetActorTickEnabled(true);
}

void AGodsOfEnneadPlayerController::StartGame()
{
    UE_LOG(LogTemp, Log, TEXT("StartGame: Deal Cards to Player"));
    DealCards(g_maxInHand, false);
    UE_LOG(LogTemp, Log, TEXT("StartGame: Deal Cards to Bot"));
    DealCards(g_maxInHand, true);

    PlayRound();
}

void AGodsOfEnneadPlayerController::PlayRound()
{
    CurrentTurnStatus = ETurnStatus::Waiting_Choose;
}

void AGodsOfEnneadPlayerController::DealCards(int32 NumCards, bool bIsPlayer)
{
    int PlayerIndex = bIsPlayer ? 1 : 0;
    if (PlayersHands.IsEmpty() || !PlayersHands.IsValidIndex(PlayerIndex))
    {
        PlayersHands.Add(NewObject<UHand>());
        PlayersHands[PlayerIndex]->bIsPlayer = bIsPlayer;
    }
    
    for (int32 i = 0; i < NumCards; ++i)
    {
        ACardActor* Card = DeckCardsActors.Last();
        DeckCardsActors.Remove(Card);
        FixDeck(DeckCardsActors);
        FVector TargetLocation = FVector(7280.0f, 5676.0f + i * 500.0f, 563.0f + i * 0.1f);

        if (!bIsPlayer)
        {
            TargetLocation = FVector(8470.0f, 5676.0f + i * 500.0f, 563.0f + i * 0.1f);
        }
        else
        {
            // Card->SetActorRotation(SHOW_ROTATION); TODO return after setting material on card
        }

        if (Card)
        {
            PlayersHands[PlayerIndex]->AddCard(Card, TargetLocation);

            Card->AnimateTo(Card->GetActorLocation(), TargetLocation);
            UE_LOG(LogTemp, Warning, TEXT("card location: %s"), *TargetLocation.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Не удалось создать карту"));
        }
    }
}

void AGodsOfEnneadPlayerController::SpawnActors()
{
    FVector SpawnStartLocation(7840.00f, 6755.00f, 563.00f);
    FRotator SpawnStartRotation(90.00f, 90.0f, -90.0f);

    SpawnedActorCount = 0;
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, [this, SpawnStartLocation, SpawnStartRotation]()
    {
        SpawnActorStep(SpawnStartLocation, SpawnStartRotation);
    }, 0.03f, true);
}

void AGodsOfEnneadPlayerController::UpdateMovement(float DeltaTime)
{
    if (!bIsMoving || !GetPawn())
    {
        return;
    }

    ElapsedTime += DeltaTime;
    const float Alpha = FMath::Clamp(ElapsedTime / MoveDuration, 0.0f, 1.0f);

    const FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);

    const FQuat StartQuat = FQuat(StartRotation);
    const FQuat EndQuat = FQuat(EndRotation);
    const FQuat NewQuat = FQuat::Slerp(StartQuat, EndQuat, Alpha);

    if (USceneComponent* PawnRootComponent = GetPawn()->GetRootComponent())
    {
        PawnRootComponent->SetRelativeLocation(NewLocation);
        PawnRootComponent->SetRelativeRotation(NewQuat);
    }

    if (APlayerController* PlayerController = Cast<APlayerController>(this))
    {
        FRotator CameraRotation = NewQuat.Rotator();
        PlayerController->SetControlRotation(CameraRotation);
    }

    if (Alpha >= 1.0f)
    {
        bIsMoving = false;
        SpawnActors();
    }
}

void AGodsOfEnneadPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateMovement(DeltaTime);
}

void AGodsOfEnneadPlayerController::FixDeck(TArray<ACardActor*> Deck)
{
    for (int i = 0; i < Deck.Num(); i++)
    {
        Deck[i]->SetActorLocation(Deck[i]->GetActorLocation() + FVector(0.0f, 0.0f, -2.0f));
    }
}

void AGodsOfEnneadPlayerController::SpawnActorStep(const FVector& StartSpawnLocation, const FRotator& StartSpawnRotation)
{
    if (SpawnedActorCount >= g_cardCount)
    {
        UE_LOG(LogTemp, Warning, TEXT("ClearTimer"));
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
        UE_LOG(LogTemp, Warning, TEXT("All actors spawned. Total: %d"), g_cardCount);
        StartGame();
        return;
    }

    FVector FinalCardLocation(7840.0f, 7680.0f, 563.0f);
    // FRotator FinalCardRotation = SHOW_ROTATION; TODO return after setting material on card
    FRotator FinalCardRotation = SHOW_ROTATION; // TODO remove after setting material on card
    FVector FinalCardScale(90.0f, 19.471221f, -160.528779f);

    FVector SpawnNewCardLocation;
    FRotator SpawnNewCardRotation;

    if (SpawnedActorCount == g_cardCount - 1)
    {
        SpawnNewCardLocation = FinalCardLocation;
        SpawnNewCardRotation = FinalCardRotation;
    }
    else
    {
        SpawnNewCardLocation = StartSpawnLocation + FVector(0.0f, 0.0f, SpawnedActorCount * 2.0f);
        // SpawnNewCardRotation = HIDE_ROTATION; TODO return after setting material on card
        SpawnNewCardRotation = FinalCardRotation; // TODO remove after setting material on card
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;

    if (ACardActor* SpawnedActor = GetWorld()->SpawnActor<ACardActor>(ACardActor::StaticClass(), SpawnNewCardLocation, SpawnNewCardRotation, SpawnParams))
    {
        
        if (SpawnedActorCount == g_cardCount - 1)
        {
            ShowDeckCardsActors.Add(SpawnedActor);
        }
        else
        {
            DeckCardsActors.Add(SpawnedActor);
        }
        
        if (!SpawnedActor->GetRootComponent())
        {
            USceneComponent* DefaultRoot = NewObject<USceneComponent>(SpawnedActor, USceneComponent::StaticClass(), TEXT("DefaultSceneRoot"));
            SpawnedActor->SetRootComponent(DefaultRoot);
            DefaultRoot->RegisterComponent();

            SpawnedActor->SetActorLocationAndRotation(SpawnNewCardLocation, SpawnNewCardRotation);
            SpawnedActor->SetActorScale3D(FinalCardScale);
        }

        TArray<FDataCardStruct*> dtRows;
        CardDataTable->GetAllRows("", dtRows);

        const int rowIndex = SpawnedActorCount % dtRows.Num();

        FDataCardStruct* Item = dtRows[rowIndex];
        SpawnedActor->SetDataCard(Item->hp, Item->attack, Item->cardName);
    
        if (UWidgetComponent* WidgetComponent = NewObject<UWidgetComponent>(SpawnedActor, UWidgetComponent::StaticClass(), TEXT("CardWidget")))
        {
            WidgetComponent->AttachToComponent(SpawnedActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            WidgetComponent->RegisterComponent();

            WidgetComponent->SetDrawSize(FVector2D(500.0f, 500.0f));
            WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
            WidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));

            const TSubclassOf<UUserWidget> WidgetClass = LoadClass<UCardUserWidget>(nullptr, TEXT("/Game/BP/UI/WBP_Card.WBP_Card_C"));
            if (WidgetClass)
            {
                WidgetComponent->SetWidgetClass(WidgetClass);
                WidgetComponent->SetVisibility(true);
                WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

                auto* cardWidget = (UCardUserWidget*)(WidgetComponent->GetWidget());
                const FDataCardStruct& dataCard = SpawnedActor->GetDataCard();
                cardWidget->new_attack = dataCard.attack;
                cardWidget->new_hp = dataCard.hp;
                cardWidget->new_name_character = dataCard.cardName;

                //TODO Set image here
                //cardWidget->new_icon_character.SetResourceObject()

                UE_LOG(LogTemp, Warning, TEXT("WidgetComponent added to actor successfully."));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to load widget class."));
            }
        }

        // UE_LOG(LogTemp, Warning, TEXT("Actor %d spawned at location: %s"), SpawnedActorCount, *SpawnNewCardLocation.ToString());
    }

    ++SpawnedActorCount;
}