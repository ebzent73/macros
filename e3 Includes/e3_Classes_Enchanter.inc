|--------------------------------------------------------------------------------|
|- e3_Classes_Enchanter.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ													-|
|--------------------------------------------------------------------------------|
|- Contains Enchanter specific functions related to,							-|
|-		Setup and Ini entry information											-|
|-		MainLoop, and AssistLoop hooks											-|
|-		Over-all functionality / flow											-|
|--------------------------------------------------------------------------------|



#event add_RuneTarget "<#1#> Rune on"
#event add_RuneTarget "<#*#> Rune on #1#"
sub event_add_RuneTarget(string line, string RuneTarget)
/if (${Me.Class.ShortName.Equal[ENC]}) {
/if (${Debug}) /echo |- event_add_RuneTarget ==>
	
	/if (!${SpawnCount[pc ${RuneTarget}]}) {
		/docommand ${ChatToggle} I don't see any PCs matching [${RuneTarget}]...
	} else {
	
		/varset RuneTarget ${Spawn[pc ${RuneTarget}].CleanName}
	
		/if (!${Defined[RuneTargets]}) {
			/declare RuneTargets[1] string outer
			/varset RuneTargets[1] ${RuneTarget}
			/docommand ${ChatToggle} Maintaining [${chainRune.Arg[1,/]}] on [${RuneTarget}].
		} else {
			
			/declare i int local
			
			/declare RuneTarget_Names string local
			
			/for i 1 to ${RuneTargets.Size}
			
				/varset RuneTarget_Names ${RuneTarget_Names}${RuneTargets[${i}]},
			
			/next i
			
			/if (${Select[${RuneTarget},${RuneTarget_Names}]}) {
				/docommand ${ChatToggle} I am already maintaining [${chainRune.Arg[1,/]}] on [${RuneTarget}].
			} else {
				/call BuildArray RuneTargets ${RuneTarget}
				/docommand ${ChatToggle} Maintaining [${chainRune.Arg[1,/]}] on [${RuneTarget_Names}${RuneTarget}].
			}
		}
	}
	
/if (${Debug}) /echo <== add_RuneTarget -|
}
/return



#event remove_RuneTarget "<#1#> Rune Off"
#event remove_RuneTarget "<#*#> Rune Off #1#"
#event remove_RuneTarget "<#*#> End Rune on #1#"
#event remove_RuneTarget "<#*#> All runes off"
#event remove_RuneTarget "<#*#> end runes"
sub event_remove_RuneTarget(line, ChatSender)
/if (${Me.Class.ShortName.Equal[ENC]}) {
/if (${Debug}) /echo |- remove_RuneTarget ==>

	/if (${Defined[RuneTargets]}) {

		/if (${Defined[ChatSender]}) {
		
			/declare i int local
			
			/for i 1 to ${RuneTargets.Size}

				/if (${RuneTargets[${i}].Equal[${ChatSender}]}) {
					/docommand ${ChatToggle} No longer maintaining [${chainRune.Arg[1,/]}] on [${RuneTargets[${i}]}].
					/call RemoveArrayElement RuneTargets "${RuneTargets[${i}]}"
				}
			
			/next i
			
		} else {
			/docommand ${ChatToggle} Ending chain runes.
			/deletevar RuneTargets
		}
		
	}
	
/if (${Debug}) /echo <== remove_RuneTarget -|
}
/return



