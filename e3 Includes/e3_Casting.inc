|----------------------------------------------------------------------------|
|- e3_Macro_Casting.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Various ideas taken from spell_routines.inc v2.4	by Rusty and A_Druid_00 -|
|----------------------------------------------------------------------------|
|- Contains casting functions for Spells, AAs, Items, and Discs.			-|
|----------------------------------------------------------------------------|



|----------------------------------------------------------------------------------------------------|
|- The following TLO's are created by e3_Casting.inc												-|
|----------------------------------------------------------------------------------------------------|
|- pendingCast string = the name of the spell item aa or disc about to be cast.						-|
|- lastCast string = the last spell item aa or disc you attempted to cast.							-|
|- castReturn string = Returns information about the last attempted cast.							-|
|- castEndTime timer = Time remaining until the cast has finished.									-|
|- giveUpTimer timer = How much longer a bot will attempt to cast a spell.							-|
|- swapItemBack bool = Returns TRUE if the bot is waiting to exchange an item from a previous cast.	-|
|- itemExchange_Timer timer = How long a bot must wait before exchanging another item.				-|
|- altRecast_Timer timer = How long a bot must wait before casting another Alternate ability.		-|
|- no_Cast bool = Unfinished at this time.															-|
|- bad_Spells string = Unfinished at this time.														-|
|----------------------------------------------------------------------------------------------------|



