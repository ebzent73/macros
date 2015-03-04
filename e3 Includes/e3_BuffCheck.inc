|----------------------------------------------------------------------------|
|- e3_Macro_buffCheck.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|----------------------------------------------------------------------------|
|- Contains buffCheck functions for Spells, AAs, Items, and Discs.			-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------------------|
|- buffCheck's mainloop hook.													-|
|--------------------------------------------------------------------------------|
|- Calls buff functions, returning to the mainloop whenever a spell is cast.	-|
|--------------------------------------------------------------------------------|
SUB check_Buffs
/if (${Debug} || ${Debug_BuffCheck}) /echo |- buffCheck ==>

	/if (${BuffCheck} && !${combatTimer} && !${activeTimer} && !${rebuffTimer}) {

		/if (${Cursor.ID} || ${Corpse.Open}) {
			/varset rebuffTimer 100
		} else {
		
			/if (!${Me.Combat} && !${Assisting} && !${Me.Feigning} && !${Me.Moving} && ${Me.PctHPs} > ${min_BuffCheckHealth}) {

				| SelfBuffs.
				/if (${SelfBuffs.Size} && ${Me.FreeBuffSlots} > ${MaxBuffs}) /call buffSelf

				/if (!${ActionTaken}) {
					| BotBuffs.
					/if (${Ini[${Character_Ini},Buffs,Bot Buff#1].Length}) /call buffBots "BotBuffs"
				}
			}
		}
	}

/if (${Debug} || ${Debug_BuffCheck}) /echo <== buffCheck -|
/RETURN

|** Check Illusion.
/declare i int local
/declare illusionRace string local

/for i 0 to 29
	
	/if (${Bool[${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip}]} && ${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip.Find[Illusion:]} && !${Select[${Me.Race},Invisible Man]}) {
		/varset illusionRace ${Me.Buff[${Math.Calc[${i}+1]}]}
		/varset illusionRace ${illusionRace.Arg[2,:].Right[${illusionRace.Arg[2,:].Length}-1]}
		/if (!${Select[${Me.Race},${illusionRace}]}) /call EVENT_RemoveBuff NULL ${Me.CleanName} "Illusion"
	}

/next i
**|

SUB check_CombatBuffs
/if (${Debug} || ${Debug_BuffCheck}) /echo |- check_CombatBuffs ==>

	/if (${BuffCheck} && !${idle} && ${CombatBuffs.Size}) /call buffBots "CombatBuffs"
	
/if (${Debug} || ${Debug_BuffCheck}) /echo <== check_CombatBuffs -|
/RETURN



|------------------------------------------------|
|- Maintain buffs that have instant cast times.	-|
|------------------------------------------------|
SUB buffInstants
/if (${Debug} || ${Debug_BuffCheck}) /echo |- buffInstants ==>

	| -Declare local variables.
	/declare castName string local
	/declare buffName string local
	/declare gemNumber string local ${DefaultGem}
	/declare castTime float local
	
	/declare i int local
	
	/for i 1 to ${InstantBuffs.Size}
	
		/varset castName ${InstantBuffs[${i}].Arg[1,/]}
		/varset buffName ${If[${Me.Book[${castName}]} || ${Me.AltAbility[${castName}]},${castName},${FindItem[${castName}].Spell}]}
		/varset castTime ${If[${Me.Book[${castName}]} || ${Me.AltAbility[${castName}]},${Spell[${castName}].CastTime},${FindItem[${castName}].CastTime}]}
		
		/if (${Debug} || ${Debug_BuffCheck}) /echo castName=${castName} || buffName=${buffName} || castTime=${castTime}
		
		| /BuffName Check.
		/if (${InstantBuffs[${i}].Find[/buffname|]}) {
			/call argueString buffname "${InstantBuffs[${i}]}"
			/varset buffName ${Macro.Return}
		}
		
		| /BuffName Check.
		/if (${InstantBuffs[${i}].Find[/CheckFor|]}) {
			/call argueString buffname "${InstantBuffs[${i}]}"
			/varset buffName ${Macro.Return}
		}
		
		| Check badSpells list
		/if (${Select[${buffName},${badSpells}]}) {
			/if (${Debug} || ${Debug_BuffCheck}) /echo [${castName}] has been listed as a badSpell.  You must zone before the macro will attempt to cast [${castName}] again. 
		} else {
		
			| Check if the buff stacks
			/if (!${Spell[${buffName}].Stacks}) {
				/if (${Debug} || ${Debug_BuffCheck}) /echo [${castName}] will not stack, skipping.
			} else {
			
				| Check buff duration
				/if (${Me.Buff[${buffName}].Duration} > 10) {
					/if (${Debug} || ${Debug_BuffCheck}) /echo [${Me.Buff[${buffName}].Duration}] ticks left on [${buffName}(${castName})].
				} else {
					
					| Check spell castTime
					/if (${castTime} > .5) {
						/docommand ${ChatToggle} Instant Buffs must have a cast time of .5 or less.  Removing [${InstantBuffs[${i}]}] from my Instant Buff list.  Please review this entry in [${Character_Ini}].
						/call RemoveArrayElement InstantBuffs "${InstantBuffs[${i}]}"
						/beep
					} else {
					
						| Attempt to cast the buff
						/call e3_Cast ${If[${Spell[${buffName}].TargetType.Equal[self]},0,${Me.ID}]} "${InstantBuffs[${i}]}/Instant"
					}
				}
			}
		}

	/if (!${ActionTaken}) /next i
	
/if (${Debug} || ${Debug_BuffCheck}) /echo <== buffInstants -|
/RETURN



|------------------------------------------------|
|- Cast and maintain buffs on this character.	-|
|------------------------------------------------|
SUB buffSelf
/if (${Debug} || ${Debug_BuffCheck}) /echo |- buffSelf ==>

	| -Declare local variables.
	/declare castName string local
	/declare buffName string local
	/declare gemNumber int local ${DefaultGem}

	/declare i int local
	
	/for i 1 to ${SelfBuffs.Size}
	
		/varset castName ${SelfBuffs[${i}].Arg[1,/]}
		/varset buffName ${If[${Me.Book[${castName}]} || ${Me.AltAbility[${castName}]},${castName},${FindItem[${castName}].Spell}]}
		
		|  /BuffName Check.
		/if (${SelfBuffs[${i}].Find[/buffname|]}) {
			/call argueString buffname "${SelfBuffs[${i}]}"
			/varset buffName ${Macro.Return}
		}
		
		|  /BuffName Check.
		/if (${SelfBuffs[${i}].Find[/CheckFor|]}) {
			/call argueString CheckFor "${SelfBuffs[${i}]}"
			/varset buffName ${Macro.Return}
		}
	
		| Check badSpells list
		/if (${Select[${buffName},${badSpells}]}) {
			/if (${Debug} || ${Debug_BuffCheck}) /echo [${castName}] has been listed as a badSpell.  You must zone before the macro will attempt to cast [${castName}] again. 
		} else {
		
			| Check if the buff stacks
			/if (!${Spell[${buffName}].Stacks}) {
				/if (${Debug} || ${Debug_BuffCheck}) /echo [${castName}] will not stack, skipping.
			} else {
			
				| Check buff duration
				/if (${Me.Buff[${buffName}].Duration} > 10) {
					/if (${Debug} || ${Debug_BuffCheck}) /echo [${Me.Buff[${buffName}].Duration}] ticks left on [${buffName}(${castName})].
				} else {
					
					| Cast the buff
					/call e3_Cast ${If[${Spell[${buffName}].TargetType.Equal[self]},0,${Me.ID}]} "${SelfBuffs[${i}]}/GiveUpTimer|${If[!${Me.Book[${castName}]},0,${If[${Me.Gem[${castName}]},20,90]}]}"
				}
			}
		}
	
	/if (!${ActionTaken}) /next i
	
/if (${Debug} || ${Debug_BuffCheck}) /echo <== buffSelf -|
/RETURN



|----------------------------------------|
|- Cast and maintain buffs on a NetBot.	-|
|----------------------------------------|
SUB buffBots(arrayName)
/if (${Debug} || ${Debug_BuffCheck}) /echo |- buffBots [${arrayName}] ==>
	
	| -Declare local variables.
	/declare buffTarget string local
	/declare castName string local
	/declare buffName string local

	/declare i int local
	
	/for i 1 to ${${arrayName}.Size}
	
		/varset castName ${${arrayName}[${i}].Arg[1,/]}
		/varset buffTarget ${${arrayName}[${i}].Arg[2,/]}
		/varset buffName ${If[${FindItemCount[${castName}]},${FindItem[${castName}].Spell},${castName}]}
	
		| -If the spawn is in zone.
		/if (${SpawnCount[pc =${buffTarget}]} && ${NetBots[${buffTarget}].Zone} == ${Zone.ID}) {
		
			|  /BuffName Check.
			/if (${${arrayName}[${i}].Find[/buffname|]}) {
				/call argueString buffname "${arrayName}"
				/varset buffName ${Macro.Return}
			}
			
			|  /BuffName Check.
			/if (${${arrayName}[${i}].Find[/CheckFor|]}) {
				/call argueString CheckFor "${arrayName}"
				/varset buffName ${Macro.Return}
			}
		
			| -Check for group only buffs.
			/if (${${arrayName}[${i}].Find[/group]} && !${Group.Member[${buffTarget}]}) {
				/if (${Debug} || ${Debug_BuffCheck}) /echo [${buffTarget}] is not in my group, skipping [${BotBuffs[${i}]}].
				/goto :skipBuff
			}
			
			| Check badSpells list
			/if (${Select[${buffName},${badSpells}]}) {
				/if (${Debug} || ${Debug_BuffCheck}) /echo [${castName}] has been listed as a badSpell.  You must zone before the macro will attempt to cast [${castName}] again. 
			} else {
	
				| Check netbot buffs
				/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${buffName}].ID}]}) {
					
					| Check range
					/if (${Spawn[pc =${buffTarget}].Distance} < ${Spell[${buffName}].MyRange} || ${Spawn[pc =${buffTarget}].Distance} < ${Spell[${buffName}].AERange}) {
						
						| Cast the buff
						/call e3_Cast ${Spawn[pc =${buffTarget}].ID} "${${arrayName}[${i}]}/GiveUpTimer|${If[!${Me.Book[${castName}]},0,${If[${Me.Gem[${castName}]},20,90]}]}" 
						
						| If the spell did not take hold
						/if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[NOHOLD]}) /call createTimer "noHoldTimer_${Target.ID}-${lastCast.Replace[ ,]}" ${Math.Calc[${Spell[${buffName}].Duration} * 60 / 2].Int}
					} else {
						/if (${Debug} || ${Debug_BuffCheck}) /echo [${buffTarget}] is out of range.
					}
				} else {
					/if (${Debug} || ${Debug_BuffCheck}) /echo [${buffTarget}] already has [${buffName}].
				}
			}
		} else {
			/if (${Debug} || ${Debug_BuffCheck}) /echo [${buffTarget}] is not in zone.
		}

	:skipBuff
	/if (!${ActionTaken}) /next i
	
