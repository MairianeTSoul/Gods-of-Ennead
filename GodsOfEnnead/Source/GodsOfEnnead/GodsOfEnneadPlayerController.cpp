#include "GodsOfEnneadPlayerController.h"

#include "AITestsCommon.h"
#include "EngineUtils.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Public/CardActor.h"
#include "Blueprint/UserWidget.h"
#include "CardUserWidget.h"
#include "ResultUserWidget.h"
#include "Task.h"
#include "GameFramework/Character.h"

AGodsOfEnneadPlayerController::AGodsOfEnneadPlayerController()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClass(TEXT("/Game/BP/BP_Character.BP_Character"));
    if (PlayerPawnClass.Succeeded())
    {
        FVector ddSpawnLocation = FVector(0.0f, 0.0f, 300.0f);
        FRotator ddSpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
        GetWorld()->SpawnActor<ACharacter>(PlayerPawnClass.Class, ddSpawnLocation, ddSpawnRotation);
    };

    static ConstructorHelpers::FObjectFinder<UDataTable> dataCardsObject(TEXT("DataTable'/Game/DataBase/DT_They.DT_They'"));
    if (dataCardsObject.Succeeded())
    {
        TObjectPtr<UDataTable> CardDataTable = dataCardsObject.Object;
        CardDataTable->GetAllRows("", dtRows);

    }
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

    EndLocation = GCamera_Position;
    EndRotation = GCamera_Rotation;
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
        InputComponent->BindAction("ExitToMainMenu", IE_Pressed, this, &AGodsOfEnneadPlayerController::ExitToMainMenu);
    }
}

void AGodsOfEnneadPlayerController::ExitToMainMenu()
{
    UE_LOG(LogTemp, Error, TEXT("Game over"));
    UGameplayStatics::OpenLevel(this, FName("/Game/Scene/Main_Menu"));
}