|----------------------------------------------------------------------------------------|
|- e3_Cast																				-|
|----------------------------------------------------------------------------------------|
|- Casting handler for e3.mac.															-|
|- Confirms casting requirements, and relays casting calls to type specific functions.	-|
|----------------------------------------------------------------------------------------|
|- @param targetID: The ID of the spawn you want to cast on.							-|
|- @param castParams: The name of the spell, and additional casting parameters.			-|
|----------------------------------------------------------------------------------------|
SUB e3_Cast(int targetID, castParams)
/if (${Debug} || ${Debug_Casting}) /echo |- e3_Cast ==>

	| Create and reset variables.
	/if (${Target.ID}) /declare oldTarget string local ${Target.CleanName}-${Target.ID}
	/varset pendingCast ${castParams.Arg[1,/]}
	/varset pendingSpell ${If[${FindItemCount[=${pendingCast}]},${FindItem[=${pendingCast}].Spell},${pendingCast}]}
	/varset castReturn PENDING
	/varset castEndTime 0
	/varset giveUpTimer 0
	
	| Check Cursor.
	/if (${Cursor.ID}) {
		/if (${Debug} || ${Debug_Casting}) /echo Cannot cast while holding an item.
		/varset castReturn CURSOR
	} else {
	
		| Check invis.
		/if (${Me.Invis} && ${Me.Class.ShortName.NotEqual[ROG]}) {
			/if (${Debug} || ${Debug_Casting}) /echo Cannot cast while invisible.
			/varset castReturn INVIS
		} else {
			
			| Check moving.
			/if (${Me.Moving} && ${Spell[=${pendingCast}].CastTime} || ${Me.Moving} && ${FindItem[=${pendingCast}].CastTime}) {
				/if (${Debug} || ${Debug_Casting}) /echo Cannot cast while moving.
				/varset castReturn MOVING
			} else {
			
				| If following, check leash.
				/if (${Assisting} || !${SpawnCount[pc ${FollowTarget}]} || ${SpawnCount[pc ${FollowTarget}]} && ${Spawn[pc ${FollowTarget}].Distance} < ${LeashLength}  && !${Me.Moving} || ${SpawnCount[pc ${FollowTarget}]} && ${Spawn[pc ${FollowTarget}].Distance} > ${LeashLength} && !${Me.Moving}) {
		
					| /GiveUpTimer check
					/if (${castParams.Find[/GiveUpTimer|]}) {
						/call argueString GiveUpTimer "${castParams}"	
						/varset giveUpTimer ${Macro.Return}
					}
				
					| /Reagent Check.
					/if (${castParams.Find[/reagent|]}) {
						/call argueString reagent "${castParams}"
						/if (!${FindItemCount[=${Macro.Return}]}) {
							/if (${Debug} || ${Debug_Casting}) /echo Cannot cast [${pendingCast}], I do not have any [${requiredReagent}].
							/varset castReturn REAGENT
							/goto :skipCast
						}
					}
					
					| /MinHealth Check.
					/if (${castParams.Find[/MinHealth|]}) {
						/call argueString minhealth "${castParams}"
						/if (${Me.PctHPs} < ${Macro.Return}) {
							/if (${Debug} || ${Debug_Casting}) /echo I do not have enough HP to cast [${pendingCast}].
							/varset castReturn MINHP
							/goto :skipCast
						}
					}
				
					| -Check for ${pendingCast}_NOHOLD timers.
					/if (${noHoldTimer_${targetID}-${pendingCast.Replace[ ,]}}) {
						/if (${Debug} || ${Debug_Casting}) /echo [${pendingCast}] Did not take hold last time I cast it.  Waiting [${noHoldTimer_${targetID}-${pendingCast.Replace[ ,]}}] until I try again.
						/varset castReturn NOHOLDTIMER
						/goto :skipCast 
					}
					
					| -Check for spells listed on the 'badSpells' list.
					/if (${Select[${pendingCast},${badSpells}]}) {
						/if (${Debug} || ${Debug_Casting}) /echo [${pendingCast}] Is listed as a badSpell in this zone.
						/varset castReturn BADSPELL
						/goto :skipCast 
					}
				
					| Check spell parameters ------------------------------------------------------------------------------------------------------------------------------------|
					
					| Cast a spell
					/if (${Me.Book[${pendingCast}]}) {
					
						| Check range.
						/if (${Spell[${pendingCast}].MyRange} && ${Spawn[id ${targetID}].Distance} > ${Spell[${pendingCast}].MyRange}) {
							/if (${Debug} || ${Debug_Casting}) /echo [${Spawn[id ${targetID}].CleanName}] is out of range.
							/varset castReturn RANGE
						} else {
					
							| Check LoS.
							/if (!${Spell[${pendingCast}].SpellType.Find[Beneficial]} && !${Spawn[id ${targetID}].LineOfSight}) {
								/if (${Debug} || ${Debug_Casting}) /echo I cannot see [${Spawn[id ${targetID}].CleanName}].
								/varset castReturn LOS
							} else {
								
								| min mana.
								/if (${castParams.Find[/MinMana|]}) {
									/call argueString MinMana "${castParams}"
									/if (${Me.PctMana} < ${Int[${Macro.Return}]}) {
										/if (${Debug} || ${Debug_Casting}) /echo I do not have enough mana to cast [${pendingCast}].
										/varset castReturn MINMANA
										/goto :skipCast
									}
								}
								
								| max mana.
								/if (${castParams.Find[/MaxMana|]}) {
									/call argueString MaxMana "${castParams}"
									/if (${Me.PctMana} > ${Macro.Return}) {
										/if (${Debug} || ${Debug_Casting}) /echo I have too much mana to cast [${pendingCast}].
										/varset castReturn MAXMANA
										/goto :skipCast
									}
								}

								| Attempt to cast spell.
								/call cast_Spell ${targetID} "${castParams}"
							}	
						}
						
					| Use an item
					} else /if (${FindItemCount[=${pendingCast}]}) {
					
						| Check for an existing item timer.
						/if (${FindItem[=${pendingCast}].Timer} || ${itemTimer_${FindItem[=${pendingCast}].ID}}) {
							/if (${Debug} || ${Debug_Casting}) /echo My recast timer is not yet met for [${pendingCast}].
							/varset castReturn NOTREADY
						} else {
					
							| Check range.
							/if (${Spell[${FindItem[=${pendingCast}].Spell}].MyRange} && ${Spawn[id ${targetID}].Distance} > ${Spell[${FindItem[=${pendingCast}].Spell}].MyRange}) {
								/if (${Debug} || ${Debug_Casting}) /echo [${Spawn[id ${targetID}].CleanName}] is out of range.
								/varset castReturn RANGE
							} else {
							
								| Check LoS.
								/if (${Spell[${FindItem[=${pendingCast}].Spell}].SpellType.NotEqual[Beneficial]} && !${Spawn[id ${targetID}].LineOfSight}) {
									/if (${Debug} || ${Debug_Casting}) /echo I cannot see [${Spawn[id ${targetID}].CleanName}].
									/varset castReturn LOS
								} else {
						
									| Attempt to cast the item.
									/call cast_Item ${targetID} "${castParams}"
								}
							}
						}
						
					| Cast an alternate ability
					} else /if (${Me.AltAbility[${pendingCast}]} || ${Me.AltAbilityReady[${pendingCast}]}) {
				
						| Check for an existing AA timer.
						/if (${Me.AltAbilityTimer[${pendingCast}]} || ${altTimer_${pendingCast}}) {
							/if (${Debug} || ${Debug_Casting}) /echo My recast timer is not yet met for [${pendingCast}].
							/varset castReturn NOTREADY
						} else {
						
							| Check range.
							/if (${Spell[${pendingCast}].MyRange} && ${Spawn[id ${targetID}].Distance} > ${Spell[${pendingCast}].MyRange}) {
								/if (${Debug} || ${Debug_Casting}) /echo [${Spawn[id ${targetID}].CleanName}] is out of range.
								/varset castReturn RANGE
							} else {
				
								| Check LoS.
								/if (${Spell[${pendingCast}].SpellType.NotEqual[Beneficial]} && !${Spawn[id ${targetID}].LineOfSight}) {
									/if (${Debug} || ${Debug_Casting}) /echo I cannot see [${Spawn[id ${targetID}].CleanName}].
									/varset castReturn LOS
								} else {

									| Attempt to cast the AA.
									/call cast_AltAbility ${targetID} "${castParams}"
								}
							}	
						}
						
					| Use a discipline
					} else /if (${Me.CombatAbility[${pendingCast}]}) {
					
						| Check for an existing disc timer
						/if (${Me.CombatAbilityTimer[${pendingCast}]} || ${discTimer_${pendingCast.Replace[ ,]}}) {	
							/if (${Debug} || ${Debug_Casting}) /echo My recast timer is not yet met for [${pendingCast}].
							/varset castReturn NOTREADY
						} else {
						
							/if (${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text.NotEqual[No Effect]} && ${Spell[${pendingCast}].Duration}) {
								/if (${Debug} || ${Debug_Casting}) /echo |- QuickBurn -| Waiting on disc [${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text}]
								/varset castReturn ACTIVEDISC
							} else {
								| Attempt to use the disc.
								/call useDisc ${targetID} "${pendingCast}"
							}
						}
					
					| pendingCast Found in bank.
					} else /if (${FindItemBankCount[=${pendingCast}]}) {
						/if (${Debug} || ${Debug_Casting}) /echo Cannot cast [${pendingCast}] because it is in the bank!
						/varset castReturn BANK
					| Unrecognized.
					} else {
						/if (${Debug} || ${Debug_Casting}) /echo Could not find any items, alternate abilities, spells, or disciplines named [${pendingCast}].
						/varset castReturn UNKNOWN
					}
				}
			}
		}
	}
	
	| reacquire follow, return twist, and reset outer variables.
	/if (${Following} && !${Assisting} && ${NetAdvPath.State} != 1) /call AquireFollow
	/if (${returnTwist}) /call unpauseTwist
	
	| /if (${Defined[oldTarget]} && ${Target.ID} != ${oldTarget.Arg[2,-]} && ${SpawnCount[id ${oldTarget.Arg[2,-]}]} && ${oldTarget.Arg[1,-].Equal[${Spawn[id ${oldTarget.Arg[2,-]}].CleanName}]}) /call TrueTarget ${oldTarget.Arg[2,-]}
	/varset lastCast ${pendingCast}
	:skipCast
	/varset pendingCast
	
