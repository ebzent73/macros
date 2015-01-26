| QuickBeg2.inc - Version 3.1
| by MacQ and A_Druid_00
| Copyright 2005 by MacQ all rights reserved.
| This macro is only authorized for the personal use by members in good standing of the MacroQuest2 VIP forums
| If you did not pay for VIP access, then you're a cheap, non contributing bastard who sucks the life out of
| others and will die in hell.
|------------------------------------------------------------------------------------------------------------
| Release History:
| Version 2.0.4
| - Fixed two pet beg problems.
| Version 2.0.3
| - Changed beg request order to group, raid, guild.
| Version 2.0.2
| - Removed some redundant messages in QBResetVar.
| - Removed CharName from buff requests that are not sent via /tells or IRC (i msg).
| - Added check to prevent begging if your class is same as the Source class.
| Version 2.0.1
| - Fixed problem in alias reset.
| - Fixed problem with startup status messages.
|------------------------------------------------------------------------------------------------------------
| Release Notes:
| Include this macro in your main macro.  You will need four subroutine calls to make QuickBeg2 operate.
| Fist add /call QuickBeg2Declares to your variable declaration section, then add /call MeBegBuff,
| /call MeBegItem, and /call PetBegBuff in your main loop.
| The macro uses one INI file called QB2Settings.ini.  It's divided into a [GeneralSettings] section
| and a [CLASS/CHARNAME] section where [CLASS/CHARNAME] is either your character name or your character
| class depending on how you set the variable GroupIniByClass in the [GeneralSettings] section.
|   Variables in INI - [GeneralSettings] Section:
| - GroupIniByClass = TRUE/FALSE if this is set to TRUE, then the INI file will be devided into sections by
|   class name.  If this is set to FALSE, then the INI file will be devided into sections by character name.
| - PetNameSuffix = TRUE/FALSE will append to the end of you pet buff request message the name of your pet.
| - SearchRadius = Maximum radius QuickBeg2 will search for a buffer.
| - SearchZRadius = Maximum zradius QuickBeg2 will search for a buffer.
| - MeBegBuffTotal = The total number of possible personal beg buffs.
| - PetBegBuffTotal = The total number of possible pet beg buffs.
| - MeBegItemTotal = The total number of possible beg items.
| - MeBegBuffNameXX = These variables contain the Name(s) of the Buff(s) you want to beg for YOURSELF along with
|   any Buff(s) that block.  Separate each item by a vertical bar "|", do not start or end with a vertical bar.
|   The XX in BuffNameXX is replaced by a "pseudo array" number (e.g., MeBegBuffName1, MeBegBuffName2, MeBegBuffName3,
|   etc.) XX should never be a number larger than ${MeBegBuffTotal}.
| - MeBegBuffSourceXX = These variables contain the Class(s) that cast the Buff listed in MeBuffNameXX.  List the
|   Primary Class FIRST and any alternate classes after.  If you set AlternateBeg to TRUE, QuickBeg2 will look
|   for alternate classes if no primary class is available. Separate each class by a vertical bar "|", do not start
|   or end with a vertical bar.  The XX in BuffSourceXX is replaced by a "pseudo array" number (e.g., MeBegBuffSource1,
|   MeBegBuffSource2, MeBegBuffSource3, etc.) XX should never be a number larger than ${MeBegBuffTotal}.
| - MeBegBuffAliasXX = These variables contain the Aliases for Buff(s) you want to beg for YOURSELF.  From inside EQ
|   an alias controls on/off the begging for this buff.  The XX in BuffSourceXX is replaced by a "pseudo array"
|   number (e.g., MeBegBuffAlias1, MeBegBuffAlias2, MeBegBuffAlias3, etc.) XX should never be a number larger
|   than ${MeBegBuffTotal}.
| - PetBegBuffNameXX = These variables contain the Name(s) of the Buff(s) you want to beg for YOUR PET along with
|   any buff(s) that block.  Separate each item by a vertical bar "|", do not start or end with a vertical bar.
|   The XX in BuffNameXX is replaced by a "pseudo array" number (e.g., PetBegBuffName1, PetBegBuffName2, PetBegBuffName3,
|   etc.) XX should never be a number larger than ${PetBegBuffTotal}.
| - PetBegBuffSourceXX = These variables contain the Class(s) that cast the Buff(s) listed in MeBuffNameXX.  List the
|   Primary Class FIRST and any alternate classes after.  If you set AlternateBeg to TRUE, QuickBeg2 will look
|   for alternate classes if no primary class is available. Separate each class by a vertical bar "|", do not start
|   or end with a vertical bar.  The XX in BuffSourceXX is replaced by a "pseudo array" number (e.g., PetBegBuffSource1,
|   PetBegBuffSource2, PetBegBuffSource3, etc.) XX should never be a number larger than ${PetBegBuffTotal}.
| - PetBegBuffAliasXX = These variables contain the Aliases for Buff(s) you want to beg for YOUR PET.  From inside EQ
|   an alias controls on/off the begging for this buff.  The XX in BuffSourceXX is replaced by a "pseudo array"
|   number (e.g., PetBegBuffAlias1, PetBegBuffAlias2, PetBegBuffAlias3, etc.) XX should never be a number larger
|   than ${PetBegBuffTotal}.
| - MeBegItemNameXX = These variables list the name(s) of the item(s) you want to beg along with any item(s) that
|   blocks it.  Separate each item by a vertical bar "|", do not start or end with a vertical bar.
|   The XX in MeBegItemNameXX is replaced by a "pseudo array" number (e.g., MeBegItemName1, MeBegItemName2,
|   MeBegItemName3, etc.) XX should never be a number larger than ${TotalMeBegItems}.
| - MeBegItemSourceXX = These variables list the class(s) that cast the Item listed in MeBegItemNameXX.  List the
|   Primary Class FIRST and any alternate classes after.  If you set AlternateBeg to TRUE, QuickBeg2 will look
|   for alternate classes if no primary class is available. Separate each class by a vertical bar "|", do not start
|   or end with a vertical bar.  The XX in MeBegItemSourceXX is replaced by a "pseudo array" number (e.g., MeBegItemSource1,
|   MeBegItemSource2, MeBegItemSource3, etc.) XX should never be a number larger than ${TotalMeBegItems}.
| - MeBegItemAliasXX = These variables contain the Aliases for Item(s) you want to beg for YOURSELF.  From inside EQ
|   an alias controls on/off the begging for this buff.  The XX in ItemSourceXX is replaced by a "pseudo array"
|   number (e.g., MeBegItemAlias1, MeBegItemAlias2, MeBegItemAlias3, etc.) XX should never be a number larger
|   than ${MeBegItemTotal}.
|   Variables in INI - [CLASS/NAME] Section:
| - AskAnyone = TRUE/FALSE if set TRUE will ask anyone for buffs regardless if they are in your raid, group, or
|   guild.  If you set this variable FALSE, QuickBeg2 will only check first for a raid member in range,
|   then a group member in range, then a guild member in range. Alias is /askanyone
| - AskDelay = int variable containing the delay (in DECI-SECONDS) between sending the same buff request. This also
|   means if you asked for a buff, received the buff then its dispelled or you click it off, QuickBeg2 will still
|   wait the time specified in AskDelay before asking again.  Alias is /askdelay
| - BegChannel = channel you will beg for buffs (e.g., tell, say, group, etc.) Alias is /begchannel.
| - BegIfInvis = TRUE/FALSE will or will not beg if you are invised. Alias is /begifinvis.
| - MeBegBuff = TRUE/FALSE master on/off for personal buff begging.  Alias is /mebegbuff.
| - MeBegItem = TRUE/FALSE master on/off for personal item begging. Alias is /mebegitem.
| - PetBegBuff = TRUE/FALSE master on/off for pet buff begging. Alias is /petbegbuff.
| - MinBegLvl = You will not ask for buffs from anyone below this level. Alias is /minbeglvl.
| - MeBegBuffTotal = Total buffs you want to beg.
| - MeBegItemTotal = Total items you want to beg.
| - PetBegBuffTotal = Total pet buffs you want to beg.
| - MeBegBuffStatusXX = TRUE/FALSE flag controlling buff begging for the each personal buff.  The XX in
|   MeBegBuffStatusXX is replaced by the "pseudo array" number (e.g., MeBegBuffStatus1, MeBegBuffStatus2,
|   MeBegBuffStatus3, etc.) XX should never be a number larger greater the ${MeBegBuffTotal}.
| - MeBegBuffMessageXX = The message you want to send when you ask for this buff.  The XX in MeBegBuffMessageXX is
|   replaced by the "pseudo array" number (e.g., MeBegBuffMessage1, MeBegBuffMessage2, MeBegBuffMessage3, etc.)
|   XX should never be a number greater than ${MeBegBuffTotal}.
| - PetBegBuffStatusXX = TRUE/FALSE flag controlling pet buff begging for the buff.  The XX in PetBegBuffStatusXX
|   is replaced by the "pseudo array" number (e.g., PetBegBuffStatus1, PetBegBuffStatus2, PetBegBuffStatus3, etc.)
|   XX should never be a number greater than ${PetBegBuffTotal}.
| - PetBegBuffPetssageXX = The message you want to send when you ask for this pet buff.  The XX in PetBegBuffPetssageXX
|   is replaced by the "pseudo array" number (e.g., PetBegBuffPetssage1, PetBegBuffPetssage2, PetBegBuffPetssage3, etc.)
|   XX should never be a number greater than ${PetBegBuffTotal}.
| - MeBegItemStatusXX = TRUE/FALSE flag controlling buff begging for an item (like a mod rod) not in your inventory or
|   on your cursor.  If QuickBeg2 finds the item in your inventory or on your cursor, it will NOT beg for it. The XX
|   in MeBegItemStatusXX is replaced by the "pseudo array" number (e.g., MeBegItemStatus1, MeBegItemStatus2,
|   MeBegItemStatus3, etc.) XX should never be a number greater than ${MeBegItemTotal}.
| - MeBegItemMessageXX = The message you want to send when you ask for this item.  The XX in MeBegItemMessageXX is
|   replaced by the "pseudo array" number (e.g., MeBegItemMessage1, MeBegItemMessage2, MeBegItemMessage3, etc.)
|   XX should never be a number greater than ${MeBegItemTotal}.
| You can change several settings while the macro is running by typing an Alias.  To use an Alias, from an EQ command
| line, type ALIAS XXX (assuming there is an alias) and XXX is the new value.  You do not need to put quotes around
| the XXX value even if it has spaces.
| Example:
| /askdelay 900 -- sets a the ask delay to 90 seconds.
| /chatchannel i msg -- sets chat channel to "i msg".
| For changing Boolean variables you have a few choices.  You can type an ALIAS, if you defined one in the INI file,
| not followed by a new parameter and QuickBeg2 will simply toggle off (FALSE) if its ON, or ON (TRUE) if its OFF. You
| can also pass a variety of parameters: 1, 0, TRUE, FALSE, ON, or OFF.
| Assuming YOU defined an alias called /begoak which, in this example, might control begging for SteelOak Skin,
| here are your options:
| /begoak on -- turns Oak begging on.
| /begoak off --  turns Oak begging off.
| /begoak TRUE -- turns Oak begging on.
| /begoak FALSE -- turns Oak begging off.
| /begoak -- turns OFF if its currently ON, or turns ON if its currently OFF (toggle style).
|------------------------------------------------------------------------------------------------------------
#event QBSetVar "[MQ2] QBSetVar:#*#"

