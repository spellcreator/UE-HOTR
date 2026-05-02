# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**HeroesOfTheRearguard** is an Unreal Engine 5.7 action RPG built in C++. It uses the Gameplay Ability System (GAS), Enhanced Input, and UMG. The single runtime module is `HeroesOfTheRearguard`.

## Building

This is a standard UE5 project. Build via:
- **Unreal Editor**: Open `HeroesOfTheRearguard.uproject`, then compile from the editor toolbar.
- **CLI (UnrealBuildTool)**: `UnrealBuildTool HeroesOfTheRearguard Win64 Development -Project="<path>/HeroesOfTheRearguard.uproject"`
- **Visual Studio**: Open `HeroesOfTheRearguard.sln`, select `Development Editor` + `Win64`, build.

There are no automated tests or lint commands in this project.

## Module Dependencies

Declared in `Source/HeroesOfTheRearguard/HeroesOfTheRearguard.Build.cs`:
- **Public**: Core, CoreUObject, Engine, InputCore, EnhancedInput, GameplayAbilities, GameplayTasks, GameplayTags, UMG, SlateCore
- **Private**: AIModule

## Architecture

### Naming Conventions
- All project C++ classes use the `HR_` prefix (e.g., `UHR_GameplayAbility`).
- Editor categories on `UPROPERTY`/`UFUNCTION` use `Crash|` as the root (e.g., `"Crash|PlayerCharacter|Camera"`).

### Character Hierarchy
- `AHR_BaseCharacter` (`Public/Characters/HR_BaseCharacter.h`) — abstract base implementing `IAbilitySystemInterface` and `IHR_Targetable`. Handles startup ability granting, attribute initialization, death, and the selection decal used by targeting.
- `AHR_PlayerCharacter` — extends `AHR_BaseCharacter`; owns `UHR_CameraInputComponent`, `UHR_AbilityTargetingComponent`, `UHR_UnitTargetingComponent`, and `UHR_InventoryComponent`. Delegates `GetAbilitySystemComponent()` / `GetAttributeSet()` to `AHR_PlayerState`.

### Gameplay Ability System (GAS)
- **ASC**: `UHR_AbilitySystemComponent` (`Public/AbilitySystem/HR_AbilitySystemComponent.h`) — lives on `AHR_PlayerState` for players. Adds tag-based ability lookup (`FindAbilityByTag`, `TryActivateAbilityByTag`) and auto-activation of abilities tagged with `HRTags::HRAbilities::ActivateOnGiven`.
- **AttributeSet**: `UHR_AttributeSet` — Health, MaxHealth, Mana, MaxMana; all replicated with OnRep handlers.
- **Base ability**: `UHR_GameplayAbility` (`Public/AbilitySystem/Core/HR_GameplayAbility.h`) — infrastructure-only base. Provides helper methods: `PlayMontage`, `WaitGameplayEvent`, `ListenForDamageNotify`, `ApplyEffectToSelf/Target/Targets`, `EndAbilitySafe`, `CancelAllTasks`. Contains `FAbilityTargetingData` with targeting config and `TryStartFromInput()` entry point.

### Ability Targeting Types
Three concrete base classes derive from `UHR_GameplayAbility`:

| Class | Type | Behavior |
|---|---|---|
| `UHR_InstantAbility` | `Instant` | Activates immediately on input |
| `UHR_GroundTargetAbility` | `GroundTarget` / `DirectionalArc` | Starts ground decal phase; activates via `GameplayEvent` when player confirms location |
| `UHR_UnitTargetAbility` | `UnitTarget` | Requires a selected unit; activates via `GameplayEvent` on confirm |

The `EHR_AbilityTargetingType` enum (`Public/AbilitySystem/Core/Enums/HR_AbilityTargetingType.h`) drives both the targeting component and the `FAbilityTargetingData` config struct.

### Combat Module
`UHR_CombatModule` (`Public/AbilitySystem/Core/HR_CombatModule.h`) is an `EditInlineNew` `UObject` embedded in abilities via `Instanced` UPROPERTY. It encapsulates:
- `FHR_DamageProfile` — hitbox shape (Sphere/Cone/Box), offsets, base damage, SetByCaller tag.
- `ApplyDamage` / `ApplyAOE` / `FindTargetsInHitbox` — hit detection and GE application.
- `ApplyDebuffToTarget` — applies the `Debuffs` array of GEs.

### Input → Ability Flow
1. `AHR_PlayerController` binds Enhanced Input actions and calls `TryActivateOrBeginTargeting(FGameplayTag)`.
2. That method calls `UHR_GameplayAbility::TryStartFromInput()` on the ability found by tag.
3. Ability's override decides: activate immediately (`Instant`) or delegate to `UHR_AbilityTargetingComponent` / `UHR_UnitTargetingComponent` to begin a targeting phase.
4. On confirm, the targeting component fires a `GameplayEvent`, which triggers the ability's activation via its registered trigger tag.

### Targeting Components (on `AHR_PlayerCharacter`)
- `UHR_AbilityTargetingComponent` — manages ground/directional decal display, exposes `BeginTargeting` / `ConfirmTargeting` / `CancelTargeting`. Fires `OnTargetingConfirmed(FVector)` / `OnTargetingCancelled`.
- `UHR_UnitTargetingComponent` — click-to-target and Tab-cycle unit selection. Tracks `target-of-target`. Fires `OnTargetChanged` / `OnTargetOfTargetChanged`. Units must implement `IHR_Targetable`.

### Gameplay Tags
All native tags declared in `Public/GameplayTags/HRTags.h` / `Private/GameplayTags/HRTags.cpp`:
- `HRTags::HRAbilities::*` — per-ability tags (LMBAbility, ChargeAbility, JumpAttack, BladeFury, DirectionalArc, FireBlast)
- `HRTags::HRAbilities::Notify::DamageNotify` — sent by animation notify to trigger damage in ability
- `HRTags::SetByCaller::PlayerMelee` / `Debuff` — SetByCaller magnitude tags for GEs
- `HRTags::Events::KillScored`, `Enemy::HitReact`, `Enemy::EndAttack`

### Inventory
`UHR_InventoryComponent` (`Public/Characters/HR_InventoryComponent.h`) is a slot-based array (`TArray<FInventorySlot>`, max 20 by default). UI is `UInventoryWidget` (UMG).

## Adding a New Ability

1. Decide targeting type → inherit from `UHR_InstantAbility`, `UHR_GroundTargetAbility`, or `UHR_UnitTargetAbility`.
2. Add a new tag in `HRTags.h/.cpp`.
3. Override `ActivateAbility`: use `PlayMontage`, bind `ListenForDamageNotify`, apply damage via an embedded `UHR_CombatModule` instance.
4. Wire an `UInputAction` in `AHR_PlayerController` and call `TryActivateOrBeginTargeting(NewTag)`.
5. Grant the ability in the character's `StartupAbilities` array (set in Blueprint defaults).