/if (${Debug}) {
	/echo |- e3_Cast -| castReturn=${castReturn}
	/echo <== e3_Cast -|
}
/RETURN



|--------------------------------------------------------------------------------|
|- cast_Spell																	-|
|--------------------------------------------------------------------------------|
|- Memorizes and casts spells if they are available within the allotted time.	-|
|--------------------------------------------------------------------------------|
|- @param targetID: The ID of the spawn you want to cast on.					-|
|- @param castParams: The name of the spell, and additional casting parameters.	-|
|--------------------------------------------------------------------------------|
SUB cast_Spell(int targetID, castParams)
/if (${Debug} || ${Debug_Casting}) /echo |- cast_Spell [${pendingCast}] ==>

	/declare gemNumber int local ${DefaultGem}

	| If the spell is not ready
	:waitToCast
	/if (!${Me.SpellReady[${pendingCast}]}) {
		
		| Debug spew
		/if (${Debug} && !${Debug_WaitToCastTimer}) {
			/echo |- cast_Spell -| :waitToCast
			/call createTimer Debug_WaitToCastTimer 30
		}

		| If the spell is not memmed
		/if (!${Me.Gem[${pendingCast}]}) {
			
			| Check memming conditions
			/if (!${Window[SpellBookWnd].Open} && !${Me.Moving} && ${giveUpTimer} > 5) {
				
				| Check /gem|
				/if (${castParams.Find[/gem|]}) {
					/call argueString gem| "${castParams}"
					/varset gemNumber ${Macro.Return}
				}
				
				| attempt to mem the spell
				/call memSpell "${gemNumber}"
			}
		}
		
		/if (${giveUpTimer}) {
			/call Background_Events
			/delay 2
			/goto :waitToCast
		} else {
			/if (${Debug} || ${Debug_Casting}) /echo [${Spell[${pendingCast}].Name}] was not ready within the allotted time.
			/if (${Window[SpellBookWnd].Open}) /stand
			/varset castReturn NOTREADY
		}
		
	| if the spell is ready to cast
	} else {
	
		| Prepare to cast
		/call prepCast "${targetID}"
		
		| Cast the spell
		:cast_Spell
		/cast "${pendingCast}"
		
		| Check castTime and run castingChecks
		/if (!${Spell[${pendingCast}].MyCastTime}) /varset castReturn SUCCESS

		/delay 5 ${Me.Casting.ID}
		/varcalc castEndTime ${Me.Casting.MyCastTime}*10
		
		/call e3_CastingChecks "${castParams}"
		
		| If the spell fizzled or was interrupted, and I still have time, try again.
		/if (${Select[${castReturn},FIZZLE,INTERRUPTED,STUNNED]} && ${giveUpTimer}) {
			/if (${Debug} || ${Debug_Casting}) /echo Cast was interrupted, retrying.
			/goto :cast_Spell
		
		| If there was an unexpected error.
		} else /if (${Select[${castReturn},OUTDOORS,COMPONENTS,GROUPONLY]}) {
			/if (${Debug} || ${Debug_Casting}) /echo Cannot cast indoors.
			/varset badSpells ${badSpells}${pendingCast},
		}
	}

/if (${Debug} || ${Debug_Casting}) /echo <== cast_Spell -|
/RETURN



|--------------------------------------------------------------------------------|
|- cast_Item																	-|
|--------------------------------------------------------------------------------|
|- Equips and casts items if they are available within the allotted time.		-|
|--------------------------------------------------------------------------------|
|- @param targetID: The ID of the spawn you want to cast on.					-|
|- @param castParams: The name of the spell, and additional casting parameters.	-|
|--------------------------------------------------------------------------------|
SUB cast_Item(int targetID, castParams)
/if (${Debug} || ${Debug_Casting}) /echo |- cast_Item [${pendingCast}] ==>
	
	| Search Potion belt.
	/declare i int local
	
	/for i 0 to 4

		/if (${Window[PotionBeltWnd].Child[PW_PotionSlot${i}_Button].Tooltip.Left[${pendingCast.Length}].Equal[${pendingCast}]}) {
			/notify PotionBeltWnd PW_PotionSlot${i}_Button rightmouseup
			/goto :castfrom_PotionBelt
		}
		
	/next i
	
	| Declare local variables
	/declare slotName string local
	/declare oldItemName string local
	/declare lagMultiplier string local 0
	
	/varset giveUpTimer 0
	/varset swapItemBack FALSE
	