Sub QuickBeg2Declares
/declare BegCureCounter    int    outer 1
/declare MeBegBuffCounter  int    outer 1
/declare MeBegItemCounter  int    outer 1
/declare PetBegBuffCounter int    outer 1
/declare SpamDelayTimer    timer  outer
/declare QBVersion         string outer 3.0
/if (${Ini[QB2Settings.ini,GeneralSettings,QBVersion,NOTFOUND].NotEqual[${QBVersion}]}) /echo QuickBeg2 writing Aliases to Macroquest.ini.
/call QBDeclareIniVar GroupIniByClass bool   GeneralSettings FALSE
/call QBDeclareIniVar PetNameSuffix   bool   GeneralSettings TRUE
/call QBDeclareIniVar SearchRadius    int    GeneralSettings 100
/call QBDeclareIniVar SearchZRadius   int    GeneralSettings 100
/call QBDeclareIniVar StatusMessages  bool   GeneralSettings FALSE
/call QBDeclareIniVar BegCure         bool   GeneralSettings FALSE
/call QBDeclareIniVar AskDelay        int    ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]} 1800  /askdelay
/call QBDeclareIniVar AskAnyone       bool   ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]} FALSE /askanyone
/call QBDeclareIniVar BegChannel      string ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]} tell  /begchannel
/call QBDeclareIniVar BegIfInvis      bool   ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]} FALSE /begifinvis
/call QBDeclareIniVar MeBegBuff       bool   ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]} FALSE /mebegbuff
/call QBDeclareIniVar PetBegBuff      bool   ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]} FALSE /petbegbuff
/call QBDeclareIniVar MeBegItem       bool   ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]} FALSE /mebegitem
/call QBDeclareIniVar MinBegLvl       int    ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]} 65    /minbeglvl
/declare i int local
/call QBDeclareIniVar MeBegBuffTotal  int GeneralSettings 1
/if (${MeBegBuffTotal}) {
  /for i 1 to ${MeBegBuffTotal}
  /declare MeBegBuffTimer${i} timer  outer
  /call QBDeclareIniVar MeBegBuffName${i}    string GeneralSettings
  /call QBDeclareIniVar MeBegBuffSource${i}  string GeneralSettings
  /call QBDeclareIniVar MeBegBuffStatus${i}  bool   ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]}
  /call QBDeclareIniVar MeBegBuffMessage${i} string ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]}
  /if (${Ini[QB2Settings.ini,GeneralSettings,QBVersion,NOTFOUND].NotEqual[${QBVersion}]}) /call QBAliasIniVar MeBegBuffAlias${i} GeneralSettings MeBegBuffStatus${i}
  /next i
}
/call QBDeclareIniVar BegCureTotal int GeneralSettings 1
/if (${BegCureTotal}) {
  /declare CurseCureTimer   timer outer
  /declare DiseaseCureTimer timer outer
  /declare PoisonCureTimer  timer outer
  /for i 1 to ${BegCureTotal}
  /call QBDeclareIniVar BegCureSource${i} string GeneralSettings
  /call QBDeclareIniVar BegCureAlias${i}  string GeneralSettings
  /call QBDeclareIniVar BegDebuffType${i} string GeneralSettings
  /call QBDeclareIniVar BegCureSource${i} string GeneralSettings
  /call QBDeclareIniVar BegCureStatus${i} bool ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]}
  /if (${Ini[QB2Settings.ini,GeneralSettings,QBVersion,NOTFOUND].NotEqual[${QBVersion}]}) /call QBAliasIniVar BegCureAlias${i} GeneralSettings BegCureStatus${i}
  /next i
  /call QBDeclareIniVar BegCurePoisoned string ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]}
  /call QBDeclareIniVar BegCureDiseased string ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]}
  /call QBDeclareIniVar BegCureCursed   string ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]}
}
/call QBDeclareIniVar PetBegBuffTotal int GeneralSettings 1
/if (${PetBegBuffTotal}) {
  /for i 1 to ${PetBegBuffTotal}
  /declare PetBegBuffTimer${i} timer outer
  /call QBDeclareIniVar PetBegBuffName${i}    string GeneralSettings
  /call QBDeclareIniVar PetBegBuffSource${i}  string GeneralSettings
  /call QBDeclareIniVar PetBegBuffStatus${i}  bool   ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]}
  /call QBDeclareIniVar PetBegBuffMessage${i} string ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]}
  /if (${Ini[QB2Settings.ini,GeneralSettings,QBVersion,NOTFOUND].NotEqual[${QBVersion}]}) /call QBAliasIniVar PetBegBuffAlias${i} GeneralSettings PetBegBuffStatus${i}
  /next i
}
/call QBDeclareIniVar MeBegItemTotal int GeneralSettings 1
/if (${MeBegItemTotal}) {
  /for i 1 to ${MeBegItemTotal}
  /declare MeBegItemTimer${i} timer outer
  /call QBDeclareIniVar MeBegItemName${i}    string GeneralSettings
  /call QBDeclareIniVar MeBegItemSource${i}  string GeneralSettings
  /call QBDeclareIniVar MeBegItemStatus${i}  bool   ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]}
  /call QBDeclareIniVar MeBegItemMessage${i} string ${If[${GroupIniByClass},${Me.Class.ShortName},${Me.CleanName}]}
  /if (${Ini[QB2Settings.ini,GeneralSettings,QBVersion,NOTFOUND].NotEqual[${QBVersion}]}) /call QBAliasIniVar MeBegItemAlias${i} GeneralSettings MeBegItemStatus${i}
  /next i
}
/if (${Ini[QB2Settings.ini,GeneralSettings,QBVersion,NOTFOUND].NotEqual[${QBVersion}]}) /ini "QB2Settings.ini" "GeneralSettings" "QBVersion" "${QBVersion}"
/return

