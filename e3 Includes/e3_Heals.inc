|------------------------------------------------------------------------|
|- e3_Macro_Heals.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains heal related functions for e3.mac							-|
|------------------------------------------------------------------------|

| add group heals
| add heals for non netbot group members
| add an event to create an array of non netbot heal targets.
| Add a list for tank only heals?

|------------------------------------------------------------|
|- Heals.inc's mainLoop hook.								-|	
|------------------------------------------------------------|					
|- Calls heal functions, as an extention of the main loop.	-|	
|------------------------------------------------------------|
SUB check_Heals
/if (${Debug} || ${Debug_Heals}) /echo |- healLoop  ==>

	| Check Group Heal array
	/if (${do_GroupHeals}) /call check_groupHeals
	/if (!${ActionTaken}) {
		
		| Check healFunctions array.
		/if (${do_Heals}) /call check_healSpells
		/if (!${ActionTaken}) {
		
			| Check HoT.
			/if (${do_HoTs}) /call check_HoTs
		}
	}
	
	| If I've healed someone in the last few seconds, set ActionTaken TRUE.  
	| This creates a stopping point in the mainLoop which prioritizes heals, and all higher functions, for a brief time.
	/if (${recentHeal_Timer}) /varset ActionTaken TRUE

/if (${Debug} || ${Debug_Heals}) /echo <== healLoop -|
/RETURN



