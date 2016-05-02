Sub Init_RKHealerBase
	/declare HealSpell string outer
	/declare TankHealThreshold int outer 90
	/declare SingleTargetHealThreshold int outer 80

	/declare EmergencyHealSpell string outer
	/declare EmergencyHealThreshold int outer 50

	/declare GroupHeal string outer
	/declare GroupHealThreshold int outer 90
	
	/declare EmergencyGroupHeal string outer
	/declare EmergencyGroupHealThreshold int outer 0
	
	/declare HoTSpell string outer
	/declare TankHoTThreshold int outer 0
	/declare SingleTargetHoTThreshold int outer 0

	/declare GroupHoT[7] int outer 0
	/declare GroupHoTPet[7] int outer 0

	/declare CurrentGroupHealthPercentage float outer

	/call AddToArray "PreTargetAcquisitionCombatActionSubs" "ExecuteHealing"

	/echo Done initializing healer base!
/return

Sub ExecuteHealing
	/if (${DebugMode}) /echo ExecuteHealing
	/if (${BOTMode} < 3 || ${BOTMode} == 5) {
		/if (${DebugMode}) /echo early escape from ExecuteHealing due to BOTMode = ${BOTMode}
		/return FALSE
	}

	/if (${Me.Moving}) /return FALSE
	
	/call CheckSingleTargetHealth ${Group.MainTank.ID} ${EmergencyHealThreshold} ${SingleTargetHoTThreshold} "${EmergencyHealSpell}"
	/if (${Macro.Return}) {
		/if (${DebugMode}) /echo need to emergency heal Main Tank
		|/return TRUE
	}
	
	/call CheckSingleTargetHealth ${Group.MainTank.ID} ${TankHealThreshold} ${SingleTargetHoTThreshold}	"${HealSpell}"
	/if (${Macro.Return}) {
		/if (${DebugMode}) /echo need to heal Main Tank
		|/return TRUE
	}
	
	/if (${GroupHealThreshold} <= 0 && ${EmergencyGroupHealThreshold} <= 0) /return FALSE
	
	/call CheckGroupHealth
	/if (${Macro.Return}) {
		/if (${DebugMode}) /echo need to heal Group Tank
		|/return TRUE
	}

	/call GetLowestHPGroupMember
	/call CheckSingleTargetHealth ${Macro.Return} ${SingleTargetHealThreshold} ${SingleTargetHoTThreshold} "${HealSpell}"
	/if (${Macro.Return}) {
		/if (${DebugMode}) /echo need to heal a group member
		|/return TRUE
	}
/return ${Macro.Return}

Sub GetLowestHPGroupMember
	/if (${DebugMode}) /echo GetLowestHPGroupMember
	/declare lowestLevel int local 100
	/declare lowestID int local ${Me.ID}
	/declare i int local 
	/for i 0 to ${Group.Members}
		/if (${Bool[${Group.Member[${i}].Spawn}]} && ${Group.Member[${i}].Spawn.PctHPs} < ${lowestLevel}) {
			/varset lowestLevel ${Group.Member[${i}].PctHPs}
			/varset lowestID ${Group.Member[${i}].ID}
		}
	/next i
/return ${lowestID}

Sub CheckSingleTargetHealth(int targetId, int healThreshold, int HoTThreshold, string healSpell)
	/if (${DebugMode}) /echo CheckSingleTargetHealth ${targetId} ${healThreshold} ${HoTThreshold}
	
	/declare needToHeal bool local FALSE
	
	/if (${Bool[${healSpell}]}) {
		/if (${Spawn[${targetId}].PctHPs} < ${healThreshold}) {
			/varset needToHeal TRUE
			/if (${DebugMode}) /echo need to heal ${targetId} ${Spawn[${targetId}].PctHPs} < ${healThreshold}
			/call RKCast "${healSpell}" ${targetId}
		}
	}

	/if (${Bool[${HoTSpell}]}) {
		/if (${DebugMode}) /echo checking hots
		/if (${Spawn[${targetId}].PctHPs} < ${HoTThreshold}) {
			/call DoesTargetAlreadyHaveHoT ${targetId}
			/if (!${Macro.Return}) {
				/varset needToHeal TRUE
				/echo target doesn't have hot - ${targetId} ${Spawn[${targetId}]}
				/call RKCast "${HoTSpell}" ${targetId}
				/if (${Macro.Return}) /return ${Macro.Return}
			}
		}
	}
	
	/if (${Bool[${Spawn[${targetId}].Pet.ID}]}) {
		/if (${DebugMode}) /echo Checking pet ${Spawn[${targetId}].Pet.ID}
		/call CheckSingleTargetHealth ${Spawn[${targetId}].Pet.ID} ${healThreshold} ${HoTThreshold}
		/if (${Macro.Return}) /varset needToHeal TRUE
	}
