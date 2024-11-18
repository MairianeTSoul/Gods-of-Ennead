#include "GodsOfEnneadPlayerController.h"
#include "EngineUtils.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Public/CardActor.h"
#include "Blueprint/UserWidget.h"

void AGodsOfEnneadPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // Включение событий кликов
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    // Настройка ввода для работы с мышью
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
    bShowMouseCursor = true;
    
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

    EndLocation = FVector(7456.00f, 6669.00f, 1726.00f);
    EndRotation = FRotator(-75.99f, -0.60f, 0.00f);

    MoveDuration = 3.0f;

    if (const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld()); CurrentLevelName == "Game")    
    {
        MovePlayerToTarget();
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

void AGodsOfEnneadPlayerController::SpawnActors()
{
    FVector SpawnStartLocation(8266.00f, 7537.00f, 563.00f);
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
        SetActorTickEnabled(false);

        OnAnimationFinished.Broadcast();
        SpawnActors();
    }
}

void AGodsOfEnneadPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateMovement(DeltaTime);
}

void AGodsOfEnneadPlayerController::SpawnActorStep(const FVector& StartSpawnLocation, const FRotator& StartSpawnRotation)
{
    constexpr int32 MaxCards = 108;

    if (SpawnedActorCount >= MaxCards)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
        UE_LOG(LogTemp, Warning, TEXT("All actors spawned. Total: %d"), SpawnedActors.Num());
        return;
    }

    FVector FinalCardLocation(7569.0f, 7538.0f, 563.0f);
    FRotator FinalCardRotation(90.0f, 19.471221f, -160.528779f);
    FVector FinalCardScale(1.0f, 0.75f, 1.0f);

    FVector SpawnNewCardLocation;
    FRotator SpawnNewCardRotation;

    if (SpawnedActorCount == MaxCards - 1)
    {
        SpawnNewCardLocation = FinalCardLocation;
        SpawnNewCardRotation = FinalCardRotation;
    }
    else
    {
        SpawnNewCardLocation = StartSpawnLocation + FVector(0.0f, 0.0f, SpawnedActorCount * 1.0f);
        SpawnNewCardRotation = FinalCardRotation; // + FRotator(0.0f, 180.0f, 0.0f);
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;

    if (ACardActor* SpawnedActor = GetWorld()->SpawnActor<ACardActor>(ACardActor::StaticClass(), SpawnNewCardLocation, SpawnNewCardRotation, SpawnParams))
    {
        SpawnedActors.Add(SpawnedActor);

        if (!SpawnedActor->GetRootComponent())
        {
            USceneComponent* DefaultRoot = NewObject<USceneComponent>(SpawnedActor, USceneComponent::StaticClass(), TEXT("DefaultSceneRoot"));
            SpawnedActor->SetRootComponent(DefaultRoot);
            DefaultRoot->RegisterComponent();

            SpawnedActor->SetActorLocationAndRotation(SpawnNewCardLocation, SpawnNewCardRotation);
            SpawnedActor->SetActorScale3D(FinalCardScale);
        }

        if (UWidgetComponent* WidgetComponent = NewObject<UWidgetComponent>(SpawnedActor, UWidgetComponent::StaticClass(), TEXT("CardWidget")))
        {
            WidgetComponent->AttachToComponent(SpawnedActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            WidgetComponent->RegisterComponent();

            WidgetComponent->SetDrawSize(FVector2D(500.0f, 500.0f));
            WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
            WidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));

            const TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/BP/UI/WBP__Card.WBP__Card_C"));
            if (WidgetClass)
            {
                WidgetComponent->SetWidgetClass(WidgetClass);
                WidgetComponent->SetVisibility(true);
                WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

                UE_LOG(LogTemp, Warning, TEXT("WidgetComponent added to actor successfully."));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to load widget class."));
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Actor %d spawned at location: %s"), SpawnedActorCount, *SpawnNewCardLocation.ToString());
    }

    ++SpawnedActorCount;
}

void AGodsOfEnneadPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (InputComponent)
    {
        InputComponent->BindAction("SelectCard", IE_Pressed, this, &AGodsOfEnneadPlayerController::HandleClick);
    }
}

void AGodsOfEnneadPlayerController::HandleClick()
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

    if (AActor* HitActor = HitResult.GetActor())
    {
        if (ACardActor* ClickedCard = Cast<ACardActor>(HitActor))
        {
            SelectCard(ClickedCard);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Clicked actor is not a card."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No actor under cursor."));
    }
}

void AGodsOfEnneadPlayerController::SelectCard(ACardActor* CardActor)
{
    if (CardActor)
    {
        SelectedCardActor = CardActor;
        UE_LOG(LogTemp, Log, TEXT("Card selected: %s"), *CardActor->GetName());

        SelectedCardActor->MoveToHand();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No card actor found to select."));
    }
}