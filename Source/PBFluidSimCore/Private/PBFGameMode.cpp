#include "PBFGameMode.h"
#include "PBFPlayerController.h"
#include "GameFramework/Character.h"  // or your own Pawn/Character

APBFGameMode::APBFGameMode()
{
    // Set default PlayerController
    PlayerControllerClass = APBFPlayerController::StaticClass();

    // Set default Pawn (this could be ACharacter, or your own custom pawn)
    DefaultPawnClass = ACharacter::StaticClass();
}