/return ${needToHeal}

Sub DoesTargetAlreadyHaveHoT(int targetId)
	/if (${DebugMode}) /echo DoesTargetAlreadyHaveHoT ${targetId}
	/declare i int local
	/declare j int local
	/for i 0 to ${Group.Members}
		/if (${Group.Member[${i}].ID} == ${targetId}) {
			/varcalc j ${i} + 1
			/return ${GroupHoT[${j}]}
		}
	/next i
/return FALSE

Sub CheckGroupHealth
	/if (${DebugMode}) /echo CheckGroupHealth
	/if (!${Bool[${GroupHeal}]} && !${Bool[${EmergencyGroupHeal}]}) {
		/if (${DebugMode}) /echo no group heals - escaping check group health
		/return FALSE
	}

	/if (!${Bool[${Group}]}) /return FALSE
	
	/declare CurrentGroupHealth int local 0
	
	/declare i int local 
	/declare groupMemberCount int local
	/for i 0 to ${Group.Members}
		/if (${Group.Member[${i}].PctHPs} != 0) {
			/varcalc CurrentGroupHealth ${CurrentGroupHealth} + ${Group.Member[${i}].PctHPs}
			/varcalc groupMemberCount ${groupMemberCount} + 1
		}
	/next i
	/varcalc CurrentGroupHealthPercentage ${CurrentGroupHealth} / ${groupMemberCount}
	/if (${DebugMode}) /echo Current group health % is ${CurrentGroupHealthPercentage}
	
	/if (${Bool[${EmergencyGroupHeal}]}) {
		/if (${CurrentGroupHealthPercentage} < ${EmergencyGroupHealThreshold}) {
			/echo Group Health % = ${CurrentGroupHealthPercentage} casting Emergency Heal - ${EmergencyGroupHeal}
			/call RKCast "${EmergencyGroupHeal}" ${Me.ID}
			/if (${Macro.Return}) /return TRUE
		}
	}

	/if (${Bool[${GroupHeal}]}) {
		/if (${CurrentGroupHealthPercentage} < ${GroupHealThreshold}) {
			/echo Group Health % = ${CurrentGroupHealthPercentage} casting Group Heal - ${GroupHeal}
			/call RKCast "${GroupHeal}" ${Me.ID}
			/if (${Macro.Return}) /return TRUE
		}
	}
/return FALSE

Sub Event_HOT_Landed_Self
	/echo HoT landed.  Setting me as having a HoT.
	/varset GroupHoT[1] 1
/return

Sub Event_HOT_Worn_off_Self
	/echo HoT wore off.  Setting me as having no HoT.
	/varset GroupHoT[1] 0
/return

Sub Event_HOT_Landed(string Line, string targetName)
	/echo HoT landed.  Setting ${targetName} as having a HoT.
	/call SetHoTData "${targetName}" 1
/return

Sub Event_HOT_Worn_off(string Line, string targetName)
	/echo HoT wore off.  Setting ${targetName} as having no HoT.
	/call SetHoTData "${targetName}" 0
/return

Sub SetHoTData(string targetName, int value)
	/if (${DebugMode}) /echo SetHotData ${targetId} ${value}
	/declare i int local
	/declare j int local
	/for i 0 to ${Group.Members}
		/if (${Group.Member[${i}].CleanName.Equal[${targetName}]}) {
			/varcalc j ${i} + 1
			/varset GroupHoT[${j}] ${value}
	    } else /if (${Group.Member[${i}].Pet.CleanName.Equal[${targetName}]}) {
			/varcalc j ${i} + 1
			/varset GroupHoTPet[${j}] ${value}
		}
	/next i
/return