Sub IncrementCounter(VarName,VarMax)
/if (${${VarName}}==${${VarMax}}) {
  /varset ${VarName} 1
  /return
} else {
  /varcalc ${VarName} ${${VarName}}+1
  /return
}
/return

Sub MeBegBuff
/if (${SpamDelayTimer} || !${MeBegBuffTotal} || !${MeBegBuff} || !${Me.FreeBuffSlots} || (${Me.Invis} && !${BegIfInvis})) /return
/declare i int local
/declare ii int local
/for i ${MeBegBuffCounter} to ${MeBegBuffTotal}
/for ii 1 to ${Math.Calc[${MeBegBuffSource${i}.Count[|]}+1]}
/if (${MeBegBuffStatus${i}} && !${MeBegBuffTimer${i}} && ${Spell[${MeBegBuffName${i}}].Stacks[0]}) /call QBRandomBeg "${MeBegBuffMessage${i}}" ${MeBegBuffSource${i}.Arg[${ii},|]} MeBegBuffTimer${i}
/if (${Macro.Return.Equal[BEG_SUCCESS]}) {
  /call IncrementCounter "MeBegBuffCounter" "MeBegBuffTotal"
  /return
}
/next ii
/call IncrementCounter "MeBegBuffCounter" "MeBegBuffTotal"
/next i
/return

