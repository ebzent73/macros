|--------------------------------------------------------------------|
|- e3_Macro_Assists.inc	( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ										-|
|--------------------------------------------------------------------|
|- Contains assisting related functions for e3.mac.					-|
|--------------------------------------------------------------------|



|--------------------------------------------------------------------------------|
|- Engages assist functions on ChatSender, and bots who are in the same zone.	-|
|--------------------------------------------------------------------------------|
|- Use, 'Assist on ${Target.ID}' in /tell, /group, or /bc.						-|
|--------------------------------------------------------------------------------|
#event Assist "<#1#> Assist on #2#"
#event Assist "#1# tell your party, 'Assist on #2#'"
#event Assist "#1# tells the group, 'Assist on #2#'"
#event Assist "#1# tells you, 'Assist on #2#'"
SUB event_Assist(line, ChatSender, int MobID)
/if (${Debug} || ${Debug_Assists}) /echo |- event_Assist ==>
	
	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		|    |--------Included Bot-----------|
		/if (!${Bool[${Macro.Return.Arg[3,_]}]}) {
			/if (!${Defined[PauseTarget]}) /declare PauseTarget int outer ${MobID}
		} else {
	
			/if (${Defined[PauseTarget]}) /deletevar PauseTarget
		
			| Check that you're not assisting on a corpse, pet or pc
			/if (${Spawn[id ${MobID}].Type.Equal[Corpse]} || !${Select[${Spawn[id ${MobID}].Type},${AcceptableTargetTypes}]}) {
				/if (${Toggle-PvP}) /goto :assistAnyway
				/if (${Debug} || ${Debug_Assists}) /echo [${Spawn[id ${MobID}].CleanName}] is not an acceptable TargetType.
			} else {
		
				:assistAnyway
				
				| Check target distance.
				/if (${Spawn[${MobID}].Distance} > ${MaxAssistDistance}) {
					/docommand ${ChatToggle} ${Spawn[${MobID}].CleanName} is too far away.
				} else {
				
					| Wait to catch up to group, before engage.
					/if (${Following} && ${Spawn[${FollowTarget}].Distance} > ${LeashLength} && ${Me.Moving}) {
						/declare Assist_Moving_Timer timer local 100
						:movetoAssist_Loop
						/call Background_Events
						/if (${Spawn[=${FollowTarget}].Distance} > ${LeashLength} && ${Assist_Moving_Timer}) /goto :movetoAssist_Loop
					}

					| Disengage follow.
					/if (${Stick.Active}) /squelch /stick off
					/if (${NetAdvPath.State}) /squelch /netfollow pause
					
					| Set Assist Variables.
					/varset Assisting TRUE
					/varset AssistTarget ${MobID}
					/varset hasTanked FALSE
					
					| If I am the assist caller, or I am feigning, don't allow macro control.
					/if (${ChatSender.Equal[${Me.CleanName}]} || ${Me.Feigning}) {
						/varset AllowControl FALSE
					} else {
						/varset AllowControl TRUE
					}
					
					| If the macro is controlling me, engage.
					/if (${AllowControl}) {

						| Output to user
						/if (${Verbosity}) /docommand ${ChatToggle} Attacking - ${AssistTarget} - ${Spawn[${AssistTarget}].CleanName}
					
						| Target AssistTarget.
						/if (${Target.ID} != ${AssistTarget}) /call TrueTarget ${AssistTarget}
						
						| Melee / Ranged assist
						/if (${AssistType.Equal[melee]}) {
						
							/attack on
							/if (${Int[${AssistDistanceSetting}]}) {
								/varset AssistStickDistance ${AssistDistanceSetting}
							} else {
								/if (${AssistDistanceSetting.Equal[MaxMelee]} || ${AssistDistanceSetting.Equal[auto]}) /call Get_AssistStickDistance
							}

							/call CreateTimer getCloserTimer 70
							/call StickToAssistTarget
							
						} else /if (${AssistType.Equal[ranged]}) {
							/autofire on
							/squelch /stick hold moveback ${RangedStickDistance}
						}
						
						| Send in pets.
						/if (${Me.Pet.ID}) {
							/if (${Target.ID} != ${AssistTarget}) /call TrueTarget ${AssistTarget}
							/delay 3
							/if (${Debug} || ${Debug_Assists}) /echo Pet Check: Sending pet in
							/pet attack
						}
					}
					
					/varset combatTimer ${combatDelay}
					/if (${Debug} || ${Debug_Assists}) /echo Assist=${Assisting} || AssistTarget=${AssistTarget} || AllowControl=${AllowControl}
				}
			}
		}
	}
	
/if (${Debug} || ${Debug_Assists}) /echo <== event_Assist -|
/RETURN



|--------------------------------------------------------------------|
|- Backs off an AssistTarget, and labels them as a 'PauseTarget'.	-|
|- You must manually call engage to re-engage a PauseTarget.		-|
|--------------------------------------------------------------------|
|- Use, '/backoff', or, 'Back off' in /tell, /group, or /bc.		-|
|--------------------------------------------------------------------|
#event BackOff "<#1#> Back Off"
#event BackOff "#1# tells the group, 'Back Off'"
#event BackOff "#1# tells you, 'Back Off'"
SUB Event_BackOff(line, ChatSender)
/if (${Debug} || ${Debug_Assists}) /echo |- Event_BackOff ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {

		/if (${Assisting} && ${AllowControl} && ${Verbosity}) /docommand ${ChatToggle} Backing off.
		/if (!${Defined[PauseTarget]}) /declare PauseTarget int outer ${AssistTarget}
		/if (${Me.Pet.ID}) /pet back off
		/if (${Me.Combat}) /attack off
		/if (${Me.AutoFire}) /autofire off
		/if (${Debuff_Targets.Size}) /deletevar Debuff_Targets
		/if (${DoT_Targets.Size}) /deletevar DoT_Targets
		/delay 30 !${Me.Casting.ID} && !${Me.Combat} && !${Me.AutoFire}
		/call AssistOff
	}

/if (${Debug} || ${Debug_Assists}) /echo <== Event_BackOff -|
/RETURN



|----------------------------------------------------------------|
|- Turns off Assist_Functions, and resets assisting variables.	-|
|----------------------------------------------------------------|
SUB AssistOff
/if (${Debug} || ${Debug_Assists}) /echo |- AssistOff -| ==>
	
	| Interrupt spells, turn off attack.
	/if (${AllowControl}) {
		/if (${Me.Casting.ID} && !${Spell[${Me.Casting}].SpellType.Find[beneficial]} && ${Me.Class.ShortName.NotEqual[BRD]}) /stopcast
		/if (${Me.Combat}) /attack off
		/if (${Me.AutoFire})  /autofire off
	}
	
	| Reset assist variables.
	/varset Assisting FALSE
	/varset AssistTarget 0
	/varset AllowControl FALSE
	/varset hasTanked FALSE
	/varset enraged FALSE
	/varset enrageAttack FALSE
	
	| Reset Burns
	/if (${Defined[use_QuickBurns]}) /deletevar use_QuickBurns
	/if (${Defined[use_FullBurns]}) /deletevar use_FullBurns
	
	| Check loot, and reacquire follow.
	/if (${Select[${Stick},ON]}) /squelch /stick off
	/if (${Following}) /call AquireFollow
	/doevents YourKill
	
/if (${Debug}) {
	/echo Assisting: [${Assisting}] || AssistTarget: [${AssistTarget}]
	/echo |- AssistOff -| <==
}
/RETURN



|--------------------------------------------------------|
|- Calculates a stick distance to your AssistTarget.	-|
|--------------------------------------------------------|
SUB Get_AssistStickDistance
/if (${Debug} || ${Debug_Assists}) /echo |- Set_AssistStickDistance ==>

	/varset AssistStickDistance ${Math.Calc[${Spawn[${Target.ID}].MaxRangeTo}*.8].Int}

	| Check MaxRangeTo, make sure it is not out of bounds.
	/if (${AssistStickDistance} > 25) /varset AssistStickDistance 25

/if (${Debug}) {
	/echo AssistStickDistance = [${AssistStickDistance}]
	/echo <== Set_AssistStickDistance -|
}
/RETURN



