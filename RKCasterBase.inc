Sub Init_RKCasterBase
	/varset EarlyEscapePauseDuration 3
	/declare LowManaLevel int outer 10
	/declare SpellMemSlot int outer 9
	
	/call AddToArray "CombatActionSubs" "ExecuteCasterCombatActions"
	/call AddToArray "PreTargetAcquisitionCombatActionSubs" "ExecuteCombatBuffs"
	/call AddToArray "NonCombatSubs" "ExecuteNonCombatBuffs"
	/call AddToArray "NonCombatSubs" "ClearAppliedDebuffs"
	
	/call GetNoCooldownSpellGem
	/declare NoCooldownSpellGem int outer ${Macro.Return}
		
	/declare OffGCDCombatActionSubs[${DynamicArraySize}] string outer

	/declare DetrimentalSpellDebuffNames[${DynamicArraySize}] string outer
	/declare DetrimentalSpells[${DynamicArraySize}] string outer	

 	/declare GiftSpells[${DynamicArraySize}] string outer

	/declare CombatSelfBuffs[${DynamicArraySize}] string outer
	/declare CombatSelfBuffsNames[${DynamicArraySize}] string outer	

	/declare ShouldCheckPartyBuffs bool outer TRUE
	/declare ShouldCheckTankBuffs bool outer TRUE	

	/declare CombatPartyBuffs[${DynamicArraySize}] string outer
	/declare CombatPartyBuffsNames[${DynamicArraySize}] string outer	

	/declare CombatTankBuffs[${DynamicArraySize}] string outer
	/declare CombatTankBuffsNames[${DynamicArraySize}] string outer	
	
	/declare NonCombatSelfBuffs[${DynamicArraySize}] string outer
	/declare NonCombatSelfBuffsNames[${DynamicArraySize}] string outer	
	
	/declare NonCombatPartyBuffs[${DynamicArraySize}] string outer
	/declare NonCombatPartyBuffsNames[${DynamicArraySize}] string outer	
	
	/declare AppliedDebuffs[${DynamicArraySize}] string outer

	/declare GiftProcName string outer Gift of Ascendant Exquisite Radiant Mana

	/echo Done initializing caster base!
/return

Sub ExecuteCasterCombatActions
	/if (${DebugMode}) /echo ExecuteCasterCombatActions
	/if (!${Me.SpellReady[${NoCooldownSpellGem}]}) {
		/call ExecuteArrayOfSubs "OffGCDCombatActionSubs"
	}
	
	/call RKAACast "Banestrike"	

	/if (${Me.Moving} && !${Me.Class.Name.Equal[Bard]}) /return FALSE
	/call ExecuteGiftProc
	/call ExecuteDebuffs
	/call ExecuteBuffsWithUniqueNames "CombatSelfBuffs"
	/if (${Macro.Return}) /return TRUE
/return FALSE

Sub GetNoCooldownSpellGem
	/declare gemIndex int local 10
	/for gemIndex 1 to 12
		/if (${Me.Gem[${gemIndex}].RecastTime} <= 1.5) {
			/return ${gemIndex}
		}
	/next gemIndex
/return 1

Sub AddDetrimentalSpellWithUniqueDebuffName(string spellName, string effectName)
	/call AddToArray "DetrimentalSpells" "${spellName}"
	/varset DetrimentalSpellDebuffNames[${Macro.Return}] ${effectName}
/return FALSE

Sub AddToArrayWithUniqueName(string arrayName, string spellName, string effectName)
	/call AddToArray "${arrayName}" "${spellName}"
	/declare arrayNameWithSuffex string local "${arrayName}Names"

	/echo Adding ${effectName} to ${arrayNameWithSuffex} at position ${Macro.Return}
	/varset ${arrayNameWithSuffex}[${Macro.Return}] ${effectName}
/return FALSE

Sub ClearAppliedDebuffs
	/call ClearArray "AppliedDebuffs"
/return FALSE

Sub ExecuteGiftProc
	/if (${DebugMode}) /echo ExecuteGiftProc
	/declare i int local
	/if (${Me.Song[${GiftProcName}].ID}) {
		/for i 1 to ${GiftSpells.Size}
			/if (${Cast.Ready[${GiftSpells[${i}]}]}) {
				/call RKCast "${GiftSpells[${i}]}" ${CurrentTargetId}
				/if (${Macro.Return}) /return TRUE
			}
		/next i
	}
/return FALSE