Sub PetBegBuff
/if (!${Me.Pet.ID} || !${PetBegBuffTotal} || ${SpamDelayTimer} || !${PetBegBuff} || (${Me.Invis} && !${BegIfInvis})) /return
/declare i int local
/declare ii int local
/for i ${PetBegBuffCounter} to ${PetBegBuffTotal}
/for ii 1 to ${Math.Calc[${PetBegBuffSource${i}.Count[|]}+1]}
/if (${PetNameSuffix}) {
  /if (${PetBegBuffStatus${i}} && !${PetBegBuffTimer${i}} && ${Spell[${PetBegBuffName${i}}].StacksPet[0]}) /call QBRandomBeg "${PetBegBuffMessage${i}} ${Me.Pet.CleanName}" ${PetBegBuffSource${i}.Arg[${ii},|]} PetBegBuffTimer${i}
} else {
  /if (${PetBegBuffStatus${i}} && !${PetBegBuffTimer${i}} && ${Spell[${PetBegBuffName${i}}].StacksPet[0]}) /call QBRandomBeg "${PetBegBuffMessage${i}}" ${PetBegBuffSource${i}.Arg[${ii},|]} PetBegBuffTimer${i}
}
/if (${Macro.Return.Equal[BEG_SUCCESS]}) {
  /call IncrementCounter "PetBegBuffCounter" "PetBegBuffTotal"
  /return
}
/next ii
/call IncrementCounter "PetBegBuffCounter" "PetBegBuffTotal"
/next i
/return