|----------------------------------------|
|- Engages /stick on an AssistTarget.	-|
|----------------------------------------|
SUB StickToAssistTarget
/if (${Debug} || ${Debug_Assists}) /echo |- StickToAssistTarget ==>

	/if (!${hasTanked}) {
		/if (${AssistStickPoint.Equal[behind]}) {
			
			/if (${Debug} || ${Debug_Assists}) /echo Attempting to stick behind [${AssistStickDistance}].
			/squelch /stick moveback snaproll 15 uw
			/delay 10 !${Me.Moving} || ${Target.Distance} < 15
			/squelch /stick hold moveback behind ${AssistStickDistance} uw	
			/delay 10 ${Me.Moving}
			
		} else /if (${AssistStickPoint.Equal[behindonce]}) {
		
			/if (${Debug} || ${Debug_Assists}) /echo Attempting to stick behindonce [${AssistStickDistance}].
			/squelch /stick moveback snaproll 15 uw
			/delay 10 !${Me.Moving} || ${Target.Distance} < 20
			/squelch /stick hold moveback behindonce ${AssistStickDistance} uw
			/delay 10 ${Me.Moving}
			
		} else /if (${AssistStickPoint.Equal[pin]}) {
		
			/if (${Debug} || ${Debug_Assists}) /echo Attempting to stick pin [${AssistStickDistance}].
			/squelch /stick moveback snaproll 15 uw
			/delay 10 !${Me.Moving} || ${Target.Distance} < 20
			/squelch /stick hold moveback pin ${AssistStickDistance} uw
			/delay 10 ${Me.Moving}
			
		} else /if (${AssistStickPoint.Equal[front]}) {
		
			/if (${Debug} || ${Debug_Assists}) /echo Attempting to stick front [${AssistStickDistance}].
			/squelch /stick hold front ${AssistStickDistance} uw
			/delay 10 ${Me.Moving}
			
		} else /if (${AssistStickPoint.Equal[!front]}) {
			
			/if (${Debug} || ${Debug_Assists}) /echo Attempting to stick !front [${AssistStickDistance}].
			/squelch /stick moveback snaproll 15 uw
			/delay 10 !${Me.Moving} || ${Target.Distance} < 20
			/squelch stick hold moveback !front ${AssistStickDistance} uw
			/delay 10 ${Me.Moving}
		} else {

			/if (${Debug} || ${Debug_Assists}) /echo [${AssistStickPoint}] is not a valid stick point.  Defaulting to stick behind.
			/squelch /stick moveback snaproll 15 uw
			/delay 10 !${Me.Moving} || ${Target.Distance} < 15
			/squelch /stick hold moveback behind ${AssistStickDistance} uw	
			/delay 10 ${Me.Moving}
		}
	}
	
	/declare miscTimer timer local 50
	:waitToStop
	/if (${Debug} && ${spamTimer_waitToStop}) {
		/echo |- StickToAssistTarget -| :waitToStop
		/call createTimer spamTimer_waitToStop 30
	}

	/call Background_Events
	
	/if (${Me.Moving}) {
		/if (${miscTimer}) /goto :waitToStop
	}

/if (${Debug} || ${Debug_Assists}) /echo <== StickToAssistTarget -|	
/RETURN



|----------------------------------------------------|
|- Checks range and combat status while assisting.	-|
|----------------------------------------------------|
SUB check_AssistStatus
/if (${Debug} || ${Debug_Assists}) {
	/echo |- check_AssistStatus ==>
	/echo AssistTarget = [${AssistTarget}] || AllowControl = [${AllowControl}] || Distance check = [${Spawn[id ${AssistTarget}].Distance} < ${MaxAssistDistance}] || TargetType = [${Spawn[id ${AssistTarget}].Type}]
}
	
	| Check range.
	/if (${Spawn[${AssistTarget}].Distance} > ${MaxAssistDistance}) {
		/if (${Debug} || ${Debug_Assists}) /echo [${Spawn[${AssistTarget}].CleanName} - ${Spawn[${AssistTarget}].ID}] is out of range,  holding assist functions.
	} else {
	
		| Check feigning
		/if (${Me.Feigning}) {
			/if (${Debug} || ${Debug_Assists}) /echo I am feigned, holding assist functions. 
		} else  {
	
			| Check AllowControl bool.
			/if (!${AllowControl}) {
			
			/if (${Debug} || ${Debug_Assists}) /echo Macro control is not allowed.  Assist functions are restricted. 
				| Check enrage when enrageAttack is on
				/if (${enraged} && ${enrageAttack} && !${Stick.Behind}) /attack off
				
			} else {
			
				| Check Target.
				/if (${Target.ID}!=${AssistTarget} && ${Target.PctHPs} && !${Spawn[${AssistTarget}].Type.Equal[Corpse]}) /call TrueTarget ${AssistTarget}
			
				| ---------------------------------------------------------------------------------------------------------------------------Melee functions
				/if (${AssistType.Equal[Melee]}) {
					/if (!${Me.AutoFire}) {
						
						| Check Target of Target, and who is tanking
						/if (!${hasTanked}) {
							/if (${Me.TargetOfTarget.CleanName.Equal[${Me.CleanName}]} && !${Bool[${Int[${Target.Speed}]}]}) {	
								/if (${Debug} || ${Debug_Assists}) /echo Setting hasTanked to TRUE, sticking without position.
								/varset hasTanked TRUE
								/stick off
								/if (${Target.Distance} > ${AssistStickDistance}) /stick hold moveback ${AssistStickDistance}
							}
						} else {
							
							| If the mob is not targeting me, or is moving
							/if (!${Me.TargetOfTarget.CleanName.Equal[${Me.CleanName}]} || ${Target.Speed}) {
								/if (${Debug} || ${Debug_Assists}) /echo I've tanked, but mob is no longer targeting me or is moving. Sticking normally.
								/varset hasTanked FALSE
								/if (${Target.Distance} > ${AssistStickDistance}) /call StickToAssistTarget
							}
						}
						
						| If mob is not enraged and I'm not attacking, start attacking.
						/if (!${enraged}) {
							/if (!${Me.Combat}) /attack on
						
						| If mob is enraged
						} else {
							
							| If enrageAttack is FALSE turn off attack
							/if (!${enrageAttack}) {
								/if (${Me.Combat}) /attack off
							} else {
							
								| Check if the mob is looking at me
								/call check_IsLookingAt
								
								| If the mob IS looking at me
								/if (${Macro.Return}) {
									/if (${Debug} || ${Debug_Assists}) /echo I am not behind the target. Stopping attack.
									/attack off
								} else /if ((!${Macro.Return} || ${Target.Speed}) && !${Me.Combat}) {
									/if (${Debug} || ${Debug_Assists}) /echo I am behind the target. Attacking.
									/attack on
								}
							}
						}
					}
					
				| --------------------------------------------------------------------------------------------------------------------------Ranged functions
				} else /if (${AssistType.Equal[Ranged]}) {
					/if (!${Me.Combat} && ${Target.Distance} < 35 || ${Target.Distance} > ${RangedStickDistance}) /squelch /stick hold moveback ${RangedStickDistance}
					/if (!${Me.AutoFire}) /autofire on
				}
			}
		}
		/varset combatTimer ${combatDelay}
	}

/if (${Debug} || ${Debug_Assists}) /echo <== check_AssistStatus -|
/RETURN



|----------------------------------------------------------------|
|- Checks to see if the AssistTarget has died, calls Assistoff.	-|
|----------------------------------------------------------------|
SUB check_AssistTarget

	/if (${Spawn[${AssistTarget}].Type.Equal[Corpse]} || ${Select[${Spawn[${AssistTarget}].Type.Equal[Corpse]},NULL]}) /call AssistOff
	/varset combatTimer ${combatDelay}

/RETURN



|--------------------------------------------------------|
|- This script was taken from,							-|
|- genbot.mac											-|
|- botcombat.inc										-|
|- Bot combat module.									-|
|- Version 13.4.0										-|
|- Date:07/08/2005										-|
|- Originally by ascii38, maintained by Mortefreddo.	-|
|- This script has been modified to work with e3.mac.	-|
|--------------------------------------------------------|
SUB check_IsLookingAt
/if (${Debug} || ${Debug_Assists}) /echo |- CheckIsLookingAt ==>

	/declare MobHeading int local 0 
	/declare HeadingToPC int local 
	/declare DeltaX local float 
	/declare DeltaY local float 
	/declare HeadingDelta local float
	/declare FacingMe int local 0
	
	/varcalc MobHeading ${Spawn[id ${AssistTarget}].Heading.Degrees}
	/varcalc DeltaX ${Spawn[id ${Me.ID}].X}-${Spawn[id ${AssistTarget}].X}
	/varcalc DeltaY ${Spawn[id ${Me.ID}].Y}-${Spawn[id ${AssistTarget}].Y}
	
	/if (${DeltaX}>0) {
		/varcalc HeadingToPC ${Math.Atan[${DeltaY}/${DeltaX}]}+270 
	} else /if (${DeltaX}<0) {
		/varcalc HeadingToPC ${Math.Atan[${DeltaY}/${DeltaX}]}+90 
	} else {
		/if (${DeltaY}>0) {
			/varcalc HeadingToPC 90 
		} else {
			/varcalc HeadingToPC 270 
		}
	}

	/varcalc HeadingDelta ${Math.Abs[${HeadingToPC}-${MobHeading}]}
	
	/if (${HeadingDelta}<4 || ${HeadingDelta}>356) {
		/if (${Debug} || ${Debug_Assists}) /echo |- CheckIsLookingAt -| [${Spawn[id ${AssistTarget}].CleanName}] is facing me.
		/varset FacingMe 1
	}

/if (${Debug} || ${Debug_Assists}) /echo <== CheckIsLookingAt -|
/RETURN ${FacingMe}