|--------------------------------------------------------------------------------------------------------------------|
|- Uses Items, Spells, Abilities, AAs, and Disciplines listed in the [LifeSupport] section of the Character_Ini.	-|
|--------------------------------------------------------------------------------------------------------------------|
SUB check_lifeSupport
/if (${Debug} || ${Debug_Heals}) /echo |- check_lifeSupport ==>

	/declare castName string local
	/declare healPct int local
	
	/declare i int local
	
	/for i 1 to ${lifeSupport.Size}
	
		/varset castName ${lifeSupport[${i}].Arg[1,/]}
		
		| get healPct
		/if (!${lifeSupport[${i}].Find[/healpct|]}) {
			/echo There is no HealPct listed with, [Life Support#${i}=${lifeSupport[${i}]}]; removing it from my Life Support list. Please review this entry, and restart the macro
		} else {
			/call argueString HealPct "${lifeSupport[${i}]}"
			/varset healPct ${Macro.Return}
		}
		
		/if (${Me.PctHPs} && ${Me.PctHPs} < ${healPct}) {
		
			/if (${Me.AbilityReady[${castName}]}) {
				/doability "${castName}"
			} else {
				/if (${Target.ID} && ${Target.ID} != ${Me.ID}) /declare oldTarget int local ${Target.ID} 
				/call e3_Cast ${Me.ID} "${lifeSupport[${i}]}"
			}
		}
		
	:skip
	/next i

	/if (${oldTarget} && ${Target.ID} != ${oldTarget}) /call TrueTarget ${oldTarget}
	
/if (${Debug} || ${Debug_Heals}) /echo <== check_lifeSupport -|
/RETURN



SUB check_healSpells
/if (${Debug} || ${Debug_Heals}) /echo |- check_healSpells ==>	

	/declare healTarget string local
	/declare healSpell_Array string local
	
	/declare healToCheck string local
	/declare healToUse string local

	/declare healPct int local
	/declare healthDifferential int local 100
	
	/declare i int local
	/declare x int local
	
	/for i 1 to ${healFunctions.Size}
		
		/if (${Bool[${healFunctions[${i}]}]}) /call check_${healFunctions[${i}]}
		
		/if (${Bool[${Macro.Return}]}) {
		
			/varset healTarget ${Macro.Return}
			/varset healSpell_Array ${If[${Defined[tankHeals]} && ${Select[${healTarget},${tankList}]},tankHeals,${If[${Defined[patchHeals]},patchHeals,NULL]}]}
			
			/if (!${Bool[${healSpell_Array}]}) {
				/if (${Debug} || ${Debug_Heals}) /echo No healSpell_Arrays defined!
				/if (${Defined[healFunctions]}) /deletevar healFunctions
			} else {
			
				/if (${NetBots[${healTarget}].PctHPs} && ${NetBots[${healTarget}].PctHPs} < 100) {
				
					| this is stupid, and I dont like being forced into using a static array name in the for loops, but when I tried using ${${healSpell_Array}.Size} I got several unexplainable errors.
					/if (${healSpell_Array.Equal[tankHeals]}) {
					
						/for x 1 to ${tankHeals.Size}

							/varset healToCheck ${tankHeals[${x}].Arg[1,/]}
					
							| /HealPct Check
							/if (!${tankHeals[${x}].Find[/HealPct|]}) {
								/echo There is no /HealPct associated with [${tankHeals[${x}]}].  Please check this entry, and restart the macro.
								/call RemoveArrayElement tankHeals "${tankHeals[${x}]}"
							} else {
								/call argueString HealPct "${tankHeals[${x}]}"
								/varset healPct ${Macro.Return}
							}
					
							| Check bot health vs healpct
							/if (${NetBots[${healTarget}].PctHPs} < ${healPct}) {
							
								| ----------------------------------------- Item -----------------------------------------|
								/if (${FindItemCount[=${healToCheck}]}) {
									
									| Check Timer 
									/if (${FindItem[=${healToCheck}].Timer} || ${itemTimer_${FindItem[=${healToCheck}].ID}}) {
										/if (${Debug} || ${Debug_Heals}) /echo [${healToCheck}] is not ready! [${If[${FindItem[=${healToCheck}].Timer},${FindItem[=${healToCheck}].Timer} ticks,${Math.Calc[${itemTimer_${FindItem[=${healToCheck}].ID}}/10].Int} sec]}] until this item is available.
									} else {
										
										| Check Range 
										/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${FindItem[=${healToCheck}].Spell}].MyRange}) {
											/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
										} else {
											/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
												/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
												/varset healToUse ${tankHeals[${x}]}
											}
										}
									}
									
								| ----------------------------------------- Alt Ability -----------------------------------------|
								} else /if (${Me.AltAbility[${healToCheck}]}) {
									
									| Check Timer 
									/if (!${Me.AltAbilityReady[${healToCheck}]} || ${altTimer_${Me.AltAbility[${nukeName}].ID}}) {
										/if (${Debug} || ${Debug_Heals}) /echo [${healToCheck}] wont be ready for another [${altTimer_${Me.AltAbility[${healToCheck}].ID}}] seconds!
									} else {
										
										| Check Range 
										/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${Me.AltAbility[${healToCheck}].Spell}].MyRange}) {
											/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
										} else {
											/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
												/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
												/varset healToUse ${tankHeals[${x}]}
											}
										}
									}
									
								| ----------------------------------------- Spell -----------------------------------------|	
								} else /if (${Me.Book[${healToCheck}]}) {
									
									| Check mana 
									/if (${Me.CurrentMana[${healToCheck}]} < ${Spell[${healToCheck}].Mana}) {
										/if (${Debug} || ${Debug_Heals}) /echo I don't have enough mana to cast [${healToCheck}].
									} else {
										
										| Check Range 
										/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${healToCheck}].MyRange}) {
											/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
										} else {
											/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
												/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
												/varset healToUse ${tankHeals[${x}]}
											}
										}
									}
								}
							}
					
						/next x
						
					} else {
						
						/for x 1 to ${patchHeals.Size}
							
							/varset healToCheck ${patchHeals[${x}].Arg[1,/]}
					
							| /HealPct Check
							/if (!${patchHeals[${x}].Find[/HealPct|]}) {
								/echo There is no /HealPct associated with [${patchHeals[${x}]}].  Please check this entry, and restart the macro.
								/call RemoveArrayElement patchHeals "${patchHeals[${x}]}"
							} else {
								/call argueString HealPct "${patchHeals[${x}]}"
								/varset healPct ${Macro.Return}
							}
					
							| Check bot health vs healpct
							/if (${NetBots[${healTarget}].PctHPs} < ${healPct}) {
							
								| ----------------------------------------- Item -----------------------------------------|
								/if (${FindItemCount[=${healToCheck}]}) {
									
									| Check Timer 
									/if (${FindItem[=${healToCheck}].Timer} || ${itemTimer_${FindItem[=${healToCheck}].ID}}) {
										/if (${Debug} || ${Debug_Heals}) /echo [${healToCheck}] is not ready! [${If[${FindItem[=${healToCheck}].Timer},${FindItem[=${healToCheck}].Timer} ticks,${Math.Calc[${itemTimer_${FindItem[=${healToCheck}].ID}}/10].Int} sec]}] until this item is available.
									} else {
										
										| Check Range 
										/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${FindItem[=${healToCheck}].Spell}].MyRange}) {
											/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
										} else {
											/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
												/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
												/varset healToUse ${patchHeals[${x}]}
											}
										}
									}
									
								| ----------------------------------------- Alt Ability -----------------------------------------|
								} else /if (${Me.AltAbility[${healToCheck}]}) {
									
									| Check Timer 
									/if (!${Me.AltAbilityReady[${healToCheck}]} || ${altTimer_${Me.AltAbility[${nukeName}].ID}}) {
										/if (${Debug} || ${Debug_Heals}) /echo [${healToCheck}] wont be ready for another [${altTimer_${Me.AltAbility[${healToCheck}].ID}}] seconds!
									} else {
										
										| Check Range 
										/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${Me.AltAbility[${healToCheck}].Spell}].MyRange}) {
											/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
										} else {
											/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
												/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
												/varset healToUse ${patchHeals[${x}]}
											}
										}
									}
									
								| ----------------------------------------- Spell -----------------------------------------|	
								} else /if (${Me.Book[${healToCheck}]}) {
									
									| Check mana 
									/if (${Me.CurrentMana[${healToCheck}]} < ${Spell[${healToCheck}].Mana}) {
										/if (${Debug} || ${Debug_Heals}) /echo I don't have enough mana to cast [${healToCheck}].
									} else {
										
										| Check Range 
										/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${healToCheck}].MyRange}) {
											/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
										} else {
											/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
												/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
												/varset healToUse ${patchHeals[${x}]}
											}
										}
									}
								}
							}
							
						/next x
		
					}
					
					| If I have a heal to cast, cast it
					/if (${Bool[${healToUse}]}) {
						/if (${Debug} || ${Debug_Heals}) /echo Attempting to cast [${healToUse}] on [${healTarget} ${NetBots[${healTarget}].PctHPs}]
						/call e3_Cast ${NetBots[${healTarget}].ID} "${healToUse}/GiveUpTimer|${If[${Me.Gem[${healToUse}]},0,100]}/SubToRun|Heal_CastingChecks"
						/varset recentHeal_Timer 20
					}
				}
			}
		}
	
	/if (!${ActionTaken}) /next i