void AGodsOfEnneadPlayerController::TakeCard()
{
    FHitResult HitResult;
    if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
    {
        if (ACardActor* ClickedCard = Cast<ACardActor>(HitResult.GetActor()))
        {
            if (CurrentTurnStatus == ETurnStatus::Waiting_Choose)
            {
                if (!DeckCardsActors.Contains(ClickedCard) && !ShowDeckCardsActors.Contains(ClickedCard)) return;
               
                UE_LOG(LogTemp, Log, TEXT("Player: Card clicked: %s"), *ClickedCard->CardsData.cardName);
                PlayersHands[1]->MoveToHand(ClickedCard);
                if (DeckCardsActors.Contains(ClickedCard))
                {
                    DeckCardsActors.Remove(ClickedCard);
                    // FixDeck(DeckCardsActors);
                }
                else
                {
                    ShowDeckCardsActors.Remove(ClickedCard);
                    // FixDeck(ShowDeckCardsActors);
                }
                PlayersHands[1]->CheckTask(CurrentTaskController->Task);
                CurrentTurnStatus = ETurnStatus::Player_Turn;
                return;
            }
            
            if (CurrentTurnStatus == ETurnStatus::Player_Turn)
            {
                if (const bool bCardFound = PlayersHands[1]->CardPositions.FindByPredicate([ClickedCard](const FCardPosition& CardPosition)
                {
                    return CardPosition.CardActor == ClickedCard;
                }) != nullptr; !bCardFound) return;
               
                UE_LOG(LogTemp, Log, TEXT("Player: Card clicked: %s"), *ClickedCard->CardsData.cardName);
                FVector TargetLocation = ShowDeckCardsActors.Num() ? ShowDeckCardsActors.Last()->GetActorLocation()
                                                    : GShow_Location;
                PlayersHands[1]->MoveToDeck(ClickedCard, TargetLocation);
                ShowDeckCardsActors.Add(ClickedCard);
                const bool bCheck = PlayersHands[1]->CheckTask(CurrentTaskController->Task);
                UE_LOG(LogTemp, Error, TEXT("CheckTask: %hhd"), bCheck);
                if (bCheck and PlayersHands[0]->CheckTask(CurrentTaskController->Task))
                {
                    StartSecondTour();
                    return;
                }
                if (bCheck && DeckCardsActors.IsEmpty() )
                {
                    bIsPlayerWin = true;
                    AddResultToViewPort();
                    CurrentTurnStatus = ETurnStatus::Waiting;
                    return;
                }
                
                CurrentTurnStatus = ETurnStatus::Computer_Turn;

                FTimerHandle DelayTimerHandle;
                GetWorld()->GetTimerManager().SetTimer(
                    DelayTimerHandle,
                    this,
                    &AGodsOfEnneadPlayerController::ComputerTurn,
                    2.0f,
                    false
                );
                return;
            }
            

            if (CurrentTurnStatus == ETurnStatus::Second_Round_Player)
            {
                SelectCard(ClickedCard);
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
    DealCards(GMaxInHand, false);
    UE_LOG(LogTemp, Log, TEXT("StartGame: Deal Cards to Bot"));
    DealCards(GMaxInHand, true);

    PlayersHands[1]->CheckTask(CurrentTaskController->Task);
    PlayRound();
}

void AGodsOfEnneadPlayerController::PlayRound()
{
    const bool bCheck = PlayersHands[1]->CheckTask(CurrentTaskController->Task);
    UE_LOG(LogTemp, Error, TEXT("CheckTask: %hhd"), bCheck);
    if (bCheck and PlayersHands[0]->CheckTask(CurrentTaskController->Task))
    {
        StartSecondTour();
        return;
    }
    CurrentTurnStatus = ETurnStatus::Waiting_Choose;
}

void AGodsOfEnneadPlayerController::StartSecondTour()
{
    UE_LOG(LogTemp, Log, TEXT("ВТОРОЙ РАУНД"));
    CurrentTurnStatus = ETurnStatus::Second_Round_Start;
    
    for (int32 PlayerIndex = 0; PlayerIndex < 2; ++PlayerIndex)
    {
        UHand* PlayerHand = PlayersHands[PlayerIndex];
        if ( PlayerIndex == 0) UE_LOG(LogTemp, Log, TEXT("Компьютер"));
        if ( PlayerIndex == 1) UE_LOG(LogTemp, Log, TEXT("Игрок"));

        TArray<FDataCardStruct> CurrentCards;
        for (const FCardPosition& Position : PlayerHand->CardPositions)
        {
            if (Position.CardActor)
            {
                CurrentCards.Add(Position.CardActor->GetDataCard());
                UE_LOG(LogTemp, Log, TEXT("%s"), *Position.CardActor->CardsData.cardName);
            }
        }

        const int32 CurrentClosestCount = CurrentTaskController->Task->GetClosestCountToCompletion(CurrentCards);
        for (const FCardPosition& Position : PlayerHand->CardPositions)
        {
            if (Position.CardActor)
            {
                ACardActor* CardToTest = Position.CardActor;

                CurrentCards.Remove(CardToTest->GetDataCard());
                const int32 NewClosestCount = CurrentTaskController->Task->GetClosestCountToCompletion(CurrentCards);
                UE_LOG(LogTemp, Log, TEXT("близость к победе для карты %s: %d, без карты: %d"), *CardToTest->CardsData.cardName, CurrentClosestCount, NewClosestCount);

                CurrentCards.Add(CardToTest->GetDataCard());

                if (NewClosestCount >= CurrentClosestCount)
                {
                    UE_LOG(LogTemp, Log, TEXT("Карта удалена: %s, так как удаление не влияло на задачу"), *CardToTest->CardsData.cardName);
                    PlayerHand->RemoveCard(CardToTest);
                    CurrentCards.Remove(CardToTest->GetDataCard());
                    CardToTest->Destroy();
                }
            }
        }
        
        PlayerHand->RearrangeCards(std::min(PlayersHands[0]->CardsInHand, PlayersHands[1]->CardsInHand));
        CurrentTurnStatus = ETurnStatus::Second_Round_Player;
        CurrentTaskController->TaskWidget->bVisibility = true;
    }

    FVector DeckHideLoc = GHide_Location;
    FVector ShowDeckHideLoc = GShow_Location;
    DeckHideLoc.Z = 0;
    ShowDeckHideLoc.Z = 0;

    for (ACardActor* Card : DeckCardsActors)
    {
        Card->AnimateTo(&DeckHideLoc);
    }
    for (ACardActor* Card : ShowDeckCardsActors)
    {
        Card->AnimateTo(&ShowDeckHideLoc);
    }
    FTimerHandle DelayTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        DelayTimerHandle,
        this,
        &AGodsOfEnneadPlayerController::RemoveDeckCads,
        5.0f,
        false
    );

    for ( FCardPosition& Position : PlayersHands[0]->CardPositions)
    {
        Position.Position.X -= 200;
        if (Position.CardActor)
        {
            Position.CardActor->AnimateTo(&Position.Position);
        }
    }
    for ( FCardPosition& Position : PlayersHands[1]->CardPositions)
    {
        Position.Position.X += 100;
        if (Position.CardActor)
        {
            Position.CardActor->AnimateTo(&Position.Position);
        }
    }
}

void AGodsOfEnneadPlayerController::RemoveDeckCads()
{
    for (ACardActor* Card : DeckCardsActors)
    {
        Card->Destroy();
    }
    for (ACardActor* Card : ShowDeckCardsActors)
    {
        Card->Destroy();
    }
    DeckCardsActors.Empty();
    ShowDeckCardsActors.Empty();
}


void AGodsOfEnneadPlayerController::ComputerTurn()
{
    ACardActor* ShowDeckTopCard = ShowDeckCardsActors.Last();
    ACardActor* DeckTopCard = DeckCardsActors.Last();

    TArray<FDataCardStruct> CurrentCards;
    for (const FCardPosition& Position : PlayersHands[0]->CardPositions)
    {
        if (Position.CardActor)
        {
            CurrentCards.Add(Position.CardActor->GetDataCard());
        }
    }

    CurrentCards.Add(ShowDeckTopCard->GetDataCard());
    int32 CountWithShowDeck = CurrentTaskController->Task->GetClosestCountToCompletion(CurrentCards);
    CurrentCards.RemoveAt(CurrentCards.Num() - 1);

    CurrentCards.Add(DeckTopCard->GetDataCard());
    int32 CountWithDeck = CurrentTaskController->Task->GetClosestCountToCompletion(CurrentCards);

    if (CountWithShowDeck > CountWithDeck)
    {
        PlayersHands[0]->MoveToHand(ShowDeckTopCard);
        ShowDeckCardsActors.Remove(ShowDeckTopCard);
        UE_LOG(LogTemp, Log, TEXT("Компьютер взял карту из открытой колоды: %s"), *ShowDeckTopCard->CardsData.cardName);
        // FixDeck(ShowDeckCardsActors);
    }
    else
    {
        PlayersHands[0]->MoveToHand(DeckTopCard);
        DeckCardsActors.Remove(DeckTopCard);
        UE_LOG(LogTemp, Log, TEXT("Компьютер взял карту из закрытой колоды: %s"), *DeckTopCard->CardsData.cardName);
        // FixDeck(DeckCardsActors);
    }

    FTimerHandle DelayTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        DelayTimerHandle,
        this,
        &AGodsOfEnneadPlayerController::DiscardUnnecessaryCard,
        2.0f,
        false
    );
}

void AGodsOfEnneadPlayerController::DiscardUnnecessaryCard()
{
    TArray<FDataCardStruct> CurrentCards;
    for (const FCardPosition& Position : PlayersHands[0]->CardPositions)
    {
        if (Position.CardActor)
        {
            CurrentCards.Add(Position.CardActor->GetDataCard());
        }
    }

    const int32 CurrentClosestCount = CurrentTaskController->Task->GetClosestCountToCompletion(CurrentCards);

    ACardActor* CardToDeck = PlayersHands[0]->CardPositions[0].CardActor;
    for (const FCardPosition& Position : PlayersHands[0]->CardPositions)
    {
        if (Position.CardActor)
        {
            ACardActor* CardToTest = Position.CardActor;

            CurrentCards.Remove(CardToTest->GetDataCard());
            const int32 NewClosestCount = CurrentTaskController->Task->GetClosestCountToCompletion(CurrentCards);
            UE_LOG(LogTemp, Log, TEXT("близость к победе для карты %s: %d, без карты: %d"), *CardToTest->CardsData.cardName, CurrentClosestCount, NewClosestCount);

            CurrentCards.Add(CardToTest->GetDataCard());

            if (NewClosestCount >= CurrentClosestCount)
            {
                CardToDeck = CardToTest;
                break;
            }
        }
    }
    FVector TargetLocation = ShowDeckCardsActors.Num() > 0
                                       ? ShowDeckCardsActors.Last()->GetActorLocation() + FVector(0.0f, 0.0f, 2.0f)
                                       : GShow_Location;
    ShowDeckCardsActors.Add(CardToDeck);
    PlayersHands[0]->MoveToDeck(CardToDeck, TargetLocation);
    
    UE_LOG(LogTemp, Log, TEXT("Компьютер положил карту в открытую колоду: %s"), *CardToDeck->CardsData.cardName);
    
    if (PlayersHands[0]->CheckTask(CurrentTaskController->Task))
    {
        if (PlayersHands[1]->CheckTask(CurrentTaskController->Task))
        {
            UE_LOG(LogTemp, Log, TEXT("Оба игрока собрали набор."));
            StartSecondTour();
            return;
        }

        UE_LOG(LogTemp, Log, TEXT("Компьютер выполнил задачу."));
        if (DeckCardsActors.IsEmpty())
        {
            AddResultToViewPort();
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Компьютер завершил ход. %d"), DeckCardsActors.Num());
        if (DeckCardsActors.IsEmpty())
        {
            bIsPlayerWin = PlayersHands[1]->CheckTask(CurrentTaskController->Task);
            AddResultToViewPort();
        }
    }

    PlayRound();
}

void AGodsOfEnneadPlayerController::SelectCard(ACardActor* Card)
{
    if (SelectedCard == nullptr)
    {
        SelectedCard = Card;
        FVector NewLocation = Card->GetActorLocation();
        NewLocation.Z += 100.0f;
        Card->AnimateTo(&NewLocation);
    }
    else if (SelectedCard == Card)
    {
        FVector OriginalLocation = Card->GetActorLocation();
        OriginalLocation.Z -= 100.0f;
        Card->AnimateTo(&OriginalLocation);
        SelectedCard = nullptr;
    }
    else
    {
        SwapCards(SelectedCard, Card);
        SelectedCard = nullptr;
    }
}

void AGodsOfEnneadPlayerController::SwapCards(ACardActor* Card1, ACardActor* Card2)
{
    FVector Location1 = Card1->GetActorLocation();
    Location1.Z -= 100.0f;
    const FVector Location2 = Card2->GetActorLocation();

    Card1->AnimateTo(&Location2);
    Card2->AnimateTo(&Location1);

    int32 Index1 = PlayersHands[1]->FirstRow.Find(Card1);
    int32 Index2 = PlayersHands[1]->FirstRow.Find(Card2);
    if (Index1 != INDEX_NONE && Index2 != INDEX_NONE)
    {
        PlayersHands[1]->FirstRow.Swap(Index1, Index2);
        return;
    }
    
    if (Index1 == INDEX_NONE && Index2 != INDEX_NONE)
    {
        Index1 = PlayersHands[1]->SecondRow.Find(Card1);

        ACardActor* TempCard = PlayersHands[1]->SecondRow[Index1];
        PlayersHands[1]->SecondRow[Index1] = PlayersHands[1]->FirstRow[Index2];
        PlayersHands[1]->FirstRow[Index2] = TempCard;
        return;
    }
    
    if (Index1 != INDEX_NONE && Index2 == INDEX_NONE)
    {
        Index2 = PlayersHands[1]->SecondRow.Find(Card2);
        
        ACardActor* TempCard = PlayersHands[1]->FirstRow[Index1];
        PlayersHands[1]->FirstRow[Index1] = PlayersHands[1]->SecondRow[Index2];
        PlayersHands[1]->SecondRow[Index2] = TempCard;
        return;
    }
    
    Index1 = PlayersHands[1]->SecondRow.Find(Card1);
    Index2 = PlayersHands[1]->SecondRow.Find(Card2);
    PlayersHands[1]->SecondRow.Swap(Index1, Index2);
}

void AGodsOfEnneadPlayerController::OnReadyButtonClicked()
{
    float Delay = 0.5f;
    int TotalAnimations = 0;
    CurrentTurnStatus = ETurnStatus::Second_Round_Waiting;

    for (int i = 0; i < 2; i++)
    {
        for (ACardActor* Card : PlayersHands[i]->SecondRow)
        {
            FVector Location1 = Card->GetActorLocation();
            Location1.Z = 2000.0f;
            TotalAnimations++;
            GetWorldTimerManager().SetTimerForNextTick(
                [this, Card, Location1, Delay]()
                {
                    FTimerHandle TempHandle;
                    GetWorldTimerManager().SetTimer(
                        TempHandle,
                        FTimerDelegate::CreateLambda([Card, Location1]()
                        {
                            Card->AnimateTo(&Location1, &GShow_Rotation);
                        }),
                        Delay,
                        false
                    );
                });
            Delay += 0.5f;
        }

        for (ACardActor* Card : PlayersHands[i]->FirstRow)
        {
            FVector Location1 = Card->GetActorLocation();
            Location1.Z = 1995.0f;
            TotalAnimations++;
            GetWorldTimerManager().SetTimerForNextTick(
                [this, Card, Location1, Delay]()
                {
                    FTimerHandle TempHandle;
                    GetWorldTimerManager().SetTimer(
                        TempHandle,
                        FTimerDelegate::CreateLambda([Card, Location1]()
                        {
                            Card->AnimateTo(&Location1, &GShow_Rotation);
                        }),
                        Delay,
                        false
                    );
                });
            Delay += 0.5f;
        }
    }

    GetWorldTimerManager().SetTimerForNextTick(
    [this, TotalAnimations]()
    {
        FTimerHandle DelayHandle;
        GetWorldTimerManager().SetTimer(
            DelayHandle,
            FTimerDelegate::CreateLambda([this]()
            {
                StartAttacks();
            }),
            TotalAnimations / 2 + 2.0f,
            false
        );
    });
}

void AGodsOfEnneadPlayerController::StartAttacks()
{
    const UWorld* World = GetWorld();
    if (!World) return;

    PlayersHands[0]->AliveCards = PlayersHands[0]->FirstRow.Num();
    PlayersHands[1]->AliveCards = PlayersHands[1]->FirstRow.Num();
    
    ScheduleRoundAttack();
}

void AGodsOfEnneadPlayerController::ScheduleRoundAttack()
{
    const UWorld* World = GetWorld();
    if (!World) return;

    static int32 Delay = 0;

    ScheduleAttacks(World, PlayersHands[1], PlayersHands[0], Delay, true);

    ScheduleAttacks(World, PlayersHands[0], PlayersHands[1], Delay, false);

    FTimerHandle NextRoundHandle;
    World->GetTimerManager().SetTimer(
        NextRoundHandle,
        FTimerDelegate::CreateLambda([this]()
        {
            if (PlayersHands[0]->AliveCards > 0 && PlayersHands[1]->AliveCards > 0)
            {
                 ScheduleRoundAttack();
            }
        }),
        Delay * 1.4 + 1.7f, false
    );
    Delay = 0;
}

void AGodsOfEnneadPlayerController::ScheduleAttacks(const UWorld* World, UHand* AttackerHand, UHand* DefenderHand, int32& Delay, bool bIsPlayerAttacker)
{
    for (int32 i = 0; i < AttackerHand->FirstRow.Num(); i++)
    {
        if (AttackerHand->FirstRow[i] && DefenderHand->FirstRow[i])
        {
            if (AttackerHand->FirstRow[i]->bIsAlive && DefenderHand->FirstRow[i]->bIsAlive)
            {
                Delay++;
                FTimerHandle AttackTimerHandle;

                World->GetTimerManager().SetTimer(
                    AttackTimerHandle,
                    FTimerDelegate::CreateLambda([this, World, AttackerHand, DefenderHand, i, bIsPlayerAttacker]()
                    {
                        ProcessAttack(World, AttackerHand, DefenderHand, i, bIsPlayerAttacker);
                    }),
                    Delay * 2, false
                );
            }
        }
    }
}

void AGodsOfEnneadPlayerController::ProcessAttack(const UWorld* World, UHand* AttackerHand, UHand* DefenderHand, int32 Index, bool bIsPlayerAttacker)
{
    if (!AttackerHand->FirstRow[Index] || !DefenderHand->FirstRow[Index] || bIsGameOver) return;

    AttackerHand->FirstRow[Index]->Attack(DefenderHand->FirstRow[Index]);

    if (!DefenderHand->FirstRow[Index]->bIsAlive)
    {
        if (DefenderHand->SecondRow.IsValidIndex(Index) && DefenderHand->SecondRow[Index])
        {
            FTimerHandle SwapCardTimerHandle;

            World->GetTimerManager().SetTimer(
                SwapCardTimerHandle,
                FTimerDelegate::CreateLambda([this, DefenderHand, Index]()
                {
                    FVector NewPos = DefenderHand->FirstRow[Index]->GetActorLocation();
                    DefenderHand->SecondRow[Index]->AnimateTo(&NewPos);
                    DefenderHand->FirstRow[Index] = DefenderHand->SecondRow[Index];
                    DefenderHand->SecondRow[Index] = nullptr;
                }),
                1, false
            );
        }
        else
        {
            DefenderHand->AliveCards--;
        }
    }

    if (DefenderHand->AliveCards <= 0)
    {
        FTimerHandle ResultTimerHandle;

        World->GetTimerManager().SetTimer(
            ResultTimerHandle,
            FTimerDelegate::CreateLambda([this, bIsPlayerAttacker]()
            {
                bIsPlayerWin = bIsPlayerAttacker;
                bIsGameOver = true;
                AddResultToViewPort();
            }),
            1.2, false
        );

    }
}


void AGodsOfEnneadPlayerController::AddResultToViewPort()
{
    UE_LOG(LogTemp, Warning, TEXT("Result : %hhd"), bIsPlayerWin);
    
    const TSubclassOf<UUserWidget> widgetClass = LoadClass<UResultUserWidget>(nullptr, TEXT("/Game/BP/UI/WBP_Result.WBP_Result_C"));
    const TObjectPtr<UResultUserWidget> ResultWidget = CreateWidget<UResultUserWidget>(GetWorld(), widgetClass);
    ResultWidget->SetResultText(bIsPlayerWin);
    ResultWidget->AddToViewport();
    
    EndLocation = FVector(6356, 11476, 4062);
    EndRotation = FRotator(-17, -14, 0);
    MovePlayerToTarget();
}

//void AGodsOfEnneadPlayerController::AddStatusToViewPort()
//{
//
//    const TSubclassOf<UUserWidget> widgetClass = LoadClass<UResultUserWidget>(nullptr, TEXT("/Game/BP/UI/WBP_Status.WBP_Status_C"));
//    const TObjectPtr<UResultUserWidget> ResultWidget = CreateWidget<UResultUserWidget>(GetWorld(), widgetClass);
//    ResultWidget->SetResultText(bIsPlayerWin);
//    ResultWidget->AddToViewport();
//
//    EndLocation = FVector(6356, 11476, 4062);
//    EndRotation = FRotator(-17, -14, 0);
//    MovePlayerToTarget();
//}

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
        FTimerHandle TimerHandle;
        FTimerDelegate TimerDelegate;

        TimerDelegate.BindLambda([this, i, PlayerIndex, bIsPlayer]()
        {
            if (DeckCardsActors.Num() == 0)
            {
                return;
            }

            ACardActor* Card = DeckCardsActors.Last();
            DeckCardsActors.Remove(Card);

            FVector TargetLocation = GPlayer_Card_Location + FVector(0, i * 550.0f, i * 0.1f);
            if (!bIsPlayer)
            {
                TargetLocation = GComputer_Card_Location + FVector(0, i * 550.0f, i * 0.1f);
            }
            else
            {
                Card->SetActorRotation(GShow_Rotation);
            }

            if (Card)
            {
                PlayersHands[PlayerIndex]->AddCard(Card, TargetLocation);
                Card->AnimateTo(&TargetLocation);
            }
        });

        GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.7f * (i + 1), false);
    }
}