#event setEnrageAttack "[MQ2] Setting enrage attack on #1# to #2#"
#event setEnrageAttack "[MQ2] Set enrage attack on #1# to #2#"
SUB Event_setEnrageAttack(line,npcName,enrageSetting)
/if (${Debug} || ${Debug_Assists}) /echo |- EVENT_setEnrageAttack ==>

	| Set enrage attack mode to enrageSetting (ON/OFF) in the Enrage Settings.ini
	| file for npcName.
	/call WriteToIni "${enrageSettings_ini},${Zone.ShortName},${npcName}" "${enrageSetting}" TRUE
	/if (${Debug} || ${Debug_Assists}) /echo Attacking on enrage set to ${enrageSetting} for ${npcName}.

/if (${Debug} || ${Debug_Assists}) /echo <== EVENT_setEnrageAttack -|
/RETURN



|------------------------------------------------------------|
|- Turns off attack when a mob you're attacking enrages		-|
|- if that mob is specified in Enrage Settings.ini.         -|
|------------------------------------------------------------|
#event EnrageOn "#1# has become ENRAGED."
SUB Event_EnrageOn(line, RageOn)
/if (${Debug} || ${Debug_Assists}) /echo |- EVENT_EnrageOn ==>

	| Verify that the enraged npc is what I am currently fighting.
	/if (${Me.Combat} && ${Target.ID} == ${Spawn[${RageOn}].ID} && ${Target.PctHPs} < 20) {
		/varset enraged TRUE
		/if (${Me.Pet.ID}) /pet back off
		/if (${Debug} || ${Debug_Assists}) /echo enraged = ${enraged}
		
		| Check if the entry for this NPC exists
		/if (!${Ini[${enrageSettings_ini},${Zone.ShortName},${Target.CleanName}].Length}) {
			/echo ${Target.CleanName} is not in Enrage Settings.ini.  Defaulting to ON
			/call WriteToIni "${enrageSettings_ini},${Zone.ShortName},${RageOn}" "ON" 0
		}
		
		| If the entry is set to 'OFF'
		/if (!${Ini[${enrageSettings_ini},${Zone.ShortName},${Target.CleanName}].Equal[ON]}) {
			/if (${Debug} || ${Debug_Assists}) /echo Attack through enrage is set to [${Ini[${enrageSettings_ini},${Zone.ShortName},${Target.CleanName}]}] for ${Target.CleanName}.  Turning attack off.
			/attack off
		} else {
			| If the entry exists, and is set to ON, check position.
			/if (${Debug} || ${Debug_Assists}) /echo ${Target.CleanName} found in enrageSettings_ini and set to ON. Checking position.
			/if (!${Stick.Behind}) {			
				/if (${Debug} || ${Debug_Assists}) /echo I am not behind the mob. Turning attack off.
				/attack off
			| Else, I am behind the mob. Keep attacking.
			} else {
				/if (${Debug} || ${Debug_Assists}) /echo I am behind the target. Continuing to attack.
				/varset enrageAttack TRUE
			}
		}
	}

/if (${Debug} || ${Debug_Assists}) /echo <== EVENT_EnrageOn -|
/RETURN



|------------------------------------------------|
|- Turns attack back on, after enrage is over.	-|
|------------------------------------------------|
#event EnrageOff "#1# is no longer enraged."
SUB Event_EnrageOff(line, RageOn)
/if (${Debug} || ${Debug_Assists}) /echo Enrage OFF, turning attack ON
		
		/if (${enraged}) {
			/if (${Target.ID} == ${Spawn[${RageOn}].ID}) {
				/attack on
				/if (${Me.Pet.ID}) /pet attack
				/varset enraged FALSE
			} else {
				/if (${Debug} || ${Debug_Assists}) /echo Not attacking enraged target. Doing nothing.
			}
		} else {
			/if (${Debug} || ${Debug_Assists}) /echo I was not holding for enrage. Doing nothing.
		}
	/if (${enrageAttack}) /varset enrageAttack FALSE

/if (${Debug} || ${Debug_Assists}) /echo <== EVENT_EnrageOff -|
/RETURN



#EVENT getCloser "Your target is too far away, get closer!"
SUB EVENT_getCloser
/if (${Debug} || ${Debug_Assists}) /echo |- EVENT_getCloser ==>

	/if (${Assisting} && ${AllowControl} && !${getCloserTimer}) {
		
		/call CreateTimer getCloserTimer 50
		
		| If the spawn has moved, stick back to the target
		/if (${Spawn[id ${AssistTarget}].Distance} <= ${AssistStickDistanceSetting} && ${AssistStickDistanceSetting} > 12) /varcalc AssistStickDistance ${AssistStickDistance} - 2
		/if (${Debug} || ${Debug_Assists}) /echo AssistStickDistance has been set to [${AssistStickDistance}]
	}

/if (${Debug} || ${Debug_Assists}) /echo <== EVENT_getCloser -|
/RETURN



SUB check_AutoAssist
/if (${Debug} || ${Debug_Assists}) /echo |- check_AutoAssist ==>

	/if (!${Assisting}) {

		| First check if I am attacking something
		/if (${Target.ID} && ${Me.Combat} || ${Target.ID} && ${Spell[${Me.Casting.ID}].SpellType.Equal[Detrimental]}) {
			/if (${Target.PctHPs} < ${EngagePct} && ${Select[${Spawn[id ${Target.ID}].Type},${AcceptableTargetTypes}]} && ${Target.ID} != ${PauseTarget}) {
				/if (${Debug} || ${Debug_Assists}) /echo I am attacking, engaging Assist functions
				/if (${NetBots.Client.Arg[${i}, ].Equal[${Me.CleanName}]}) /delay 15
				/call EVENT_Assist "<${Me.CleanName}> Assist on ${Target.ID}" ${Me.CleanName} ${Target.ID}
			}
		
		| Check all other netbots
		} else {
		
			/declare i int local
			/for i 1 to ${NetBots.Counts}

				/if (${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetID} && ${NetBots[${NetBots.Client.Arg[${i}, ]}].Attacking} || ${Spell[${NetBots[${NetBots.Client.Arg[${i}, ]}].Casting}].SpellType.Equal[Detrimental]}) {
					/if (!${Spawn[id ${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetID}].LineOfSight}) {
						/if (${Debug} || ${Debug_Assists}) /echo I do not have line of sight on [${Spawn[id ${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetID}].CleanName}].  Holding auto assist
					} else {
				
						/if (${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetHP} < ${EngagePct} && ${Select[${Spawn[id ${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetID}].Type},${AcceptableTargetTypes}]} && ${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetID} != ${PauseTarget} && ${Spawn[id ${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetID}].Distance} < ${MaxAssistDistance}) {
							/if (${Debug} || ${Debug_Assists}) /echo ${NetBots[${NetBots.Client.Arg[${i}, ]}]} is attacking, assisting...
							/call EVENT_Assist PLACEHOLDER ${NetBots.Client.Arg[${i}, ]} ${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetID}
						}
					}
				}
			
			/if (!${Assisting}) /next i
		}
	}

/if (${Debug} || ${Debug_Assists}) /echo <== check_AutoAssist -|
/RETURN