/if (${Debug} || ${Debug_Heals}) /echo <== check_healSpells -|
/RETURN



|----
|- check_tanks
|-----
|- Find the tank to be healed, and passes the name to determineHeal function.
|---
SUB check_tanks
/if (${Debug} || ${Debug_Heals}) /echo |- check_tanks ==>

	/declare currentTank string local NULL
	/declare healSpell_Range int local ${If[${Select[${Me.Class.ShortName},CLR,PAL]},100,200]}
	/declare i int local

	/for i 1 to ${tanks.Size}
	
		/if (${NetBots[${tanks[${i}]}].ID} && ${SpawnCount[pc ${tanks[${i}]}]} && ${Spawn[pc ${tanks[${i}]}].Distance} < ${healSpell_Range}) /varset currentTank ${tanks[${i}]}

	/if (!${Bool[${currentTank}]}) /next i
	
/if (${Debug} || ${Debug_Heals}) /echo <== check_tanks -|
/RETURN ${currentTank}



|------------------------------------------------|
|- check_PriorityTargets						-|
|------------------------------------------------|
|- Checks NetBots listed as PriorityTargets.	-|
|------------------------------------------------|
SUB check_importantBots
/if (${Debug} || ${Debug_Heals}) /echo |- check_ImportantBots ==>

	/declare currentTarget string local NULL
	/declare healSpell_Range int local ${If[${Select[${Me.Class.ShortName},CLR,PAL]},100,200]}
	/declare i int local
	
	/for i 1 to ${importantBots.Size}
	
		/if (${SpawnCount[pc ${importantBots[${i}]}]} && ${Spawn[pc ${importantBots[${i}]}].Distance} < ${healSpell_Range}) {
			/if (!${Bool[${currentTarget}]} || ${NetBots[${importantBots[${i}]}].PctHPs} < ${NetBots[${currentTarget}].PctHPs}) /varset currentTarget ${importantBots[${i}]}
		}
	
	/next i

/if (${Debug} || ${Debug_Heals}) /echo <== check_ImportantBots -|
/RETURN ${currentTarget}



