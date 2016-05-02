#include RKEvents.inc
#include RKPullerBase.inc
#include RKCollections.inc
#include RKTargeting.inc
#include RKPositioning.inc

Sub Init_RKBase
	/declare RKCommander string outer ${Group.Leader.Name}
	/if (!${Bool[${RKCommander}]}) /varset RKCommander ${Me.Name}
	/declare TankId int outer ${Group.MainTank.ID}
	
	/declare memberIndex int outer 0
	/for memberIndex 0 to ${Group.Members}
		/if (!${Group.MainTank.ID}) {
			/if (${Group.Member[${memberIndex}].Class.Name.Equal[Warrior]} || ${Group.Member[${memberIndex}].Class.Name.Equal[Paladin]} && ${Group.Member[${memberIndex}].Class.Name.Equal[Shadow Knight]}) {
				/varset TankId ${Group.Member[${memberIndex}].ID}
			}
		}
	/next memberIndex
	
	/i quit	
	/makecamp off
	
	|---------- BEHAVIOR ----------
	/declare InCombatStickToCommander bool outer TRUE
	/declare HideCorpsesOnAttack bool outer TRUE
	/declare SitBetweenCombat bool outer TRUE
	/declare IsAutoSitting bool outer FALSE
	/declare MarkMainTarget bool outer TRUE
	/declare PullMode bool outer FALSE
	/declare PullTargets[100] string outer
	/declare LoSPullTargets[100] string outer
	/declare LoSPullRadius int outer 150
	/declare PriorityTargets[100] string outer	
	/declare SideStepRight bool outer TRUE
	/declare SaveNewPullTargets bool outer TRUE
	
	/declare Descending bool outer FALSE
	
	/declare MercBurnThreshold int outer 90
	/declare UpperAggroThreshold int outer 80	
	/declare TargetHpEngagePercentage int outer 99
	/declare MaxTargetDistance int outer 5000
	/declare MaxMeleeDistance int outer 13
	/declare StickTimer timer outer 10
	/declare StickDistance string outer 9
	/declare StickFlags string outer 
	
	/declare LowManaMed bool outer FALSE
	/declare MeddingMessageTimer timer outer 10
	
	/declare ForageMode bool outer FALSE

	/declare KeepAgroOnAdds bool outer FALSE
	|---------- POSITION ----------
	/declare MobsCentroidX float outer
	/declare MobsCentroidY float outer
	
	|---------- SYSTEM ----------
	/declare MacroPaused bool outer FALSE
	/declare DebugMode bool outer FALSE
	/declare BOTMode int outer 3 | 0 = nothing / medding, 1 = follow & prevent action, 2 = set up camp & prevent action, 3 = follow & attack, 4 = set up camp & attack, 5 = manual piloting
	/declare MobMode int outer 1 | 0 = nothing, 1 = standard, 2 = burn, 3 = burn 2
	/declare DynamicArraySize int outer 50
	/declare EarlyEscapePreviousTick bool outer FALSE
	/declare EarlyEscapePauseDuration int outer 0
	
	|---------- SYSTEM - COMMUNICATION ----------
	/declare IrcConnectionString string outer irc.lavishsoft.com 6667 #${RKCommander}
	/declare ChatCommand string outer i say
	/declare ChatChannelName string outer ${RKCommander}
	
	|---------- SYSTEM - TARGETING ----------
	/declare CurrentTargetId int outer 0
	/declare CurrentStickTargetId int outer 0	
	/declare TargetSyncTimer timer outer 50
	/declare BadTargetIds[100] int outer
	
	|---------- SYSTEM - LIFE CYCLE ----------
	/declare InvisModeSubs[${DynamicArraySize}] string outer
	/call AddToArray "InvisModeSubs" "CheckCommanderFollow"	
	/call AddToArray "InvisModeSubs" "CheckInvisModeCombat"
	/call AddToArray "InvisModeSubs" "CheckSitting"

	/declare NonCombatSubs[${DynamicArraySize}] string outer
	/call AddToArray "NonCombatSubs" "CheckSitting"
	/call AddToArray "NonCombatSubs" "CheckCommanderFollow"	
	
	/declare PreTargetAcquisitionCombatActionSubs[${DynamicArraySize}] string outer
	/call AddToArray "PreTargetAcquisitionCombatActionSubs" "CheckCommanderFollow"
	/call AddToArray "PreTargetAcquisitionCombatActionSubs" "ForageAndBag"
	
	/declare CombatActionSubs[${DynamicArraySize}] string outer
	/call AddToArray "CombatActionSubs" "CheckPosition"
	/declare BurnCombatActionSubs[${DynamicArraySize}] string outer
	/declare Burn2CombatActionSubs[${DynamicArraySize}] string outer	
	/declare DefensiveActionSubs[${DynamicArraySize}] string outer	
	/declare AssistCalledSubs[${DynamicArraySize}] string outer
	/declare XPEarnedSubs[${DynamicArraySize}] string outer
	
	|---------- PULLING ----------
	/call Init_RKPullerBase
	
	|---------- CHANNELS & CHAT ----------
	/call ConfigureChannels	
	/declare ChatLanguageVerified bool outer FALSE
	
	/hidecorpse always
	/echo Done initializing RK Base!
