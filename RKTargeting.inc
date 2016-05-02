
Sub GetTarget
	/if (${DebugMode}) /echo GetTarget
	/declare i int local
	
	/if (${TankId} == ${Me.ID} || ${Me.Name.Equal[${RKCommander}]}) {
		/call GetMainTankTargetID
		/if (${DebugMode}) /echo main tank target = ${Macro.Return}
		/if (!${Macro.Return}) /return FALSE
		/varset CurrentTargetId ${Macro.Return}
	} else {
		/if (${Me.XTarget} == 0 || (${Me.XTarget} == 1 && !${Me.XTarget[1].Type.Equal[NPC]})) /return FALSE
		/call GetMainAssistTargetID
		/if (${DebugMode}) /echo main assist target = ${Macro.Return}		
		/varset CurrentTargetId ${Macro.Return}
	}
	
	/if (${CurrentTargetId} != 0) /call SelectNewTarget ${CurrentTargetId}
/return ${Macro.Return}


Sub SelectNewTarget(int targetId)
	/if (${DebugMode}) /echo SelectNewTarget ${targetId}
	/if (!${targetId}) /return FALSE
	/if (${targetId} == 0) /return FALSE
	/if (${Target.ID} == ${targetId}) /return ${Target.ID}
	
	/call ValidateTarget ${targetId}
	
	/if (${Macro.Return}) {
		/attack off
		/call TargetSync ${targetId}
		
		/if (${CurrentTargetId} != ${targetId}) {
			/varset CurrentTargetId ${targetId}
			/if (${DebugMode}) /echo SelectNewTarget - set CurrentTargetId to ${targetId} - ${CurrentTargetId}
		}
		
		/if (${Me.LAHoTT} > 0) {
			/if (${TankId} && ${TankId} != ${Me.ID} && !${Spawn[id ${TankId}].Class.Name.Equal[Magician]} && !${Me.Class.Name.Equal[Magician]}) {
				/if (${Me.TargetOfTarget.ID} && ${Me.TargetOfTarget.ID} != ${TankId}) /return FALSE
			}
		}
		
		/if (${Me.Pet.ID} && ${Me.Pet.Following.ID} != ${targetId}) {		
			/if (${DebugMode}) /echo backing pet off
			/pet back off
			/pet attack
		}
		
		/return ${targetId}
	}
/return FALSE

Sub GetMainTankTargetID
	/if (${DebugMode}) /echo GetMainTankTargetID
	/if (!${KeepAgroOnAdds}) {
		/call GetMainAssistTargetID
		/return ${Macro.Return}
	}
	
	/declare xTargetId int local
	/declare i int local	
	/if (${Me.Class.Name.Equal[Magician]}) {
		/call AreTargetsFacingPet
		/if (!${Macro.Return}) {
			/for i 1 to ${Me.XTarget}
				/varset xTargetId ${Me.XTarget[${i}].ID}		
				/call IsTargetFacingPet ${xTargetId}
				/if (!${Macro.Return}) {
					/call ValidateTarget ${xTargetId}
					/if (${Macro.Return}) {
						/xtar ${i}
						/delay 2
							
						/if (${Me.PctAggro} != 100 && ${Bool[${Target.Mezzed}]}) {
							/if (${DebugMode}) /echo I need to get agro on ${Target}
							/call SelectNewTarget ${xTargetId}
							/return ${xTargetId}
						}
					}
				}
			/next i
		} else {
			/call GetMainAssistTargetID
			/return ${Macro.Return}
		}
	}
	
	/call AreTargetsFacingMe
	/if (!${Macro.Return}) {
		/for i 1 to ${Me.XTarget}
			/varset xTargetId ${Me.XTarget[${i}].ID}		
			/call IsTargetFacingMe ${xTargetId}
			/if (!${Macro.Return}) {
				/call ValidateTarget ${xTargetId}
				/if (${Macro.Return}) {
					/xtar ${i}
					/delay 3
						
					/if (${Me.PctAggro} != 100) {
						/if (${DebugMode}) /echo I need to get agro on ${Target}
						/call SelectNewTarget ${xTargetId}
						/return ${xTargetId}
					}
				}
			}
		/next i
	} else {
		/call GetMainAssistTargetID
		/return ${Macro.Return}
	}
/return FALSE