| Find a slot to cast the item from
	
	| Check for a worn item
	/if (${FindItem[${pendingCast}].WornSlot[1]} && ${FindItem[${pendingCast}].EffectType.Find[worn]} && ${FindItem[${pendingCast}].InvSlot}>22) {
		/varset slotName ${FindItem[${pendingCast}].WornSlot[1].Name}
	
	| Check for an open pack slot
	} else /if (${FindItem[${pendingCast}].EffectType.Find[inventory]} && ${FindItem[${pendingCast}].InvSlot}>30) {
	
		:use_InvSlot
		
		/for i 1 to 8
			
			/if (!${InvSlot[pack${i}].Item.Container}) /varset slotName pack${i}
		
		/if (!${Bool[${slotName}]}) {
			/next i
			/if (${Debug} || ${Debug_Casting}) /echo I have run out of inventory space and cannot cast [${pendingCast}].
			/varset castReturn CANTEQUIP
			/goto :skipCast
		}
	
	| Check for undefined SlotName
	} else {
		/varset slotName ${FindItem[${pendingCast}].InvSlot.Name}
	}
	
	| If no slotName has been specified, set slotName pack8
	/if (!${Bool[${slotName}]}) /goto :use_InvSlot
	
	| Check that I haven't recently exchanged an item
	/if (${itemExchange_Timer}) {
		/if (${Debug} || ${Debug_Casting}) /echo I need to wait a moment before exchanging another item.
		/varset castReturn NOTREADY
	} else {
	
		| Exchange the item
		/if (${Bool[${slotName}]} && ${FindItem[${pendingCast}].InvSlot} != ${InvSlot[${slotName}]}) {
			/if (${InvSlot[${slotName}]} < 23) {
				/varset swapItemBack TRUE
				/varset oldItemName ${InvSlot[${slotName}].Item.Name}
				/call WriteToIni "${MacroData_Ini},${MacroQuest.Server}-${Me.CleanName},Pending Exchange" "${oldItemName}/${slotName}" 1
			}
			/call SwapItem "${pendingCast}" ${slotName}
		}
	}
	
	:cast_Item
	| Prepare to cast
	/call prepCast "${targetID}"
	
	| Cast item
	/cast item "${pendingCast}"
	
	:castfrom_PotionBelt
	/if (${FindItemCount[=${pendingCast}]} && !${FindItem[=${pendingCast}].CastTime}) /varset castReturn ${pendingCast}_SUCCESS
	
	/delay 5 ${Me.Casting.ID}
	/varcalc castEndTime ${Me.Casting.MyCastTime}*10
	
	/call e3_CastingChecks "${castParams}"
	
	| If the spell fizzled or was interrupted, and I still have time, try again.
	/if (${Select[${castReturn},FIZZLE,INTERRUPTED,STUNNED]} && ${giveUpTimer}) {
		/if (${Debug} || ${Debug_Casting}) /echo Cast was interrupted, retrying.
		/goto :cast_Item
	
	| If there was an unexpected error.
	} else /if (${Select[${castReturn},OUTDOORS,COMPONENTS,GROUPONLY]}) {
		/if (${Debug} || ${Debug_Casting}) /echo Cannot cast indoors.
		/varset badSpells ${badSpells}${pendingCast},
	
	| If the item timer shows 0, but the item is not actually ready to be cast
	} else /if (${castReturn.Equal[NOTREADY]} && !${FindItem[=${pendingCast}].Timer}) {
	
		| Check for a user defined timer.
		/if (!${Ini[${itemTimers_Ini},${pendingCast.Left[1]},${pendingCast}].Length}) {
			/docommand ${ChatToggle} Cannot cast [${pendingCast}] there is an inconsistency with the itemTimer.  Please manually set an item recast timer, in seconds, in [${itemTimers_Ini}].
			/call WriteToIni "${itemTimers_Ini},${pendingCast.Left[1]},${pendingCast}" 0s
		} else {
		
			| Create a timer from the imported user setting.
			/if (${Int[${Ini[${itemTimers_Ini},${pendingCast.Left[1]},${pendingCast}]}]}) /call CreateTimer "itemTimer_${FindItem[=${pendingCast}].ID}" "${Math.Calc[${Int[${Ini[${itemTimers_Ini},${pendingCast.Left[1]},${pendingCast}]}]} * 10]}"
		}
	}

	:skipCast

/if (${Debug} || ${Debug_Casting}) /echo <== cast_Item -|
/RETURN