|------------------------------------------------|
|- Uses combat abilities, AAs, and disciplines.	-|
|------------------------------------------------|
SUB CombatAbilities
/if (${Debug} || ${Debug_Assists}) /echo |- CombatAbilities ==>

	| Check Target distance.
	/if (${Target.Distance} > ${Spawn[${Target.ID}].MaxRangeTo}) {
		/if (${Debug} || ${Debug_Assists}) /echo I am too far away from [${Spawn[id ${AssistTarget}].CleanName}] to use abilities.
	} else {
	
		/declare abilityName string local
		/declare minEnd int local
		/declare i int local
	
		| For 1 to the size of MyAbilities array, use each ability.
		/for i 1 to ${MyAbilities.Size}
			
			/varset abilityName ${MyAbilities[${i}].Arg[1,/]}
			
			| Check for MinEnd
			/if (${MyAbilities[${i}].Find[/MinEnd|]}) {
				/call argueString MinEnd "${MyAbilities[${i}]}"
				/varset minEnd ${Macro.Return}
			} else {
				/varset minEnd 0
			}
	
			/if (${Me.PctEndurance} < ${minEnd}) {
				/if (${Debug} || ${Debug_Assists}) /echo I don't have enough endurance to use [${abilityName}].
			} else {
			
				| Use ability. 
				/if (${Me.Ability[${abilityName}]}) {
					
					/if (!${Me.AbilityReady[${abilityName}]}) {
						/if (${Debug} || ${Debug_Assists}) /echo Ability [${abilityName}] is not ready.
					} else {
				
						| Use Backstab.
						/if (${abilityName.Equal[Backstab]}) {
							/call Attempt_Backstab
							
						| Use Bash.
						} else /if (${abilityName.Equal[Bash]}) {
							/call Attempt_Bash
							
						| Use Taunt.
						} else /if (${abilityName.Equal[Taunt]}) {
							/if (${AutoAggro}) /call Attempt_Taunt
						
						| Reset Tail Rake to Dragon Punch.  Tail Rake causes an error because it returns as "Dragon Punch" when /echoed in Me.Ability[#]
						} else /if (${abilityName.Equal[Tail Rake]}) {
							/varset ${MyAbilities[${i}]} Dragon Punch
						
						| Use a misc Ability.
						} else {
						
							/doability "${abilityName}"
						}
					}	
					
				| Use alternate ability
				} else /if (${Me.AltAbility[${abilityName}]}) {
					/if (!${Me.AltAbilityReady[${abilityName}]} || ${altTimer_${Me.AltAbility[${abilityName}].ID}}) {
						/if (${Debug} || ${Debug_Assists}) /echo Alternate Ability [${abilityName}] is not ready.
					} else {
						/call e3_Cast ${AssistTarget} "${abilityName}"
					}
				
				| Use combat ability (discipline)
				} else /if (${Me.CombatAbility[${abilityName}]}) {
					/if (!${Me.CombatAbilityReady[${abilityName}]}) {
						/if (${Debug} || ${Debug_Assists}) /echo Discipline [${abilityName}] is not ready.
					} else {
						
						| Rogue Assassin's Strike.
						/if (${Select[${abilityName},${rogueStrikes}]}) {
							/call check_rogStrike "${abilityName}"
						
						| Warrior provoke.
						} else /if (${Select[${abilityName},${warriorBellows}]}) {
							/if (${AutoAggro}) /call check_warProvoke "${abilityName}"
							
						| All other disciplines.
						} else {
							/call e3_Cast ${AssistTarget} "${abilityName}/Instant"
						}
					}
				| Use rogue evade as a combat ability 
				} else /if (${abilityName.Equal[Evade]}) {
					/if (${Me.Ability[Hide]}) /call AutoEvade
				}
			}
		/next i
	}

/if (${Debug} || ${Debug_Assists}) /echo <== CombatAbilities -|
/RETURN



SUB Attempt_Bash
/if (${Debug} || ${Debug_Assists}) /echo |- Attempt_Bash ==>

	/if (!${Select[${Me.Inventory[Offhand].Type},Shield]} && !${Me.AltAbility[2 Hand Bash]}) {
		/if (${Debug} || ${Debug_Assists}) /echo You must equip a shield, or train in the alternate ability "2 Hand Bash" to use the ability [Bash].
	} else {
		/doability Bash
	}
	
/if (${Debug} || ${Debug_Assists}) /echo <== Attempt_Bash -|
/RETURN



SUB Attempt_Taunt
/if (${Debug} || ${Debug_Assists}) /echo |- Attempt_Taunt ==> 

	/if (${Select[${Me.TargetOfTarget},${Me.CleanName}]}) {
		| /if (${Me.Skill[Taunt]} < Find max skill formula) /goto :tauntAnyway
		/if (${Debug} || ${Debug_Assists}) /echo I currently have aggro on [${Target.CleanName}], skipping taunt.
	} else {
		:tauntAnyway
		/doability Taunt
	}

/if (${Debug} || ${Debug_Assists}) /echo <== Attempt_Taunt -|
/RETURN



|--------------------------------------------------------------------------------|
|- Attempt_Backstab																-|
|--------------------------------------------------------------------------------|
|- Attempts to arm 'Assassin's Strike' type disciplines before using backstab.	-|
|--------------------------------------------------------------------------------|
Sub Attempt_Backstab
/if (${Debug} || ${Debug_Assists}) /echo |- Attempt_Backstab ==>

	| Check that I have a piercer in my main hand
	/if (${Me.Inventory[Mainhand].Type.NotEqual[Piercing]}) {
		/if (${Debug} || ${Debug_Assists}) /echo I need a piercing weapon in my main hand to backstab...
	} else {
		
		| If backstab is ready, use it.
		/if (${Me.AbilityReady[Backstab]}) /doability Backstab
	}

/if (${Debug} || ${Debug_Assists}) /echo <== Attempt_Backstab -|
/return



|--------------------------------------------------------|
|- Casts direct damage spells on a specified target.	-|
|--------------------------------------------------------|
SUB check_Nukes
/if (${Debug} || ${Debug_Assists}) /echo |- check_Nukes ==>

	/if (${MyNukes.Size} && ${Assisting} && ${use_Nukes}) {
		
		/if (${delayTimer_MyNukes}) {
			/if (${Debug} || ${Debug_Assists}) /echo Waiting [${delayTimer_MyNukes}] to cast next nuke.
		} else {
		
			/if (${Target.ID} != ${AssistTarget}) {
				/if (${Debug} || ${Debug_Assists}) /echo I am not targeting the assistTarget, holding nukes.
			} else {
			
				/declare castName string local
			
				/if (!${Defined[nukeCounter]}) {
					/declare nukeCounter int outer 1
				} else {
					/if (${nukeCounter} > ${MyNukes.Size}) /varset nukeCounter 1
				}
				
				/varset castName ${MyNukes[${nukeCounter}].Arg[1,/]}
				/call e3_Cast ${AssistTarget} "${MyNukes[${nukeCounter}]}${If[${Me.Book[${castName}]},/GiveUpTimer|${If[${Me.Gem[${castName}]} || ${Me.Combat},0,90]},]}"
				
				| If cast was a success, update 
				/if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[SUCCESS]}) {
					
					| /Delay| Check.
					/if (${MyNukes[${nukeCounter}].Find[/Delay|]}) {
						/call argueString Delay "${MyNukes[${nukeCounter}]}"
						/call createTimer "delayTimer_MyNukes" "${Int[${Macro.Return}]}0"
					}
					
					| Update nukeCounter
					/varcalc nukeCounter ${nukeCounter} + 1
				} else /if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[NOTREADY]}) {
					| Update nukeCounter, skip to next nuke
					/varcalc nukeCounter ${nukeCounter} + 1
				}
			}
		}
	}

/if (${Debug} || ${Debug_Assists}) /echo <== check_Nukes -|
/RETURN



SUB check_Debuffs
/if (${Debug} || ${Debug_Assists}) /echo |- check_Debuffs ==>
	
	/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
	/if (${Assisting} && ${Assist_Debuffs.Size}) /call cast_longTermSpells "${AssistTarget}" "Assist_Debuffs"

/if (${Debug} || ${Debug_Assists}) /echo <== check_Debuffs -|
/RETURN



SUB check_DoTs
/if (${Debug} || ${Debug_Assists}) /echo |- check_DoTs ==>
	
	/if (${DoT_Targets.Size}) /call DoTs_OnCommand
	/if (${Assisting} && ${Assist_DoTs.Size}) /call cast_longTermSpells "${AssistTarget}" "Assist_DoTs"

/if (${Debug} || ${Debug_Assists}) /echo <== check_DoTs -|
/RETURN



|----------------------------------------------------|
|- Casts debuffs and dots on various assist targets -|
|----------------------------------------------------|
SUB cast_longTermSpells(int spellTarget, string arrayName)
/if (${Debug} || ${Debug_Assists}) /echo |- cast_longTermSpells - [${arrayName}] ==>

	/declare castName string local
	/declare spellName string local
	
	/declare spellTimer string local
	/declare newTimerSetting int local
	
	/declare i int local
	
	/for i 1 to ${${arrayName}.Size}
	
		/varset castName ${${arrayName}[${i}].Arg[1,/]}
		/varset spellName ${If[${FindItemCount[=${castName}]},${FindItem[=${castName}].Spell},${castName}]}
		/varset spellTimer spellTimer_${spellTarget}_${arrayName}-${i}
		/varset newTimerSetting ${Math.Calc[${Spell[${spellName}].Duration} * 60].Int}
		
		/if (${${spellTimer}}) {
			/if (${Debug} || ${Debug_Assists}) /echo |- cast_longTermSpells - [${arrayName}] -|	Waiting to recast [${castName}].
		} else {
		
			| cast the spell
			/call e3_Cast ${spellTarget} "${${arrayName}[${i}]}${If[${Me.Book[${castName}]},/GiveUpTimer|${If[${Me.Gem[${castName}]},3s,${If[${Me.Combat} || ${AssistType.Equal[melee]},0s,${Math.Calc[${Spell[${castName}].RecastTime} + 5].Int}s]}]},]}"
			
			| If the spell was resisted.
			/if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[RESISTED]}) {
				/if (${Target.ID}) /docommand ${ChatToggle} **Warning** ${Spawn[id ${spellTarget}].CleanName} resisted ${If[${castType.Equal[item]},${FindItem[=${castName}].Spell},${castName}]}.

			| If the spell did not take hold.
			} else /if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[NOHOLD]}) {
				/call createTimer "${spellTimer}" "30m"
				
			| If the target was immune.
			} else /if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[IMMUNE]}) {
				/if (${Target.ID}) /docommand ${ChatToggle} **Warning** ${Spawn[id ${spellTarget}].CleanName} is IMMUNE to ${If[${castType.Equal[item]},${FindItem[=${castName}].Spell},${castName}]}.
				/call createTimer "${spellTimer}" "30m"
				
			| If the cast was successful.
			} else /if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[SUCCESS]}) {
				/if (${Verbosity} && ${Target.ID}) /docommand ${ChatToggle} Landed ${If[${castType.Equal[item]},${FindItem[=${castName}].Spell},${castName}]} on ${Spawn[id ${spellTarget}].CleanName}!!.
				/if (${Debug} || ${Debug_Assists}) /echo |- longTermSpells - [${arrayName}] -| Creating timer [${spellTimer}] -- [${newTimerSetting}]
				/if (${longTerm_recastBuffer}) /varcalc newTimerSetting ${newTimerSetting} - ${longTerm_recastBuffer}
				/call createTimer "${spellTimer}" "${newTimerSetting}"
			
			| If the spell was not ready
			} else /if (${lastCast.Equal[${castName}]} && ${castReturn.Equal[NOTREADY]}) {
				
				/if (${FindItemCount[=${castName}]} || ${Me.AltAbility[${castName}]}) {
					/if (${Debug} || ${Debug_Assists}) /echo |- longTermSpells - [${arrayName}] -| [${castName}] is not ready.  Creating a recast timer for 1/2 the spells normal duration.
					/varcalc newTimerSetting ${newTimerSetting} / 2
					/call createTimer "${spellTimer}" "${newTimerSetting}"
				}
			}
		}

	/if (!${ActionTaken}) /next i

