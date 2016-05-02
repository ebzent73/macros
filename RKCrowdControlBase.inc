#Event MezBroken "#*#has been awakened by#*#"
#Event MezImmune "#*#Your target cannot be mesmerized.#*#"
#Event CharmImmune "#*#cannot be charmed#*#"

Sub Event_MezBroken
	/echo Mez was broken - clearing dictionary
	/call ClearMezzedMobArrays
/return

Sub Event_MezImmune
	/i say ${Spawn[id ${CurrentMezTargetId}].CleanName} is unmezzable, adding to unmezzable mobs
	/call AddToArray "MezImmuneMobs" "${Spawn[id ${CurrentMezTargetId}].CleanName}"
/return

Sub Event_CharmImmune
	/i say ${Spawn[id ${CurrentCharmTargetId}].CleanName} is uncharmable, switching to nocharm mode
	/call AddToArray "CharmImmuneMobs" "${Spawn[id ${CurrentCharmTargetId}].CleanName}"
	/varset ShouldICharm FALSE
/return

Sub Init_RKCrowdControlBase
	/declare CCMode bool outer TRUE
	/declare MaxCrowdControlDistance int outer 110
	
	/declare AECentroidX float outer
	/declare AECentroidY float outer

	/declare CcTargetsInRange int outer

	/declare CurrentCharmTargetId int outer
	/declare CurrentMezTargetId int outer

	/declare SingleTargetMez string outer Lullaby of the Forsaken Rk. II
	/declare SingleTargetMezLevelCap int outer 98	

	/declare SingleTargetMez2 string outer
	/declare SingleTargetMez2LevelCap int outer 98	

	/declare MultipleTargetMez string outer
	/declare MultipleTargetMezLevelCap int outer 98	
	
	/declare AEMezSub string outer
	/declare MezSub string outer	

	/declare ShouldICharm bool outer TRUE
	/declare CharmMode bool outer TRUE
	/declare CharmSpell string outer
	/declare CharmSpellLevelCap int outer 87	
	
	/call CreateDictionary "MezzedMobTimers" 20
	/declare MezImmuneMobs[50] string outer

	/declare CharmImmuneMobs[50] string outer	
	
	/call AddToArray "NonCombatSubs" "ClearMezzedMobArrays"
	
	/call AddToArray "PreTargetAcquisitionCombatActionSubs" "ExecuteCrowdControl"
	
	/echo Done initializing crowd control base!
/return

Sub ExecuteCrowdControl
	/if (${DebugMode}) /echo Execute Crowd Control
	/if (!${CCMode}) /return FALSE
	/if (${Me.XTarget} == 0 || ${Me.XTarget} == 1) {
		/if (${DebugMode}) /echo ExecuteCrowdControl Early Escape
		/return FALSE
	}
		
	/if (${Bool[${MultipleTargetMez}]}) {
		/if (${Me.GemTimer[${MultipleTargetMez}].TotalSeconds} == 0) {
			/call CountAECrowdControlTargetsInRange
		
			/if (${Macro.Return} >= 3) {
				/if (${DebugMode}) /echo Attempting to AE Mez
				
				/if (${Bool[${AEMezSub}]}) {
					/call ${AEMezSub}
					/if (${Macro.Return}) /return TRUE
				}
				
				/call RKCast "${MultipleTargetMez}" ${CurrentTargetId}
				/return TRUE
			}
		}
	}
	
	/declare ccTargetId int local 0	
	/declare mezTimer int local 0
	/declare mainTargetId int local 0
	
	/if (${DebugMode}) /echo checking single target CC

	/call GetMainAssistTargetID
	/varset mainTargetId ${Macro.Return}
	/declare i int local 0
	/for i 1 to ${Me.XTarget}
		/if (${DebugMode}) /echo checking xtarget ${i}
		/varset ccTargetId ${Me.XTarget[${i}].ID}
		/call IsMobMezzable ${ccTargetId}
		/if (${Macro.Return}) {

			/if (${Spawn[id ${ccTargetId}].Level} <= ${SingleTargetMezLevelCap}) {
				/if (${ccTargetId} != ${mainTargetId}) {
					/call GetCrowdControlTimer ${ccTargetId}
					/varset mezTimer ${Macro.Return}
					
					/if (${mezTimer} < 3000) {
						/call TargetSync ${ccTargetId}
						/delay 5 ${Bool[${Target.Buff}]}
						
						/if (${Target.Buff[${SingleTargetMez}].ID} && ${Target.Buff[${SingleTargetMez}].Duration.TotalSeconds} > 3) {
							/varcalc mezTimer ${MacroQuest.Running} + (${Target.Buff[${SingleTargetMez}].Duration.TotalSeconds} * 1000)
							/if (${DebugMode}) /echo target is mezzed - adding timer to dictionary id ${ccTargetId} - ${mezTimer}
							/call AddToDictionary "MezzedMobTimers" "${ccTargetId}" ${mezTimer}
							/i say Mezzed ${Target.CleanName}
							/g Mezzed ${Target.CleanName}
						} else {
							/if (${DebugMode}) /echo target needs to be CCd
							/call CrowdControlTarget ${ccTargetId}
							/if (${Macro.Return}) /return TRUE
						}
					}
				}
			}
		}
	/next i

	/if (${twisting}) /twist start
/return FALSE

