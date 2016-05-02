Sub CheckPosition
	/if (${TankId} != ${Me.ID}) /return FALSE
	/if (!${Bool[${Me.XTarget}]}) /return FALSE
	
	/declare moveForward bool local FALSE	
	/declare moveSideways bool local FALSE		
	/declare i int local
	
	/for i 1 to ${Me.XTarget}
		/call AmIFacingTarget ${Me.XTarget[${i}].ID} 70
		/if (!${Macro.Return}) {
			/call IsTargetFacingMe ${Me.XTarget[${i}].ID}		
			/if (${Macro.Return}) /varset moveForward TRUE
		}
	/next i	

	/if (${moveForward}) {
		/if (${DebugMode}) /echo need to move

		/if (${DebugMode}) /echo side stepping
		/if (${SideStepRight}) {
			/keypress e hold
			/keypress w hold
			/delay 3
			/keypress e
			/keypress w
		} else {
			/keypress q hold
			/keypress w hold
			/delay 3
			/keypress q
			/keypress w
		}
		/delay 2
		/if (${Target.ID}) /face fast nolook
		
		/if (${Defined[PostBackupAction]}) /call ${PostBackupAction}
	}
/return FALSE

Sub CalculateTankingPosition
	|Dead code, kept for future use
	/if (${DebugMode}) /echo CalculateTankingPosition

	/call CalculateMobsCentroid
	
	/declare angleToCentroid float local ${Target.HeadingToLoc[}
	
	/declare desiredHeading float local ${Me.HeadingToLoc[${MobsCentroidY}, ${MobsCentroidX}].DegreesCCW}
	/declare deltaFromDesiredHeading float local ${Math.Calc[${Math.Abs[${desiredHeading} - ${Me.Heading.DegreesCCW}]}]}

	/if (${DebugMode}) /echo Mob centroid is at angle ${deltaFromDesiredHeading} - from ${Target.CleanName}

	/declare angleToTankingPosition float local
	
	/if (${deltaFromDesiredHeading} >= 180) {
		/varcalc angleToTankingPosition ${deltaFromDesiredHeading} - 180
	} else {
		/varcalc angleToTankingPosition ${deltaFromDesiredHeading} + 180
	}

	/declare tankingY float local ${Math.Sin[${angleToTankingPosition}]} * 10;
	/declare tankingX float local ${Math.Cos[${angleToTankingPosition}]} * 10;
	
	/echo pre adjustment Y = ${tankingY} pre adjust X = ${tankingX}
	
	/varcalc tankingY ${Target.Y} + ${tankingY}
	/varcalc tankingX ${Target.X} + ${tankingX}	
	
	/squelch /stick off
	/squelch /moveto loc ${tankingY} ${tankingX}
	/delay 2s
	|/face fast nolook
	
	/if (${DebugMode}) /echo moving to location X: ${tankingX} Y: ${tankingY}
/return FALSE


Sub CalculateMobsCentroid
	/if (${DebugMode}) /echo CalculateMobsCentroid
	/declare i int local
		
	/declare cumulativeX float local 0
	/declare cumulativeY float local 0
	/declare analyzedTargetCount int local 0

	/for i 1 to ${Me.XTarget}
		/if (${Spawn[id ${Me.XTarget[${i}].ID}].Distance} <= ${MaxTargetDistance}) {
		
			/varcalc cumulativeX ${cumulativeX} + ${Spawn[id ${Me.XTarget[${i}].ID}].X}
			/varcalc cumulativeY ${cumulativeY} + ${Spawn[id ${Me.XTarget[${i}].ID}].Y}			
	
		}
		/varcalc analyzedTargetCount ${analyzedTargetCount} + 1
	/next i
	
	/varcalc MobsCentroidX ${cumulativeX} / ${analyzedTargetCount}
	/varcalc MobsCentroidY ${cumulativeY} / ${analyzedTargetCount}	
/return FALSE


Sub AmIFacingTarget(int targetId, int tolerance)
	/if (${DebugMode}) /echo AmIFacingTarget ${targetId} ${Spawn[id ${targetId}].CleanName}
	
	/declare desiredHeading float local ${Me.HeadingToLoc[${Spawn[id ${targetId}].Y}, ${Spawn[id ${targetId}].X}].DegreesCCW}
	/declare deltaFromDesiredHeading float local ${Math.Calc[${desiredHeading} - ${Me.Heading.DegreesCCW}]}
	
	/if (${DebugMode}) /echo ${Spawn[id ${targetId}].CleanName} is at angle ${deltaFromDesiredHeading}
	/if (${DebugMode}) /echo  Desired Heading = ${desiredHeading} my heading = ${Me.Heading.DegreesCCW}
	
	/if (${BOTMode} != 5 && ${Math.Abs[${deltaFromDesiredHeading}]} > ${tolerance}) {
		/if (${DebugMode}) /echo ${targetId} isn't infront of me, my angle is off by ${deltaFromDesiredHeading}
		
		/if (${deltaFromDesiredHeading} > 0) {
			/varset SideStepRight TRUE
		} else {
			/varset SideStepRight FALSE
		}
		
		/return FALSE
	}
/return TRUE

Sub IsTargetFacingPet(int targetId)
	/if (${DebugMode}) /echo IsTargetFacingPet
	/declare desiredHeading float local ${Spawn[id ${targetId}].HeadingToLoc[${Me.Pet.Y}, ${Me.Pet.X}].DegreesCCW}
	/if (${DebugMode}) /echo desiredHeading ${desiredHeading}
	/declare deltaFromDesiredHeading float local ${Math.Calc[${Math.Abs[${desiredHeading} - ${Spawn[id ${targetId}].Heading.DegreesCCW}	]}]}

	/if (${BOTMode} != 5 && ${deltaFromDesiredHeading} > 9) {
		/if (${DebugMode}) /echo target id ${targetId} isn't facing ${Me.Pet.Name} correctly - his angle is off by ${deltaFromDesiredHeading}
		/if (${DebugMode}) /echo target id ${targetId} angle to pet is ${deltaFromDesiredHeading}
		/return FALSE
	}
/return TRUE


Sub IsTargetFlankingMe(int targetId)
	/if (${DebugMode}) /echo IsTargetFlankingMe ${targetId} ${Spawn[id ${targetId}].CleanName}
	
	/declare desiredHeading float local ${Me.HeadingToLoc[${Spawn[id ${targetId}].Y}, ${Spawn[id ${targetId}].X}].DegreesCCW}
	/declare deltaFromDesiredHeading float local ${Math.Abs[${Math.Calc[${desiredHeading} - ${Me.Heading.DegreesCCW}]}]}

	/if (${DebugMode}) /echo ${Spawn[id ${targetId}].CleanName} is at angle ${deltaFromDesiredHeading} - Desired Heading = ${desiredHeading} my heading = ${Me.Heading.DegreesCCW}

	
	/if (${deltaFromDesiredHeading} > ${tolerance}) {
		/if (${DebugMode}) /echo ${targetId} isn't infront of me, my angle is off by ${deltaFromDesiredHeading}
		/return FALSE
	}
/return TRUE

Sub AreTargetsFacingMe
	/if (${Me.XTarget} == 0) /return TRUE
	/declare i int local
	/for i 1 to ${Me.XTarget}
		/call IsTargetFacingMe ${Me.XTarget[${i}].ID}
		/if (!${Macro.Return}) /return FALSE
	/next i
/return TRUE

Sub AreTargetsFacingPet
	/if (${Me.XTarget} == 0) /return TRUE
	/declare i int local
	/for i 1 to ${Me.XTarget}
		/call IsTargetFacingPet ${Me.XTarget[${i}].ID}
		/if (!${Macro.Return}) /return FALSE
	/next i
/return TRUE

Sub IsTargetFacingMe(int targetId)
	/if (${DebugMode}) /echo IsTargetFacingMe
	
	/if (${Me.Class.Name.Equal[Magician]} && ${Me.Pet.ID}) {
		/call IsTargetFacingPet ${targetId}
		/return ${Macro.Return}
	}
	
	/declare desiredHeading float local ${Spawn[id ${targetId}].HeadingToLoc[${Me.Y}, ${Me.X}].DegreesCCW}
	/if (${DebugMode}) /echo desiredHeading ${desiredHeading}
	/declare deltaFromDesiredHeading float local ${Math.Calc[${Math.Abs[${desiredHeading} - ${Spawn[id ${targetId}].Heading.DegreesCCW}	]}]}

	/if (${BOTMode} != 5 && ${deltaFromDesiredHeading} > 9) {
		/if (${DebugMode}) /echo target id ${targetId} isn't facing me correctly - his angle is off by ${deltaFromDesiredHeading}
		/return FALSE
	}
/return TRUE


Sub SetStickTarget(int targetId, int distance)
	/if (${DebugMode}) /echo SetStickTarget
	/if (${targetId} == 0) /return FALSE
	/if (${DebugMode}) /echo setting stick target - CurrentID = ${CurrentStickTargetId} - new ID = ${targetId}

	/if (${CurrentStickTargetId} == ${targetId}) {
		/if (${StickTimer} != 0) /return FALSE
		/varset StickTimer 50

		/if (!${Stick.Active}) /squelch /stick id ${targetId} ${StickFlags} ${distance} 
		/return FALSE
	}
		
	/stick off
	/delay 2
	/keypress s
	/delay 2
	
	/if (!${Bool[${distance}]} || ${distance} == 0) /varset distance ${StickDistance}

	/varset CurrentStickTargetId ${targetId}
	

	/if (${Spawn[id ${targetId}].Type.Equal[NPC]}) {
		/if (${DebugMode}) /echo stick id ${targetId} ${StickFlags} ${distance}
		/squelch /stick id ${targetId} ${StickFlags} ${distance}
	} else {
		/squelch /stick id ${targetId} ${distance}
	}	
/return FALSE

Sub CheckCommanderFollow
	/if (${DebugMode}) /echo CheckCommanderFollow
	/if (${Spawn[${RKCommander} pc].ID} == ${Me.ID}) /return FALSE
	/if (${BOTMode} == 1 || ${BOTMode} == 3) {
		/if (${Me.XTarget} > 0) {
		
			/if (!${InCombatStickToCommander} && ${Stick.Active}) {
				/if (${CurrentStickTargetId} == ${Group.Leader.ID}) {
					/echo InCombatStickToCommander = true, dropping stick to commander
					/stick off
				}
			}
			
		}
		
		/if (${Me.XTarget} == 0) {
			/if (!${Stick.Active}) /echo sticking to Commander ${RKCommander}
			/call SetStickTarget ${Spawn[${RKCommander} pc].ID} ${StickDistance}
		}

		/if (${Me.XTarget} > 0 && ${InCombatStickToCommander}) {
			/if (!${Stick.Active}) {
				/if (${DebugMode}) /echo sticking to Commander ${RKCommander}
				/call SetStickTarget ${Spawn[${RKCommander} pc].ID} ${StickDistance}
			}
		}

		/if (${Spawn[${RKCommander}].Distance} < 20) {
			/if (${Spawn[${RKCommander}].Z} < (${Me.Z} - 5)) {
				/if (!${Descending}) {
					/if (${DebugMode}) /echo Commander: ${Spawn[${RKCommander}].Z} < me: ${Me.Z} - Descending
					|/keypress end hold
					/varset Descending TRUE
				}
			} else {
				/if (${Descending}) {
					|/keypress end
					/varset Descending FALSE
				}
			}
		}
	}
/return FALSE

Sub ClearStickTarget
	/if (${DebugMode}) /echo ClearStickTarget
	/if (${CurrentStickTargetId} != 0) /varset CurrentStickTargetId 0
	/if (${CurrentStickTargetId} == 0 && !${Stick.Active}) /return TRUE
	/squelch /stick off
/return TRUE