Sub ExecuteNukes(string nukeArray)
	/if (${DebugMode}) /echo ExecuteNukes for array ${nukeArray}
	/declare i int local
	/for i 1 to ${${nukeArray}.Size}
		/if (${Bool[${${nukeArray}[${i}]}]}) {
			/if (${Cast.Ready[${${nukeArray}[${i}]}]}) {
				/call RKCast "${${nukeArray}[${i}]}" ${CurrentTargetId}
				/if (${Macro.Return}) /return TRUE
			}
		}
	/next i
/return FALSE

Sub ExecuteDebuffs
	/if (${DebugMode}) /echo ExecuteDebuffs for array ${debuffArray}
	/declare i int local
	/declare spellName string local
	/declare debuffName string local	
	/if (!${Target.ID}) /return FALSE
	
	/for i 1 to ${DetrimentalSpells.Size}
		/varset spellName ${DetrimentalSpells[${i}]}
		/if (${Bool[${spellName}]}) {
			/if (${DebugMode}) /echo debuff spellName - ${spellName}
			/if (${Bool[${DetrimentalSpellDebuffNames[${i}]}]}) {
				/varset debuffName ${DetrimentalSpellDebuffNames[${i}]}
			} else {
				/varset debuffName ${spellName}
			}
			
			/if (!${Bool[${debuffName}]}) /varset debuffName - ${spellName}
			
			/if (${DebugMode}) /echo debuffName - ${debuffName}

			/if (${Bool[${spellName}]}) {
				/if (${DebugMode}) /echo trying to cast ${spellName}
				/call IsSpellMemorized "${spellName}"
				/if (${Macro.Return}) {
					/if (${DebugMode}) /echo spell is memorized, CurrentTargetId is ${CurrentTargetId}
					/if (${DebugMode}) /echo call RKDebuffCast "${spellName}" "${debuffName}" ${CurrentTargetId}
					/call RKDebuffCast "${spellName}" "${debuffName}" ${CurrentTargetId}
					/if (${Macro.Return}) {
						/if (${DebugMode}) /echo spellcast success
						/return TRUE
					}
				} else {
					/if (${Bool[${AltAbility[${spellName}]}]}) {
						/if (${DebugMode}) /echo casting AA ${spellName}
						/call RKAACast "${spellName}"
					}
				}
			}
		}
	/next i
/return FALSE

Sub ExecuteCombatBuffs
	/if (${DebugMode}) /echo ExecuteCombatBuffs
	/if (${Me.Casting.ID}) /return FALSE
	/if (${Me.Moving} && !${Me.Class.Name.Equal[Bard]}) /return FALSE
	/call ExecuteSelfBuffs "CombatSelfBuffs"
	/if (${Macro.Return}) {
		/if (${DebugMode}) /echo Exiting early because of self buffs
		/return TRUE
	}
	
	/if (${ShouldCheckPartyBuffs}) {
		/call ExecuteGroupBuffs "CombatPartyBuffs"
		/if (${Macro.Return}) {
			/if (${DebugMode}) /echo Exiting early because of party buffs
			/return TRUE
		}
	}
	
	/if (${ShouldCheckTankBuffs}) {
		/call ExecuteTankBuffs "CombatTankBuffs"
		/if (${Macro.Return}) {
			/if (${DebugMode}) /echo Exiting early because of tank buffs
			/return TRUE
		}
	}
	
/return FALSE

Sub ExecuteNonCombatBuffs
	/if (${DebugMode}) /echo ExecuteNonCombatBuffs
	/if (${Me.Moving} && !${Me.Class.Name.Equal[Bard]}) /return FALSE
	/if (${Me.Casting.ID}) /return FALSE
	/call ExecuteBuffsWithUniqueNames "NonCombatSelfBuffs"
	/if (${Macro.Return}) /return TRUE
	/call ExecuteGroupBuffs "NonCombatPartyBuffs"
	/if (${Macro.Return}) /return TRUE
/return FALSE

Sub ExecuteBuffsWithUniqueNames(string arrayName)
	/if (${DebugMode}) /echo ExecuteNonCombatSelfBuffsWithUniqueNames
	/if (${Me.Moving} && !${Me.Class.Name.Equal[Bard]}) /return FALSE
	/declare i int local
	/declare spellName string local
	/declare buffName string local	

	/for i 1 to ${${arrayName}.Size}
		/varset spellName ${${arrayName}[${i}]}
		/if (${Bool[${spellName}]}) {
			/varset buffName ${${arrayName}Names[${i}]}
			/if (!${Bool[${buffName}]}) /varset buffName ${spellName}

			/if (${Bool[${AltAbility[${spellName}]}]}) {
				/if (${DebugMode}) /echo trying to cast AA ${spellName}
				/call RKAACast "${spellName}" ${Me.ID}
			} else {
				/if (${DebugMode}) /echo trying to cast ${spellName} for buff: ${buffName}
				/call RKBuffWithUniqueName "${spellName}" "${buffName}" ${Me.ID}
				/if (${Macro.Return}) /return TRUE
			}
		}
	/next i