Sub MeBegItem
/if (${SpamDelayTimer} || !${MeBegItemTotal} || !${MeBegItem} || (${Me.Invis} && !${BegIfInvis})) /return
/declare i int local
/declare ii int local
/for i ${MeBegItemCounter} to ${MeBegItemTotal}
/for ii 1 to ${Math.Calc[${ItemName${i}.Count[|]}+1]}
/if (!${MeBegItemStatus${i}} || ${MeBegItemTimer${i}} || ${FindItem[${MeBegItemName${i}.Arg[${ii},|]}].ID} || ${Cursor.Name.Equal[${MeBegItemName${i}.Arg[${ii},|]}]}) {
  /call IncrementCounter "MeBegItemCounter" "MeBegItemTotal"
  /return
}
/next ii
/for ii 1 to ${Math.Calc[${MeBegItemSource${i}.Count[|]}+1]}
/if (${MeBegItemStatus${i}} && !${MeBegItemTimer${i}}) /call QBRandomBeg "${MeBegItemMessage${i}}" ${MeBegItemSource${i}.Arg[${ii},|]} MeBegItemTimer${i}
/if (${Macro.Return.Equal[BEG_SUCCESS]}) {
  /call IncrementCounter "MeBegItemCounter" "MeBegItemTotal"
  /return
}
/next ii
/call IncrementCounter "MeBegItemCounter" "MeBegItemTotal"
/next i
/return

