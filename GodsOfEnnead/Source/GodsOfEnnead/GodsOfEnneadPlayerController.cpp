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

    for (int i = 0; i < )
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

    EndLocation = FVector(7107.0f, 6736.00f, 1927.00f);
    EndRotation = FRotator(-65.00f, 0.00f, 0.00f);
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
    UE_LOG(LogTemp, Log, TEXT("AGodsOfEnneadPlayerController::TakeCard"));

   FHitResult HitResult;
    if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
    {
        if (ACardActor* ClickedCard = Cast<ACardActor>(HitResult.GetActor()))
        {
            UE_LOG(LogTemp, Log, TEXT("AGodsOfEnneadPlayerController Card clicked: %s"), *ClickedCard->GetName());
            if (AGadsOfEnneadCharacter* PlayerCharacter = Cast<AGadsOfEnneadCharacter>(GetPawn()))
            {
                PlayerCharacter->TakeCard(ClickedCard);
                if (ClickedCard->GetActorLocation().X < 7500.0f)
                {
                    ClickedCard->MoveToHand(--CardsInHand);
                }
                else
                {
                    ClickedCard->MoveToHand(CardsInHand++);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Actor under cursor is not a card."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No actor under cursor."));
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
    FString Arr[] = { TEXT("of"), TEXT("Tomorrow") };

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
        UDataTable* DT;
        FSoftObjectPath UnitDataTablePath = FSoftObjectPath(TEXT("/Game/DataBase/DT_Characters.DT_Characters"));
        DT = Cast<UDataTable>(UnitDataTablePath.ResolveObject());
        //TODO need c++ class for struct!!!
        //FCardCharacterStruct* Item = DT->FindRow<FCardCharacterStruct>("New_Row_0", "");

    
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
                cardWidget->new_attack = DataVal[SpawnedActorCount % g_cardTypes].a;
                cardWidget->new_hp = DataVal[SpawnedActorCount % g_cardTypes].b;
                cardWidget->new_name_character = DataVal[SpawnedActorCount % g_cardTypes].str;

                //TODO Set image here
                //cardWidget->new_icon_character.SetResourceObject()


                /*WidgetComponent->AddChildToVerticalBox(NewInventoryItemWidget);*/
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