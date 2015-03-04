|----------------------------------------------------------------------------|
|- e3_Classes_Cleric.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Cleric specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



SUB check_Yaulp
/if (${Debug}) /echo |- check_clrYaulp ==>
	
	/if (${AutoYaulp} && !${medBreak})  {	

		/declare castName string local ${yaulpSpell.Arg[1,/]}

		| Auto-yaulp spell
		/if (!${Bool[${Me.Buff[${castName}]}]} && ${Me.PctMana} < 95) {
			/if (${Target.ID}) /declare tempTarget int local ${Target.ID}
			/squelch /target clear
			/call e3_Cast 0 "${yaulpSpell}/giveuptimer|${If[${Me.Gem[${castName}]} || ${FindItemCount[=${castName}]},0,80]}"
			/if (${tempTarget}) /call TrueTarget ${tempTarget}
		}
	}
	
/if (${Debug}) /echo <== check_clrYaulp -|
/RETURN



sub check_DivineArb
/if (${Debug}) /echo |- check_DivineArb ==>

	/if (${DivineArb_Pct} && ${Group}>1) {
		/declare i int local

		/for i 0 to ${Group}
		
			/if (${SpawnCount[pc ${Group.Member[${i}]}]}) {
				
				/if (${Bool[${NetBots[${Group.Member[${i}]}]}]} && ${NetBots[${Group.Member[${i}]}].PctHPs} < ${DivineArb_Pct} || ${Spawn[pc ${Group.Member[${i}]}].PctHPs} < ${DivineArb_Pct}) {
					
					/if (${NetBots[${Group.Member[${i}]}].PctHPs}<${DivineArb_Pct}) {
						/if (${FindItemCount[Ancient Shield of the Divine]} && !${FindItem[Ancient Shield of the Divine].Timer} && !${itemTimer_${FindItem[=Ancient Shield of the Divine].ID}}) {
							/call e3_Cast ${Me.ID} "Ancient Shield of the Divine"
						} else /if (${Me.AltAbilityReady[Divine Arbitration]}) {
							/call e3_Cast ${Me.ID} "Divine Arbitration"
						}
					}
				}
			}
			
		/if (!${ActionTaken}) /next i
	}

/if (${Debug}) /echo <== check_DivineArb -|
/return



sub check_celestialRegen
/if (${Debug}) /echo |- check_celestialRegen ==>

		/if (${celestialRegen_Pct} && ${Group}>2 && ${Me.AltAbilityReady[Celestial Regeneration]}) {
		/declare groupSize int local
		/declare averageHP int local
		/declare i int local
		
		| Count group members in zone
		/for i 0 to ${Group}
			
			/if (${SpawnCount[pc ${Group.Member[${i}]}]}) {
				/varcalc averageHP ${averageHP} + ${If[${NetBots[${Group.Member[${i}]}].ID},${NetBots[${Group.Member[${i}]}].PctHPs},${Group.Member[${i}].PctHPs}]}
				/varcalc groupSize ${groupSize} + 1
			}
			
		/next i
		
		/if (${groupSize}) {
			/varcalc averageHP ${averageHP} / ${groupSize}
			/if (${averageHP} < ${celestialRegen_Pct}) /call e3_Cast ${Me.ID} "Celestial Regeneration"
		}
	}

/if (${Debug}) /echo <== check_celestialRegen -|
/return