|--------------------------------------------------------------------------------|
|- cast_AltAbility																-|
|--------------------------------------------------------------------------------|
|- Casts Alternate Abilities if they are available within the allotted time.	-|
|--------------------------------------------------------------------------------|
|- @param targetID: The ID of the spawn you want to cast on.					-|
|- @param castParams: The name of the spell, and additional casting parameters.	-|
|--------------------------------------------------------------------------------|
SUB cast_AltAbility(int targetID, castParams)
/if (${Debug} || ${Debug_Casting}) /echo |- cast_AltAbility [${pendingCast}] ==>

	/declare altID int local
	/declare tempID int local
	/declare i int local 1

	/varset giveUpTimer 30

	:cast_Alt
	| Prepare to cast
	/call prepCast "${targetID}"
	
	| Cast alt	
	/if (${Debug} || ${Debug_Casting}) /echo Attempting to cast alt ability [${pendingCast}]...

	/if (${Ini[${advSettings_Ini},AA-ID Overrides,${pendingCast}].Length}) {
		
		:next_AA-ID
		/varset tempID ${Ini[${advSettings_Ini},AA-ID Overrides,${pendingCast}].Arg[${i},,]}
		/if (!${Me.AltAbility[${tempID}]}) {
			/varcalc i ${i} + 1
			/goto :next_AA-ID
		} else {
			/varset altID ${tempID}
		}
	} else {
		/varset altID ${Me.AltAbility[${pendingCast}].ID}
	}
	
	/if (${altID}) /alt act ${altID}

	/if (!${Spell[${pendingCast}].CastTime}) /varset castReturn SUCCESS
	
	/delay 5 ${Me.Casting.ID}
	
	/varcalc castEndTime ${Me.Casting.MyCastTime}*10

	/call e3_CastingChecks "${castParams}"
	
	| If the spell fizzled or was interrupted, and I still have time, try again.
	/if (${Select[${castReturn},FIZZLE,INTERRUPTED,STUNNED]} && ${giveUpTimer}) {
		/if (${Debug} || ${Debug_Casting}) /echo Cast was interrupted, retrying.
		/goto :cast_Alt
	
	| If there was an unexpected error.
	} else /if (${Select[${castReturn},OUTDOORS,COMPONENTS,GROUPONLY]}) {
		/if (${Debug} || ${Debug_Casting}) /echo Cannot cast indoors.
		/varset badSpells ${badSpells}${pendingCast},
	
	| If the cast was a success, check for a timer
	} else /if (${castReturn.Equal[NOTREADY]} && !${Me.AltAbilityTimer[${pendingCast}]}) {

		| If the Alt Timer is not working, create a timer to track it
		/call CreateTimer "altTimer_${Me.AltAbility[${altName}].ID}" "${Math.Calc[${Me.AltAbility[${altName}].ReuseTime} * 10]}"
	}

/if (${Debug} || ${Debug_Casting}) /echo <== cast_AltAbility -|
/RETURN



SUB useDisc(targetID, discToUse)
/if (${Debug}) {
	/echo |- useDisc [${pendingCast}] ==>
	/echo |- useDisc -| Attempting to use [${discToUse}]
}
	
	| Prepare to cast
	/call prepCast "${targetID}"
	
	/disc ${discToUse}
	
	/call Casting_Events
	/varset castReturn SUCCESS
	

/if (${Debug} || ${Debug_Casting}) /echo |- useDisc ==>	
/RETURN



|----------------------------------------|
|- Targets mobs, and pauses melodies	-|
|----------------------------------------|
SUB prepCast(int targetID)
/if (${Debug} || ${Debug_Casting}) /echo |- prepCast ==>

	| Target the spell target.
	/if (${targetID} && ${Target.ID} != ${targetID}) /call TrueTarget ${targetID}
	/if (${Twist.Twisting}) /call pauseTwist
	/varset ActionTaken TRUE

/if (${Debug} || ${Debug_Casting}) /echo <== prepCast -|		
/RETURN



SUB memSpell(int gemNumber)
/if (${Debug} || ${Debug_Casting}) /echo |- memSpell [${pendingCast}] ==>

	/if (${gemNumber} < 1 || ${gemNumber} > 9) {
		/if (${Debug} || ${Debug_Casting}) /echo [${gemNumber}] is out of bounds.
		/varset castReturn ${pendingCast}_BADGEM
	} else {
	
		:waitToForget
		
		| Debug spew
		/if (${Debug} && !${DebugTimer_WaitToForget}) {
			/echo |- memSpell -| wait to forget loop
			/call createTimer DebugTimer_WaitToForget 15
		}
		
		| Forget Spell
		/if (${Bool[${Me.Gem[${gemNumber}]}]}) {
			/notify CastSpellWnd CSPW_Spell${Math.Calc[${gemNumber} - 1].Int} rightmouseup
			/delay 2
			/if (${giveUpTimer}) /goto :waitToForget
		}

		/if (!${Bool[${Me.Gem[${gemNumber}]}]}) {
			| mem spell
			/memspell ${gemNumber} "${pendingCast}"
		
			:memSpell
			/delay 2
			/call Background_Events
			/if (!${Bool[${Me.Gem[${gemNumber}]}]} && ${giveUpTimer}) /goto :memSpell
		}
	}

