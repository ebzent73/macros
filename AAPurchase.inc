| AAPurchase.inc Revamped
| Rewritten by 3PO with ideas and code snippets from smattowers
| Revised by gSe7eN for El_Nene's Raid Druid macro
| Date 17/12/2013
|
| Version 1.5
|
| Added 2 new commands (see below), changed ini file by class.
|
| Previous  Changes: Added subscription. If you're not Gold sub, your limit with this 
| include is 1000's AA's. I don't care if you purchased more and neither does this include.
| Added AABuy support, either TRUE or FALSE in your macro so you can turn on the include on the fly
| Seperated 30 aa cap and 50 aa cap. No real point, I just did.
| Cleaned up comments at the top of file.
| Removed comments in code and changed code formatting.
| Added Support for gaining several AA's at once.
| Happy Veteran's Day.
|
| This is a complete revamp of the AAPicker routine.
| It now parses text from the AA Window to determine the Level required to purchase an
| AA as well as the current level and max level of the AA.
| If your level is below the level required to purchae the AA it will skip it.
| It will detect and skip an AA that you do not have the expansion for.
|    This is performed by first making sure you are the right level to purchase it and
|    then if your banked AA is equal to or higher than that needed it will skip it.
| It will also detect if your class is not able to purchase a skill defined if you
| accidentally add a wrong ability and skip it.
| It will tell you when all your abilities are maxed, will warn you if your banked AAs
| are over 27, and if you reach 30 banked it will switch you to full Level XP.
| Can choose to restrict how many levels of an ability to purchase by putting |# where
| where # would equal the max skill you want to purchase. M would be used to purchase
| the to the max possible level of the ability.
| You can also have multiple setcions in the ini file now. AAtoBuy is the default but
| you can now call the init routine like this "/call AAInit inisection" and setup a special
| section to purchase skills for a shroud without having to lose your section for your
| characters Alt Ability skills.
|
| To do: Verbose mode on/off
|
| Usage:
| Setup your ini file with =Name|Ranks|Tab number AA appears on (e.g. General would be 1, Class 3, etc)
|
| For example:
|
|   Your file is AA_Frank.INI
|   [AAtoBuy]
|   AACount=2
|   AA1=Combat Agility|2|1
|   AA2=Combat Stability|M|1
|
| Put "/call AAInit" or "/call AAInit inisection" into the initialization section of your macro.
|
| If you want to use a special purchase section other than AAtoBuy in your ini. For instance to define AA
| you want to purchase when in a shroud change "/call AAInit" to "/call AAInit yoursectionnamehere"
|
| /aachatchannel <channel>   This will change your AAPurchase Spew; default is echo
| /aabuy on/off         This will enable or disable AA buying; default is *OFF*

#Event AAEarned         "#*#You have gained #*# ability point#*#"
#Event AAToggleVar    "[MQ2] AAToggleVar #1#,#2#,#3#,#4#,#5#"
#Event AAToggleString "[MQ2] AAToggleString #1#,#2#,#3#,#4#,#5#"

Sub AAInit(IniSection)
/if (!${Defined[ListNum]}) /declare ListNum string outer
/declare AAIniFile string outer "AA_${Me.Class.ShortName}.ini"
/declare a int local
/declare AAVersion string local 1.511
/if (${AAVersion.NotEqual[${Ini[${AAIniFile},Settings,AAVersion]}]}) {
   /echo AAPurchase version mismatch detected, loading aliases.
   /squelch /alias /aabuy        /echo AAToggleVar AABuying,AABuy,Settings,AABuy,
   /squelch /alias /aachatchannel      /echo ToggleString AA Chat Channel,AAChatChannel,Settings,ChatChannel,
   /ini "${AAIniFile}" Settings AAVersion ${AAVersion}
}
/if (!${Defined[IniSection]}) /declare IniSection string local AAtoBuy
/if (${IniSection.Equal[NULL]}) /varset IniSection AAtoBuy
/call LoadINIVar Settings AABuy TRUE AABuy ${AAIniFile} bool
/call LoadINIVar Settings AAVersion ${AAVersion} AAVersion ${AAIniFile} string
/call LoadINIVar ${IniSection} AACount 1 AACount ${AAIniFile} int
/call LoadINIVar Settings AAChatChannel echo AAChatChannel ${AAIniFile} string
/if (${AACount}) {
      /declare AA[${AACount}] string outer
      /for a 1 to ${AACount}
      /call LoadINIVar ${IniSection} AA${a} " " AA[${a}] ${AAIniFile}
      /next a
}
/return