Sub GetMainAssistTargetID
	/if (${DebugMode}) /echo GetMainAssistTargetID
	/if (${Bool[${Me.XTarget}]}) {
		/if (${TankId} != ${Me.ID}) {
			/if (${Me.GroupMarkNPC[1].ID} && ${Me.GroupMarkNPC[1].Type.Equal[NPC]}) /return ${Me.GroupMarkNPC[1].ID}
		}
	}

	/declare i int local

	/if (!${Bool[${Me.XTarget}]}) {			
		/if (${DebugMode}) /echo out of combat pull

		/if (!${PullMode}) {
			/if (${DebugMode}) /echo early pull escape
			/return FALSE
		}
		
		/declare pullTargetId int local 0
		/declare pullTargetDistance int local 10000
		
		/declare pullTargetEvaluationId int local 
		
		/for i 1 to ${PriorityTargets.Size}
			/if (${Bool[${PriorityTargets[${i}]}]}) {
				/if (${DebugMode}) /echo evaluating PriorityTarget - ${PriorityTargets[${i}]}
				/varset pullTargetEvaluationId ${Spawn[NPC radius 4000 ${PriorityTargets[${i}]}].ID}
				/if (${pullTargetEvaluationId} && ${Spawn[id ${pullTargetEvaluationId}].Distance} < ${pullTargetDistance}) {
					/if (${DebugMode}) /echo Found PriorityTarget - ${PriorityTargets[${i}]}
					/varset pullTargetDistance ${Spawn[id ${pullTargetEvaluationId}].Distance}
					/varset pullTargetId ${pullTargetEvaluationId}
				}
			}
		/next i
		
		/for i 1 to ${LoSPullTargets.Size}
			/if (${Bool[${LoSPullTargets[${i}]}]}) {
				/if (${DebugMode}) /echo evaluating los pull target ${LoSPullTargets[${i}]}
				/varset pullTargetEvaluationId ${Spawn[npc radius ${LoSPullRadius} ${LoSPullTargets[${i}]}].ID}
				/if (${pullTargetEvaluationId} && ${Spawn[id ${pullTargetEvaluationId}].Distance} < ${pullTargetDistance}) {
					/if (${DebugMode}) /echo distance is good on ${Spawn[id ${pullTargetEvaluationId}].Name} - type: ${Spawn[id ${pullTargetEvaluationId}].Type}
					/if (${Spawn[npc id ${pullTargetEvaluationId}].Type.Equal[NPC]}) {
						/if (${DebugMode}) /echo Spawn type is good

						/if (!${Spawn[id ${pullTargetEvaluationId}].CleanName.Equal[a sentry stone]} && !${Spawn[id ${pullTargetEvaluationId}].CleanName.Equal[a red coral plant]}) {

							/if (${Spawn[id ${pullTargetEvaluationId}].LineOfSight}) {
								/if (${DebugMode}) /echo pulling ${Spawn[id ${pullTargetEvaluationId}].CleanName}
								/varset pullTargetDistance ${Spawn[id ${pullTargetEvaluationId}].Distance}
								/varset pullTargetId ${Spawn[id ${pullTargetEvaluationId}].ID}
							}
						}
					}
				}
			}
		/next i
		
		/if (${pullTargetId} != 0) {
			/if (${DebugMode}) /echo returning targetId ${pullTargetId}
			/return ${pullTargetId}
		}
		
		/for i 1 to ${PullTargets.Size}
			/if (${Bool[${PullTargets[${i}]}]}) {
				/if (${DebugMode}) /echo evaluating pull target ${PullTargets[${i}]}
				/varset pullTargetEvaluationId ${Spawn[NPC zradius 100 radius 1000 ${PullTargets[${i}]}].ID}
				/if (${Spawn[npc id ${pullTargetEvaluationId}].Type.Equal[NPC]}) {
					/if (!${Spawn[id ${pullTargetEvaluationId}].CleanName.Equal[a sentry stone]} && !${Spawn[id ${pullTargetEvaluationId}].CleanName.Equal[Breinne]}) {
						/if (${pullTargetEvaluationId} && ${Spawn[id ${pullTargetEvaluationId}].Distance} < ${pullTargetDistance}) {
							/if (${pullTargetEvaluationId} != ${Me.GroupMarkNPC[2].ID} || ${Me.GroupMarkNPC[2].PctHPs} < 40) {
								/varset pullTargetDistance ${Spawn[id ${pullTargetEvaluationId}].Distance}
								/varset pullTargetId ${pullTargetEvaluationId}
							}
						}
					}
				}
			}
		/next i
					
		/if (${DebugMode}) /echo GetMainAssistTargetID - Got target ${pullTargetId}
		/call SelectNewTarget ${pullTargetId}

		/return ${pullTargetId}
	}
	
	/declare lowestHPPct int local 101
	/declare lowestHPID int local 0
	/declare xTargetId int local
	
	/for i 1 to ${Me.XTarget}
		/varset xTargetId ${Me.XTarget[${i}].ID}
		/if (${Spawn[id ${xTargetId}].Type.Equal[NPC]}) {
			/if (${Spawn[id ${xTargetId}].PctHPs} < ${lowestHPPct}) {
				/call ValidateTarget ${xTargetId}
				/if (${Macro.Return}) {
					/varset lowestHPPct ${Spawn[id ${xTargetId}].PctHPs}
					/varset lowestHPID ${xTargetId}
				}
			}
		}
	/next i
	
	/if (${lowestHPID}) {
		/call SelectNewTarget ${lowestHPID}
		/if (${Macro.Return}) {
			/call AddToIntArray "BadTargetIds" ${lowestHPID}
			/return ${lowestHPID}
		}
	}