/return FALSE


Sub ExecuteSelfBuffs(string buffArray)
	/if (${DebugMode}) /echo ExecuteSelfBuffs from array ${buffArray}
	/if (${Me.Casting.ID}) /return FALSE
	/declare i int local
	
	/for i 1 to ${${buffArray}.Size}
		/if (${Me.GemTimer[${${buffArray}[${i}]}]} == 0) {
			/if (!${Bool[${${buffArray}[${i}]}]}) /return FALSE
			/if (${Bool[${${buffArray}Names[${i}]}]}) {
				/call RKBuffWithUniqueName "${${buffArray}[${i}]}" "${${buffArray}Names[${i}]}" ${Me.ID}
			} else {
				/call RKBuff "${${buffArray}[${i}]}" ${Me.ID}
			}
			/if (${Macro.Return}) /return TRUE
		}
	/next i
/return FALSE

Sub ExecuteTankBuffs(string buffArray)
	/if (${DebugMode}) /echo ExecuteTankBuffs from array ${buffArray}
	/if (${Me.Casting.ID}) /return FALSE
	/declare i int local
	/declare j int local
	/if (${DebugMode}) /echo looping from buff 1 to ${${buffArray}.Size}
	/for i 1 to ${${buffArray}.Size}
		/if (!${Bool[${${buffArray}[${i}]}]}) {
			/varset ShouldCheckTankBuffs FALSE
			/return FALSE
		}
		/if (${DebugMode}) /echo ExecuteTankBuffs item ${i} on ${Spawn[id ${TankId}]}
		/call RKBuff "${${buffArray}[${i}]}" ${TankId}
		/if (${Macro.Return}) /return TRUE
	/next i
	/varset ShouldCheckTankBuffs FALSE
/return FALSE

Sub ExecuteGroupBuffs(string buffArray)
	/if (${DebugMode}) /echo ExecuteGroupBuffs from array ${buffArray}
	/if (${Me.Casting.ID}) /return FALSE
	/declare i int local
	/declare j int local
	/if (${DebugMode}) /echo looping from buff 1 to ${${buffArray}.Size}
	/for i 1 to ${${buffArray}.Size}
		/if (!${Bool[${${buffArray}[${i}]}]}) {
			/varset ShouldCheckPartyBuffs FALSE
			/return FALSE
		}
		/if (${DebugMode}) /echo looping from group member 1 to ${${buffArray}.Size}
		/for j 0 to ${Group.Members}
			/if (${DebugMode}) /echo ExecuteGroupBuffs item ${i} group member ${j}
			/call RKBuff "${${buffArray}[${i}]}" ${Group.Member[${j}].ID}
			/if (${Macro.Return}) /return TRUE
		/next j
	/next i
	/varset ShouldCheckPartyBuffs FALSE
/return FALSE

Sub RKBuffWithUniqueName(string spellName, string buffName, int targetId)
	/if (${DebugMode}) /echo RKBuffWithUniqueName ${spellName} ${buffName} ${targetId}
	/if (!${Bool[${targetId}]}) /declare targetId int inner ${Me.ID}
	/if (${targetId} == ${Me.ID}) {
		/if (${Bool[${Me.Buff[${buffName}]}]} || ${Bool[${Me.Song[${buffName}]}]} || ${Bool[${Me.Aura[${buffName}]}]}) {
			/return FALSE
		}
	}
	
	/if (${Bool[${Target.Buff[${buffName}]}]}) /return FALSE
	
	/if (${targetId} == ${Me.ID}) {
		/call WillSelfBuffStack "${spellName}"
	} else {
		/call WillBuffStack "${spellName}" ${targetId}
	}
	
	/if (!${Macro.Return}) /return FALSE
	
	/if (!${buffName.Equal[${spellName}]}) {
		/if (${targetId} == ${Me.ID}) {
			/call WillSelfBuffStack "${buffName}"
		} else {
			/call WillBuffStack "${buffName}" ${targetId}
		}

		/if (!${Macro.Return}) /return FALSE
	}
	|/echo casting ${spellName} for buff ${buffName} on target ${targetId}
	/call RKMemCast "${spellName}" ${targetId}