|----------------------------------------------------|
|- check_NetBots									-|
|----------------------------------------------------|
|- Checks all NetBots, and heals whoever is lowest.	-|
|----------------------------------------------------|
SUB check_NetBots
/if (${Debug} || ${Debug_Heals}) /echo |- check_NetBots ==>

	/declare currentTarget string local NULL
	/declare healSpell_Range int local ${If[${Select[${Me.Class.ShortName},CLR,PAL]},100,200]}
	/declare i int local
	
	/for i 1 to ${NetBots.Counts}
		/if (${SpawnCount[pc ${NetBots.Client[${i}]}]} && ${Spawn[pc ${NetBots.Client[${i}]}].Distance} < ${healSpell_Range}) {
			/if (!${Bool[${currentTarget}]} || ${NetBots[${NetBots.Client[${i}]}].PctHPs} < ${NetBots[${currentTarget}].PctHPs}) /varset currentTarget ${NetBots.Client[${i}]}
		}
	/next i

/if (${Debug} || ${Debug_Heals}) /echo <== check_NetBots -|
/RETURN ${currentTarget}



|----------------------------------------|
|- Checks to make while casting heals.	-|
|----------------------------------------|
SUB Heal_CastingChecks
/if (${Debug} || ${Debug_Heals}) /echo |- Heal_CastingChecks ==>

	/if (${Debug} && !${debugTimer_Heal_CastingChecks}) {
		/echo [${Target.CleanName}|${NetBots[${Target.CleanName}].PctHPs} >=90
		/call createTimer debugTimer_Heal_CastingChecks 15
	}
	
	/if (${castEndTime}<5 && ${NetBots[${Target.CleanName}].PctHPs}>=90) {
		/call interrupt
		/varset castReturn CANCELLED
	}

/if (${Debug} || ${Debug_Heals}) /echo <== Heal_CastingChecks -|
/RETURN



|----------------|
|- check_HoT	-|
|----------------|
SUB check_HoTs
/if (${Debug} || ${Debug_Heals}) /echo |- check_HoTs ==>	
	
	/declare healTarget string local
	
	/declare healToCheck string local
	/declare healToUse string local
	/if (!${Defined[dontOverwrite]}) /declare dontOverwrite string outer

	/declare healPct int local
	/declare healthDifferential int local 100
	
	/declare bot_BuffList string local
	
	/declare i int local
	/declare x int local
	
	/for x 1 to ${hotFunctions.Size}
	
		/call check_${hotFunctions[${x}]}
	
		/if (!${Bool[${Macro.Return}]}) {
			/if (${Debug} || ${Debug_Heals}) /echo Could not find any targets associated with [${healFunctions[${x}]}].
		} else {
			
			/varset healTarget ${Macro.Return}
			/if (${NetBots[${healTarget}].PctHPs} && ${NetBots[${healTarget}].PctHPs} < 100) {
			
				/for i 1 to ${hotSpells.Size}
	
					/varset healToCheck ${hotSpells[${i}].Arg[1,/]}
					/varset dontOverwrite

					| /HealPct Check
					/if (!${hotSpells[${i}].Find[/HealPct|]}) {
						/echo There is no /HealPct associated with [${hotSpells[${i}]}].  Please check this entry, and restart the macro.
						/call RemoveArrayElement hotSpells "${hotSpells[${i}]}"
					} else {
						/call argueString HealPct "${hotSpells[${i}]}"
						/varset healPct ${Macro.Return}
					}
					
					/if (${NetBots[${healTarget}].PctHPs} < ${healPct}) {
					
						| ----------------------------------------- Item -----------------------------------------|
						/if (${FindItemCount[=${healToCheck}]}) {
							
							| Check Timer 
							/if (${FindItem[=${healToCheck}].Timer} || ${itemTimer_${FindItem[=${healToCheck}].ID}}) {
								/if (${Debug} || ${Debug_Heals}) /echo [${healToCheck}] is not ready! [${If[${FindItem[=${healToCheck}].Timer},${FindItem[=${healToCheck}].Timer} ticks,${Math.Calc[${itemTimer_${FindItem[=${healToCheck}].ID}}/10].Int} sec]}] until this item is available.
							} else {
								
								| Check Range 
								/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${FindItem[=${healToCheck}].Spell}].MyRange}) {
									/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
								} else {
									/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
										/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
										/varset healToUse ${hotSpells[${i}]}
									}
								}
							}
							
						| ----------------------------------------- Alt Ability -----------------------------------------|
						} else /if (${Me.AltAbility[${healToCheck}]}) {
							
							| Check Timer 
							/if (!${Me.AltAbilityReady[${healToCheck}]} || ${altTimer_${Me.AltAbility[${nukeName}].ID}}) {
								/if (${Debug} || ${Debug_Heals}) /echo [${healToCheck}] wont be ready for another [${altTimer_${Me.AltAbility[${healToCheck}].ID}}] seconds!
							} else {
								
								| Check Range 
								/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${Me.AltAbility[${healToCheck}].Spell}].MyRange}) {
									/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
								} else {
									/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
										/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
										/varset healToUse ${hotSpells[${i}]}
									}
								}
							}
							
						| ----------------------------------------- Spell -----------------------------------------|	
						} else /if (${Me.Book[${healToCheck}]}) {
							
							| Check mana 
							/if (${Me.CurrentMana[${healToCheck}]} < ${Spell[${healToCheck}].Mana}) {
								/if (${Debug} || ${Debug_Heals}) /echo I don't have enough mana to cast [${healToCheck}].
							} else {
								
								| Check Range 
								/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${healToCheck}].MyRange}) {
									/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
								} else {
									/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
										/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
										/varset healToUse ${hotSpells[${i}]}
									}
								}
							}
						}
					}
				
				/next i
				
				| cast heal
				/if (${Bool[${healToUse}]}) {
				
					| Create a list of the healTargets Buffs, replacing the spaces with commas to be used in a Select
					/varset bot_BuffList ${NetBots[${healTarget}].ShortBuff.Replace[ ,,]}${NetBots[${healTarget}].Buff.Replace[ ,,]}

					| Use Select to search for the ID if the spell you want to cast, in the list of NetBot buffs.
					/if (${Select[${Spell[${healToUse.Arg[1,/]}].ID},${bot_BuffList}]}) {
						/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] already has [${healToUse.Arg[1,/]}], waiting to cast.
					} else {
					
						| Check for spells you dont want to overwrite
						/if (${healToUse.Find[/DontOverwrite|]}) {
							/call argueString DontOverwrite "${healToUse}"
							/varset dontOverwrite ${Macro.Return}
						}
						
						| Check for spells you dont want to overwrite
						/if (${healToUse.Find[/CheckFor|]}) {
							/call argueString CheckFor "${healToUse}"
							/varset dontOverwrite ${Macro.Return}
						}
						
						| Check for spells you don't want to overwrite
						/if (${Select[${Spell[${dontOverwrite}].ID},${bot_BuffList}]}) {
							/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] already has [${dontOverwrite}], waiting to cast.
						} else {
						
							| Cast the HoT
							/call e3_Cast ${NetBots[${healTarget}].ID} "${healToUse}/GiveUpTimer|${If[${Me.Gem[${healToUse.Arg[1,/]}]},0,100]}/SubToRun|HoT_CastingChecks"
							/varset recentHeal_Timer 30
						}
					}
				}
			}
		}

	/if (!${ActionTaken}) /next x