/return

Sub Tick
	/if (${DebugMode}) /echo ------------- BEGIN LOOP! -------------		
	/if (!${ChatLanguageVerified}) /lang
		
	/doevents
	
	/if (${EarlyEscapePreviousTick}) /delay ${EarlyEscapePauseDuration}
	
	/if (${MacroPaused} || ${Me.State.Equal[HOVER]}) /return FALSE
	/if (${Me.Casting.ID} && !${Me.Class.Name.Equal[Bard]}) /return TRUE

	/if (${Group.Leader.ID} && !${Group.Leader.Name.Equal[${RKCommander}]}) {
		/echo Changing commander from ${RKCommander} to ${Group.Leader.Name}
		/varset RKCommander ${Group.Leader.Name}
	}
	
	/if (${Group.MainTank.ID} && ${Group.MainTank.ID} != ${TankId}) {
		/echo Changing main tank from ${Spawn[id ${TankId}].Name} to ${Group.MainTank.Name}
		/varset TankId ${Group.MainTank.ID}
	}

	|HACK: sometimes the pet and corpses report as an NPC for some reason, either that or delay (it's probably delay) is causing pets to not be cleared when targeted 
	/if (${Target.Type.Equal[Pet]} || ${Target.Type.Equal[Corpse]}) /target clear
	
	/if (${CurrentTargetId} != ${Me.ID} && !${Spawn[id ${CurrentTargetId}].ID}) /varset CurrentTargetId ${Me.ID}
	

	/if (${SaveNewPullTargets} && ${Target.ID} && ${Target.PctHPs} < 90 &&  ${Target.Type.Equal[NPC]}) {
		/call TrimName "${Target.CleanName}"
		/declare trimmedName string local ${Macro.Return}
		/call GetStringArrayIndex "PullTargets" "${trimmedName}"
		/if (!${Macro.Return}) {
			/echo Adding ${Target.CleanName} to pull targets
			/call AddToArray "PullTargets" "${trimmedName}"
			/echo Adding ${Target.CleanName} to LoS pull targets			
			/call AddToArray "LoSPullTargets" "${trimmedName}"
		}
	}
	
	
	/if (${CurrentStickTargetId} && ${CurrentStickTargetId} != ${Target.ID} && ${Spawn[id ${CurrentStickTargetId}].Type.Equal[NPC]}) /squelch /stick off
		
	/call UpdateMercenaryStance

	/if (!${Bool[${Me.XTarget}]}) {
		/call DoOutOfCombatRoutine
		/if (!${PullMode}) /return ${Macro.Return}
	} else {
		/varset LowManaMed FALSE
	}

	/call DoCombatRoutine
	/if (${Macro.Return}) /return TRUE
/return FALSE

Sub DoCombatRoutine
	/call ExecuteArrayOfSubs "DefensiveActionSubs"
	/if (${DebugMode}) /echo Done with defensives
		
	/if (${BOTMode} != 5) {
		/call ExecuteArrayOfSubs "PreTargetAcquisitionCombatActionSubs"
		/if (${Macro.Return}) {
			/echo Returning early from PreTargetAcquisitionCombatActionSubs
			/return TRUE
		}
		
		/if (${DebugMode}) /echo Starting targeting sequence
		/call ClearArray "BadTargetIds"
		/call GetTarget
		
		/if (!${Macro.Return}) /return FALSE
		/if (${CurrentTargetId} != ${Macro.Return}) {
			/varset CurrentTargetId ${Macro.Return}
			/echo CurrentTargetId set to ${CurrentTargetId}
		}
		|/attack on
		
		/if (${MarkMainTarget} && ${Group} && ${Group.Leader.ID} == ${Me.ID} && (!${Me.GroupMarkNPC[1].ID} || ${Me.GroupMarkNPC[1].ID} != ${CurrentTargetId})) /gmarknpc 1
	}

	/if (!${Target.ID}) /return FALSE
	
	/if (${DebugMode}) /echo Selected target ${Target.ID}


	/if (${TankId} == ${Me.ID}) {
		/if (${Me.PctAggro} < 100) /doability "Taunt"
	} else { 
		/if (${Me.PctAggro} > ${UpperAggroThreshold}) {
			/if (${DebugMode}) /echo Exiting early because of agro
			/return FALSE
		}
	}
	
	/if (${Bool[${Me.XTarget}]} || ${Target.ID}) {
		/if (${MobMode} >= 2 || ${Me.XTarget} > 10) {
			/call ExecuteArrayOfSubs "BurnCombatActionSubs"
			/if (${Macro.Return}) /return TRUE
		}
		
		/if (${MobMode} == 3 || ${Me.XTarget} > 10) {
			/call ExecuteArrayOfSubs "Burn2CombatActionSubs"
			/if (${Macro.Return}) /return TRUE
		}

		/if (${MobMode} == 1 && ${Target.Named}) /varset MobMode 2
		
		/call ExecuteArrayOfSubs "CombatActionSubs"
		/if (${Macro.Return}) /return TRUE
	}
	/varset EarlyEscapePreviousTick FALSE
/return FALSE

Sub DoOutOfCombatRoutine
		/if (${BOTMode} < 3 || ${BOTMode} == 5 && ${Me.Invis}) {
			/call ExecuteArrayOfSubs "InvisModeSubs"
			/delay 1s
			/return FALSE
		}
	
		/if (${Me.Class.CanCast} && ${Me.PctMana} < 10) {
			/varset LowManaMed TRUE
		}	
		
		/if (${LowManaMed}) {
			/if (${Stick.Active}) /stick off
			/declare doneMedding bool local TRUE
			/declare i int local 
			/for i 0 to ${Group.Members}
				/if (${Group.Member[${i}].ID} && !${Group.Member[${i}].Mercenary}) {
					/if (${Group.Member[${i}].Class.CanCast} && ${Group.Member[${i}].PctMana} < 99) {
						/varset doneMedding FALSE
					}
				}
			/next i
		
			/if (${doneMedding}) {
				/varset LowManaMed FALSE
				/i say full mana, all done
				/sit off			
			}
		}

		/if (${LowManaMed} && !(${Me.Sitting} || ${Me.Mount.ID})) {
			/if (${Twist}) /twist off
			/if (${MeddingMessageTimer} == 0) {
				/i say medding, you should med
				/varset MeddingMessageTimer 10
			}
			/sit on
		}

		/if (${LowManaMed}) /return TRUE
	
		/call ExecuteArrayOfSubs "NonCombatSubs"
/return FALSE

Sub ForageAndBag
	/if (!${ForageMode}) /return FALSE
	/if (${Me.AbilityReady[Forage]}) {
		/doability "Forage"
		/delay 1s
		/autoinv
		/delay 5
		/autoinv		
		/autoinv				
	}
/return FALSE

Sub ConfigureChannels
	/if (${DebugMode}) /echo ConfigureChannels
	/if (!${Irc}) {
		/echo /iconnect ${IrcConnectionString} ${Me.Name}
		/iconnect ${IrcConnectionString} ${Me.Name}
		/delay 10s ${Irc}
	}
	
	/if (${Irc}) {
		/varset ChatCommand i say
	} else {
		/if (${MacroQuest.ChatChannels} == 0) {
			/echo Universal chat service is down.
			/varset ChatCommand echo
			/return FALSE
		}

		/if (!${MacroQuest.ChatChannel[1].Equal[${ChatChannelName}]}) {
			/echo Fixing chat channels - leaving ${MacroQuest.ChatChannels} channels
			/declare currentChannel int local
			/declare channelsToLeave int local ${MacroQuest.ChatChannels}
		
			/for currentChannel 1 to ${channelsToLeave}
				/echo leaving channel ${currentChannel}
				/leave 1
				/delay 1s
			/next currentChannel

			/autojoin ${ChatChannelName}, General
			/delay 5
			/join ${ChatChannelName}, General
		}
		
		/varset ChatCommand 1
	}
/return FALSE

Sub CheckSitting
	/if (${DebugMode}) /echo CheckSitting
	/if (!${SitBetweenCombat} || (${BOTMode} != 2 && ${BOTMode} != 4 && ${BOTMode} != 0)) /return FALSE
	/if (!${Bool[${Me.XTarget}]} && !${Me.Mount.ID} && !${Me.Sitting} && !${Me.Moving} && (${Me.PctMana} < 96 || ${Me.PctEndurance} < 96)) {
		/sit on
		/varset IsAutoSitting TRUE
	}
		
	/if (${Me.Sitting} && ${IsAutoSitting} && ${Me.PctMana} >= 99 && ${Me.PctEndurance} >= 99) {
		/sit off
		/varset IsAutoSitting FALSE
	}
/return FALSE

|.. check http://www.macroquest2.com/wiki/index.php/Slot_Names
Sub RKItemSlotCast(string itemSlot)
	/declare itemSpell string local ${Me.Inventory[${itemSlot}].Spell}
	/if (!${Bool[${itemSpell}]}) {
		/echo trying to cast item ${itemSlot} - ${Me.Inventory[${itemSlot}]}, but it doesn't have a clicky
		/return FALSE
	}

	/call WillSelfBuffStack "${itemSpell}"
	/if (${Macro.Return}) {
		/if (${DebugMode}) /echo cast item "${Me.Inventory[${itemSlot}].Name}"
		/cast item "${Me.Inventory[${itemSlot}].Name}"
	} else {
		/echo ${itemSpell} won't stack
	}
/return ${Cast.Result.Equal[CAST_SUCCESS]}

Sub RKItemCast(string buffOrSongName, string itemName)
	/if (${DebugMode}) /echo RKItemCast ${buffOrSongName} ${itemName}
	/if (${Me.Buff[${buffOrSongName}].ID} || ${Me.Song[${buffOrSongName}].ID}) /return FALSE
	
	/call WillSelfBuffStack "${buffOrSongName}"
	/if (${Macro.Return}) /cast item "${itemName}"
/return ${Cast.Result.Equal[CAST_SUCCESS]}

Sub WillSelfBuffStack(string spellName)
	/if (${DebugMode}) /echo WillSelfBuffStack 
	/declare i int local
	/for i 1 to 42
		/if (${Me.Buff[${i}].ID} && !${Spell[${spellName}].WillStack[${Me.Buff[${i}]}]}) {
			/if (${DebugMode}) /echo Self buff: ${spellName} will not stack with ${Me.Buff[${i}].Name}
			/return FALSE
		}
	/next i
	
	/if (${DebugMode}) /echo ${spellName} WILL stack
/return TRUE

Sub WillBuffStack(string spellName, int targetId)
	/if (${DebugMode}) /echo WillSelfBuffStack 
	/call TargetSync ${targetId}
	/delay 5 ${Bool[${Target.Buff}]}
	/declare i int local
	/for i 1 to 42
		/if (${Target.Buff[${i}].ID} && !${Spell[${spellName}].WillStack[${Target.Buff[${i}]}]}) {
			/if (${DebugMode}) /echo Buff: ${spellName} will not stack with ${Target.Buff[${i}].Name} on Target Id: ${targetId}
			/return FALSE
		}
	/next i
	
	/if (${DebugMode}) /echo ${spellName} WILL stack
/return TRUE

Sub UpdateMercenaryStance
	/if (${DebugMode}) /echo UpdateMercenaryStance
	/if (!${Bool[${Me.Mercenary}]} || ${Me.Mercenary.State.Equal[Suspended]}) /return FALSE

	/if (${Me.Mercenary.Class.ClericType}) /return FALSE
	
	/if (${Me.Mercenary.Stance.Equal[Burn]}) {
		/if (!${Target.ID} || ${Target.PctHPs} > ${MercBurnThreshold} || ${Me.TargetOfTarget.ID} == ${Me.Mercenary.ID}) {
			/stance balanced
		}
	} else /if (${Me.Mercenary.Stance.Equal[Balanced]}) {
		/if (${Target.ID} && ${Target.PctHPs} < ${MercBurnThreshold} && ${Me.TargetOfTarget.ID} != ${Me.Mercenary.ID}) {
			/stance burn
		}
	}
/return FALSE

Sub CheckInvisModeCombat
	/if (${DebugMode}) /echo CheckInvisModeCombat

	/if (${Bool[${Me.XTarget}]}) {
		/docommand /${ChatCommand} attack, but stay on me
		/delay 5
		/return FALSE
	}
/return FALSE

Sub RKAACast(string AAName)
	/if (${DebugMode}) /echo RKAACast ${AAName}
	/if (${Me.AltAbilityReady[${AAName}]}) {
		/alt activate ${AltAbility[${AAName}].ID}
		/return TRUE
	}
/return FALSE

Sub RKAACast(string AAName, int targetId)
	/if (${DebugMode}) /echo RKAACast ${AAName} ${targetId}
	
	/if (${Target.ID} != ${targetId}}) {
		/target id ${targetId}
		/delay 5
	}
	
	/if (${Me.AltAbilityReady[${AAName}]}) {
		/if (!${Target.Buff[${Me.AltAbility[${AAName}].Spell.Name}].ID}) {
			/alt activate ${AltAbility[${AAName}].ID}
			/return TRUE
		}
	}