Sub AAPicker
/declare x int local
/declare Cost int local
/declare AALevelStr string local
/declare curAALevel int local
/declare maxAALevel int local
/declare maxPurchaseLevel int local
/declare nameAA string local
/declare requiredLevel int local
/declare requiredExpansion string local
/declare maxCounter int local 0
/declare lvlDigits int local 2
/if (${Me.Level}<10) /varset lvlDigits 1
/for x 1 to ${AACount}
/varset nameAA ${AA[${x}].Arg[1,"|"]}
/varset ListNum List${AA[${x}].Arg[3,"|"]}
:BuyAdditional
/if (${Window[AAWindow].Child[CanPurchaseFilter].Checked}) /nomodkey /notify AAWindow CanPurchaseFilter leftmouseup
/if (${AA[${x}].Arg[1,"|"].Equal[${nameAA}]}) {
/varset AALevelStr ${Window[AAWindow].Child[${ListNum}].List[${Window[AAWindow].Child[${ListNum}].List[=${nameAA}]}, 2]}
/varset curAALevel ${AALevelStr.Arg[1,"/"]}
/varset maxAALevel ${AALevelStr.Arg[2,"/"]}
/if (${AA[${x}].Arg[2,"|"].Equal[M]}) {
   /varset maxPurchaseLevel ${maxAALevel}
} else /if (${AA[${x}].Arg[2,"|"].Equal[E]}) {
   /${AAChatChannel} Error. ${nameAA} is set to equal but this feature is not active yet.
   /return
} else {
   /varset maxPurchaseLevel ${AA[${x}].Arg[2,"|"]}
   /if (${maxPurchaseLevel}<=0) {
      /${AAChatChannel} Error. ${nameAA} is set to an invalid level to train to. Skipping.
               /next x
        }
}
/call AASelect "${nameAA}"
/varset requiredLevel ${Window[AAWindow].FirstChild.Next.Text.Mid[${Math.Calc[${Window[AAWindow].FirstChild.Next.Text.Find[Level:]} + 7]},${lvlDigits}]}
/varset requiredExpansion ${Window[AAWindow].Child[${ListNum}].List[${Window[AAWindow].Child[${ListNum}].List[=${nameAA}]}, 4]}
/if (!${Me.Subscription.Equal[GOLD]} && ${Me.AAPointsTotal}>1000) {
   /if (${AABuy}) {
      /varset atq FALSE
      /echo Turning off AA autobuying.
      /return
   }
}
/if (${requiredLevel}<=${Me.Level}) {
/if (${curAALevel}<${maxPurchaseLevel}) {
/varset Cost ${Window[AAWindow].Child[${ListNum}].List[${Window[AAWindow].Child[${ListNum}].List[=${nameAA}]}, 3]}
/nomodkey /notify AAWindow CanPurchaseFilter leftmouseup
/if (${Cost}<=${Me.AAPoints} && !${Window[AAWindow].Child[${ListNum}].List[=${nameAA}]}==NULL) {
   /call AASelect "${nameAA}"
        /call AAPurchase
        /varcalc curAALevel ${curAALevel}+1
        /${AAChatChannel} ${Time} Purchased level ${curAALevel}/${maxAALevel} of ${nameAA} for ${Cost} points.
        /delay 2s
        /if (${Me.AAPoints}>0) /goto :BuyAdditional
   /return
   } else /if (${Cost}>${Me.AAPoints}) {
      /echo Unable to purchase ${nameAA} at this time. You have ${Me.AAPoints} AA and need ${Cost} AA.
      /return
   } else {
      /echo You have ${Me.AAPoints} AA and ${nameAA} costs ${Cost} AA. You must not have ${requiredExpansion} or you have not met the prerequisites. Skipping.
      /varcalc maxCounter ${maxCounter}+1
   }
} else {
   /if (${curAALevel}==${maxAALevel} && ${maxPurchaseLevel}==${maxAALevel}) /echo ${nameAA} is Maxed
   /varcalc maxCounter ${maxCounter}+1
       }
} else {
   /echo You are ${Me.Level} and ${nameAA} requires ${requiredLevel}. Skipping.
   }
} else /if (${AltAbility[${nameAA}].ID}==NULL) {
   /${AAChatChannel} ${nameAA} is not a ${Me.Class} Ability.
        /varcalc maxCounter ${maxCounter}+1
      }