/if (${Debug} || ${Debug_Assists}) /echo <== cast_longTermSpells - [${arrayName}] -|
/RETURN



|---------------------------------------------------------------------------------------------|
|- Validates targets and adds them to a list of Debuff_Targets. Used for Debuffs_OnCommand.	 -|
|---------------------------------------------------------------------------------------------|
|- Use, '/Debuff' or 'Debuffs on ${Target.ID}' in /tell, /group, or /bc.					 -|
|---------------------------------------------------------------------------------------------|
#EVENT DebuffsOn "<#1#> Debuffs on #2#"
#EVENT DebuffsOn "#1# tells you, 'Debuffs on #2#'"
#EVENT DebuffsOn "#1# tells the group, 'Debuffs on #2#'"
 SUB Event_DebuffsOn(line, ChatSender, MobID)
/if (${Debug} || ${Debug_Assists}) /echo |- event_DebuffsOn ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| Check if this character debuffs to cast.
		/if (${Command_Debuffs.Size}) {
			
			| Check if the target is a corpse
			/if (${Spawn[id ${MobID}].Type.Equal[Corpse]}) {
				/echo Why would I debuff a corpse?
			} else {
			
				| Check range to the debuff target.
				/if (${Spawn[id ${MobID}].Distance} > ${MaxAssistDistance}) {
					/docommand ${ChatToggle} ${Spawn[${MobID}].CleanName} is too far away.
				} else {
				
					| Check if the Debuff_Targets array is already defined
					/if (${Debuff_Targets.Size}) {
						/declare i int local
						/for i 1 to ${Debuff_Targets.Size}
							/if (${Debuff_Targets[${i}].Arg[1,_]} == ${MobID}) {
								/docommand ${ChatToggle} I am already debuffing ${Spawn[id ${MobID}].CleanName}.
								/goto :Skip
							}
						/next i
					}
				
					| If the target was not found on the DebuffTargets array, add it.
					/call BuildArray Debuff_Targets ${MobID}_${Zone.ID}
				}
			}
		}
	}

	:Skip
	
/if (${Debug} || ${Debug_Assists}) /echo <== event_DebuffsOn -|
/RETURN



|------------------------------------------------|
|- Cancels debuffs on command on all targets. 	-|
|------------------------------------------------|
#EVENT DebuffsOff "<#1#> End debuffs"
#EVENT DebuffsOff "#1# tells you, 'End Debuffs'"
#EVENT DebuffsOff "#1# tells the group, 'End Debuffs'"

#EVENT DebuffsOff "<#1#> End debuffs on #2#"
#EVENT DebuffsOff "#1# tells you, 'End Debuffs on #2#'"
#EVENT DebuffsOff "#1# tells the group, 'End Debuffs on #2#'"

#EVENT DebuffsOff "<#1#> End debuffs |-|#*#"
#EVENT DebuffsOff "#1# tells you, 'End Debuffs |-|#*#'"
#EVENT DebuffsOff "#1# tells the group, 'End Debuffs |-|#*#'"

#EVENT DebuffsOff "<#1#> End debuffs on #2# |-|#*#"
#EVENT DebuffsOff "#1# tells you, 'End Debuffs on #2# |-|#*#'"
#EVENT DebuffsOff "#1# tells the group, 'End Debuffs on #2# |-|#*#'"
SUB event_DebuffsOff(line, ChatSender, MobID)
/if (${Debug} || ${Debug_Assists}) /echo |- event_DebuffsOff ==>

	| If I'm casting debuffs
	/if (${Debuff_Targets.Size}) {

		| -Verify the event
		/call verifyEvent "${ChatSender}" "${line}"
		/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
		|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|
		/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {
		
			/if (${Defined[${MobID}]}) {
				/docommand ${ChatToggle} Ending Debuffs on ${Spawn[id ${MobID}].CleanName}.
				/call RemoveArrayElement Debuff_Targets ${MobID}_${Zone.ID}
			} else {
				/docommand ${ChatToggle} Ending Debuffs on all targets.
				/deletevar Debuff_Targets
			}
		}
	}

/if (${Debug} || ${Debug_Assists}) /echo <== event_DebuffsOff -|
/RETURN



|--------------------------------------------------------------------|
|- Casts Command_Debuffs on targets listed in DebuffTargets array.	-|
|--------------------------------------------------------------------|
SUB Debuffs_OnCommand
/if (${Debug} || ${Debug_Assists}) /echo |- Debuffs_OnCommand ==>

	/declare targetID int local
	/declare targetZone int local
	/declare i int local
	
	/for i 1 to ${Debuff_Targets.Size}
	
		/varset targetID ${Debuff_Targets[${i}].Arg[1,_]}
		/varset targetZone ${Debuff_Targets[${i}].Arg[2,_]}
	
		| Check Mob Zone
		/if (${Zone.ID} != ${targetZone}) {
			/call RemoveArrayElement Debuff_Targets ${Debuff_Targets[${i}]}
		} else {	
			| If the Debuff_Target has died, remove them from the array.
			/if (${Spawn[id ${targetID}].Type.Equal[Corpse]} || ${Select[${Spawn[id ${targetID}].Type.Equal[Corpse]},NULL]}) {
				/call RemoveArrayElement Debuff_Targets ${Debuff_Targets[${i}]}
			} else {
		
				/call cast_longTermSpells "${targetID}" "Command_Debuffs"
		
			}
		}
		
	/if (!${ActionTaken}) /next i
	
/if (${Debug} || ${Debug_Assists}) /echo <== Debuffs_OnCommand -|
/RETURN



|---------------------------------------------------------------------------------------------|
|- Validates targets and adds them to a list of Debuff_Targets. Used for Debuffs_OnCommand.	 -|
|---------------------------------------------------------------------------------------------|
|- Use, '/DoTs' or 'DoTs on ${Target.ID}' in /tell, /group, or /bc.							 -|
|---------------------------------------------------------------------------------------------|
#EVENT DoTsOn "<#1#> DoTs on #2#"
#EVENT DoTsOn "#1# tells you, 'DoTs on #2#'"
#EVENT DoTsOn "#1# tells the group, 'DoTs on #2#'"
SUB Event_DoTsOn(line, ChatSender, MobID)
/if (${Debug} || ${Debug_Assists}) /echo |- event_DoTsOn ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| Check if this character debuffs to cast.
		/if (${Command_DoTs.Size}) {
			
			| Check if the target is a corpse
			/if (${Spawn[id ${MobID}].Type.Equal[Corpse]}) {
				/echo Why would I DoT a corpse?
			} else {
			
				| Check range to the DoTs target.
				/if (${Spawn[id ${MobID}].Distance} > ${MaxAssistDistance}) {
					/docommand ${ChatToggle} ${Spawn[${MobID}].CleanName} is too far away.
				} else {
				
					| Check if the DoT_Targets array is already defined
					/if (${DoT_Targets.Size}) {
						/declare i int local
						/for i 1 to ${DoT_Targets.Size}
							/if (${DoT_Targets[${i}].Arg[1,_]} == ${MobID}) {
								/docommand ${ChatToggle} I am already debuffing ${Spawn[id ${MobID}].CleanName}.
								/goto :Skip
							}
						/next i
					}
				
					| If the target was not found on the DoT Targets array, add it.
					/call BuildArray DoT_Targets ${MobID}_${Zone.ID}
				}
			}
		}
	}

	:Skip
	
/if (${Debug} || ${Debug_Assists}) /echo <== event_DoTsOn -|
/RETURN



|------------------------------------------------|
|- Cancels debuffs on command on all targets. 	-|
|------------------------------------------------|
#EVENT DoTsOff "<#1#> End DoTs"
#EVENT DoTsOff "#1# tells you, 'End DoTs'"
#EVENT DoTsOff "#1# tells the group, 'End DoTs'"

#EVENT DoTsOff "<#1#> End DoTs on #2#"
#EVENT DoTsOff "#1# tells you, 'End DoTs on #2#'"
#EVENT DoTsOff "#1# tells the group, 'End DoTs on #2#'"

#EVENT DoTsOff "<#1#> End DoTs |-|#*#"
#EVENT DoTsOff "#1# tells you, 'End DoTs |-|#*#'"
#EVENT DoTsOff "#1# tells the group, 'End DoTs |-|#*#'"