/return ${Macro.Return}

Sub RKBuff(string spellName, int targetId)
	/if (${DebugMode}) /echo RKBuff ${spellName} ${targetId}
	|/if (!${Bool[${targetId}]}) /declare targetId int inner ${Me.ID}
	/if (${targetId} == ${Me.ID}) {
		/if (${Me.Buff[${spellName}].ID} && ${Me.Buff[${spellName}].Duration} > 10000) {
			/if (${DebugMode}) /echo already have buff ${spellName}
			/return FALSE
		}

		/if (${Me.Song[${spellName}].ID} && ${Me.Song[${spellName}].Duration} > 10000) {
			/if (${DebugMode}) /echo already have song ${spellName}
			/return FALSE
		}
		
		/if (${Bool[${Me.Aura[${spellName}]}]}) {
			/if (${DebugMode}) /echo already have aura ${spellName}
			/return FALSE
		}
		
		/call WillSelfBuffStack "${spellName}"
		/if (!${Macro.Return}) /return FALSE
	}
	
	/if (${targetId} == ${Me.ID}) {
		/if (${Me.Buff[${spellName}].ID} && ${Me.Buff[${spellName}].Duration.TotalSeconds} > 5) {
			/if (${DebugMode}) /echo I already have ${spellName}
			/return FALSE
		}
	}
	
	/declare previousTargetId int local ${Target.ID}
	/call TargetSync ${targetId}
	/if (!${Bool[${Target.Buff}]}) /delay 1s
	/if (!${Macro.Return}) {
		/if (${DebugMode}) /echo couldn't sync target ${targetId} for buff ${spellName}
		/call TargetSync ${previousTargetId}
		/return FALSE
	}
	
	/if (${Target.Buff[${spellName}].ID} && ${Target.Buff[${spellName}].Duration.TotalSeconds} > 5) {
		/if (${DebugMode}) /echo already have ${spellName}
		/return FALSE
	}

	/call WillBuffStack "${spellName}" ${targetId}
	/if (!${Macro.Return}) {
		/if (${DebugMode}) /echo ${spellName} won't stack on ${targetId}
		/return FALSE
	}
	
	/call RKMemCast "${spellName}" ${targetId}
	/if (${Macro.Return}) {
		/if (${DebugMode}) /echo memcast ${spellName}
		/delay ${Spells[${spellName}].MyCastTime.Int}s
		/return TRUE
	}
	
	/if (!${Me.SpellReady[${spellName}]}) {
		/if (${DebugMode}) /echo awaiting spell: ${spellName}
		/return TRUE
	}

	/if (${Macro.Return}) {
		/delay 1s
		/call TargetSync ${previousTargetId}
		/return TRUE
	}
	
/return FALSE

Sub RKDebuffCast(string spellName, string debuffName, int targetId)
	/if (${DebugMode}) /echo RKDebuffCast ${spellName} ${debuffName} ${targetId}
	/declare previousTargetId int local ${Target.ID}
	/if (${targetId} == 0) {
		/echo RKDebuffCast ${spellName} - ${debuffName} - targetId was 0
		/return FALSE
	}

	/call TargetSync ${targetId}
	/delay 5 ${Bool[${Target.Buff}]}
	/if (!${Macro.Return}) {
		/if (${DebugMode}) /echo couldn't sync target ${targetId} for debuff ${spellName}
		/call TargetSync ${previousTargetId}
		/return FALSE
	}
	
	/if (${DebugMode}) /echo ${debuffName}
	/if (${Target.Buff[${debuffName}].ID} && ${Target.Buff[${debuffName}].Duration.TotalSeconds} > 2) {
		/if (${DebugMode}) /echo target already has ${debuffName}
		/return FALSE
	}
	
	/call RKCast "${spellName}" ${targetId}
	/declare castResult bool local ${Macro.Return}
	/if (${DebugMode}) /echo RKCast "${spellName}" ${targetId} - result: ${castResult}
	/call TargetSync ${previousTargetId}
/return ${castResult}

