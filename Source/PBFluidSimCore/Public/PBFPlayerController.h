#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PBFPlayerController.generated.h"

UCLASS()
class PBFLUIDSIMCORE_API APBFPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    APBFPlayerController();

protected:
    virtual void SetupInputComponent() override;

    /** Example: Move forward/backward */
    void MoveForward(float Value);

    /** Example: Move right/left */
    void MoveRight(float Value);

    /** Example: Jump */
    void JumpAction();
};