#EVENT DoTsOff "<#1#> End DoTs on #2# |-|#*#"
#EVENT DoTsOff "#1# tells you, 'End DoTs on #2# |-|#*#'"
#EVENT DoTsOff "#1# tells the group, 'End DoTs on #2# |-|#*#'"
SUB event_DoTsOff(line, ChatSender, MobID)
/if (${Debug} || ${Debug_Assists}) /echo |- event_DoTsOff ==>

	| If I'm casting debuffs
	/if (${DoT_Targets.Size}) {

		| -Verify the event
		/call verifyEvent "${ChatSender}" "${line}"
		/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
		|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|
		/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {
		
			/if (${Defined[${MobID}]}) {
				/docommand ${ChatToggle} Ending DoTs on ${Spawn[id ${MobID}].CleanName}.
				/call RemoveArrayElement DoT_Targets ${MobID}_${Zone.ID}
			} else {
				/docommand ${ChatToggle} Ending DoTs on all targets.
				/deletevar DoT_Targets
			}
		}
	}

/if (${Debug} || ${Debug_Assists}) /echo <== event_DoTsOff -|
/RETURN



|----------------------------------------------------------------|
|- Casts Command_DoTs on targets listed in DoT_Targets array.	-|
|----------------------------------------------------------------|
SUB DoTs_OnCommand
/if (${Debug} || ${Debug_Assists}) /echo |- DoTs_OnCommand ==>

	/declare targetID int local
	/declare targetZone int local
	/declare i int local
	
	/for i 1 to ${DoT_Targets.Size}
	
		/varset targetID ${DoT_Targets[${i}].Arg[1,_]}
		/varset targetZone ${DoT_Targets[${i}].Arg[2,_]}
	
		| Check Mob Zone
		/if (${Zone.ID} != ${targetZone}) {
			/call RemoveArrayElement DoT_Targets ${DoT_Targets[${i}]}
		} else {	
			| If the DoT_Target has died, remove them from the array.
			/if (${Spawn[id ${targetID}].Type.Equal[Corpse]} || ${Select[${Spawn[id ${targetID}].Type.Equal[Corpse]},NULL]}) {
				/call RemoveArrayElement DoT_Targets ${DoT_Targets[${i}]}
			} else {
		
				/call cast_longTermSpells "${targetID}" "Command_DoTs"
		
			}
		}
		
	/if (!${ActionTaken}) /next i
	
/if (${Debug} || ${Debug_Assists}) /echo <== DoT_OnCommand -|
/RETURN



|----------------------------|
|- Turns rain functions on.	-|
|----------------------------|
#event RainOn "<#1#> Rain on"
#event RainOn "#1# tells you#*#Rain on'"
#event RainOn "#1# tells the group, 'Rain on"
SUB Event_RainOn(line, ChatSender)
/if (${Debug} || ${Debug_Assists}) /echo |- Event_RainOn ==>

/if (${Rain_Spells.Size}) {
	
	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		
		| If I'm already casting rain spells
		/if (!${Defined[do_Rain]}) {
			/declare do_Rain bool outer TRUE
			/docommand ${ChatToggle} Casting rain spells...
		}
	}
}	
/if (${Debug} || ${Debug_Assists}) /echo <== Event_RainOn -|
/RETURN



|--------------------------------|
|- Turns rain functions off.	-|
|--------------------------------|
#event RainOff "<#1#> Rain off"
#event RainOff "#1# tells you#*#RainOff'"
#event RainOff "#1# tells the group, 'Rain off"
SUB Event_RainOff(line, ChatSender)
/if (${Debug} || ${Debug_Assists}) /echo |- Event_RainOff ==>

/if (${Defined[do_Rain]}) {
	
	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |---------Included Bot----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {	
		
		/if (${Me.Casting.ID}) /interrupt 
		/deletevar do_Rain
		/docommand ${ChatToggle} Ending rain.
		/doevents flush
		/varset combatTimer 10s
	}
}

/if (${Debug} || ${Debug_Assists}) /echo <== Event_RainOff -|	
/RETURN



|------------------------------------------------------------------------------------------------------------|
|- Casts spells listed in the [Rain] section of the Character_Ini on mobs which wander into your AE_Radius.	-|
|------------------------------------------------------------------------------------------------------------|
SUB check_Rain
/if (${Debug} || ${Debug_Assists}) /echo |- check_Rain ==>

	/if (${do_Rain} && ${Rain_Spells.Size}) {
		
		/declare castName string local

		/declare i int local

		/for i 1 to ${Rain_Spells.Size}
		
			/varset castName ${Rain_Spells[${i}].Arg[1,/]}
	 
			/if (${SpawnCount[npc radius ${AE_Radius}]}) {
			
				/if (!${rainTimer_RainSpell${i}}) {
				
					/call e3_Cast ${NearestSpawn[npc radius ${AE_Radius}].ID} "${Rain_Spells[${i}]}${If[${Me.Book[${castName}]},/GiveUpTimer|${If[${Me.Gem[${castName}]} || ${Me.Combat},0,90]},]}"
				
				}
			}
			
		/if (!${ActionTaken}) /next i
		
		/varset combatTimer ${combatDelay}
	}
/if (${Debug} || ${Debug_Assists}) /echo <== check_Rain -|	
/RETURN



|----------------------------------------------------------------|
|- Casts swarm pets from Mages, Necros, Shaman, and Wizards.	-|
|----------------------------------------------------------------|
#event SwarmPets "<#1#> Swarm Pets on #2#"
SUB event_SwarmPets(line, ChatSender, int swarmTarget)
/if (${Debug} || ${Debug_Assists}) /echo |- event_SwarmPets ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| Validate target.
		/if (${SpawnCount[id ${swarmTarget}]} && ${Spawn[${swarmTarget}].Type.NotEqual[Corpse]} && ${Select[${Spawn[${swarmTarget}].Type},${AcceptableTargetTypes}]} && ${SpawnCount[pc ${ChatSender}]}) {

			| --------------- Declare a swarmPets array based on class
		
			| Magician abilities.
			/if (${Me.Class.ShortName.Equal[MAG]}) {
				/declare swarmPets[2] string local
				/varset swarmPets[1] Servant of Ro
				/varset swarmPets[2] Host of the Elements
			| Necromancer abilities.
			} else /if (${Me.Class.ShortName.Equal[NEC]}) {
				/declare swarmPets[2] string local
				/varset swarmPets[1] Swarm of Decay
				/varset swarmPets[2] Wake the Dead
				
			| Shaman abilities.
			} else /if (${Me.Class.ShortName.Equal[SHM]}) {
				/declare swarmPets[1] string local
				/varset swarmPets[1] Spirit Call
			| Wizard abilities.
			} else /if (${Me.Class.ShortName.Equal[WIZ]}) {
				/declare swarmPets[1] string local
				/varset swarmPets[1] Call of Xuzl
			}
			
			| Cast swarm pets
			
			/declare i int local
			
			/for i 1 to ${swarmPets.Size}
			
				/call e3_Cast ${swarmTarget} "${swarmPets[${i}]}"
				/delay 10
				
			/next i
	
		}
	}
	
	/varset combatTimer ${combatDelay}
	
/if (${Debug} || ${Debug_Assists}) /echo <== event_SwarmPets -|
/RETURN



SUB check_Burns
/if (${Debug} || ${Debug_Assists}) /echo |- check_Burns ==>

	/if (${use_FullBurns}) /call useBurns ${AssistTarget} "FullBurns"
	/if (${use_QuickBurns}) /call useBurns ${AssistTarget} "QuickBurns"

/if (${Debug} || ${Debug_Assists}) /echo <== check_Burns -|
/RETURN



|----------------------------------------------------|
|- Engages QuickBurns listed in the Character_Ini.	-|
|----------------------------------------------------|
#event QuickBurn "<#1#> Quick Burn"
SUB event_QuickBurn(line, ChatSender)
/if (${Debug} || ${Debug_Assists}) /echo |- event_QuickBurn ==>

	| Check Assisting
	/if (!${Assisting}) {
		/echo You must be attacking something to burn.
	} else {
	
		| Check for quickburns to cast
		/if (${Defined[QuickBurns]}) {
		
			| -Verify the event
			/call verifyEvent "${ChatSender}" "${line}"
			/if (${Defined[ChatSender]}) /if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
			|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
			/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
				/docommand ${ChatToggle} Using Quick Burns...
				/if (!${Defined[use_QuickBurns]}) /declare use_QuickBurns bool outer TRUE
			}
		}
	}

/if (${Debug} || ${Debug_Assists}) /echo <== event_QuickBurn -|
/RETURN



|----------------------------------------------------|
|- Engages FullBurns listed in the Character_Ini.	-|
|----------------------------------------------------|
#event FullBurn "<#1#> Full Burn"
SUB event_FullBurn(line, ChatSender)
/if (${Debug} || ${Debug_Assists}) /echo |- event_FullBurn ==>

	| Check Assisting
	/if (!${Assisting}) {
		/echo You must be attacking something to burn.
	} else {
	
		| Check for quickburns to cast
		/if (${Defined[FullBurns]}) {
		
			| -Verify the event
			/call verifyEvent "${ChatSender}" "${line}"
			/if (${Defined[ChatSender]}) /if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
			|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
			/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
				/docommand ${ChatToggle} Using Full Burns...
				/if (!${Defined[use_FullBurns]}) /declare use_FullBurns bool outer TRUE
				/if (!${Defined[use_QuickBurns]}) /declare use_QuickBurns bool outer TRUE
			}
		}
	}