#event Rez "<#1#> Rez"
#event Rez "<#1#> Rezz"
#event Rez "<#1#> Rez me"
#event Rez "<#1#> Rezz me"
#event Rez "<#*#> RezOn #1#"
#event Rez "<#*#> Rez On #1#"
#event Rez "#1# tells you, 'Rez'"
#event Rez "#1# tells you, 'Rezz'"
#event Rez "#1# tells you, 'Rez me'"
#event Rez "#1# tells you, 'Rezz me'"
#event Rez "#*# tells you, 'RezOn #1#'"
#event Rez "#*# tells you, 'Rez On #1#'"
#event Rez "#1# tells the group, 'Rez'"
#event Rez "#1# tells the group, 'Rezz'"
#event Rez "#1# tells the group, 'Rez me'"
#event Rez "#1# tells the group, 'Rezz me'"
#event Rez "#*# tells the group, 'RezOn #1#'"
#event Rez "#*# tells the group, 'Rez On #1#'"
sub Event_Rez(string line, string ChatSender)
/if (${Me.Class.ShortName.Equal[CLR]}) {
	
	/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
	/if ( ${Me.Class.ShortName.Equal[CLR]} && ${Spawn[${ChatSender}'s].ID}) {
		/tell ${ChatSender} Wait4Rez
		/if (${Select[${Me.Race},High Elf]}) /em pops a toot!
	 
		/if (${FindItem[Water Sprinkler of Nem Ankh].ID}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Water Sprinkler of Nem Ankh"
		} else /if (${Me.Book[Reviviscence]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Reviviscence/giveuptimer|45s"
		} else /if (${Me.Book[Ressurection]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Resurrection/giveuptimer|45s"
		} else /if (${Me.Book[Restoration]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Restoration/giveuptimer|45s"
		} else /if (${Me.Book[Resuscitate]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Resuscitate/giveuptimer|45s"
		} else /if (${Me.Book[Renewal]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Renewal/giveuptimer|45s"
		} else /if (${Me.Book[Revive]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Revive/giveuptimer|45s"
		} else /if (${Me.Book[Reparation]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Reparation/giveuptimer|45s"
		} else /if (${Me.Book[Reconstitution]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Reconstitution/giveuptimer|45s"
		} else /if (${Me.Book[Reanimation]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Reanimation/giveuptimer|45s"
		}
	} else /if (!${Spawn[${ChatSender}'s].ID}) {
		/tell ${ChatSender} I don't see any of your corpses in ${Zone}.
	}
}
/return



#EVENT AE_Rez "[MQ2] AE Rez"
#EVENT AE_Rez "[MQ2] Rez all"
SUB EVENT_AE_Rez
/if (${Select[${Me.Class},Cleric,Paladin]}) {
/if (${Debug} || ${Debug_Basics}) /echo |- EVENT_AE_Rez ==>
	
	/if (!${SpawnCount['s radius 100]}) {
		/echo I couldn't summon any corpses to ressurect...
	} else {
	
		/declare playerName string local
		/declare corpseID int local
		/declare miscTimer timer local
		/declare lastChecked int outer
		/declare rezzable string outer
		/declare i int local
		
		/for i 1 to ${SpawnCount['s radius 100]}
		
			/varset playerName ${NearestSpawn[${i},'s radius 100].CleanName}
			/varset playerName ${playerName.Arg[1,']}
			/varset corpseID ${NearestSpawn[${i},'s radius 100].ID}
			/varset miscTimer 30
			/varset rezzable UNKNOWN
			
			/if (!${playerName.Find[_]}) {
			
				| Check if the corpse is rezzable
				/call TrueTarget ${corpseID}
				/consider
				
				:waitforEvent
				/delay 5
				/doevents CorpseExpired
				/if (${lastChecked} != ${corpseID} && ${miscTimer}) /goto :waitforEvent
				
				/if (!${Bool[${rezzable}]}) {
					
					/if (${rezzable.Equal[UNKNOWN]}) {
						/say I could not determine whether [${Spawn[id ${corpseID}].CleanName}] is rezzable.  Attempting to rez.
						/goto :retry
					} else {
						/say I cannot ressurect [${Spawn[id ${corpseID}].CleanName}] at this time.
					}
				} else {
				
					:retry
					/if (${Target.Distance} > 35) {
						/if (${Select[${playerName},${consentTargets}]}) {
							/if (${miscTimer}) {
								/corpse 
								/delay 10 ${Target.Distance} > 10
								/goto :retry
							} else {
								/echo I failed to pull [${Spawn[id ${corpseID}].CleanName}],  skipping rez.
							}
						} else {
							/tell ${playerName} You're too far away to rez, and I don't have concent to move your corpse.  Please consent me, then tell me 'rez'.
						}
					} else {

						/tell ${playerName} Wait4Rez

						/if (${FindItem[Water Sprinkler of Nem Ankh].ID}) {
							/call e3_Cast ${corpseID} "Water Sprinkler of Nem Ankh" item
						} else /if (${Me.Book[Reviviscence]}) {
							/call e3_Cast ${corpseID} "Reviviscence" Gem${DefaultGem} 45s
						} else /if (${Me.Book[Ressurection]}) {
							/call e3_Cast ${corpseID} "Ressurection" Gem${DefaultGem} 45s
						} else /if (${Me.Book[Restoration]}) {
							/call e3_Cast ${corpseID} "Restoration" Gem${DefaultGem} 45s
						} else /if (${Me.Book[Resuscitate]}) {
							/call e3_Cast ${corpseID} "Resuscitate" Gem${DefaultGem} 45s
						} else /if (${Me.Book[Renewal]}) {
							/call e3_Cast ${corpseID} "Renewal" Gem${DefaultGem} 45s
						} else /if (${Me.Book[Revive]}) {
							/call e3_Cast ${corpseID} "Revive" Gem${DefaultGem} 45s
						} else /if (${Me.Book[Reparation]}) {
							/call e3_Cast ${corpseID} "Reparation" Gem${DefaultGem} 45s
						} else /if (${Me.Book[Reconstitution]}) {
							/call e3_Cast ${corpseID} "Reconstitution" Gem${DefaultGem} 45s
						} else /if (${Me.Book[Reanimation]}) {
							/call e3_Cast ${corpseID} "Reanimation" Gem${DefaultGem} 45s
						}
					}
				}
			}
		
		/next i
		
		/deletevar lastChecked
		/deletevar rezzable
		
		/docommand ${ChatToggle} All viable corpses nearby, have been ressurected.
	}

/if (${Debug} || ${Debug_Basics}) /echo <== EVENT_AE_Rez -|
}
/RETURN



#event CorpseExpired "This corpse #1# be resurrected."
Sub Event_CorpseExpired(line, corpseExpired)
/if (${Defined[lastChecked]}) {
	/if (${Debug}) /echo |- EVENT_CorpseExpired ==>

		/varset lastChecked ${Target.ID}
		/if (${corpseExpired.Equal[cannot]}) {
			/varset rezzable FALSE
		} else {
			/varset rezzable TRUE
		}

	/if (${Debug}) /echo <== EVENT_CorpseExpire -|
}
/return



Sub CLR_Setup
/if (${Debug}) /echo |- CLR_Setup ==>

	/call InitoVar "${Character_Ini},Cleric,Divine Arbitration (Pct)" DivineArb_Pct int outer
	/call InitoVar "${Character_Ini},Cleric,Celestial Regeneration (Pct)" celestialRegen_Pct int outer
	/call InitoVar "${Character_Ini},Cleric,Auto-Yaulp (On/Off)" AutoYaulp bool outer
	/call InitoVar "${Character_Ini},Cleric,Yaulp Spell" yaulpSpell string outer
	/if (${AutoYaulp}) {
		/if (!${Defined[yaulpSpell]} || !${yaulpSpell.Length} || !${Me.Book[${yaulpSpell.Arg[1,/]}]}) {
			/echo ERROR! ${Character_Ini} > [Cleric] > Yaulp Spell is not correctly defined.  Please check this entry, and restart the macro.
			/varset AutoYaulp FALSE
		}
	}

/if (${Debug}) /echo <== CLR_Setup -|
/return



Sub CLR_Background_Events
/return



SUB CLR_MacroSettings
/if (${Debug}) /echo |- CLR_MacroSettings ==>
	
	| Adv Settings
	/call WriteToIni "${advSettings_Ini},CLR Functions,CLR Function#1" "check_DivineArb" 1
	/call WriteToIni "${advSettings_Ini},CLR Functions,CLR Function#2" "check_Yaulp" 1
	/call WriteToIni "${advSettings_Ini},CLR Functions,CLR Function#3" "check_celestialRegen" 1
	/call WriteToIni "${advSettings_Ini},CLR Functions,CLR Function#4" "check_healChain" 1
	/call WriteToIni "${advSettings_Ini},CLR Functions,CLR Function#5" "check_Heals" 1
	/call WriteToIni "${advSettings_Ini},CLR Functions,CLR Function#6" "check_CombatBuffs" 1
	/call WriteToIni "${advSettings_Ini},CLR Functions,CLR Function#7" "check_Debuffs" 1
	/call WriteToIni "${advSettings_Ini},CLR Functions,CLR Function#8" "check_Nukes" 1
	/call WriteToIni "${advSettings_Ini},CLR Functions,CLR Function#9" "check_Buffs" 1

/if (${Debug}) /echo <== CLR_MacroSettings -|
/RETURN



SUB CLR_CharacterSettings
/if (${Debug}) /echo |- CLR_CharacterSettings ==>

	/call WriteToIni "${Character_Ini},Cleric,Divine Arbitration (Pct)" 30 0
	/call WriteToIni "${Character_Ini},Cleric,Celestial Regeneration (Pct)" 65 0
	/call WriteToIni "${Character_Ini},Cleric,Auto-Yaulp (On/Off)" On 0
	/call WriteToIni "${Character_Ini},Cleric,Yaulp Spell"
	/if (${Ini[${Character_Ini},Cleric,Yaulp#1].Length}) /call WriteToIni "${Character_Ini},Cleric,Yaulp#1" "*DEPRECATED*  This entry can be deleted'" 1

/if (${Debug}) /echo <== CLR_CharacterSettings -|
/RETURN



Sub CLR_Aliases
/if (${Debug}) /echo |- CLR_Aliases ==>

	/noparse /squelch /alias /RezIt /bc Rez on ${Target.CleanName.Arg[1,']}
	/squelch /alias /AERez /echo AE Rez

/if (${Debug}) /echo <== CLR_Aliases -|
/return