#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace HRTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(None);
	namespace HRAbilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActivateOnGiven);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(LMBAbility);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChargeAbility);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(JumpAttack);
	}
	
	namespace Events
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(KillScored)
		
		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(EndAttack);
		}
	}
	
}