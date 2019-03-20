#include "..\\defines.h"
#include "AI_Helper_Functions.h"
#include "..\\Helper_Functions.h"



u8 AI_Script_Positives(u8 bankAtk, u8 bankDef, u16 move, u8 viability) {
	u8 i;
	
	// get relevant params
	u8 moveEffect = gBattleMoves[move].effect;
	u8 bankAtkPartner = PARTNER(bankAtk);
	u8 bankDefPartner = PARTNER(bankDef);
	
	u16 atkItem = ITEM(bankAtk);
	u16 defItem = ITEM(bankDef);
	u8 atkItemEffect = ITEM_EFFECT(bankAtk);
	u8 defItemEffect = ITEM_EFFECT(bankDef);
	u8 atkItemQuality = ITEM_QUALITY(bankAtk);
	
	u8 atkAbility = ABILITY(bankAtk);
	u8 defAbility = ABILITY(bankDef);
	
	u32 atkStatus1 = gBattleMons[bankAtk].status1;
	u32 atkStatus2 = gBattleMons[bankAtk].status2;
	u32 atkStatus3 = gBattleMons[bankAtk].status3;
	u32 defStatus1 = gBattleMons[bankDef].status1;
	u32 defStatus2 = gBattleMons[bankDef].status2;
	u32 defStatus3 = gBattleMons[bankDef].status3;
	
	
	
	switch (moveEffect) {
		case EFFECT_HIT:
			// to do
			break;
		
		case EFFECT_SLEEP:
		case EFFECT_YAWN:
			if (defItemEffect == ITEM_EFFECT_CURE_SLP || defItemEffect == ITEM_EFFECT_CURE_STATUS)
				break;
			else if (defAbility == ABILITY_EARLYBIRD || defAbility == ABILITY_SHEDSKIN)
				break;
			else
				viability += 6;
			break;
			
		case EFFECT_ABSORB:
		AI_DRAIN_HP_CHECK: ;
			if (atkItemEffect == ITEM_EFFECT_BIGROOT || GetHealthPercentage(bankAtk) <= 50)
				viability += 5;
			break;
			
		case EFFECT_EXPLOSION:
		case EFFECT_MEMENTO:
			if (atkItemEffect == ITEM_EFFECT_GEM)
			{
				if (atkItemQuality == 0)
					viability += 6;
				break;
			}
			else if (atkItemEffect == ITEM_EFFECT_CUSTAP_BERRY)
			{
				if (GetHealthPercentage(bankAtk) <= 25)
					viability += 6;
				break;
			}
			break;
			
		case EFFECT_DREAM_EATER:
			if (defAbility == ABILITY_COMATOSE)
				viability += 5;
			else if (defStatus1 & STATUS1_SLEEP)
				viability += 5;
			goto AI_DRAIN_HP_CHECK;
			
		// increased stat effects
		case EFFECT_ATTACK_UP:
		case EFFECT_ATTACK_UP_2:
		AI_ATTACK_PLUS: ;
			if (move == MOVE_HONECLAWS)
			{
				if (STAT_STAGE(bankAtk,STAT_STAGE_ATK) > 7)
					goto AI_ACCURACY_PLUS;
				else if (PhysicalMoveInMoveset(bankAtk))
					viability += 6;
				goto AI_ACCURACY_PLUS;
			}
			else
			{
				if (STAT_STAGE(bankAtk,STAT_STAGE_ATK) > 7)
					break;
				else if (PhysicalMoveInMoveset(bankAtk))
					viability += 6;
				break;
			}
			break;
			
		case EFFECT_DEFENSE_UP:
		case EFFECT_DEFENSE_UP_2:
		AI_DEFENSE_PLUS: ;
			switch (move)
			{
				case MOVE_FLOWERSHIELD:
					if (IsOfType(bankAtkPartner, TYPE_GRASS))
					{
						viability += 5;
						break;
					}
					break;
					
				case MOVE_MAGNETICFLUX:
					if (ABILITY(bankAtkPartner) == ABILITY_PLUS || ABILITY(bankAtkPartner) == ABILITY_MINUS)
					{
						viability += 5;
						break;
					}
					else if (STAT_STAGE(bankDef,STAT_STAGE_ATK) > STAT_STAGE(bankDef,STAT_STAGE_SPATK))
						break;
					else
						goto AI_SPDEF_PLUS;
					
				case MOVE_AROMATICMIST:
					viability += 5;
					break;
			}
			// continue defense check
			if (STAT_STAGE(bankAtk,STAT_STAGE_DEF) > 7)
				break;
			else if (STAT_STAGE(bankDef,STAT_STAGE_ATK) > STAT_STAGE(bankDef,STAT_STAGE_SPATK) && defAbility != ABILITY_CONTRARY)
				viability += 5;
			break;	
			
		case EFFECT_SPEED_UP:
		case EFFECT_SPEED_UP_2:
		AI_SPEED_PLUS: ;
			if (MoveWouldHitFirst(move, bankAtk, bankDef))
				break;
			else if (defAbility != ABILITY_CONTRARY)
				viability += 6;
			break;
			
		case EFFECT_SPECIAL_ATTACK_UP:
		case EFFECT_SPECIAL_ATTACK_UP_2:
		AI_SPATK_PLUS: ;
			switch (move)
			{
				case MOVE_WORKUP:
					if (PhysicalMoveInMoveset(bankAtk))
						goto AI_ATTACK_PLUS;
					else if (SpecialMoveInMoveset(bankAtk))
						break;
					goto FUNCTION_RETURN;
					
				case MOVE_ROTOTILLER:
					if (IsOfType(bankAtkPartner,TYPE_GRASS))
						viability += 5;
					else if (PhysicalMoveInMoveset(bankAtk))
						goto AI_ATTACK_PLUS;
					else if (SpecialMoveInMoveset(bankAtk))
						break;
					goto FUNCTION_RETURN;
					
				case MOVE_GEARUP:
					if (ABILITY(bankAtkPartner) == ABILITY_PLUS || ABILITY(bankAtkPartner) == ABILITY_MINUS)
					{
						viability += 5;
						goto FUNCTION_RETURN;
					}
					else if (PhysicalMoveInMoveset(bankAtk))
						goto AI_ATTACK_PLUS;
					else if (SpecialMoveInMoveset(bankAtk))
						break;
					goto FUNCTION_RETURN;
			}
			// continue spatk plus check
			if (STAT_STAGE(bankAtk,STAT_STAGE_SPATK) > 7)
				break;
			else if (SpecialMoveInMoveset(bankAtk) && defAbility != ABILITY_CONTRARY)
				viability += 6;
			break;
			
		case EFFECT_SPECIAL_DEFENSE_UP:
		case EFFECT_SPECIAL_DEFENSE_UP_2:
		AI_SPDEF_PLUS: ;
			if (STAT_STAGE(bankAtk,STAT_STAGE_SPDEF) > 7)
				break;
			else if (STAT_STAGE(bankDef,STAT_STAGE_SPATK) > STAT_STAGE(bankDef,STAT_STAGE_ATK) && defAbility != ABILITY_CONTRARY)
				viability += 5;
			break;
			
		case EFFECT_ACCURACY_UP:
		case EFFECT_ACCURACY_UP_2:
		case EFFECT_MINIMIZE:
		AI_ACCURACY_PLUS: ;
			if (STAT_STAGE(bankAtk,STAT_STAGE_ACC) < 6 && defAbility != ABILITY_CONTRARY)
				viability += 5;
			break;
			
		case EFFECT_EVASION_UP:
		case EFFECT_EVASION_UP_2:
			if (defAbility != ABILITY_CONTRARY)
				viability += 6;
			break;
			
		// decreased stat effects 
		case EFFECT_ATTACK_DOWN:
		case EFFECT_ATTACK_DOWN_2:
			if (STAT_STAGE(bankDef,STAT_STAGE_ATK) < 5)
				break;
			else if (STAT_STAGE(bankDef,STAT_STAGE_ATK) > STAT_STAGE(bankDef,STAT_STAGE_SPATK) && defAbility != ABILITY_CONTRARY)
				viability += 3;
			break;
			
		case EFFECT_DEFENSE_DOWN:
		case EFFECT_DEFENSE_DOWN_2:
			if (STAT_STAGE(bankDef,STAT_STAGE_DEF) < 5)
				break;
			else if (PhysicalMoveInMoveset(bankAtk) && defAbility != ABILITY_CONTRARY)
				viability += 3;
			break;
			
		case EFFECT_SPEED_DOWN:
		case EFFECT_SPEED_DOWN_2:
			if (MoveWouldHitFirst(move, bankAtk, bankDef))
				break;
			else if (defAbility != ABILITY_CONTRARY)
				viability += 5;
			break;
			
		case EFFECT_SPECIAL_ATTACK_DOWN:
		case EFFECT_SPECIAL_ATTACK_DOWN_2:
			if (STAT_STAGE(bankDef, STAT_STAGE_SPATK) < 5)
				break;
			else if (STAT_STAGE(bankDef, STAT_STAGE_SPATK) > STAT_STAGE(bankDef, STAT_STAGE_ATK) && defAbility != ABILITY_CONTRARY)
				viability += 3;
			break;
			
		case EFFECT_SPECIAL_DEFENSE_DOWN:
		case EFFECT_SPECIAL_DEFENSE_DOWN_2:
			if (STAT_STAGE(bankDef, STAT_STAGE_SPDEF) < 5)
				break;
			else if (SpecialMoveInMoveset(bankAtk) && defAbility != ABILITY_CONTRARY)
				viability += 3;
			break;
			
		case EFFECT_ACCURACY_DOWN:
		case EFFECT_ACCURACY_DOWN_2:
			if (defAbility != ABILITY_CONTRARY)
				viability += 3;
			break;
			
		case EFFECT_EVASION_DOWN:
		case EFFECT_EVASION_DOWN_2:
			if (STAT_STAGE(bankDef,STAT_STAGE_EVASION) < 5)
				break;
			else if (defAbility != ABILITY_CONTRARY)
				viability += 3;
			break;
			
		case EFFECT_ROAR:
		case EFFECT_HAZE:
			// if any stat buffs are below 6, increase viability
			for (i = 0; i <= BATTLE_STATS_NO-1; ++i)
			{
				if (STAT_STAGE(bankAtk, i) < 6)
				{	
					viability += 5;
					break;
				}
			}
			break;
			
		case EFFECT_BIDE:
			if (DamagingMoveInMoveset(bankAtk))
				break;
			else 
				viability += 3;
			break;
			
		case EFFECT_RAMPAGE:
			if (atkItemEffect == ITEM_EFFECT_CURE_CONFUSION || atkItemEffect == ITEM_EFFECT_CURE_STATUS)
				viability += 5;
			else if (atkAbility == ABILITY_OWNTEMPO)
				viability += 5;
			break;
			
		case EFFECT_MULTI_HIT:
			if (atkAbility == ABILITY_SKILLLINK)
				viability += 5;
			else if (defStatus2 & STATUS2_SUBSTITUTE)
				viability += 5;
			else if (atkItemEffect == ITEM_EFFECT_KINGS_ROCK)
				viability += 5;
			break;
			
		case EFFECT_FLINCH_HIT:
			if (defAbility == ABILITY_INNERFOCUS)
				break;
			else if (defStatus2 & STATUS2_SUBSTITUTE)
				break;
			else if (MoveWouldHitFirst(move, bankDef, bankAtk)
				break;
			else if (atkAbility == ABILITY_SERENEGRACE)
				viability += 5;
			else if (defStatus1 & STATUS1_PARALYSIS)
				viability += 5;
			else if (defStatus2 & (STATUS2_INFATUATION | STATUS2_CONFUSION))
				viability += 5;
			break;
			
		case EFFECT_RESTORE_HP:
		case EFFECT_MORNING_SUN:
		case EFFECT_SYNTHESIS:
		case EFFECT_MOONLIGHT:
		case EFFECT_SOFTBOILED:
		case EFFECT_SWALLOW:
		case EFFECT_WISH:
			if (GetHealthPercentage(bankAtk) <= 50)
				viability += 5;
			break;
			
		case EFFECT_TOXIC:
		case EFFECT_POISON:
			if (defItemEffect == ITEM_EFFECT_CURE_PSN || defItemEffect == ITEM_EFFECT_CURE_STATUS)
				break;
			else if (defAbility == ABILITY_SHEDSKIN || defAbility == ABILITY_TOXICBOOST || defAbility == ABILITY_POISONHEAL )
				break;
			else if (atkAbility == ABILITY_POISONTOUCH)
				break;
			else if ((FindMovePositionInMoveset(MOVE_VENOSHOCK,bankAtk) != 4)
			|| (FindMovePositionInMoveset(MOVE_HEX,bankAtk) != 4)
			|| (FindMovePositionInMoveset(MOVE_VENOMDRENCH,bankAtk) != 4))
				viability += 6;
			else if (atkAbility == ABILITY_MERCILESS)
				viability += 6;
			else if (CanBePoisoned(bankDef, bankAtk))
				viability += 5;	//AI enjoys poisoning
			break;
			
		case EFFECT_LIGHT_SCREEN:
			if (SpecialMoveInMoveset(bankDef))
				viability += 5;
			break;
			
		case EFFECT_REST:
			if (GetHealthPercentage(bankAtk) > 50)
				break;
			else if (atkAbility == ABILITY_LEAFGUARD && (gBattleWeather & WEATHER_SUN_ANY))
				break;
			else if (CheckGrounding(bankAtk) == GROUNDED && (gBattleTerrain == ELECTRIC_TERRAIN || gBattleTerrain == MISTY_TERRAIN))
				break;
			else if (IsUproarBeingMade)
				break;
			else if (!(CanBePutToSleep(bankAtk)))
				break;
			else if (atkItemEffect == ITEM_EFFECT_CURE_SLP || atkItemEffect == ITEM_EFFECT_CURE_STATUS)
				viability += 5;
			else if ((FindMovePositionInMoveset(MOVE_SLEEPTALK, bankAtk) != 4) || (FindMovePositionInMoveset(MOVE_SNORE, bankAtk) != 4))
				viability += 5;
			else if (atkAbility == ABILITY_SHEDSKIN)
				viability += 5;
			else if ((gBatttleWeather & WEATHER_RAIN_ANY) && atkAbility == ABILITY_HYDRATION)
				viability += 4;
			else if (atkAbility == ABILITY_EARLYBIRD)
				viability += 3;
			break;
			
		case EFFECT_0HKO:
		#ifdef OKAY_WITH_AI_SUICIDE
			if (atkAbility == ABILITY_NOGUARD || defAbility == ABILITY_NOGUARD)
				viability += 5;
			else if (atkStatus3 & (STATUS3_ALWAYS_HITS))
				viability += 7;
			break;
		#else
			break;
		
		case EFFECT_SUPER_FANG:
			if (GetHealthPercentage(bankAtk) > 70)
				viability += 5;
			break;
			
		case EFFECT_TRAP:
			if (FindMovePositionInMoveset(MOVE_RAPIDSPIN, bankDef) != 4)
				break;
			else if (atkItemEffect == EFFECT_BINDINGBAND || atkItemEffect == EFFECT_GRIPCLAW)
				viability += 5;
			else
				goto AI_TRAP_TARGET_CHECK;	// check conditions to trap target
			break;
			
		case EFFECT_HIGH_CRITICAL:
			if (defAbility == ABILITY_SHELLARMOR || defAbility == ABILITY_BATTLEARMOR)
				break;
			else if (atkAbility == ABILITY_BATTLEARMOR || atkAbility == ABILITY_SNIPER)
				viability += 5;
			else if (atkItemEffect == ITEM_EFFECT_SCOPE_LENS)
				viability += 5;
			else if (atkSpecies == PKMN_CHANSEY && atkItemEffect == ITEM_EFFECT_LUCKY_PUNCH)
				viability += 5;
			else if (atkSpecies == PKMN_FARFETCHD && atkItemEffect == ITEM_EFFECT_STICK)
				viability += 5;
			else if (atkStatus2 & STATUS2_FOCUS_ENERGY)
				break;
			else if (move == MOVE_RAZORWIND)
				goto AI_CHARGEUP_CHECK;
			break;
			
		case EFFECT_DOUBLE_HIT:
			if (defStatus2 & STATUS2_SUBSTITUTE)
				viability += 5;
			break;
			
		case EFFECT_FOCUS_ENERGY:
			if (defAbility == ABILITY_SHELLARMOR || defAbility == ABILITY_BATTLEARMOR)
				break;
			else if (atkAbility == ABILITY_SUPERLUCK || atkAbility == ABILITY_SNIPER)
				viability += 6;
			else if (atkItemEffect == ITEM_EFFECT_SCOPE_LENS)
				viability += 6;
			break;
			
		case EFFECT_RECOIL:
			if (atkAbility == ABILITY_RECKLESS)
				viability += 5;
			else if (atkAbility == ABILITY_ROCKHEAD)
				viability += 4;
			break;
			
		case EFFECT_TEETER_DANCE:
			if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
				goto AI_CONFUSE_CHECK;
			if (atkAbility == ABILITY_SERENEGRACE)
				viability += 6;
			else if ((CanBeConfused(bankDef)) || (CanBeConfused(bankDefPartner)))
				viability += 6;
			break;	
			
		case EFFECT_CONFUSE:
		case EFFECT_FLATTER:
		AI_CONFUSE_CHECK: ;
			if (atkAbility == ABILITY_SERENEGRACE)
				viability += 6;
			else if (defStatus1 & STATUS1_PARALYSIS)
				viability += 6;
			else if (defStatus2 & (STATUS2_INFATUATION))
				viability += 6;
			else if (CanBeConfused(bankDef))
				viability += 5;
			break;
		
		case EFFECT_REFLECT:
			if (move == MOVE_AURORAVEIL)
				viability += 5;
			else if (PhysicalMoveInMoveset(bankDef))
				viability += 5;
			break;
			
		case EFFECT_PARALYZE:
			if (defItemEffect == ITEM_EFFECT_CURE_PAR || defItemEffect == ITEM_EFFECT_CURE_STATUS)
				break;
			else if (defAbility == ABILITY_SHEDSKIN)
				break;
			else if (FindMovePositionInMoveset(MOVE_HEX, bankAtk) != 4)
				viability += 6;
			else if ((MoveEffectInMoveset(EFFECT_FLINCH_HIT,bankAtk)) || (MoveEffectInMoveset(EFFECT_FLINCH_HIT_2,bankAtk)))
				viability += 6;
			else if (defStatus2 & (STATUS2_CONFUSION | STATUS2_INFATUATION))
				viability += 6;
			else if (CanBeParalyzed(bankDef))
				viability += 5;
			break;
			
		case EFFECT_SKY_ATTACK:
		case EFFECT_SKULL_BASH:
		case EFFECT_SEMI_INVULNERABLE:
		AI_CHARGEUP_CHECK: ;
			if (atkItemEffect == ITEM_EFFECT_POWER_HERB)
				viability += 5;
			break;
			
		case EFFECT_SUBSTITUTE:
			if (GetHealthPercentage(bankAtk) > 50)
				viability += 6;
			break;
			
		case EFFECT_RECHARGE:
			if (atkAbility == ABILITY_TRUANT)
				viability += 7;
			break;
			
		case EFFECT_LEECH_SEED:
			if (IsOfType(bankDef, TYPE_GRASS))
				break;
			else if (gStatuses3[bankDef] & STATUS3_LEECHSEED)
				break;
			else if (gStatuses2[bankDef] & STATUS2_SUBSTITUTE)
				break;
			else if (FindMovePositionInMoveset(MOVE_RAPIDSPIN, bankDef) != 4)
				break;
			else if (atkAbility == ABILITY_LIQUIDOOZE)
				break;
			else
				viability += 6;
			break;
			
		case EFFECT_ENCORE:
			if (MoveWouldHitFirst(move, bankDef, bankAtk))
				break;
			else if (gBattleMoves[gLastUsedMoves[bankDef]].power == 0)
				viability += 5;
			break;
			
		case EFFECT_SNORE:
		case EFFECT_SLEEP_TALK:
			if ((atkStatus1 & STATUS1_SLEEP))
				viability += 7;
			break;
			
		case EFFECT_LOCK_ON:
			if (MoveEffectInMoveset(EFFECT_0HKO, bankAtk))
				viability += 6;
			break;
		
		case EFFECT_DESTINY_BOND:
		case EFFECT_FLAIL:
			if (MoveWouldHitFirst(move, bankDef, bankAtk))
				break;
			else if (GetHealthPercentage(bankAtk) > 25)
				break;
			else
				viability += 7;
			break;
			
		case EFFECT_HEAL_BELL:
			if ((atkStatus1 & STATUS1_ANY) || (gBattleMons[bankAtkPartner].status1 & STATUS1_ANY))
				viability += 5;
			break;
			
		case EFFECT_QUICK_ATTACK:
			if (GetHealthPercentage(bankAtk) <= 20)
				viability += 7;
			else if (GetHealthPercentage(bankDef) <= 15)
				viability += 7;
			break;
			
		case EFFECT_MEAN_LOOK:
		AI_TRAP_TARGET_CHECK: ;	
			if (defStatus1 & STATUS1_ANY)	//better to trap statused target
			{
				viability += 5;
				break;
			}
			// better to trap target with low stats
			for (i = 0; i <= BATTLE_STATS_NO-1; ++i)
			{
				if (STAT_STAGE(bankDef, i) < 6)
				{
					if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))	// single battle
					{
						if (i == STAT_STAGE_DEF && (PhysicalMoveInMoveset(bankAtk)))
							viability += 5;
						else if (i == STAT_STAGE_SPDEF && (SpecialMoveInMoveset(bankAtk)))
							viability += 5;
					}
					else	// double battle
					{
						if ((i == STAT_STAGE_DEF) &&
						(PhysicalMoveInMoveset(bankAtk) || PhysicalMoveInMoveset(bankAtkPartner)))
							viability += 5;
						else if ((i == STAT_STAGE_SPDEF) &&
						(SpecialMoveInMoveset(bankAtk) || SpecialMoveInMoveset(bankAtkPartner)))
							viability += 5;
					}
					break;
				}
			}
			break;
			
		case EFFECT_NIGHTMARE:
			if (defAbility == ABILITY_COMATOSE)
				viability += 5;
			else if (defStatus1 & STATUS1_SLEEP)
				viability += 5;
			break;
		
		case EFFECT_CURSE:
			if (IsOfType(bankAtk), TYPE_GHOST)
			{
				if (atkAbility == ABILITY_SHADOWTAG || atkAbility == ABILITY_ARENATRAP)
					viability += 5;
				else if (atkStatus2 & (STATUS2_ESCAPE_PREVENTION | STATUS2_WRAPPED))
					viability += 5;
				break;
			}
			else
			{
				if (atkAbility == ABILITY_CONTRARY)
					break;
				else if (FindMovePositionInMoveset(MOVE_GYROBALL, bankAtk) != 4)
					viability += 5;
				else if (STAT_STAGE(bankAtk, STAT_STAGE_SPEED) < 3)
					break;
				else if (STAT_STAGE(bankAtk, STAT_STAGE_ATK) < 8)
					viability += 5;
				else if (STAT_STAGE(bankAtk, STAT_STAGE_DEF) < 8)
					viability += 5;
				break;
			}
			break;
			
		case EFFECT_PROTECT:
			switch (move)
			{
				case MOVE_QUICKGUARD:
					if (defAbility == ABILITY_GALEWINGS
					|| defAbility == ABILITY_PRANKSTER)
						viability += 5;
					else if (ABILITY(bankAtkPartner) == ABILITY_GALEWINGS
					|| ABILITY(bankAtkPartner) == ABILITY_PRANKSTER)
						viability += 5;
					else if (MoveEffectInMoveset(EFFECT_QUICK_ATTACK, bankDef)
					|| MoveEffectInMoveset(EFFECT_QUICK_ATTACK, bankAtkPartner))
						viability += 5;
					goto FUNCTION_RETURN;
					
				case MOVE_WIDEGUARD:
					// check if attack targets multiple foes (opponent side or all)
					for (i = 0; i <= 3; ++i)
					{
						if (gBattleMoves[gBattleMons[bankDef].moves[i]].target & (MOVE_TARGET_ALL | MOVE_TARGET_OPPONENTS_FIELD | MOVE_TARGET_BOTH))
							viability += 5;
						else if (gBattleMoves[gBattleMons[bankDefPartner].moves[i]].target 
						& (MOVE_TARGET_ALL | MOVE_TARGET_OPPONENTS_FIELD | MOVE_TARGET_BOTH))
							viability += 5;
						goto FUNCTION_RETURN;
					}
					break;
					
				case MOVE_CRAFTYSHIELD:
					if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
						viability += 5;
					break;
				
				case MOVE_MATBLOCK:
					if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
					{
						if (gDisableStructs[bankAtk].isFirstTurn)
							viability += 7;
						break;
					}
					break;
				
				default:
					if (atkAbility == ABILITY_POISONHEAL && (atkStatus1 & STATUS1_PSN_ANY))
						goto AI_PROTECT_LOW_HEALTH_CHECK;
					if (move == MOVE_SPIKYSHIELD)
					{
						if (PhysicalMoveInMoveset(bankDef))
							goto AI_CHECK_PROTECT_CHANCE;
						else
							goto AI_RANDOM_DOUBLES_PROTECT;
					}
					else if (move == MOVE_BANEFULBUNKER)
					{
						if (defStatus1 & STATUS1_ANY)
							goto AI_RANDOM_DOUBLES_PROTECT;
						else if (PhysicalMoveInMoveset(bankDef))
							goto AI_CHECK_PROTECT_CHANCE;
						goto AI_RANDOM_DOUBLES_PROTECT;
					}
					else if (atkStatus1 & (STATUS1_PSN_ANY | STATUS1_BURN))
						goto AI_CHECK_PROTECT_CHANCE;
					else if (atkStatus1 & (STATUS1_SLEEP | STATUS1_FREEZE))
						goto FUNCTION_RETURN;
					else if (atkItemEffect == ITEM_EFFECT_LEFTOVERS || atkItemEffect == ITEM_EFFECT_BLACK_SLUDGE)
						goto AI_PROTECT_LOW_HEALTH_CHECK;
					else
						goto AI_RANDOM_DOUBLES_PROTECT;
					break;
			}
			break;
			
		AI_PROTECT_LOW_HEALTH_CHECK: ;
			if (GetHealthPercentage(bankAtk) > 35)
				goto AI_RANDOM_DOUBLES_PROTECT;
		AI_CHECK_PROTECT_CHANCE: ;
			if (gDisableStructs[bankAtk].protectUses > 0)
				break;
			viability += 5;
			break;
		AI_RANDOM_DOUBLES_PROTECT:
			if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && (umodsi(Random(), 100) < 50))
				goto AI_CHECK_PROTECT_CHANCE;
			break;
			
		// entry hazards
		case EFFECT_SPIKES:
			// to do
			break;
			
		case EFFECT_FORESIGHT:
			// to do
			break;
			
		case EFFECT_PERISH_SONG;
			// todo
			break;
			
		case EFFECT_SANDSTORM:
			// todo 
			break;
			
		case EFFECT_ENDURE:
			// todo
			break;
		
		case EFFECT_ROLLOUT:
			// to do
			break;
			
		case EFFECT_SWAGGER:
			// to do
			break;
			
		case EFFECT_FURY_CUTTER:
			// to do
			break;
			
		case EFFECT_ATTRACT:
			// to do
			break;
			
		case EFFECT_BATON_PASS:
			// todo
			break;
			
		case EFFECT_RAPID_SPIN:
			//todo
			break;
			
		case EFFECT_RAIN_DANCE:
			//todo
			break;
			
		case EFFECT_SUNNY_DAY:
			//todo
			break;
			
		case EFFECT_ATTACK_UP_HIT:
			//todo
			break;
			
		case EFFECT_BELLY_DRUM:
			//todo
			break;
			
		case EFFECT_PSYCH_UP:
			//todo
			break;
			
		case EFFECT_TWISTER:
		case EFFECT_GUST:
		case EFFECT_THUNDER:
			//todo
			break;
		
		case EFFECT_MAGNITUDE:
		case EFFECT_EARTHQUAKE:
			//todo
			break;
			
		case EFFECT_FLINCH_HIT_2:
			//todo
			break;
			
		case EFFECT_SOLARBEAM:
			//todo
			break;
			
		case EFFECT_DEFENSE_CURL:
			//todo`
			break;
			
		case EFFECT_FAKE_OUT:
			//todo
			break;
			
		case EFFECT_STOCKPILE:
			//todo
			break;
			
		case EFFECT_HAIL:
			//todo
			break;
			
		case EFFECT_WILL_O_WISP:
			//todo
			break;
			
		case EFFECT_FOLLOW_ME:
		case EFFECT_HELPING_HAND:
			//todo
			break;
			
		case EFFECT_TAUNT:
			//todo
			break;
			
		case EFFECT_TRICK:
			//todo
			break;
			
		case EFFECT_INGRAIN:
			//todo
			break;
			
		case EFFECT_RECYCLE:
		case EFFECT_BRICK_BREAK:
			//todo
			break;
			
		case EFFECT_ENDEAVOR:	//flail?
			//todo
			break;
			
		case EFFECT_REFRESH:
			if (atkStatus1 & STATUS1_ANY)
				viability += 5;
			break;
			
		case EFFECT_TICKLE:
			//todo
			break;
			
		case EFFECT_COSMIC_POWER:
			//todo
			break;
			
		case EFFECT_BULK_UP:
			//todo;
			break;
			
		case EFFECT_CALM_MIND:
			//todo;
			break;
			
		case EFFECT_DRAGON_DANCE:
			if (move == MOVE_SHELLSMASH)
			{
				if (atkItem == ITEM_EFFECT_WHITE_HERB)
					viability += 6;
				else if (gBattleMons[bankAtk].statStages[STAT_SPECIAL_ATTACK-1] <= 6 && (SpecialMoveInMoveset(bankAtk)))
					viability += 6;
			}
			if (gBattleMons[bankAtk].statStages[STAT_ATTACK-1] > 7)
				goto AI_SPEED_PLUS;
			else if (PhysicalMoveInMoveset(bankAtk))
				viability += 6;
			goto AI_SPEED_PLUS;
			
		
		// other effects ???
		case EFFECT_PLEDGE:
			//todo - if partner going to use same pledge move
			break;
		
		
	}
	FUNCTION_RETURN: ;
	return viability;
}