SUB check_Rune
/if (${Debug}) /echo |- chainRune ==>

	/if (!${idle} && ${RuneTargets.Size}) {
		
		/declare i int local
		
		/declare runeSpell string local ${chainRune.Arg[1,/]}
		/declare runeGem string local ${chainRune.Arg[2,/].Right[1]}
		/if (${chainRune.Arg[3,/].Length} && ${chainRune.Arg[3,/].Find[reagent::]}) /declare runeReagent string local ${chainRune.Arg[3,/].Arg[2,::]}
		
		/if (${Defined[runeReagent]} && !${FindItemCount[${runeReagent}]}) {
			/docommand ${ChatToggle} I have run out of [${runeReagent}] and can no longer maintain [${runeSpell}].  Ending chain runes.
			/deletevar RuneTargets
		} else {
		
			/for i 1 to ${RuneTargets.Size}
				/if (${Spawn[pc ${RuneTargets[${i}]}].Distance} < ${Spell[${runeSpell}].MyRange} && ${Me.CurrentMana} > ${Spell[${runeSpell}].Mana} && !${NetBots[${RuneTargets[${i}]}].Buff.Find[${Spell[${runeSpell}].ID}]}) {
					/call e3_Cast ${NetBots[${RuneTargets[${i}]}].ID} "${runeSpell}" gem${runeGem} ${If[${Me.Gem[${runeSpell}]},3s,${Spell[${runeSpell}].RecastTime}s]}
				}
			/if (!${ActionTaken}) /next i
		}
	}

/if (${Debug}) /echo <== chainRune -|
/RETURN



Sub ENC_Setup
/if (${Debug}) /echo |- ENC_Setup ==>

	/if (${Ini[${Character_Ini},Enchanter,Chain Rune].Length}) /call IniToVar "${Character_Ini},Enchanter,Chain Rune" chainRune string outer

/if (${Debug}) /echo <== ENC_Setup -|
/return



Sub ENC_Background_Events

	/doevents add_RuneTarget
	/doevents remove_RuneTarget

/return



SUB ENC_MacroSettings
/if (${Debug}) /echo |- ENC_MacroSettings ==>

	| Adv Settings
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#1" "check_Rune" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#2" "check_CombatBuffs" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#3" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#4" "check_Debuffs" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#5" "check_DoTs" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#6" "check_Rain" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#7" "check_Nukes" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#8" "check_Buffs" 1
	/call WriteToIni "${advSettings_Ini},ENC Functions,ENC Function#9" "check_Pets" 1

/if (${Debug}) /echo <== ENC_MacroSettings -|
/RETURN



SUB ENC_CharacterSettings
/if (${Debug}) /echo |- ENC_CharacterSettings ==>

	/call WriteToIni "${Character_Ini},Enchanter,Chain Rune"

/if (${Debug}) /echo <== ENC_CharacterSettings -|
/RETURN



Sub ENC_Aliases
/if (${Debug}) /echo |- ENC_Aliases ==>

	/squelch /alias /runeon /bc Rune on
	/squelch /alias /runeoff /bc Rune off

/if (${Debug}) /echo <== ENC_Aliases -|
/return


|**