/next x
/if (${maxCounter}==${AACount}) {
   /${AAChatChannel} All defined skills maxed or unable to be purchased due to expansion/prerequisite issues. You should define new ones.
   /if (${AABuy}) {
      /varset AABuy FALSE
      /echo Turning off AABuying.
      /return
   }
} else {
   /${AAChatChannel} Cycled through all AA and nothing to purchase this round.
}
/if (${Me.AAPoints}>=29 && ${Me.AAPoints}<30) /${AAChatChannel} WARNING!!! You are at ${Me.AAPoints} banked AA.
/if (${Me.Level}<91) {
   /if (${Me.AAPoints}>=30) {
      /${AAChatChannel} You have maxed your AAs and are unable to purchase any of your defined AAs.
      /${AAChatChannel} Changing you to full level XP. Hopefully you are not max level yet.
      /alt off
   } else {
   /if (${Me.AAPoints}>=50) {  
      /${AAChatChannel} You have maxed your AAs and are unable to purchase any of your defined AAs.
      /${AAChatChannel} Changing you to full level XP. Hopefully you are not max level yet.
      /alt off
      }
   }
}
/return

Sub AASelect(string AAName)
/nomodkey /notify AAWindow AAW_Subwindows tabselect ${ListNum.Right[1]}
/nomodkey /notify AAWindow ${ListNum} listselect ${Window[AAWindow].Child[${ListNum}].List[=${AAName}]}
/nomodkey /notify AAWindow ${ListNum} leftmouse ${Window[AAWindow].Child[${ListNum}].List[=${AAName}]}
/return

Sub AAPurchase
| The actual purchase code.
/nomodkey /notify AAWindow TrainButton leftmouseup
/delay 2s
/if (!${Window[OptionsWindow].Child[OptionsGeneralPage].Child[OGP_AANoConfirmCheckbox].Checked}) {
   :ConfirmWait
   /delay 10s ${Window[ConfirmationDialogBox].Open}
   /if (!${Window[ConfirmationDialogBox].Open}) /goto :ConfirmWait
   /delay 2s
   /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
   /delay 2s
}
/return

Sub Event_AAEarned
/if (${AABuy}==TRUE) /call AAPicker
/return

Sub Event_AAToggleVar(Stuff,TempMessage,TempVar,IniSection,IniEntry,Line)
/if (${Defined[${TempVar}]}) {
   /if (!${Line.Length}) {
      /if (${${TempVar}}) {
         /varset ${TempVar} 0
      } else {
         /varset ${TempVar} 1
      }
   }
   /if (${Line.Arg[1].Equal[ON]} || ${Line.Arg[1].Equal[TRUE]} || ${Line.Arg[1].Equal[1]}) /varset ${TempVar} 1
   /if (${Line.Arg[1].Equal[OFF]} || ${Line.Arg[1].Equal[FALSE]} || ${Line.Arg[1].Equal[0]}) /varset ${TempVar} 0
   /if (${${TempVar}}) /${AAChatChannel} AA Buying is now ON
   /if (!${${TempVar}}) /${AAChatChannel} AA Buying is now OFF
   /ini "AAIniFile" Settings AABuy ${${TempVar}}
}
/return

Sub LoadINIVar(IniSection,IniVar,IniValue,MacroVar,MyIni,VarType)
/if (!${Defined[${MacroVar}]} && ${Defined[VarType]}) /declare ${MacroVar} ${VarType} outer
/declare IniString string local ${Ini[${MyIni},${IniSection},${IniVar},NOTFOUND]}
/varset ${MacroVar} ${IniString}
/if (${IniString.Equal["NOTFOUND"]}) {
   /varset ${MacroVar} ${IniValue}
   /ini ${MyIni} "${IniSection}" "${IniVar}" "${${MacroVar}}"
}
/return

Sub AAEvent_ToggleString(Stuff,TempMessage,TempVar,IniSection,IniEntry,Line)
/if (${Defined[${TempVar}]}) {
   /if (${Line.Length}) {
      /varset ${TempVar} ${Line}
      /ini "${AAIniFile}" ${IniSection} ${IniEntry} "${${TempVar}}"
   }
   /${AAChatChannel} ${TempMessage} is now: ${${TempVar}}
}
/return