/if (${Debug} || ${Debug_Heals}) /echo <== check_HoTs -|
/RETURN



|--------------------------------------------|
|- Checks to make while casting HoT spells.	-|
|--------------------------------------------|
SUB HoT_CastingChecks
/if (${Debug} || ${Debug_Heals}) /echo |- HoT_CastingChecks ==>
	
	/if (${Select[${Me.Casting.ID},${NetBots[${Target.CleanName}].ShortBuff.Replace[ ,,]}]} || ${Select[${Me.Casting.ID},${NetBots[${Target.CleanName}].Buff.Replace[ ,,]}]} || ${Select[${Spell[${dontOverwrite}].ID},${NetBots[${Target.CleanName}].ShortBuff.Replace[ ,,]}]} || ${Select[${Spell[${dontOverwrite}].ID},${NetBots[${Target.CleanName}].Buff.Replace[ ,,]}]}) {
		/if (${Debug} || ${Debug_Heals}) /echo [${Target.CleanName}] already has a HoT, canceling
		/call interrupt
		/varset castReturn CANCELLED
	}
	
/if (${Debug} || ${Debug_Heals}) /echo <== HoT_CastingChecks -|
/RETURN



|------------------------------
|- check_GroupHeals
|------------------------------
|- Finds the average health of the healers group when groups have more than 3 members.
|------------------------------
SUB check_groupHeals
/if (${Debug} || ${Debug_Heals}) /echo |- check_Group ==>	

	| Check that I have a group.
	/if (${Group}) {
	
		/declare avgGrpHealth int local
	
		/declare healTarget string local ${Me.CleanName}
	
		/declare healToCheck string local
		/declare healToUse string local
		
		/declare healPct int local
		/declare healthDifferential int local 100
		
		/declare i int local
		/declare x int local
		
		/declare targetCount int local 
			
		| Check range of group members and add their healths together.
		/for i 0 to ${Group}
			/if (${Spawn[${Group.Member[${i}]}].Distance} < 70) {
				/varcalc targetCount ${targetCount} + 1
				/varcalc avgGrpHealth ${avgGrpHealth} + ${NetBots[${Group.Member[${i}]}].PctHPs}
			}
		/next i
		
		| Calculate avgGrpHealth.
		/varcalc avgGrpHealth ${avgGrpHealth} / ${targetCount}
		
		| If Enough group members are in range, check group heals.
		/if (${targetCount} > 2 && ${avgGrpHealth} < 100) {
			
			/for i 1 to ${groupHeals.Size}
			
				/varset healToCheck ${groupHeals[${i}].Arg[1,/]}
				
				| /HealPct Check
				/if (!${groupHeals[${i}].Find[/HealPct|]}) {
					/echo There is no /HealPct associated with [${groupHeals[${i}]}].  Please check this entry, and restart the macro.
					/call RemoveArrayElement groupHeals "${groupHeals[${i}]}"
				} else {
					/call argueString HealPct "${groupHeals[${i}]}"
					/varset healPct ${Macro.Return}
				}
			
				| Check bot health vs healpct
				
				/if (${avgGrpHealth} < ${healPct}) {
				
					| ----------------------------------------- Item -----------------------------------------|
					/if (${FindItemCount[=${healToCheck}]}) {
						
						| Check Timer 
						/if (${FindItem[=${healToCheck}].Timer} || ${itemTimer_${FindItem[=${healToCheck}].ID}}) {
							/if (${Debug} || ${Debug_Heals}) /echo [${healToCheck}] is not ready! [${If[${FindItem[=${healToCheck}].Timer},${FindItem[=${healToCheck}].Timer} ticks,${Math.Calc[${itemTimer_${FindItem[=${healToCheck}].ID}}/10].Int} sec]}] until this item is available.
						} else {
							
							| Check Range 
							/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${FindItem[=${healToCheck}].Spell}].MyRange}) {
								/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
							} else {
								/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
									/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
									/varset healToUse ${groupHeals[${i}]}
								}
							}
						}
						
					| ----------------------------------------- Alt Ability -----------------------------------------|
					} else /if (${Me.AltAbility[${healToCheck}]}) {
						
						| Check Timer 
						/if (!${Me.AltAbilityReady[${healToCheck}]} || ${altTimer_${Me.AltAbility[${nukeName}].ID}}) {
							/if (${Debug} || ${Debug_Heals}) /echo [${healToCheck}] wont be ready for another [${altTimer_${Me.AltAbility[${healToCheck}].ID}}] seconds!
						} else {
							
							| Check Range 
							/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${Me.AltAbility[${healToCheck}].Spell}].MyRange}) {
								/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
							} else {
								/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
									/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
									/varset healToUse ${groupHeals[${i}]}
								}
							}
						}
						
					| ----------------------------------------- Spell -----------------------------------------|	
					} else /if (${Me.Book[${healToCheck}]}) {
						
						| Check mana 
						/if (${Me.CurrentMana[${healToCheck}]} < ${Spell[${healToCheck}].Mana}) {
							/if (${Debug} || ${Debug_Heals}) /echo I don't have enough mana to cast [${healToCheck}].
						} else {
							
							| Check Range 
							/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${healToCheck}].MyRange}) {
								/if (${Debug} || ${Debug_Heals}) /echo [${healTarget}] is out of range for [${healToCheck}].
							} else {
								/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
									/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
									/varset healToUse ${groupHeals[${i}]}
								}
							}
						}
					}
				}
			/next i
			
			| If I have a heal to cast, cast it
			/if (${Bool[${healToUse}]}) {
				/if (${Debug} || ${Debug_Heals}) /echo Attempting to cast [${healToUse}] on [${healTarget} ${NetBots[${healTarget}].PctHPs}]
				/call e3_Cast ${NetBots[${healTarget}].ID} "${healToUse}/GiveUpTimer|${If[${Me.Gem[${healToUse}]},0,100]}"
				/varset recentHeal_Timer 20
			}
		}
	}