#event AutoMez "<#*#> Mez All"
sub event_AutoMez
/if (${Debug}) /echo |- AutoMez -| ==>
/if (!${Defined[AutoMez_Spell]} && ${Ini[${Character_Ini},Enchanter,Auto-Mez (Spell/Gem/MaxLevel)].Length}) {
	/call INItoVar "${Character_Ini},Enchanter,Auto-Mez (Spell/Gem/MaxLevel)" UNDEFINED string local
	/declare AutoMez_Spell string outer ${Macro.Return.Arg[1,/]}
	/declare AutoMez_Gem int outer ${Macro.Return.Arg[2,/]}
	/declare MaxMezLevel int outer ${Macro.Return.Arg[3,/]}
}
/call INItoVar ${Macro_Ini},Settings,AE_Radius Rain_Radius int outer
/declare i int local
/for i 1 to ${SpawnCount[npc radius ${Rain_Radius}]}
/if (${Spawn[${NearestSpawn[${i},NPC radius ${Rain_Radius}]}].LineOfSight} && ${Spawn[${NearestSpawn[${i},NPC radius ${Rain_Radius}]}].Type.NotEqual[Untargetable]} && ${Spawn[${NearestSpawn[${i},NPC radius ${Rain_Radius}]}].Type.NotEqual[Pet]} && ${Spawn[${NearestSpawn[${i},NPC radius ${Rain_Radius}]}].Level} <= ${MaxMezLevel}) {
	/call TrueTarget ${NearestSpawn[${i},NPC radius ${Rain_Radius}].ID}
	/if (${Spawn[${Me.TargetOfTarget}].Type.Equal[PC]}) {
		/if (${Debug}) /echo Spawn: [${NearestSpawn[${i},NPC radius ${Rain_Radius}].CleanName},${NearestSpawn[${i},NPC radius ${Rain_Radius}].ID}] || TargetOfTarget: [${Me.TargetOfTarget}]
		/call MezTarget
	}
	/squelch /target clear
}
/delay 1
/next i
/if (${Debug}) {
	/echo |- AutoMez -| <==
}
/return
sub MezTarget
/declare GiveUpValue int local 3
/if (${${Target.ID}${Spell[${AutoMez_Spell}].ID}} == 0) {
	/if (!${Defined[${Target.ID}${Spell[${AutoMez_Spell}].ID}]}) /declare ${Target.ID}${Spell[${AutoMez_Spell}].ID} timer outer 0
	:recast
	/call e3_Cast ${Target.ID} "${AutoMez_Spell}" Gem${AutoMez_Gem} 5s
	/if (${Select[${castReturn},CAST_RESISTED]}) {
		/if (!${Defined[ResistRetry]}) /declare ResistRetry int local 0
		/docommand ${ChatToggle} **Warning** ${Target.CleanName} resisted ${AutoMez_Spell}.
		/if (${Defined[ResistRetry]}) /varcalc ResistRetry ${ResistRetry}+1
		/if (${ResistRetry} < ${GiveUpValue}) /goto :recast
	} else /if (${Select[${castReturn},CAST_IMMUNE]}) {
		/varset ${Target.ID}${Spell[${AutoMez_Spell}].ID} 20000000
		/docommand ${ChatToggle} **Warning** ${Target.CleanName} is IMMUNE to ${AutoMez_Spell}.
	} else /if (${Select[${castReturn},CAST_CANNOTSEE]} && ${spamTimer}==0) {
		/if (!${Defined[Retry]}) /declare LOSRetry int local 0
		/docommand ${ChatToggle} **Warning** I cannot see ${Target.CleanName}!!.
		/if (${Defined[spamTimer]}) /varset spamTimer 15s
		/if (${Defined[LOSRetry]}) /varcalc LOSRetry ${LOSRetry}+1
		/if (${LOSRetry} < ${GiveUpValue}) /goto :recast
	} else /if (${Select[${castReturn},CAST_OUTOFMANA]} && ${spamTimer}==0) {
		/docommand ${ChatToggle} **Warning** I do not have enough mana to cast ${MyDebuffs[${i}]}!!.
		/if (${Defined[spamTimer]}) /varset spamTimer 15s
	} else /if (${Select[${castReturn},CAST_OUTOFRANGE]} && ${spamTimer}==0) {
		/docommand ${ChatToggle} **Warning** ${MyDebuffs[${i}]} is out of range for ${MyDebuffs[${i}]}!!.
		/if (${Defined[spamTimer]}) /varset spamTimer 15s
	} else /if (${Select[${castReturn},CAST_SUCCESS]})	{
		/docommand ${ChatToggle} Landed ${AutoMez_Spell} on ${Target.CleanName}!!.
		/if (${Defined[${Target.ID}${Spell[${AutoMez_Spell}].ID}]}) /varset ${Target.ID}${Spell[${AutoMez_Spell}].ID} ${Math.Calc[${Spell[${AutoMez_Spell}].Duration} * 6-6]}s
	}
}
/delay 5s !${Me.Casting.ID}
/return
#event AutoMez_Off "<#*#> End AutoMez"
#event AutoMez_Off "<#*#> AutoMez off"
sub event_AutoMez_Off
/if (${do_AutoMez}) {
	/docommand ${ChatToggle} I will no longer mez targets.
	/varset do_AutoMez FALSE
}
/return
#event AutoMez_On "<#*#> Start AutoMez"
#event AutoMez_On "<#*#> AutoMez On"
sub event_AutoMez_On
/if (!${do_AutoMez}) {
	/docommand ${ChatToggle} Attempting to mez adds.
	/varset do_AutoMez TRUE
}
/return