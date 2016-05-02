Sub Init_RKMeleeBase
	/declare CombatAbilities[${DynamicArraySize}] string outer
	/declare CombatDiscs[${DynamicArraySize}] string outer	
	/declare BurnDiscs[${DynamicArraySize}] string outer
	
	/declare MeleeMode bool outer TRUE
	
	/call AddToArray "CombatActionSubs" "MeleeSubs"
	/call AddToArray "BurnCombatActionSubs" "ExecuteBurnDiscs"
	/call AddToArray "PreTargetAcquisitionCombatActionSubs" "CheckNoTargetMelee"

	/varset InCombatStickToCommander FALSE
	
	/echo Done initializing melee base!
/return

Sub CheckNoTargetMelee
	/if (${BOTMode} != 5 && ${Me.XTarget} == 0) /attack off
/return FALSE

Sub MeleeSubs
	/if (${DebugMode}) /echo MeleeSubs
	/if (!${MeleeMode}) /return FALSE
	/if (!${Target.ID}) /return FALSE
	/if (${Target.Distance} < ${MaxMeleeDistance}) /attack on

	/if (${Me.Haste} < 170 && !${Me.Buff[Resurrection Sickness].ID} && !${Bool[${Me.Hasted}]}) /cast item "Distillate of Alacrity X"

	/if (${BOTMode} != 5) {
		/call CheckTargetDistance ${CurrentTargetId}
		/if (!${Target.ID}) /return FALSE
		/if (${Target.ID}) /face fast nolook
		/keypress HOME
	}
	/call ExecuteCombatAbilities
/return FALSE

Sub CheckTargetDistance(int targetId)
	/if (${DebugMode}) /echo CheckTargetDistance
	/if (${Me.LAHoTT} > 0 && ${Me.ID} != ${TankId} && ${Me.TargetOfTarget.ID} != ${TankId} && !${Me.Name.Equal[${RKCommander}]} && !${Me.TargetOfTarget.Type.Equal[Pet]}) {
		/if (${DebugMode}) /echo Escaping CheckTargetDistance because of target's target
		/return FALSE
	}
	
	/if (${Spawn[${targetId}].Distance} < 20) {
		/if (${Spawn[${targetId}].Z} < (${Me.Z} - 5)) {
			/if (!${Descending}) {
				/if (${DebugMode}) /echo target: ${Spawn[${targetId}].Z} < me: ${Me.Z} - Descending
				/keypress end hold
				/varset Descending TRUE
			}
		} else {
			/if (${Descending}) {
				/keypress end
				/varset Descending FALSE
			}
		}
	}

	/call SetStickTarget ${targetId} ${StickDistance}
	
	|/if (${Spawn[${targetId}].Distance} > ${MaxMeleeDistance}) {
	|	/if (${DebugMode}) /echo target's too far away
	|}
/return FALSE

Sub ExecuteCombatAbilities
	/if (${DebugMode}) /echo ExecuteCombatAbilities
	/declare i int local
	/call RKAACast "Banestrike"	
	/declare currentAbility string local
	/for i 1 to ${CombatAbilities.Size}
		/varset currentAbility ${CombatAbilities[${i}]}
		/if (${Bool[${currentAbility}]}) {
			/if (${Bool[${Me.Ability[${currentAbility}]}]}) {
				/if (!${${currentAbility}.Equal[Taunt]} || ${Me.LAHoTT} == 0 || ${Me.TargetOfTarget.ID} != ${Me.ID}) {
					/doability ${Me.Ability[${currentAbility}]}
				} else {
					/doability "Taunt"
				}
			} else /if (${AltAbility[${currentAbility}].ID}) {
				/call RKAACast "${currentAbility}"
			}
		}
	/next i

	/if (${Me.PctEndurance} > 5) {
		/for i 1 to ${CombatDiscs.Size}
			/if (${Bool[${CombatDiscs[${i}]}]}) {
				/call RKDisc "${CombatDiscs[${i}]}"
			}
		/next i	
	}
/return TRUE

Sub ExecuteBurnDiscs
	/if (${DebugMode}) /echo ExecuteBurnDiscs
	/declare i int local
	/for i 1 to ${BurnDiscs.Size}
		/if (${Bool[${BurnDiscs[${i}]}]}) {
			/call RKDisc "${BurnDiscs[${i}]}"
		/next i
	}
/return FALSE