#include "XivSimCheatManager.h"
#include "XivSimGameState.h"

UXivSimCheatManager::UXivSimCheatManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UXivSimCheatManager::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
#if UE_WITH_CHEAT_MANAGER
	// If on the client and calling a cheat function marked as BlueprintAuthorityOnly, automatically route it through the ServerExec() RPC to the server
	APlayerController* MyPC = GetOuterAPlayerController();
	if (MyPC->GetLocalRole() != ROLE_Authority)
	{
		const TCHAR* TestCmd = Cmd;

		FString FunctionNameStr;
		bool bCallDSFunction = false;
		if (FParse::Token(TestCmd, FunctionNameStr, true))
		{
			const TCHAR* NewCmd = TestCmd;
			if (FunctionNameStr == "ds")
			{
				bCallDSFunction = true;
				if (!FParse::Token(TestCmd, FunctionNameStr, true))
				{
					return false;
				}
			}

			const FName FunctionName = FName(*FunctionNameStr, FNAME_Find);

			if (FunctionName != NAME_None)
			{
				// Check first in this class
				UFunction* Function = FindFunction(FunctionName);

				// Failing that, check in each of the child cheat managers for a function by this name
				if (Function == nullptr)
				{
					for (UObject* CheatObject : CheatManagerExtensions)
					{
						Function = CheatObject ? CheatObject->FindFunction(FunctionName) : nullptr;
						if (Function != nullptr)
						{
							break;
						}
					}
				}

				if ((Function != nullptr) && MyPC->IsNetMode(NM_Client) && bCallDSFunction)
				{
					MyPC->ServerExec(NewCmd);
					return true;
				}
			}
		}
	}
#endif

	for (UObject* CheatObject : CheatManagerExtensions)
	{
		if ((CheatObject != nullptr) && CheatObject->ProcessConsoleExec(Cmd, Ar, Executor))
		{
			return true;
		}
	}

	return CallFunctionByNameWithArguments(Cmd, Ar, Executor);
}

void UXivSimCheatManager::AllGod()
{
	if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
	{
		if (GameState->IsIgnorePlayerDamage())
		{
			GameState->SetIgnorePlayerDamage(false);
		}
		else
		{
			GameState->SetIgnorePlayerDamage(true);
		}
	}
}

void UXivSimCheatManager::Training()
{
	if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
	{
		if (GameState->IsTrainingMode())
		{
			GameState->SetTrainingMode(false);
		}
		else
		{
			GameState->SetTrainingMode(true);
		}
	}
}
