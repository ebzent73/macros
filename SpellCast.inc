|
| SpellCast.inc
|
| Last Modified: 5/14/2004 9:30pm
| This will cast a spell reliably for you...
|
| Usage:
|       /call Cast "spellname|itemname|AA#|AAskillname" [item|activate|gem#] [nocheck|dismount|nodismount]
|
| If no dismount or nodismount is provided it defaults to nodismount.
|
| This would essentially: /cast "Death Peace"
| example: /call Cast "Death Peace"
|
| This would essentially: Check for the spell in your spell slots, if not there mem it to spell
|                         gem slot 7 and then /cast "Death Peace"
| example: /call Cast "Death Peace" gem7
|
| This would essentially: /cast item "White Rope Bridle"
| example: /call Cast "White Rope Bridle" item
|
| This would essentially: /alt activate 169
| example: /call Cast "169" activate
|  or...
| example: /call Cast "Divine Arbitration" activate
|
| This would dismount if your target was lost or dies mid-cast, cast Burn and mem it to spell
| slot 3 if it wasn't already memmed:
| example: /call Cast "Burn" gem3 dismount
|
| This would not dismount but would move you back and forth if your target was lost or dies mid-cast, cast Burn and mem
| it to spell slot 3 if it wasn't already memmed:
| example: /call Cast "Burn" gem3 nodismount
|
| 'nocheck' is the default for the "loss of target/target dying" value.  If nocheck is specified (Or nothing was specified)
| no target checking will be performed and spellcast.inc should act like the spellcast.inc of the past.
|
| It will return the following values:
| CAST_SUCCESS
| CAST_UNKNOWNSPELL
| CAST_OUTOFMANA
| CAST_OUTOFRANGE
| CAST_CANNOTSEE
| CAST_STUNNED
| CAST_RESISTED
| CAST_TOOK2LONG
| CAST_ABILITYNOTREADY
| CAST_IMMUNESLOW
| CAST_LOSTTARGET
|
| New Vars Modification
| Plazmic's no globals needed version
|
| Oct 09, 2003 - Updated to work with new vars and $char(casting) -gf
| Oct 11, 2003 - switched some logic, removed defines -gf
| Oct 15, 2003 - Item support added by -EqMule
| XXX xx, xxxx - Modified to add automeming of spells. -Goofmester1
| Dec 26, 2003 - fd fail 1 added -m0nk
| Jan 01, 2004 - timeout(5s) added to stop "dead-time" -m0nk
| Jan 01, 2004 - switchd fdfail to a standing check. -m0nk
| Jan 01, 2004 - added silence checking as a stun check -m0nk
| Feb 17, 2004 - added AA activate capabilities -ml2517
| Apr 11, 2004 - Updated for new Parm system -ml2517
| Apr 12, 2004 - Will spit out a different message on immune to slows. -ml2517
| Apr 16, 2004 - Removed /sendkeys and replaced with /keypress. -ml2517
| Apr 17, 2004 - Various code enhancements. -Wassup
| Apr 20, 2004 - Updated all of the /if's to have parenthesis. -ml2517
| Apr 25, 2004 - Updated to new variable system. -ml2517
| Apr 29, 2004 - Fixed Item problem -ml2517
| Apr 29, 2004 - Changed the alt ability to use AltAbilityReady instead of an event. -ml2517
| May 02, 2004 - Added the ability to specify a gem slot to mem spells to. -ml2517
| May 10, 2004 - Updated for new event system.
| May 12, 2004 - Added suggestions for loss of target and stun handling change. -ml2517
| May 13, 2004 - Activate now accepts AA skill by name or number. -ml2517
| May 14, 2004 - Added the nocheck value, it is the default. This bypasses the target checking code. -ml2517
|

#event Fizzle "#*#Your spell fizzles#*#"
#event Interrupt "#*#Your casting has been interrupted#*#"
#event Interrupt "#*#Your spell is interrupted.#*#"
#event Recover "#*#You haven't recovered yet...#*#"
#event Recover "#*#Spell recovery time not yet met.#*#"
#event Resisted "#*#Your target resisted the #*#"
#event OutOfMana "#*#Insufficient Mana to cast this spell!#*#"
#event OutOfRange "#*#Your target is out of range, get closer!#*#"
#event NoLOS "#*#You cannot see your target.#*#"
#event Stunned "#*#You cannot cast while stunned#*#"
#event Stunned "#*#You *CANNOT* cast spells, you have been silenced!#*#"
#event Standing "#*#You must be standing to cast a spell#*#"
#event Standing "#*#has fallen to the ground.#*#"
#event Collapse "#*#Your gate is too unstable, and collapses.#*#"
#event ImmuneSlow "#*#Your target is immune to changes in its attack speed.#*#"


Sub Cast(SpellName,ItemFlag,DismountFlag)
   /declare HaveTarget int local 0
   /declare CastBarTime timer local
   /declare CastCurrLocY float local 0
   /declare CastCurrLocX float local 0
   /varset CastCurrLocY ${Me.Y}
   /varset CastCurrLocX ${Me.X}
   /if (${Target.ID}>0) /varset HaveTarget 1
   /if (!${Defined[CastGiveUpTime]}) /declare CastGiveUpTime timer local
   /if (!${Defined[ItemFlag]}) /declare ItemFlag string local
   /if (!${Defined[DismountFlag]}) {
       /declare DismountFlag string local nocheck
       /if (${ItemFlag.Find["dismount"]}) /varset DismountFlag ${ItemFlag}
   }
   /if (!${Me.Standing} && !${Me.Mount.ID}>0) /stand
   /if (${Me.Moving}) {
       /keypress forward
       /keypress back
       /delay 8
   } else {
       /delay 4
   }
   /if (${ItemFlag.Equal["Item"]} || ${ItemFlag.Equal["Activate"]}) /goto :StartCast

   /if (!${Me.Gem["${SpellName}"]}) {
       /if (${ItemFlag.Find[gem]}) {
           /if (${Int[${ItemFlag.Right[1]}]}>0 && ${Int[${ItemFlag.Right[1]}]}<9) {
               /memspell ${ItemFlag.Right[1]} "${SpellName}"
               /delay 5s
           } else {
               /goto :GenericMem
           }
       } else {       
           :GenericMem
           /memspell 5 "${SpellName}"
           /delay 5s
       }

   }
   :StartCast
      /varset CastGiveUpTime 5s
   :CastNow
      /if (${ItemFlag.Equal["Item"]}) {
         /call ClearReturnValue
         /cast item "${SpellName}"
      } else /if (${ItemFlag.Equal["Activate"]}) {
         /call ClearReturnValue
         /if (!${Me.AltAbilityReady[${SpellName}]}) /return CAST_ABILITYNOTREADY
         /alt activate ${AltAbility[${SpellName}].ID}
         /varset CastBarTime ${Me.Casting.CastTime}
      } else {
         /if (!${Me.Gem["${SpellName}"]}) /return CAST_UNKNOWNSPELL
         /call ClearReturnValue
         /if (!${Me.SpellReady["${SpellName}"]}) {
            /if (${CastGiveUpTime}==0) /return CAST_TOOK2LONG
            /delay 1
            /goto :CastNow
         }
         /cast "${SpellName}"
         /varset CastBarTime ${Math.Calc[${Me.Casting.CastTime}*10]}
      }
   :WaitCast
      /if (${Me.Casting.ID}) {
         /if ((!${Target.ID}>0 || ${Target.Type.Equal[CORPSE]}) && !${DismountFlag.Find[nocheck]} && ${HaveTarget}==1) {
             /if (${Me.Mount.ID}>0) {
                 /if (${DismountFlag.Equal[dismount]}) {
                     /dismount
                 } else {
                     /if (!${ItemFlag.Equal["Item"]}) {
                         /if (${CastBarTime}<7) {
                             :Interrupt
                             /keypress FORWARD hold
                             /delay 6
                             /keypress FORWARD
                             /keypress BACK hold
                             /delay 8
                             /keypress BACK
                         } else {
                             :HoldForSpell
                             /delay 1
                             /if (${CastBarTime}<7) /goto :Interrupt
                             /goto :HoldForSpell
                         }
                     } else {
                         /keypress FORWARD hold
                         :Forward
                         /delay 1
                         /if (${Math.Distance[${CastCurrLocY},${CastCurrLocX}]}<6) /goto :Forward
                         /keypress FORWARD
                         /keypress BACK hold
                         :Backward
                         /delay 1
                         /if (${Math.Distance[${CastCurrLocY},${CastCurrLocX}]}>4) /goto :Backward
                         /keypress BACK
                         /if (!${Me.Casting.ID}>0) /goto :DuckTime
                     }
                 }
             }
             :DuckTime
             /keypress FORWARD
             /keypress BACK
             /if (!${Me.Ducking}) /keypress DUCK
             /delay 1
             /if (${Me.Ducking}) /keypress DUCK
             /return CAST_LOSTTARGET
         }
         /delay 1
         /goto :WaitCast
      }
   /delay 4
   /doevents Fizzle
   /doevents Interrupt
   /doevents Interrupt
   /doevents Recover
   /doevents Standing
   /doevents OutOfRange
   /doevents OutOfMana
   /doevents NoLOS
   /doevents Resisted
   /doevents ImmuneSlow
   /doevents Stunned
   /doevents Collapse
   /delay 1
   /if (${Macro.Return.Equal["CAST_RESTART"]}) /goto :StartCast
   /if (!${Macro.Return.Equal["NULL"]}) /return ${Macro.Return}
/return CAST_SUCCESS

Sub ClearReturnValue
/return NULL

Sub Event_Fizzle
/return CAST_RESTART

Sub Event_Interrupt
/return CAST_RESTART

Sub Event_Recover
   /delay 5
/return CAST_RESTART

Sub Event_Standing
   /stand
/return CAST_RESTART

Sub Event_Collapse
/return CAST_RESTART

Sub Event_OutOfMana
/return CAST_OUTOFMANA

Sub Event_OutOfRange
/return CAST_OUTOFRANGE

Sub Event_NoLOS
/return CAST_CANNOTSEE

Sub Event_Stunned
   /delay 2s !${Me.Stunned}
/return CAST_RESTART

Sub Event_Resisted
/return CAST_RESISTED

Sub Event_ImmuneSlow
/return CAST_IMMUNESLOW