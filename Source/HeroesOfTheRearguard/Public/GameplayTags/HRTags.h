#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace HRTags
{
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(PlayerMelee);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Debuff);
	}
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(None);
	namespace HRAbilities
	{
		namespace Notify
    	{
    		UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageNotify);
    	}
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActivateOnGiven);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(LMBAbility);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ChargeAbility);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(JumpAttack);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(BladeFury);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(DirectionalArc);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireBlast);
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