/if (${Debug} || ${Debug_Assists}) /echo <== event_FullBurn -|
/RETURN



|----------------------------------------------------------------------------------------------------|
|- FullBurn's MainLoop hook, calls FullBurns, and QuickBurns, in succession until your target dies.	-|
|----------------------------------------------------------------------------------------------------|
SUB FullBurns
/if (${Debug} || ${Debug_Assists}) /echo |- FullBurns ==>

	/declare castName string local
	
	/declare i int local
	
	/for i 1 to ${FullBurns.Size}
	
		/varset castName ${FullBurns[${i}].Arg[1,/]}
	
		| If the Quickburn to use is a discipline
		/if (${Me.CombatAbility[${castName}]) {
			
			| Check the discipline window to see if something is already active before casting
			/if (${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text.NotEqual[No Effect]}) {
				/if (${Debug} || ${Debug_Assists}) /echo |- QuickBurn -| Waiting on disc [${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text}]
			} else {
				/call e3_Cast ${AssistTarget} "${QuickBurns[${i}]}"
			}
		} else {
			/call e3_Cast ${AssistTarget} "${QuickBurns[${i}]}/GiveUpTimer|${If[${Me.Gem[${castName}]},0,100]}"
		}
	
	/if (!${ActionTaken}) /next i
	
	/varset combatTimer ${combatDelay}

/if (${Debug} || ${Debug_Assists}) /echo <== FullBurns -|
/RETURN



|--------------------------------------------------------------------------------|
|- Casts spells, abilities, and burns through disciplines on a given targetID.	-|
|--------------------------------------------------------------------------------|
SUB useBurns(int burnTarget, arrayName)
/if (${Debug} || ${Debug_Assists}) /echo |- useBurns Target=${burnTarget}- Array=${arrayName} ==>

	/declare castName string local
	
	/declare i int local
	
	/for i 1 to ${${arrayName}.Size}
	
		/varset castName ${${arrayName}[${i}].Arg[1,/]}
		
		/call e3_Cast ${burnTarget} "${${arrayName}[${i}]}/GiveUpTimer|${If[${Me.Gem[${castName}]},0,100]}"
		
	/if (!${ActionTaken}) /next i
	
	/varset combatTimer ${combatDelay}

/if (${Debug} || ${Debug_Assists}) /echo <== useBurns -|
/RETURN



#EVENT toggle_PvP "[#1#] Toggle PvP #2#"
#EVENT toggle_PvP "<#1#> Toggle PvP #2#"
#EVENT toggle_PvP "#1# tells you, 'Toggle PvP #2#'"
#EVENT toggle_PvP "#1# tells the group, 'Toggle PvP #2#'"
#EVENT toggle_PvP "#1# you tell your party, 'Toggle PvP #2#'"
SUB EVENT_toggle_PvP(line, ChatSender, eventParams)
/if (${Debug} || ${Debug_Assists}) /echo |- EVENT_toggle_PvP ==>

	/declare pvpSet string local ${eventParams.Arg[1, ]}

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		
		| Turn PvP On.
		/if (${pvpSet.Find[on]} || ${pvpSet.Find[true]} || ${Int[${pvpSet}]}) {
			/varset Toggle-PvP TRUE
			/varset AcceptableTargetTypes ${AcceptableTargetTypes},PC
			
		| Turn PvP Off.
		} else {
			/varset Toggle-PvP FALSE
			/call IniToVar ${genSettings_Ini},Assists,Allowed_TargetTypes AcceptableTargetTypes string outer
		}

		/echo PvP mode [${If[${Toggle-PvP},ON,OFF]}]
	}
	
/if (${Debug} || ${Debug_Assists}) /echo <== EVENT_toggle_PvP -|
/RETURN



#EVENT toggle_Taunt "[#1#] Toggle Taunt #2#"
#EVENT toggle_Taunt "<#1#> Toggle Taunt #2#"
#EVENT toggle_Taunt "#1# tells you, 'Toggle Taunt #2#'"
#EVENT toggle_Taunt "#1# tells the group, 'Toggle Taunt #2#'"
#EVENT toggle_Taunt "#1# you tell your party, 'Toggle Taunt #2#'"
SUB EVENT_toggle_Taunt(line, ChatSender, eventParams)
/if (${Debug} || ${Debug_Assists}) /echo |- EVENT_toggle_Taunt ==>

	/declare tauntSet string local ${eventParams.Arg[1, ]}

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		
		| Turn taunt On
		/if (${tauntSet.Find[on]} || ${tauntSet.Find[true]} || ${Int[${tauntSet}]}) {
			/varset AutoAggro TRUE
			
		| Turn PvP Off.
		} else {
			/varset AutoAggro FALSE
		}

		/echo Auto-Aggro [${If[${AutoAggro},ON,OFF]}]
	}
	
/if (${Debug} || ${Debug_Assists}) /echo <== EVENT_toggle_Taunt -|
/RETURN



#EVENT toggle_assistType "[#1#] Toggle Assist Type #2#"
#EVENT toggle_assistType "<#1#> Toggle Assist Type #2#"
#EVENT toggle_assistType "#1# tells you, 'Toggle Assist Type #2#'"
#EVENT toggle_assistType "#1# tells the group, 'Toggle Assist Type #2#'"
#EVENT toggle_assistType "#1# you tell your party, 'Toggle Assist Type #2#'"
SUB EVENT_toggle_assistType(line, ChatSender, eventParams)
/if (${Debug} || ${Debug_Assists}) /echo |- EVENT_toggle_assistType ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		
		/if (${Defined[AssistType]}) {
		
			/declare assistType_Set string local ${eventParams.Arg[1, ]}
		
			/if (${assistType_Set.Find[range]}) {
				/varset AssistType Ranged
			} else /if (${assistType_Set.Find[melee]}) {
				/varset AssistType Melee
			} else {
				/echo [${assistType_Set}] is not a recognized assist type.
			}

			/echo Assist Type [${AssistType}]
		}
	}
	
/if (${Debug} || ${Debug_Assists}) /echo <== EVENT_toggle_assistType -|
/RETURN



Sub assist_Setup
/call INItoVar "${advSettings_Ini},Debug,Debug Assists (On/Off)" Debug_Assists bool outer
/if (${Debug} || ${Debug_Assists}) /echo |- assist_Setup ==>
	
	| Add Assists file paths
	/if (!${Ini[${MacroData_Ini},File Paths,Enrage Settings].Length}) /call WriteToIni "${MacroData_Ini},File Paths,Enrage Settings" "e3 Macro Inis\Enrage Settings.ini"
	
	| Enrage Settings
	/if (!${Ini[${MacroData_Ini},File Paths,Enrage Settings].Length}) {
		/echo ERROR: Could not find designated file path for [Enrage Settings].  Please review review settings in [${MacroData_Ini} > File Paths].
		/endmacro
	} else {
		/declare enrageSettings_ini string outer ${Ini[${MacroData_Ini},File Paths,Enrage Settings]}
	}
	
	| Create variables used in Assist scripts.
	/declare Toggle-PvP bool outer FALSE
	/declare Assisting bool outer FALSE
	/declare AssistTarget int outer 0
	/declare AllowControl bool outer FALSE
	/declare AssistStickDistance int outer
	/declare AutoAggro bool outer TRUE
	/declare longTerm_recastBuffer int outer 180
	/declare use_Nukes bool outer TRUE
	
	/declare hasTanked bool outer FALSE
	/declare enraged bool outer FALSE
	/declare enrageAttack bool outer FALSE
	
| ------------------------------------- Import Macro_Ini Settings.
	/call IniToVar "${genSettings_Ini},Assists,Max Engage Distance" MaxAssistDistance int outer
	/call IniToVar "${genSettings_Ini},Assists,AE Threat Range" AE_Radius int outer
	/call INItoVar "${genSettings_Ini},Assists,Acceptable Target Types" AcceptableTargetTypes string outer
	/call INItoVar "${genSettings_Ini},Assists,Auto-Assist (On/Off)" auto_AssistOn bool outer
	