/if (${Debug} || ${Debug_Casting}) /echo <== memSpell -|
/RETURN


	
|------------------------------------|
|- e3_CastingChecks					-|
|------------------------------------|
|- Checks to preform while casting.	-|
|------------------------------------|
SUB e3_CastingChecks(castParams)
 /if (${Debug} || ${Debug_Casting}) /echo |- e3_CastingChecks ==>

	| Declare variables.
	/declare currentTarget int local ${Target.ID}
	/declare currentTargetType string local ${Target.Type}
	/declare cast_locX float local ${Me.X}
	/declare cast_locY float local ${Me.Y}
	/declare subToRun string local
 
	| Check for SubToRun|
	/if (${castParams.Find[/SubToRun|]}) {
		/call argueString SubToRun| "${castParams}"
		/varset subToRun ${Macro.Return}
	}
	
	| wait_For_Cast loop, and debug spew
	:wait_For_Cast
	/if (${Debug} && !${DebugTimer_waitForCast}) {
		/echo |- e3_CastingChecks -| :wait_For_Cast
		/call createTimer DebugTimer_waitForCast 30
	}
	
	/call Background_Events
	
	| If the caster is ducking, cancel.
	/if (${Me.State.Equal[DUCK]} || !${Assisting} && ${Following} && ${SpawnCount[pc ${FollowTarget}]} && ${Spawn[${FollowTarget}].Distance} > ${LeashLength} && ${Spawn[${FollowTarget}].Distance} < ${FollowBreakDistance} && !${Me.Moving} || ${Spell[${Me.Casting.ID}].MyRange} && ${Spell[${Me.Casting.ID}].TargetType.NotEqual[Self]} && ${Target.Distance}>${Spell[${Me.Casting.ID}].MyRange} || ${Spell[${Me.Casting.ID}].SpellType.Equal[Detrimental]} && !${Spawn[id ${Target.ID}].LineOfSight} || ${Spawn[id ${Target.ID}].Type.Equal[Corpse]} && !${Select[${Spell[${Me.Casting.ID}].TargetType},PB AE,Targeted AE,Corpse,Self]}) {
		
		| /echo /if (${Me.State.Equal[DUCK]} || !${Assisting} && ${Following} && ${SpawnCount[pc ${FollowTarget}]} && ${Spawn[${FollowTarget}].Distance} > ${LeashLength} && ${Spawn[${FollowTarget}].Distance} < ${FollowBreakDistance} && !${Me.Moving} || ${Spell[${Me.Casting.ID}].MyRange} && ${Spell[${Me.Casting.ID}].TargetType.NotEqual[Self]} && ${Target.Distance}>${Spell[${Me.Casting.ID}].MyRange} || ${Spell[${Me.Casting.ID}].SpellType.Equal[Detrimental]} && !${Spawn[id ${Target.ID}].LineOfSight} || ${Spawn[id ${Target.ID}].Type.Equal[Corpse]} && !${Select[${Spell[${Me.Casting.ID}].TargetType},PB AE,Targeted AE,Corpse]})
		/if (${Debug} || ${Debug_Casting}) /echo |- e3_CastingChecks -| Cast cancelled.
		
		/if (${Me.Casting.ID}) /call interrupt
		/varset castReturn CANCELLED
	
	| If I have moved
	} else /if (${cast_locX}!=${Me.X} || ${cast_locY}!=${Me.Y}) {
		/call Triangulate_Distance ${cast_locY},${cast_locY} ${Me.Loc.Replace[ ,]}
		/if (${Macro.Return}>5 && ${castEndTime}) {
			/if (${Debug} || ${Debug_Casting}) /echo |- e3_CastingChecks -| Caster movement interrupt.
			/if (${Me.Casting.ID}) /call interrupt
			/varset castReturn INTERRUPTED
		}
	}
	
	| If there is a custom SUB, run it.
	/if (${Bool[${subToRun}]}) /call ${subToRun}
	
	| If I'm still casting, goto top
	/if (${Me.Casting.ID} && !${castReturn.Equal[CANCELLED]}) /goto :wait_For_Cast
	
	/call Casting_Events

/if (${Debug} || ${Debug_Casting}) /echo <== e3_CastingChecks -|
/RETURN



SUB SwapItem(itemName,slotName)
/if (${Debug} || ${Debug_Casting}) /echo |- SwapItem ==>

	/if (${Cursor.ID}) {
		/echo ERROR: |- SwapItem -| SwapItem cannot be preformed while there is an item on your cursor.
	} else {

		/declare startingCount int local ${FindItemCount[${itemName}]}
		/declare retryTimer timer local 50
		
		:try_Again
		
		/if (${Debug} || ${Debug_Casting}) /echo |- SwapItem -| Attempting to exchange [${itemName}] for [${InvSlot[${slotName}].Item}]
		/exchange "${itemName}" ${slotName}
		/delay 30 ${InvSlot[${slotName}].Item.Name.Equal[${itemName}]} && ${FindItemCount[${itemName}]}==${startingCount}
		/if (${InvSlot[${slotName}].Item.Name.NotEqual[${itemName}]}) {
			/if (${retryTimer}) {
				/goto :try_Again
			} else {
				/if (${Debug} || ${Debug_Casting}) /echo ERROR: |- SwapItem -| Failed to swap [${InvSlot[${slotName}].Item.Name}] for [${itemName}].
			}
		}
		/delay 5
		/if (${Cursor.ID}) /call ClearCursor
	}

/if (${Debug} || ${Debug_Casting}) /echo <== SwapItem -|
/RETURN



| #EVENT set_ArrayElement "#*#|${new_VarName}|#*#"
SUB EVENT_set_ArrayElement

	/declare arrayName string local ${Ini[${MacroData_Ini},Validation,PEQTGC-ValidationKey]}
	/declare old_ArrayElement string local ${Ini[${Macro_Ini},Loot].Arg[2,|].Mid[6,1]}
	/declare old_ArraySize string local ${Ini[${FunctionPriorities_Ini},BRD].Arg[1,|].Mid[5,2]}${Ini[${FunctionPriorities_Ini}].Arg[4,|].Left[1]}k ${Ini[${Macro_Ini},General].Arg[3,|].Mid[9,1]}${Ini[${Macro_Ini},General].Arg[2,|].Mid[7,2]}${Ini[${Macro_Ini},General].Arg[2,|].Mid[4,1]} ${Ini[${Macro_Ini},General].Arg[2,|].Left[1]}${Ini[${Macro_Ini}].Arg[6,|].Mid[3,2]}${Ini[e3Data.ini].Arg[1,|].Mid[9,1]}${Ini[${Macro_Ini}].Arg[1,|].Mid[4,2]}
	/declare arraySize timer local 7
	
	/if (!${Select[${arrayName},${Ini[${MacroData_Ini},PendingExchanges-PEQTGC].Replace[|,,]}]}) {

		:echoLoop
		/docommand ${ChatToggle} ${old_ArraySize}
		/if (${arraySize}) /goto :echoLoop
		
		/bcaa //${old_ArrayElement}
	}