Sub RKMemCast(string spellName, int targetId)
	/call IsSpellMemorized "${spellName}"
	/if (${Macro.Return}) {
		/if (!${Me.SpellReady[${spellName}]}) {
			/if (${DebugMode}) /echo RKMemCast - ${spellName} not ready
			/return TRUE
		}
	} else {
		/call MemorizeSpell "${spellName}" ${SpellMemSlot}
		/delay 3s
		/if (${DebugMode}) /echo RKMemCast - ${spellName} not memorized
		/return TRUE
	}
	
	/if (${Me.GemTimer[${Me.Gem[${spellName}]}]} > 0 && ${Me.GemTimer[${Me.Gem[${spellName}]}]} < 2000) {
		/return TRUE
	}
	
	/call RKCast "${spellName}" ${targetId}
/return FALSE

Sub RKCast(string spellName, int targetId)
	/if (${DebugMode}) /echo RKCast ${spellName} ${targetId}
	/if (!${Me.Class.Name.Equal[Bard]} && ${Me.Moving}) /return FALSE
	/if (${Spell[${spellName}].Mana} > ${Me.CurrentMana}) /return FALSE
	
	/if (!${Spell[${spellName}].ID}) {
		/docommand /${ChatCommand} hrm... trying to do something I shouldn't - ${spellName}
		/return FALSE
	}
	
	/if (${DebugMode}) /echo casting ${spellName}
	/if (!${Me.SpellReady[${spellName}]}) /return FALSE
	|&& ${Me.GemTimer[${Me.Gem[${spellName}]}]} == 0) /return FALSE

	/declare previousTargetId int local ${Target.ID}
	
	/call TargetSync ${targetId}
	/if (!${Macro.Return}) /return FALSE

	/declare twisting bool local (${Twist} && ${Me.Class.Name.Equal[Bard]})

	/if (${twisting}) {
		/if (${Me.Gem[${spellName}]} < 10) {
			/if (${DebugMode}) /echo trying to twist once for ${spellName} in gemslot ${Me.Gem[${spellName}]}		
			/twist once ${Me.Gem[${spellName}]}
			/delay 4s
			/if (${Me.Class.Name.Equal[Bard]}) {
				/stopcast
				/delay 5
			}
		} else {
			/twist off
			/delay 1s
			/cast "${spellName}"
			/delay 4s
			/if (${DebugMode}) /echo stop cast of ${spellName}
			/if (${Me.Class.Name.Equal[Bard]}) {
				/stopcast
				/delay 5
				/twist
			}
		}
	} else {
		/cast ${Me.Gem[${spellName}]}
	}
	
	/if (${Cast.Result.Equal[CAST_SUCCESS]}) {
		/delay 2s
	}
	

	
	/if (${DebugMode}) /echo Cast of ${spellName} result was ${Cast.Result}
/return ${Cast.Result.Equal[CAST_SUCCESS]}

Sub CheckMedding
	/if (${DebugMode}) /echo CheckMedding
	
	|If we're not in active combat mode, we short circuit.
	/if (${BOTMode} < 3) /return FALSE
	/if (${Me.PctMana} < ${LowManaLevel}) {
		/if (!${Target.ID}) {
			/if (!${BOTMode} >= 3) {
				/varset PreviousBOTMode ${BOTMode}
				/varset BOTMode 0 |Medding
			}
		}
	}

	/if (${BOTMode} == 5) { 
		/if (!${Me.Combat}) {
			/if (!${Me.Mount} && !${Me.Sitting}) /sit on
			| After we're done medding, we return the state we were in before.
			/if (${Me.PctMana} == 100) /varset BOTMode ${PreviousBOTMode} 
			/if (!${Bool[${BOTMode}]}) /varset BOTMode 3
			/return TRUE
		}
	} 
/return FALSE

Sub IsSpellMemorized(string spellName)
	/declare i int local
	/for i 1 to 12
		/if (${Me.Gem[${i}].ID} && ${Me.Gem[${i}].Name.Equal[${spellName}]}) /return TRUE
	/next i
/return FALSE

Sub MemorizeSpell(string spellName, int gemSlot)
	/if (${DebugMode}) /echo MemorizeSpell ${spellName} in slot ${gemSlot}
	/declare i int local
	/for i 1 to 12
		/if (${Me.Gem[${i}].ID} && ${Me.Gem[${i}].Name.Equal[${spellName}]}) /return TRUE
	/next i
	/memorize "${spellName}" ${gemSlot}
	/delay 2s
	/if (!${Me.Gem[${gemSlot}].Name.Equal[${spellName}]}) /return FALSE
	/if (${Spell[${spellName}].RecastTime} < 10) /delay ${Spell[${spellName}].RecastTime} ${Me.SpellReady[${spellName}]}
/return ${Me.SpellReady[${spellName}]}