/return FALSE

Sub RKAACast(string AAName, string buffName, int targetId)
	/if (${DebugMode}) /echo RKAACast ${AAName} buffName: ${buffName} targetId: ${targetId}

	if (${Me.Buff[${buffName}].ID}) /return FALSE
	
	/if (${Target.ID} != ${targetId}}) {
		/target id ${targetId}
		/delay 5 ${Target.ID} == ${targetId}
	}
	
	/if (${Me.AltAbilityReady[${AAName}]}) {
		/alt activate ${AltAbility[${AAName}].ID}
		/return TRUE
	}
/return FALSE

Sub RKDisc(string discName)
	/if (${DebugMode}) /echo RKDisc ${discName}
	
	/if (!${Me.CombatAbilityReady[${discName}]}) /return FALSE
	
	/if (${Bool[${Me.ActiveDisc}]}) {
		/if (${Me.CombatAbility[${Me.CombatAbility[${discName}]}].Duration} > 0) {
			|/if (${Me.ActiveDisc.Name.Equal[Steadfast Defense]}) {
			|	/stopdisc
			|	/delay 1s !${Bool[${Me.ActiveDisc}]}
			|} else {
			|	/echo Skipping ${discName} - already running disc - ${Me.ActiveDisc}
			|	/return FALSE
			|}
		}
	}
	
	/disc ${discName}
/return FALSE

Sub ExecuteBaseDefensiveActions

/return FALSE

Sub DropInvis
	/attack on
	/delay 5
	/attack off
	/delay 1s !${Me.Invis}
/return FALSE