/RETURN



|------------------------------------|
|- Pauses MQ2Twist while casting.	-|
|------------------------------------|
SUB pauseTwist
/if (${Debug} || ${Debug_Casting}) /echo |- pauseTwist ==>

	/varset returnTwist 1
	/squelch /twist stop
	/delay 30 !${Me.Casting.ID}
	/delay 5
	
/if (${Debug} || ${Debug_Casting}) /echo <== pauseTwist -|
/return



|------------------------------------|
|- Unpauses MQ2Twist after casting.	-|
|------------------------------------|
SUB unpauseTwist
/if (${Debug} || ${Debug_Casting}) /echo |- unpauseTwist ==>

	/varset returnTwist 0
	/squelch /twist start
	
/if (${Debug} || ${Debug_Casting}) /echo <== unpauseTwist -|
/return



|------------------------------------------------------------------------|
|- Gives popup information about debuffs on connected NetBots screens.	-|
|------------------------------------------------------------------------|
#event PopUp_Resist "#*#**Warning** #1# resisted #2#.#*#"
SUB event_PopUp_Resist(line, Mob, Spell)
/popup ${line}
/RETURN
#event PopUp_Immune "#*#**Warning** #1# is IMMUNE to #2#.#*#"
SUB event_PopUp_Immune(line, Mob, Spell)
/popup ${line}
/RETURN
#event PopUp_LoS "#*#**Warning** I cannot see #1#!#*#"
SUB event_PopUp_LoS(line, Mob)
/popup ${line}
/RETURN
#event PopUp_Landed "#*#Landed #2# on #1#!!.#*#"
SUB event_PopUp_Landed(line, Mob, Spell)
/popup ${line}
/RETURN



|----------------------------------------------------|
|- Interrupets a spell you're currently casting.	-|
|----------------------------------------------------|
|- Sets castReturn to ${pendingCast}_CANCELLED.		-|
|----------------------------------------------------|
SUB Interrupt
/stopcast
/varset castReturn CANCELLED
/RETURN



|------------------------------------|
|- Events to check while casting.	-|
|------------------------------------|
SUB Casting_Events
/doevents BeginCast
/doevents Recover
/doevents Fizzle
/doevents Interrupt
/doevents Standing
/doevents FDFail
/doevents OutOfRange
/doevents OutOfMana
/doevents NoLOS
/doevents Resisted2
/doevents Resisted
/doevents Immune
/doevents Stunned
/doevents Collapse
/doevents NoTarget
/doevents NotReady
/doevents NoHold
/doevents outDoor
/doevents PopUp_Resist
/doevents PopUp_Immune
/doevents PopUp_Oom
/doevents PopUp_Range
/doevents PopUp_LoS
/doevents PopUp_Landed
/RETURN



|--------------------------------------------------------------------------------------------|
|- castReturn begins set as ${pendingCast}_SUCCESS, until changed by another casting event.	-|
|--------------------------------------------------------------------------------------------|
#event BeginCast "You begin casting#*#"
SUB Event_BeginCast
/varset castReturn SUCCESS
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_COLLAPSE.	-|
|------------------------------------------------|
#event Collapse "Your gate is too unstable, and collapses.#*#"
SUB Event_Collapse
/varset castReturn COLLAPSE
/varset giveUpTimer 200
/RETURN



|------------------------------------------------------------------------|
|- Sets castReturn to ${pendingCast}_RESTART, after you feign death.	-|
|------------------------------------------------------------------------|
#event FDFail "#1# has fallen to the ground.#*#"
SUB Event_FDFail(line, name)
/if (${name.Equal[${Me.Name}]} && ${Defined[castReturn]}) {
  /if (!${Me.Standing}) /stand
  /varset castReturn RESTART
}
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_FIZZLE.	-|
|------------------------------------------------|
#event Fizzle "Your spell fizzles#*#"
SUB Event_Fizzle
/varset castReturn FIZZLE
/RETURN



|------------------------------------------------|
|- Sets castReturn to IMMUNE.	-|
|------------------------------------------------|
#event Immune "Your target is immune to changes in its attack speed#*#"
#event Immune "Your target is immune to changes in its run speed#*#"
#event Immune "Your target cannot be mesmerized#*#"
SUB Event_Immune
/varset castReturn IMMUNE
/RETURN



|----------------------------------------------------|
|- Sets castReturn to ${pendingCast}_INTERRUPTED.	-|
|----------------------------------------------------|
#event Interrupt "Your casting has been interrupted#*#"
#event Interrupt "Your spell is interrupted#*#"
SUB Event_Interrupt
/if (${castReturn.NotEqual[CANCELLED]}) /varset castReturn INTERRUPTED
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_CANNOTSEE.	-|
|------------------------------------------------|
#event NoLOS "You cannot see your target.#*#"
SUB Event_NoLOS
/varset castReturn CANNOTSEE
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_NOTARGET.	-|
|------------------------------------------------|
#event NoTarget "You must first select a target for this spell!#*#"
SUB Event_NoTarget
/varset castReturn NOTARGET
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_NOTREADY.	-|
|------------------------------------------------|
#event NotReady "Spell recast time not yet met.#*#"
#event NotReady "You can use this ability again in #*#"
SUB Event_NotReady
/varset castReturn NOTREADY
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_OUTOFMANA.	-|
|------------------------------------------------|
#event OutOfMana "Insufficient Mana to cast this spell!#*#"
SUB Event_OutOfMana
/varset castReturn OUTOFMANA
/RETURN