Sub GetCharmTarget
	/if (${Me.GroupMarkNPC[2].ID}) {
		/if (${Me.GroupMarkNPC[2].PctHPs} > 30) /return ${Me.GroupMarkNPC[2].ID}
	}
	
	/declare highestPct int local 0
	/declare highestId int local 0
	/declare i int local 0	
	/for i 1 to ${Me.XTarget}
		/if (!${Me.XTarget[${i}].Named} && ${Me.XTarget[${i}].PctHPs} > ${highestPct}) {
			/if (${Me.XTarget[${i}].Level} <= ${CharmSpellLevelCap} && ${Me.XTarget[${i}].PctHPs} > 50) {
				/call IsMobCharmable ${Me.XTarget[${i}].ID}
				/if (${Macro.Return}) {
					/varset highestPct ${Me.XTarget[${i}].PctHPs}
					/varset highestId ${Me.XTarget[${i}].ID}
				}
			}
		}
	/next i
	/echo ${highestId} is best charm target with ${highestPct} % hps
	/varset CurrentCharmTargetId ${highestId}
/return ${highestId}

Sub ClearMezzedMobArrays
	/call ClearDictionary "MezzedMobTimers"
/return FALSE

Sub IsMobCharmable(int targetId)
	/call GetStringArrayIndex "CharmImmuneMobs" "${Spawn[id ${targetId}].CleanName}"
	/if (${Bool[${Macro.Return}]}) /return FALSE
/return ${Spawn[id ${targetId}].LineOfSight}

Sub IsMobMezzable(int targetId)
	/call GetStringArrayIndex "MezImmuneMobs" "${Spawn[id ${targetId}].CleanName}"
/return !${Bool[${Macro.Return}]}

Sub GetCrowdControlTimer(int targetId)
	/declare mezTimer int local 0
	/call GetFromOrAddToDictionary "MezzedMobTimers" "${targetId}" 0
	/varcalc mezTimer ${Int[${Macro.Return}]} - ${MacroQuest.Running}
	/if (${DebugMode}) {
		/declare mezTimerInSeconds float local ${mezTimer} / 1000
		/echo Mez timer for TargetId ${ccTargetId} - ${Spawn[id ${ccTargetId}].CleanName} is ${mezTimerInSeconds}
	}
/return ${mezTimer}

Sub CountAECrowdControlTargetsInRange
	/if (${DebugMode}) /echo CountTargetsInRange
	/declare spellRange int local ${Spell[${MultipleTargetMez}].AERange}
	/declare i int local
		
	/declare cumulativeX float local 0
	/declare cumulativeY float local 0
	/declare analyzedTargetCount int local 0
	
	/varset CcTargetsInRange 0
	
	/for i 1 to ${Me.XTarget}
		/if (${Spawn[id ${Me.XTarget[${i}].ID}].Distance} <= ${spellRange} && ${Spawn[id ${Me.XTarget[${i}].ID}].Level} <= ${MultipleTargetMezLevelCap}) {
		
			/varcalc cumulativeX ${cumulativeX} + ${Spawn[id ${Me.XTarget[${i}].ID}].X}
			/varcalc cumulativeY ${cumulativeY} + ${Spawn[id ${Me.XTarget[${i}].ID}].Y}			
		
			/call GetCrowdControlTimer ${Me.XTarget[${i}].ID}
			/if (${Macro.Return} < 1000) /varcalc CcTargetsInRange ${CcTargetsInRange} + 1
		}
		/varcalc analyzedTargetCount ${analyzedTargetCount} + 1
	/next i
	
	/varcalc AECentroidX ${cumulativeX} / ${analyzedTargetCount}
	/varcalc AECentroidY ${cumulativeY} / ${analyzedTargetCount}	
/return ${CcTargetsInRange}

Sub CrowdControlTarget(int targetId)
	/if (${DebugMode}) /echo CrowdControlTarget ${targetId}
	/if (!${Spawn[id ${targetId}].Distance} > ${Spell[${SingleTargetMez}].MyRange}) /return FALSE
	/attack off
	/if (${Bool[${MezSub}]}) {
		/call ${MezSub} ${targetId}
		/if (${Macro.Return}) /return TRUE 
	}
		
	/if (!${Me.Casting.Name.Equal[${MultipleTargetMez}]} && !${Me.Casting.Name.Equal[${SingleTargetMez}]} && !${Me.Casting.Name.Equal[${SingleTargetMez2}]}) {
		/stopcast
		/delay 1s
	}
	
	/call RKDebuffCast "${SingleTargetMez}" "${SingleTargetMez}" ${targetId}
	/if (${Macro.Return}) {
		/varset CurrentMezTargetId ${targetId}
		/if (${Me.Class.Name.Equal[Bard]}) {
			/delay 4s !${Me.Casting.ID}
			/stopcast
		}
		/target clear
		/return TRUE
	}
	
	/if (${Bool[${SingleTargetMez2}]}) {	
		/call RKDebuffCast "${SingleTargetMez2}" "${SingleTargetMez2}" ${targetId}
		/if (${Macro.Return}) {
			/varset CurrentMezTargetId ${targetId}
			/if (${Me.Class.Name.Equal[Bard]}) {
				/delay 4s !${Me.Casting.ID}
				/stopcast
			}
			/target clear
		}
	}
		
/return ${Macro.Return}
