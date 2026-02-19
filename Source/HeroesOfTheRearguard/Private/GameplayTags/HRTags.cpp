#include "GamePlayTags/HRTags.h"


namespace HRTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(None, "HRTags.None", "None")
	namespace HRAbilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateOnGiven, "HRTags.HRAbilities.ActivateOnGiven", "Tag for Abilities that should activate immediately once given")
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LMBAbility, "HRTags.HRAbilities.LMBAbility", "Tag for Abilities on LMB")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ChargeAbility, "HRTags.HRAbilities.ChargeAbility", "Tag for Charge Ability")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(JumpAttack, "HRTags.HRAbilities.JumpAttack", "Tag for JumpAttack Ability")
	}
	
	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(KillScored, "HRTags.Events.KillScored", "Tag for KillScored Event")
		
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "HRTags.Events.Enemy.HitReact", "Tag for Enemy HitReact Event")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(EndAttack, "HRTags.Events.Enemy.EndAttack", "Tag for EndAttack Event")
		}
	}
	
}