void AGodsOfEnneadPlayerController::SpawnActors()
{
    FVector SpawnStartLocation = GHide_Location;
    FRotator SpawnStartRotation = GHide_Rotation;

    SpawnedActorCount = 0;
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, [this, SpawnStartLocation, SpawnStartRotation]()
    {
        SpawnActorStep(SpawnStartLocation, SpawnStartRotation);
    }, 0.03f, true);
}

void AGodsOfEnneadPlayerController::DisplayTask()
{
    CurrentTaskController = NewObject<UTaskController>();
    CurrentTaskController->init(GetWorld());

    UE_LOG(LogTemp, Log, TEXT("Task created: Pattern = %hhd, Count = %d, Type = %hhd"),
        CurrentTaskController->Task->Pattern,
        CurrentTaskController->Task->Count,
        CurrentTaskController->Task->Type);
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
        if (!bIsGameOver) SpawnActors();
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
    if (SpawnedActorCount >= GCardCount)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
        UE_LOG(LogTemp, Warning, TEXT("All actors spawned. Total: %d"), GCardCount);
        DisplayTask();
        StartGame();
        return;
    }
    
    FVector SpawnNewCardLocation;
    FRotator SpawnNewCardRotation;

    if (SpawnedActorCount == GCardCount - 1)
    {
        SpawnNewCardLocation = GShow_Location;
        SpawnNewCardRotation = GShow_Rotation;
    }
    else
    {
        SpawnNewCardLocation = StartSpawnLocation + FVector(0.0f, 0.0f, SpawnedActorCount * 2.0f);
        SpawnNewCardRotation = GHide_Rotation;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;

    if (ACardActor* SpawnedActor = GetWorld()->SpawnActor<ACardActor>(ACardActor::StaticClass(), SpawnNewCardLocation, SpawnNewCardRotation, SpawnParams))
    {        
        if (SpawnedActorCount == GCardCount - 1)
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
            SpawnedActor->SetActorScale3D(GCard_Scale);
        }
        
        const int RandomIndex = FMath::RandRange(0, dtRows.Num() - 1);
        FDataCardStruct* Item = dtRows[RandomIndex];
        SpawnedActor->SetDataCard(Item->hp, Item->attack, Item->cardName);
        dtRows.RemoveAt(RandomIndex);
    
        if (UWidgetComponent* WidgetComponent = NewObject<UWidgetComponent>(SpawnedActor, UWidgetComponent::StaticClass(), TEXT("CardWidget")))
        {
            WidgetComponent->AttachToComponent(SpawnedActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            WidgetComponent->RegisterComponent();

            WidgetComponent->SetDrawSize(FVector2D(500.0f, 500.0f));
            WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
            WidgetComponent->SetTwoSided(true);
            WidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));

            const TSubclassOf<UUserWidget> WidgetClass = LoadClass<UCardUserWidget>(nullptr, TEXT("/Game/BP/UI/WBP_Card.WBP_Card_C"));
            if (WidgetClass)
            {
                WidgetComponent->SetWidgetClass(WidgetClass);
                WidgetComponent->SetVisibility(true);
                // WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

                auto* CardWidget = static_cast<UCardUserWidget*>(WidgetComponent->GetWidget());
                const FDataCardStruct& dataCard = SpawnedActor->GetDataCard();
                CardWidget->new_attack = dataCard.attack;
                CardWidget->new_hp = dataCard.hp;
                CardWidget->new_name_character = dataCard.cardName;
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to load widget class."));
            }

            if (UWidgetComponent* BackWidgetComponent = NewObject<UWidgetComponent>(SpawnedActor, UWidgetComponent::StaticClass(), TEXT("CardBackWidget")))
            {
                BackWidgetComponent->AttachToComponent(SpawnedActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
                BackWidgetComponent->RegisterComponent();

                BackWidgetComponent->SetDrawSize(FVector2D(500.0f, 500.0f));
                BackWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
                BackWidgetComponent->SetTwoSided(true);
                BackWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));


                const TSubclassOf<UUserWidget> BackWidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/BP/UI/WBP_Card_Back.WBP_Card_Back_C"));
                if (BackWidgetClass)
                {
                    BackWidgetComponent->SetRelativeLocation(FVector(-1.0f, 0.0f, 0.0f));
                    BackWidgetComponent->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
                    BackWidgetComponent->SetWidgetClass(BackWidgetClass);
                    BackWidgetComponent->SetVisibility(true);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to load back widget class."));
                }
            }
        }
    }

    ++SpawnedActorCount;
}