/return FALSE

Sub TargetSync(int targetId)
	/if (${DebugMode}) /echo TargetSync ${targetId} - current target id = ${Target.ID}
	/if (${targetId} == 0) /return FALSE
	
	/target id ${targetId}
	/delay 1s ${Target.ID} == ${targetId}
	/if (${Target.ID} == ${targetId}) {
		/return TRUE
	} else {
		/if (${TargetSyncTimer} == 0) {
			/target clear
			/varset TargetSyncTimer 50
			/return FALSE
		}
		/delay 1
		/call TargetSync ${targetID}
		/return ${Macro.Return}
	}
/return FALSE

Sub ValidateTarget(int targetId)
	/if (${DebugMode}) /echo ValidateTarget ${targetId}

	/if (${Me.GroupMarkNPC[2].ID} && ${targetId} == ${Me.GroupMarkNPC[2].ID}) /return FALSE
	/if (${Spawn[id ${targetId}].Type.Equal[Pet]}) /return FALSE
	
	|/if (!${Spawn[id ${targetId}].LineOfSight})) {
	|	/if (${DebugMode}) /echo ${targetId} is bad because of LOS
	|	/return FALSE
	|}
	
	/if (!${Spawn[id ${targetId}].Type.Equal[NPC]}) {
		/if (${DebugMode}) /echo ${targetId} is not an NPC
		/return FALSE	
	}
	
	/if (${Spawn[id ${targetId}].Name.Equal[Breinne]}) /return FALSE
	
	/if (${Spawn[id ${targetId}].Distance} > ${MaxTargetDistance}) {
		/if (${DebugMode}) /echo ${targetId}'s distance (${Spawn[id ${targetId}].Distance}) is outside of the MaxTargetDistance (${MaxTargetDistance})
		/return FALSE
	}

	/if (${TankId} != ${Me.ID} && ${Spawn[id ${targetId}].PctHPs} > ${TargetHpEngagePercentage}) {
		/if (${DebugMode}) /echo target ${targetId}'s PctHPs are too high to engage ${Spawn[id ${targetId}].PctHPs} vs ${TargetHpEngagePercentage}
		/return FALSE
	}
		
	/declare i int local
	/for i 1 to ${${BadTargetIds}.Size}
		/if (${Bool[${BadTargetIds}[${i}]]} && ${currentBadTargetId} == ${targetId}) /return FALSE
	/next i
	/if (${DebugMode}) /echo Target ${targetId} is valid
/return TRUE


Sub CountTargetsInRange(int range)
	/if (${DebugMode}) /echo CountTargetsInRange
	/declare targetsInRange int local
	/declare i int local
		
	/for i 1 to ${Me.XTarget}
		/if (${Spawn[id ${Me.XTarget[${i}].ID}].Distance} <= ${range}) {
			/varcalc targetsInRange ${targetsInRange} + 1
		}
	/next i
/return ${targetsInRange}