Sub MeBegCure
/if (${SpamDelayTimer} || !${BegCureTotal} || !${BegCure} || (${Me.Invis} && !${BegIfInvis} || !${Debuff})) /return
/declare xx int local
/declare ii int local
/for xx ${BegCureCounter} to ${BegCureTotal}
/if (${Debuff.${BegDebuffType${xx}}} && ${BegCureStatus${xx}}) {
	/for ii 1 to ${Math.Calc[${BegCureSource${xx}.Count[|]}+1]}
  /if (${Debuff.Poisoned} && !${PoisonCureTimer}) /call QBRandomBeg "${BegCurePoisoned}" ${BegCureSource${xx}.Arg[${ii},|]} PoisonCureTimer
  /if (${Debuff.Diseased} && !${DiseaseCureTimer}) /call QBRandomBeg "${BegCureDiseased}" ${BegCureSource${xx}.Arg[${ii},|]} DiseaseCureTimer
  /if (${Debuff.Cursed} && !${CurseCureTimer}) /call QBRandomBeg "${BegCureCursed}" ${BegCureSource${xx}.Arg[${ii},|]} CurseCureTimer
  /if (${Macro.Return.Equal[BEG_SUCCESS]}) {
    /call IncrementCounter "BegCureCounter" "BegCureTotal"
    /return
  }
  /next ii
}
/call IncrementCounter "BegCureCounter" "BegCureTotal"
/next xx
/return

Sub QBRandomBeg(BegBuff,BegClass,BegTimer)
/if (${Me.Class.ShortName.Equal[${BegClass}]})  /return BEG_SUCCESS
/if (${NearestSpawn[PC group ${BegClass} range ${MinBegLvl} 100 radius ${SearchRadius} zradius ${SearchZRadius}].ID}) /call BegCommon "${BegBuff}" ${NearestSpawn[PC group ${BegClass} range ${MinBegLvl} 100]} ${BegTimer}
/if (${Macro.Return.NotEqual[BEG_SUCCESS]} && ${NearestSpawn[PC raid ${BegClass} range ${MinBegLvl} 100 radius ${SearchRadius} zradius ${SearchZRadius}].ID} ) /call BegCommon "${BegBuff}" ${NearestSpawn[PC raid ${BegClass} range ${MinBegLvl} 100]} ${BegTimer}
/if (${Macro.Return.NotEqual[BEG_SUCCESS]} && ${NearestSpawn[PC guild ${BegClass} range ${MinBegLvl} 100 radius ${SearchRadius} zradius ${SearchZRadius}].ID} && ${Me.Guild.NotEqual[NULL]} ) /call BegCommon "${BegBuff}" ${NearestSpawn[PC guild ${BegClass} range ${MinBegLvl} 100]} ${BegTimer}
/if (${Macro.Return.NotEqual[BEG_SUCCESS]} && ${AskAnyone} && ${NearestSpawn[PC ${BegClass} range ${MinBegLvl} 100 radius ${SearchRadius} zradius ${SearchZRadius}].ID} ) /call BegCommon "${BegBuff}" ${NearestSpawn[PC ${BegClass} range ${MinBegLvl} 100]} ${BegTimer}
/return ${Macro.Return}