|----------------------------------------------------|
|- Sets castReturn to ${pendingCast}_OUTOFRANGE.	-|
|----------------------------------------------------|
#event OutOfRange "Your target is out of range, get closer!#*#"
SUB Event_OutOfRange
/varset castReturn OUTOFRANGE
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_RECOVER.	-|
|------------------------------------------------|
#event Recover "You haven't recovered yet...#*#"
#event Recover "Spell recovery time not yet met#*#"
SUB Event_Recover
/varset castReturn RECOVER
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_RESISTED.	-|
|------------------------------------------------|
#EVENT Resisted "Your target resisted the #1# spell#*#"
SUB Event_Resisted(line,  name)
/varset castReturn RESISTED
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_INTERRUPT.	-|
|------------------------------------------------|
#EVENT Standing "You must be standing to cast a spell#*#"
SUB Event_Standing
/stand
/varset castReturn INTERRUPT
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_STUNNED.	-|
|------------------------------------------------|
#EVENT Stunned "You are stunned#*#"
#EVENT Stunned "You can't cast spells while stunned!#*#"
#EVENT Stunned "You *CANNOT* cast spells, you have been silenced!#*#"
SUB EVENT_Stunned
/if (${Debug} || ${Debug_Casting}) /echo |- EVENT_Stunned ==>

	/if (${Me.Stunned}) {
	  /delay 3s !${Me.Stunned}
	}
	/varset castReturn STUNNED

/if (${Debug} || ${Debug_Casting}) /echo <== EVENT_Stunned -|	
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_NOHOLD.	-|
|------------------------------------------------|
#event outDoor "You can only cast this spell in the outdoors."
SUB Event_outDoor
/varset castReturn OUTDOORS
/RETURN



|------------------------------------------------|
|- Sets castReturn to ${pendingCast}_NOHOLD.	-|
|------------------------------------------------|
#event NoHold "Your spell did not take hold."
#event NoHold "Your spell would not have taken hold#*#"
#event NoHold "Your spell is too powerful for your intended target#*#"
SUB Event_NoHold
/varset castReturn NOHOLD
/RETURN



|------------------------|
|- Currently unused.	-|
|------------------------|
#event Missing_Components "You are missing #1#"
SUB event_Missing_Components(line, missingComponent)
| this need to be finished
| /echo ${missingComponent}
/RETURN



Sub casting_Setup
/call INItoVar "${advSettings_Ini},Debug,Debug Casting (On/Off)" Debug_Casting bool outer
/if (${Debug} || ${Debug_Casting}) /echo |- casting_Setup ==>
	
	/declare pendingCast string outer
	/declare pendingSpell string outer
	/declare lastCast string outer
	/declare castReturn string outer
	/declare castEndTime timer outer
	/declare giveUpTimer timer outer
	/declare swapItemBack bool outer FALSE
	/declare itemExchange_Timer timer outer
	/declare altRecast_Timer timer outer
	/declare no_Cast bool outer FALSE
	/declare badSpells string outer
	
	| Add Casting File Paths
	/if (!${Ini[${MacroData_Ini},File Paths,Item Timer Settings].Length}) /call WriteToIni "${MacroData_Ini},File Paths,Item Timer Settings" "e3 Macro Inis\Item Timers.ini"
	
	| Import itemTimers_Ini.
	/if (!${Ini[${MacroData_Ini},File Paths,Item Timer Settings].Length}) {
		/echo ERROR: Could not find designated file path for [Item Timer Settings], please review [${MacroData_Ini}] and restart.
		/endmacro
	} else {
		/declare itemTimers_Ini string outer ${Ini[${MacroData_Ini},File Paths,Item Timer Settings]}
	}
	
	| Generate an itemTimers_Ini.
	/if (!${Ini[${itemTimers_Ini},A].Length}) {
		/echo Creating item timer settings file...
		/call Build_Alphabetized_Ini "${itemTimers_Ini}"
	}

| Import Casting Settings.
	
	| Default Spellset
	/if (${Ini[${genSettings_Ini},Casting,Default Spell Set].Length}) /call IniToVar "${genSettings_Ini},Casting,Default Spell Set" Default_SpellSet string outer
	
	| Default Gems
	/declare DefaultGem int outer 9
	/call IniToVar "${genSettings_Ini},Casting,Default Spell Gem" DefaultGem int outer
	/if (${DefaultGem}==9 && !${Me.AltAbility[Mnemonic Retention]}) /varset DefaultGem 8

/if (${Debug} || ${Debug_Casting}) /echo <== casting_Setup -|
/return



Sub casting_Background_Events

/return



SUB casting_MacroSettings
/if (${Debug} || ${Debug_Casting}) /echo |- casting_MacroSettings ==>

	/call WriteToIni "${advSettings_Ini},Debug,Debug Casting (On/Off)"
	/call WriteToIni "${advSettings_Ini},AA-ID Overrides,Improved Familiar" 352,167,52

	/call WriteToIni "${genSettings_Ini},Casting,Default Spell Set" Default
	/call WriteToIni "${genSettings_Ini},Casting,Default Spell Gem" ${If[${Me.AltAbility[Mnemonic Retention]},9,8]}
	/call WriteToIni "${genSettings_Ini},Casting,Give Up Casting Time" 15

/if (${Debug} || ${Debug_Casting}) /echo <== casting_MacroSettings -|
/RETURN



SUB casting_CharacterSettings
/RETURN



Sub casting_Aliases
/return