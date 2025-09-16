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

void APBFPlayerController::JumpAction()
{
    if (ACharacter* ControlledChar = Cast<ACharacter>(GetPawn()))
    {
        ControlledChar->Jump();
    }
}