/if (${Debug} || ${Debug_BuffCheck}) /echo <== buffBots -|
/RETURN



|--------------------------------------------------------|
|- Casts GroupBuffs array on the specified buffTarget.	-|
|--------------------------------------------------------|
|- @param line: '<Name> Buff me'.						-|
|- @param ChatSender: The buff requestor.				-|
|--------------------------------------------------------|
#event castGroupBuffs "<#1#> Buff #*#"
SUB event_castGroupBuffs(line, ChatSender)
/if (${Debug} || ${Debug_BuffCheck}) /echo |- event_castGroupBuffs ==>
	
	/if (${GroupBuffs.Size}) {
		| -Verify the event
		/call verifyEvent "${ChatSender}" "${line}"
		/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
		|    |--------Included Bot-----------|
		/if (${Bool[${Macro.Return.Arg[3,_]}]} && ${line.Arg[3, ].NotEqual[all]}) {
		
			| -------------------Find buffTarget------------------- |
			/declare buffTarget int local
			/declare castName string local
			/declare gemNumber string local gem${DefaultGem}
			/declare groupList string local
			
			/declare i int local
			/declare x int local
			
			/if (${Int[${line.Arg[3, ]}]}) {
				/varset buffTarget ${line.Arg[3, ]}
			} else /if (${line.Find[Buff me]}) {
				/varset buffTarget ${Spawn[pc ${ChatSender}].ID}
			} else /if (${line.Find[Buff pet]} || ${line.Find[Buff my pet]}) {
				/varset buffTarget ${Spawn[pet ${Spawn[pc ${ChatSender}].Pet}].ID}
			} else  /if (${SpawnCount[pc ${line.Arg[3, ]}]}) {
				/varset buffTarget ${Spawn[pc ${line.Arg[3, ]}].ID}
			} else /if (${line.Find['s pet]}) {
				/varset buffTarget ${Spawn[pet ${Spawn[pc ${line.Arg[3, ].Arg[1,']}].Pet}].ID}
			} else /if (${line.Find[s pet]}) {
				/varset buffTarget ${Spawn[pet ${Spawn[pc ${line.Arg[3, ].Left[${Math.Calc[${line.Arg[3, ].Length} - 1].Int}]}].Pet}].ID}
			}
			
			| -------------------Verify the buffTarget------------------- |
			/if (!${buffTarget}) {
				/echo Could not find a matching SpawnID.
			} else {
		
				/if (!${SpawnCount[id ${buffTarget} radius 200]}) {
					/bc [${Spawn[id ${buffTarget}].CleanName}] is not in range.
				} else {
					
					/for i 1 to ${GroupBuffs.Size}
					
						/call e3_Cast ${buffTarget} "${GroupBuffs[${i}]}/giveuptimer|45s"	
					
					/next i
					
					/docommand ${ChatToggle} Done buffing ${Spawn[id ${buffTarget}].CleanName}.
				}
			}
		}
	}

/if (${Debug} || ${Debug_BuffCheck}) /echo <== castGroupBuffs -|
/RETURN



|----------------------------------------------------|
|- Command group leaders to request group buffs.	-|
|----------------------------------------------------|
#event BuffAll "<#*#> Buff All"
SUB event_BuffAll
/if (${Debug} || ${Debug_BuffCheck}) /echo |- EVENT_BuffAll ==>

	/if (${Select[${Group.Leader},${Me}]}) /bc Buff me

/if (${Debug} || ${Debug_BuffCheck}) /echo <== EVENT_BuffAll -|
/RETURN



#EVENT toggleBuffs "[#1#] Toggle Buffs #2#"
#EVENT toggleBuffs "<#1#> Toggle Buffs #2#"
#EVENT toggleBuffs "#1# tells you, 'Toggle Buffs #2#'"
#EVENT toggleBuffs "#1# tell the group, 'Toggle Buffs #2#'"
#EVENT toggleBuffs "#1# tells the group, 'Toggle Buffs #2#'"
SUB EVENT_toggleBuffs(line, ChatSender, buffSetting)
/if (${Debug} || ${Debug_BuffCheck}) /echo |- EVENT_toggleBuffs ==>

	/if (${buffSetting.Equal[on]}) {
		/varset buffSetting TRUE
	} else /if (${buffSetting.Equal[off]}) {
		/varset buffSetting FALSE
	}
	
	/varset BuffCheck ${buffSetting}
	
	/docommand ${ChatToggle} BuffCheck [${If[${BuffCheck},ON,OFF]}]

/if (${Debug} || ${Debug_BuffCheck}) /echo <== EVENT_toggleBuffs -|
/RETURN



Sub buffCheck_Setup
/call IniToVar "${advSettings_Ini},Debug,Debug BuffCheck (On/Off)" Debug_BuffCheck bool outer
/if (${Debug} || ${Debug_BuffCheck}) /echo |- buffCheck_Setup ==>
	
	| -Adjust MaxBuffs to account for the Mystical Attuning AAs.
	/declare MaxBuffs int outer 0
	/if (${Me.AltAbility[Mystical Attuning]}) /varcalc MaxBuffs ${MaxBuffs} - (${Me.AltAbility[Mystical Attuning]} / 5 - 1)

	/declare BuffCheck bool outer FALSE
	/declare min_BuffCheckHealth int outer 70
	/declare buffRecheck_Delay int outer 100

	/if (${Ini[${Character_Ini},Buffs,Instant Buff#1].Length}) /call IniToArray "${Character_Ini},Buffs,Instant Buff#" InstantBuffs
	/if (${Ini[${Character_Ini},Buffs,Self Buff#1].Length}) /call IniToArray "${Character_Ini},Buffs,Self Buff#" SelfBuffs
	/if (${Ini[${Character_Ini},Buffs,Group Buff#1].Length}) /call IniToArray "${Character_Ini},Buffs,Group Buff#" GroupBuffs
	/if (${Ini[${Character_Ini},Buffs,Bot Buff#1].Length}) /call IniToArray "${Character_Ini},Buffs,Bot Buff#" BotBuffs
	/if (${Ini[${Character_Ini},Buffs,Combat Buff#1].Length}) /call IniToArray "${Character_Ini},Buffs,Combat Buff#" CombatBuffs
	
	/if (${InstantBuffs.Size} || ${SelfBuffs.Size} || ${BotBuffs.Size} || ${CombatBuffs.Size}) /varset BuffCheck TRUE
	
/if (${Debug} || ${Debug_BuffCheck}) /echo <== buffCheck_Setup -|
/return



Sub buffCheck_Background_Events
/return



SUB buffCheck_MacroSettings
/if (${Debug} || ${Debug_BuffCheck}) /echo |- buffCheck_MacroSettings ==>

	/call WriteToIni "${advSettings_Ini},Debug,Debug BuffCheck (On/Off)"

/if (${Debug} || ${Debug_BuffCheck}) /echo <== buffCheck_MacroSettings -|
/RETURN



SUB buffCheck_CharacterSettings
/if (${Debug} || ${Debug_BuffCheck}) /echo |- buffCheck_CharacterSettings ==>

	/call WriteToIni "${Character_Ini},Buffs,Instant Buff#1"
	/call WriteToIni "${Character_Ini},Buffs,Self Buff#1"
	/call WriteToIni "${Character_Ini},Buffs,Bot Buff#1"
	/call WriteToIni "${Character_Ini},Buffs,Combat Buff#1"
	/call WriteToIni "${Character_Ini},Buffs,Group Buff#1"

/if (${Debug} || ${Debug_BuffCheck}) /echo <== buffCheck_CharacterSettings -|
/RETURN



Sub buffCheck_Aliases
/if (${Debug} || ${Debug_BuffCheck}) /echo |- buffCheck_Aliases ==>

	/squelch /alias /Buff /bc Buff me
	/squelch /alias /BuffMe /bc Buff me
	/squelch /alias /Buffs /bc Buff me
	/noparse /squelch /alias /BuffT /bc Buff ${Target.ID}
	/noparse /squelch /alias /BuffIt /bc Buff ${Target.ID}
	/squelch /alias /BuffAll /bc Buff All
	/squelch /alias /BuffGroups /bc Buff All
	/noparse /squelch /alias /BuffPet /bc Buff ${Spawn[pet ${Spawn[id ${Target.ID}].Pet}].ID}
	/squelch /alias /BuffCheck /bc Toggle Buffs

/if (${Debug} || ${Debug_BuffCheck}) /echo <== buffCheck_Aliases -|
/return