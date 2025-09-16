#include "PBFPlayerController.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

APBFPlayerController::APBFPlayerController()
{
    bShowMouseCursor = true;   // optional, useful if you want cursor visible
}

void APBFPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    check(InputComponent);

    // Bind axes (values from -1 to 1)
    InputComponent->BindAxis("MoveForward", this, &APBFPlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &APBFPlayerController::MoveRight);

    // Focus/unfocus
    InputComponent->BindAction("FocusViewport", IE_Pressed, this, &APBFPlayerController::FocusViewport);
    InputComponent->BindAction("UnfocusViewport", IE_Pressed, this, &APBFPlayerController::UnfocusViewport);

    // Bind actions (pressed/released)
    InputComponent->BindAction("Jump", IE_Pressed, this, &APBFPlayerController::JumpAction);
}

void APBFPlayerController::MoveForward(float Value)
{
    if (APawn* ControlledPawn = GetPawn())
    {
        ControlledPawn->AddMovementInput(ControlledPawn->GetActorForwardVector(), Value);
    }
}

void APBFPlayerController::MoveRight(float Value)
{
    if (APawn* ControlledPawn = GetPawn())
    {
        ControlledPawn->AddMovementInput(ControlledPawn->GetActorRightVector(), Value);
    }
}

void APBFPlayerController::FocusViewport()
{
    bShowMouseCursor = false;
    SetInputMode(FInputModeGameOnly());
    bIsFocused = true;

    UE_LOG(LogTemp, Log, TEXT("Viewport focused"));
}

void APBFPlayerController::UnfocusViewport()
{
    bShowMouseCursor = true;
    SetInputMode(FInputModeGameAndUI());
    bIsFocused = false;

    UE_LOG(LogTemp, Log, TEXT("Viewport unfocused"));
}

void APBFPlayerController::JumpAction()
{
    if (ACharacter* ControlledChar = Cast<ACharacter>(GetPawn()))
    {
        ControlledChar->Jump();
    }
}