Sub BegCommon(BegBuff,BegGuy,BegTimer)
/if (${BegChannel.Equal[tell]} || ${BegChannel.Equal[t]} || ${BegChannel.Equal[i msg]} || ${BegChannel.Equal[bct]}) {
  /if (TRUE) /${BegChannel} ${BegGuy} ${BegBuff}
} else {
  /if (TRUE) /${BegChannel} ${BegBuff}
}
/varset ${BegTimer} ${AskDelay}
/varset SpamDelayTimer 3s
/return BEG_SUCCESS

Sub QBDeclareIniVar(VarName,VarType,SectionName,VarValue,Alias)
/if (!${Defined[${VarName}]}) /declare ${VarName} ${VarType} outer
/declare TempString string local ${Ini[QB2Settings.ini,${SectionName},${VarName},NOTFOUND]}
/if (${TempString.Equal[NOTFOUND]}) {
  /varset ${VarName} ${VarValue}
  /ini "QB2Settings.ini" "${SectionName}" "${VarName}" "${${VarName}}"
} else {
  /varset ${VarName} ${TempString}
}
/if (${Ini[QB2Settings.ini,GeneralSettings,QBVersion,NOTFOUND].NotEqual[${QBVersion}]} && ${Defined[Alias]}) /squelch /alias ${Alias} /echo QBSetVar: ${VarName}
}
/return

Sub QBAliasIniVar(VarName,SectionName,AliasVar)
/declare TempString string local ${Ini[QB2Settings.ini,${SectionName},${VarName},NOTFOUND]}
/if (${TempString.NotEqual[NOTFOUND]} && ${Ini[QB2Settings.ini,GeneralSettings,QBVersion,NOTFOUND].NotEqual[${QBVersion}]}) /squelch /alias ${TempString} /echo QBSetVar: ${AliasVar}
/return

Sub Event_QBSetVar(Line)
/declare VarName  string local ${Line.Arg[3]}
/declare NewValue string local ${Line.Arg[4]}
/if (!${Defined[${VarName}]}) /return
/if (${${VarName}(type).Name.Equal[bool]} && ${NewValue.Equal[NULL]}) {
  /if (${${VarName}}) {
    /varset ${VarName} FALSE
    /echo ${VarName} is now OFF
  } else {
    /varset ${VarName} TRUE
    /echo ${VarName} is now ON
  }
} else {
  /if (${${VarName}(type).Name.Equal[bool]}) {
    /if (${NewValue.Equal[on]} || ${NewValue.Equal[1]} || ${NewValue.Equal[true]}) {
      /varset ${VarName} TRUE
    } else {
      /if (${NewValue.Equal[off]} || ${NewValue.Equal[0]} || ${NewValue.Equal[false]}) {
        /varset ${VarName} FALSE
      } else {
        /echo ${VarName} was NOT changed
      }
    }
  } else {
    /if (!${NewValue.Equal[NULL]}) {
      /varset ${VarName} ${NewValue}
      /echo ${VarName} is now ${${VarName}}
    } else {
      /echo ${VarName} was NOT changed
    }
  }
}
/if (${GroupIniByClass}) {
  /ini "QB2Settings.ini" "${Me.Class.ShortName}" "${VarName}" "${${VarName}}"
} else {
  /ini "QB2Settings.ini" "${Me.CleanName}" "${VarName}" "${${VarName}}"
}
/return