| ------------------------------------- Import Character_Ini Settings.

	| [Melee/Ranged].
	/if (${Ini[${Character_Ini},Assist Settings,Assist Type (Melee/Ranged)].Length}) /call IniToVar "${Character_Ini},Assist Settings,Assist Type (Melee/Ranged)" AssistType string outer
	/if (${Ini[${Character_Ini},Assist Settings,Melee Stick Point].Length}) /call IniToVar "${Character_Ini},Assist Settings,Melee Stick Point" AssistStickPoint string outer
	/if (${Ini[${Character_Ini},Assist Settings,Melee Distance].Length}) /call IniToVar "${Character_Ini},Assist Settings,Melee Distance" AssistDistanceSetting string outer
	/if (${Ini[${Character_Ini},Assist Settings,Ranged Distance].Length}) /call IniToVar "${Character_Ini},Assist Settings,Ranged Distance" RangedStickDistance string outer
	/if (${Ini[${Character_Ini},Assist Settings,Auto-Assist Engage Percent].Length}) /call IniToVar "${Character_Ini},Assist Settings,Auto-Assist Engage Percent" EngagePct int outer
	/if (!${Defined[EngagePct]}) {
		/if (${Debug} || ${Debug_Assists}) /echo ERROR: @[EngagePct] - EngagePct is invalid, defaulting to 98%
		/declare EngagePct int outer 98
	} else /if (${Defined[EngagePct]} && !${EngagePct}) {
		/if (${Debug} || ${Debug_Assists}) /echo ERROR: @[EngagePct] - EngagePct is invalid, defaulting to 98%
		/varset EngagePct 98
	}
	
	| [Melee Abilities].
	/if (${Ini[${Character_Ini},Melee Abilities,Ability#1].Length}) /call INItoArray "${Character_Ini},Melee Abilities,Ability#" MyAbilities

	| [Nukes].
	/if (${Ini[${Character_Ini},Nukes,Nuke#1].Length}) /call INItoArray "${Character_Ini},Nukes,Nuke#" MyNukes
	
	| [DoTs].
	
	|- On Assist
	/if (${Ini[${Character_Ini},DoTs,DoT on Assist#1].Length}) /call INItoArray "${Character_Ini},DoTs,DoT on Assist#" Assist_DoTs

	|- On Command
	/if (${Ini[${Character_Ini},DoTs,DoT on Command#1].Length}) /call INItoArray "${Character_Ini},DoTs,DoT on Command#" Command_DoTs

	| [Debuffs].

	|- On Assist
	/if (${Ini[${Character_Ini},Debuffs,Debuff on Assist#1].Length}) /call INItoArray "${Character_Ini},Debuffs,Debuff on Assist#" Assist_Debuffs

	|- On Command
	/if (${Ini[${Character_Ini},Debuffs,Debuff on Command#1].Length}) /call INItoArray "${Character_Ini},Debuffs,Debuff on Command#" Command_Debuffs
	
	| [Rain].
	/if (${Ini[${Character_Ini},Rain,Rain#1].Length}) /call INItoArray "${Character_Ini},Rain,Rain#" Rain_Spells

	| [QuickBurns].
	/if (${Ini[${Character_Ini},Burn,Quick Burn#1].Length}) /call IniToArray "${Character_Ini},Burn,Quick Burn#" QuickBurns
	
	| [FullBurns].	
	/if (${Ini[${Character_Ini},Burn,Full Burn#1].Length}) /call IniToArray "${Character_Ini},Burn,Full Burn#" FullBurns
	
	| If Assisting variables are defined, turn assists on.
	/if (${Defined[AssistType]} || ${Defined[MyAbilities]} || ${Defined[${arrayName}]} || ${Defined[Assist_DoTs]} || ${Defined[Command_DoTs]} || ${Defined[Assist_Debuffs]} || ${Defined[Command_Debuffs]} || ${Defined[Rain_Spells]} || ${Defined[QuickBurns]} || ${Defined[FullBurns]}) /declare AssistsOn bool outer TRUE

/if (${Debug} || ${Debug_Assists}) /echo <== assist_Setup -|
/return



Sub assist_Background_Events
	
	/if (${Assisting}) {
		/doevents Assist
		/doevents BackOff
		/call check_AssistTarget
		/call check_AssistStatus
		/doevents EnrageOn
		/doevents EnrageOff
	}
	
	/if (${auto_AssistOn}) /call check_AutoAssist
	/if (!${Me.Casting.ID} && ${Me.Combat} && ${MyAbilities.Size}) /call CombatAbilities
	
	/doevents getCloser
	/doevents DebuffsOn
	/doevents DebuffsOff
	/doevents DoTsOn
	/doevents DoTsOff
	/doevents TauntOff
	/doevents TauntOn
	/doevents RainOff
	/doevents RainOn
	/doevents QuickBurn
	/doevents FullBurn
	/doevents toggle-PvP
	/doevents setEnrageAttack

/return



SUB assist_MacroSettings
/if (${Debug} || ${Debug_Assists}) /echo |- assist_MacroSettings ==>

	/call WriteToIni "${advSettings_Ini},Debug,Debug Assists (On/Off)"

	/call WriteToIni "${genSettings_Ini},Assists,Auto-Assist (On/Off)" On
	/if (${Ini[${genSettings_Ini},Assists,Auto-Assist Engage Percent].Length}) /call WriteToIni "${genSettings_Ini},Assists,Auto-Assist Engage Percent" "DEPRECATED - YOU MAY DELETE THIS ENTRY" 1
	/call WriteToIni "${genSettings_Ini},Assists,Max Engage Distance" 250
	/call WriteToIni "${genSettings_Ini},Assists,AE Threat Range" 100
	/call WriteToIni "${genSettings_Ini},Assists,Acceptable Target Types" NPC,Pet
	
/if (${Debug} || ${Debug_Assists}) /echo <== assist_MacroSettings -|
/RETURN



SUB assist_CharacterSettings
/if (${Debug} || ${Debug_Assists}) /echo |- assist_CharacterSettings ==>

	| Add Assist_Stick entries.
	/call WriteToIni "${Character_Ini},Assist Settings,Assist Type (Melee/Ranged)" ${If[${Select[${Me.Class.ShortName},BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]},Melee,]}
	/call WriteToIni "${Character_Ini},Assist Settings,Auto-Assist Engage Percent" 98
	/call WriteToIni "${Character_Ini},Assist Settings,Melee Stick Point" ${If[${Select[${Me.Class.ShortName},BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]},Behind,]}
	/call WriteToIni "${Character_Ini},Assist Settings,Melee Distance" ${If[${Select[${Me.Class.ShortName},BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]},MaxMelee,]}
	/call WriteToIni "${Character_Ini},Assist Settings,Ranged Distance" 100

	| If I'm a melee class, add Melee Abilities entries.
	/if (${Select[${Me.Class.ShortName},BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]}) {
		/if (!${Ini[${Character_Ini},Melee Abilities].Length}) /call WriteToIni "${Character_Ini},Melee Abilities,Ability#1"
	}

	| If I'm a casting class, add Nuke entries.
	/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,MAG,PAL,NEC,RNG,SHD,SHM,WIZ]}) {
		/if (!${Ini[${Character_Ini},Nukes].Length}) /call WriteToIni "${Character_Ini},Nukes,Nuke#1"
	}

	| If I'm a casting class, add DoT entries.
	/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,NEC,RNG,SHD,SHM]}) {
		/if (!${Ini[${Character_Ini},DoTs].Length}) {
			/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#1"
			/call WriteToIni "${Character_Ini},DoTs,DoT on Command#1"
		}
	}

	| If I'm a casting class, add Debuff entries.
	/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,MAG,NEC,SHM]}) {
		/if (!${Ini[${Character_Ini},Debuffs].Length}) {
			/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#1"
			/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#1"
		}
	}

	| If I'm a casting class, add Rain entries.
	/if (${Select[${Me.Class.ShortName},DRU,MAG,SHM,WIZ]}) {
		/if (!${Ini[${Character_Ini},Rain].Length}) /call WriteToIni "${Character_Ini},Rain,Rain#1"
	}

	| Burn entries.
	/if (!${Ini[${Character_Ini},Burn].Length}) {
		/call WriteToIni "${Character_Ini},Burn,Quick Burn#1"
		/call WriteToIni "${Character_Ini},Burn,Full Burn#1"
	}

/if (${Debug} || ${Debug_Assists}) /echo <== assist_CharacterSettings -|
/RETURN



Sub assist_Aliases
/if (${Debug} || ${Debug_Assists}) /echo |- assist_Aliases ==>

	/noparse /squelch /alias /AssistOn /bc Assist on ${Target.ID}
	/noparse /squelch /alias /AssistMe /bc Assist on ${Target.ID}
	/squelch /alias /BackOff /bc Back off
	/noparse /squelch /alias /debuff /bc Debuffs on ${Target.ID}
	/noparse /squelch /alias /debuffson /bc Debuffs on ${Target.ID}
	/squelch /alias /debuffsoff /bc End Debuffs
	/noparse /squelch /alias /dot /bc DoTs on ${Target.ID}
	/noparse /squelch /alias /dotson /bc DoTs on ${Target.ID}
	/squelch /alias /dotsoff /bc End DoTs
	/squelch /alias /rain /bc Rain on
	/squelch /alias /rainOn /bc Rain on
	/squelch /alias /rainOff /bc Rain off
	/noparse /squelch /alias /SwarmPets /bc Swarm pets on ${Target.ID}
	/squelch /alias /QuickBurns /bc Quick burns
	/squelch /alias /FullBurns /bc Full burns
	/squelch /alias /pvpOn /bc Toggle PvP ON
	/squelch /alias /pvpOff /bc Toggle PvP OFF
	/squelch /alias /TauntOn /bc Toggle Taunt ON
	/squelch /alias /TauntOff /bc Toggle Taunt OFF
	/squelch /alias /AssistType /bc Toggle Assist Type

/if (${Debug} || ${Debug_Assists}) /echo <== assist_Aliases -|
/return