/if (${Debug} || ${Debug_Heals}) /echo <== check_Group -|
/RETURN





|** THIS NEEDS TO BE FINISHED
SUB check_Pets
/declare i int local
/for i 1 to ${NetBots.Counts}
/if (${NetBots[${NetBots.Client[${i}]}].PetID}) {
	/if (${NetBots[${NetBots.Client[${i}]}].PetHP} < ${PetHeal_Pct} && ${Spawn[${NetBots[${NetBots.Client[${i}]}].PetID}].Distance} < ${Spell[${PetHeal_Spell}].MyRange}) {
		/if (${Target.ID} != ${NetBots[${NetBots.Client[${i}]}].PetID}) /call TrueTarget ${NetBots[${NetBots.Client[${i}]}].PetID}
		/call e3_Cast ${NetBots[${NetBots.Client[${i}]}].PetID} "${Spell[${PetHeal_Spell}].ID}" Gem${PetHeal_Gem} 3s
		/varset ActionTaken TRUE
	}
}
/next i
/RETURN
**|



|--------------------------------------------------------------------|
|- setup_Heals														-|
|--------------------------------------------------------------------|
|- Imports and validates ini settings from Character Ini,[Heals].	-|
|--------------------------------------------------------------------|
SUB heal_Setup
/call INItoVar "${advSettings_Ini},Debug,Debug Heals (On/Off)" Debug_Heals bool outer
/if (${Debug} || ${Debug_Heals}) /echo |- heal_Setup ==>

	/declare recentHeal_Timer timer outer

	| Import settings from Character_Ini > [Life Support]
	/if (${Ini[${Character_Ini},Life Support,Life Support#1].Length}) /call IniToArray "${Character_Ini},Life Support,Life Support#" lifeSupport

	| Import settings from Character_Ini > [Heals]
	/if (${Bool[${Ini[${Character_Ini},Heals]}]}) {
		/if (${Ini[${Character_Ini},Heals,Tank#1].Length})  /call IniToArray "${Character_Ini},Heals,Tank#" tanks
		/if (${Ini[${Character_Ini},Heals,Important Bot#1].Length})  /call IniToArray "${Character_Ini},Heals,Important Bot#" importantBots
		
		/if (${Ini[${Character_Ini},Heals,Tank Heal#1].Length})  /call IniToArray "${Character_Ini},Heals,Tank Heal#" tankHeals
		/if (${Ini[${Character_Ini},Heals,Patch Heal#1].Length})  /call IniToArray "${Character_Ini},Heals,Patch Heal#" patchHeals
		/if (${Ini[${Character_Ini},Heals,Heal Over Time Spell#1].Length})  /call InitoArray "${Character_Ini},Heals,Heal Over Time Spell#" hotSpells
		/if (${Ini[${Character_Ini},Heals,Heal Over Time Spell#1].Length})  /call InitoArray "${Character_Ini},Heals,Group Heal#" groupHeals

		/if (${Ini[${Character_Ini},Heals,Who to Heal].Length})  /call IniToVar "${Character_Ini},Heals,Who to Heal" WhoToHeal string outer
		/if (${Ini[${Character_Ini},Heals,Who to HoT].Length})  /call IniToVar "${Character_Ini},Heals,Who to HoT" WhoToHoT string outer
	}
	
	/declare i int local

	| Convert tank array to a string, to be used in ${Select[x,y]} variables
	/if (${Defined[tanks]}) {
		/declare tankList string outer
		/for i 1 to ${tanks.Size}
			/varset tankList ${tankList}${tanks[${i}]},
		/next i
		/if (${Debug} || ${Debug_Heals}) /echo tankList=${tankList}
	}
	
	| Convert WhoToHeal to an array of healFunctions
	/if (${Defined[WhoToHeal]} && ${Defined[tankHeals]} || ${Defined[WhoToHeal]} && ${Defined[patchHeals]}) {
		
		/varset i 1
		:next_healFunction
		
		/if (${WhoToHeal.Arg[${i},/].Length}) {
			/if (${WhoToHeal.Arg[${i},/].Equal[Tanks]} || ${WhoToHeal.Arg[${i},/].Equal[Tank]}) {
				/call BuildArray "healFunctions" "tanks"
			} else /if (${WhoToHeal.Arg[${i},/].Equal[Important bots]} || ${WhoToHeal.Arg[${i},/].Equal[Importantbots]}) {
				/call BuildArray "healFunctions" "importantBots"
			} else /if (${WhoToHeal.Arg[${i},/].Equal[All]} || ${WhoToHeal.Arg[${i},/].Equal[NetBots]}) {
				/call BuildArray "healFunctions" "NetBots"
			} else /if (${WhoToHeal.Arg[${i},/].Equal[Group]} || ${WhoToHeal.Arg[${i},/].Equal[Groups]}) {
				/call BuildArray "healFunctions" "Group"
			}
			/varcalc i ${i} + 1
			/goto :next_healFunction
		}
		
		/if (${Defined[healFunctions]}) /declare do_Heals bool outer TRUE
	}
	
	| Convert WhoToHoT to an array of healFunctions
	/if (${Defined[WhoToHoT]} && ${Defined[hotSpells]}) {
		
		/varset i 1
		:next_hotFunction
		
		/if (${WhoToHoT.Arg[${i},/].Length}) {
			/if (${WhoToHoT.Arg[${i},/].Equal[Tanks]} || ${WhoToHeal.Arg[${i},/].Equal[Tank]}) {
				/call BuildArray "hotFunctions" "tanks"
			} else /if (${WhoToHoT.Arg[${i},/].Equal[Important bots]} || ${WhoToHoT.Arg[${i},/].Equal[Importantbots]}) {
				/call BuildArray "hotFunctions" "importantBots"
			} else /if (${WhoToHoT.Arg[${i},/].Equal[All]} || ${WhoToHoT.Arg[${i},/].Equal[NetBots]}) {
				/call BuildArray "hotFunctions" "NetBots"
			}
			/varcalc i ${i} + 1
			/goto :next_hotFunction
		}
		
		/if (${Defined[hotFunctions]}) /declare do_HoTs bool outer TRUE
	}
	
	/if (${Defined[groupHeals]}) /declare do_GroupHeals bool outer TRUE
	
/if (${Debug} || ${Debug_Heals}) /echo <== heal_Setup -|
/RETURN


 
Sub heal_Background_Events

/return



SUB heal_MacroSettings
/if (${Debug} || ${Debug_Heals}) /echo |- assist_MacroSettings ==>

	/call WriteToIni "${advSettings_Ini},Debug,Debug Heals (On/Off)"

/if (${Debug} || ${Debug_Heals}) /echo <== heal_MacroSettings -|
/RETURN



SUB heal_CharacterSettings
/if (${Debug} || ${Debug_Heals}) /echo |- heal_CharacterSettings ==>

	| LifeSupport entries.
	/call WriteToIni "${Character_Ini},Life Support,Life Support#1"

	| If I'm a healer, add e3_Macro_Heals.inc entries.
	/if (${Me.Class.HealerType}) {
		/call WriteToIni "${Character_Ini},Heals,Tank#1"
		/call WriteToIni "${Character_Ini},Heals,Important Bot#1"
		
		/call WriteToIni "${Character_Ini},Heals,Tank Heal#1"
		/call WriteToIni "${Character_Ini},Heals,Patch Heal#1"
		/call WriteToIni "${Character_Ini},Heals,Who to Heal" "Tanks/ImportantBots/All"
		
		/call WriteToIni "${Character_Ini},Heals,Heal Over Time Spell#1"
		/call WriteToIni "${Character_Ini},Heals,Who to HoT" "Tanks/ImportantBots/All"
		
		/if (${Me.Class.ShortName.Equal[CLR]} || ${Me.Class.ShortName.Equal[PAL]}) /call WriteToIni "${Character_Ini},Heals,Group Heal#1"
		
		| /call WriteToIni "${Character_Ini},Heals,Pet Heal Spell"
		
		| /if (${Me.Class.ShortName.Equal[CLR]} || ${Me.Class.ShortName.Equal[DRU]}) /call WriteToIni "${Character_Ini},Heals,Join CH-Chain (On/Off)"
	}

/if (${Debug} || ${Debug_Heals}) /echo <== heal_CharacterSettings -|
/RETURN



Sub heal_Aliases
/if (${Debug} || ${Debug_Heals}) /echo |- heal_Aliases ==>

	/squelch /tankheals /bc -Set- Tank Heals
	/squelch /patchheals /bc -Set- Patch Heals
	/squelch /HoTSpells /bc -Set- HoT Spells
	/squelch /groupheals /bc -Set- Group Heals
	
/if (${Debug} || ${Debug_Heals}) /echo <== heal_Aliases -|
/return