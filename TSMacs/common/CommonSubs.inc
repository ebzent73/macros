|
| CommonSubs.inc
|
| 1 APR 2006: fixed Clear cursor
|             fixed the EVENT Container in use 
|             fixed The NEW UI - Favorite recipe selection
|
| 26 MAR 2006: Moved Salvage and Readini Sub to here
|              Changed some redundant coding
|              Changed clear cursor to A_Druid_00 clear cursor code
|              Added /nomodkey to all /notify statements
|
#event CombineError "#*#There was no place to put that#*#" 
#event CombineError "#*#You cannot combine these items in this container type!#*#" 
#event CombineError "#*#did not accept these items#*#" 
#event CombineError "#*#You are missing#*#" 
#event Advance "#*#You can no longer advance #*#" 
#event ContUsed "#*#Someone else is using that. Try again later.#*#" 
#event End "#*#can not afford#*#" 
#Event Skillup "#*#become better at#*#" 
#Event Salvage "You failed the combine, but you managed to recover #*#" 
#event LearnedRecipe "#*#You have learned the recipe #1#!"

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| Type input string "Recipe" into search box in new tradeskill interface, 
| click search and select the first recipe returned in the search results. 
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Sub ChooseRecipe 
|--- lets type it in and do a search
	/delay 1s
	/nomodkey /notify TradeskillWnd COMBW_SearchTextEdit leftmouseup 
	/delay 5s 
	/echo Searching for Recipe
	/call TypeThis "${ItemTo}"
	/if (!${Checkrecipe}) {
		/delay 1m ${Window[TradeskillWnd].Child[COMBW_SearchButton].Enabled}  
		/nomodkey /notify TradeskillWnd COMBW_SearchButton leftmouseup 
		/delay 10s
		/nomodkey /notify TradeskillWnd COMBW_RecipeList listselect ${Window[TradeskillWnd].Child[COMBW_RecipeList].List[${ItemTo}]} 
		/varset Checkrecipe ${Window[TradeskillWnd].Child[COMBW_RecipeList].List[${ItemTo}]}

|--- Search could not find it either so lets do a old school combine
           /if (!${Checkrecipe}) {
               /nomodkey /notify TradeskillWnd COMBW_ExperimentButton leftmouseup
               /varset NewUI 2
               /call OLDUICOMBINE
               /nomodkey /notify ${PackCode} Container_DoneButton Leftmouseup
               /return
               } else {
                     /call DoCombine
                     }
        } else {
              /call DoCombine
              }
    /nomodkey /notify TradeskillWnd COMBW_CloseButton Leftmouseup
/return

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| Type out the input string.   
| 
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 
Sub TypeThis(InStr) 
   /echo trying to enter Recipe Desired
   /declare char string local 
   /declare loopctr int local 
   /for loopctr 1 to ${InStr.Length} 
      /varset char ${InStr.Mid[${loopctr},1]} 
      /if (!${char.Length}) { 
         /nomodkey /keypress space chat 
      } else { 
         /nomodkey /keypress ${char} chat 
      } 
   /next loopctr 
/return 

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Sub DoCombine
|
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sub DoCombine
        /call ClearCursor 
    :Loop 
	/nomodkey /notify TradeskillWnd CombineButton leftmouseup 
        /delay 2s ${Cursor.ID} 
        /call ClearCursor 

      	/doevents 
        /delay 2
      /if (${EndingVar}==1) /goto :Loop
      /varset EndingVar 1 

    /return

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Clear Cursor
|
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Sub ClearCursor 
/declare x int local 
:auto_inv 
/if (${Cursor.ID}) { 
  /if (${Cursor.Container}) { 
    /for x 1 to 8 
    /if (!${InvSlot[pack${x}].Item.Container}) /nomodkey /itemnotify pack${x} leftmouseup 
    /next x 
  } else { 
    /timed 5 /autoinventory 
  } 
  /goto :auto_inv 
} 
/return 

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Sub OLD UI COMBINE
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Sub OLDUICOMBINE
   /call OpenPacks
   /call cleanPack "${Container}"

    :Begin
       /doevents
       /if (${EndingVar}==1) {
       /call ClearCursor 
       /for i 1 to 10 
       /call AddComp "${Component[${i}]}" ${i} 
       /next i
       /delay ${Math.Calc[${DelayMult}*1]}s
       /call OldDoCombine 
       /doevents
       /goto :Begin
}
/return


|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| Movement Sub
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sub Movement(Start,Finish,Door)
  /declare Location int inner
  /for Location ${Start} to ${Finish}
     :MoveLoop
     /face nolook loc ${LocArray[${Location}]}
     /if (${Math.Distance[${LocArray[${Location}]}]}>4) {
        /nomodkey /keypress forward hold
        /face nolook loc ${LocArray[${Location}]}

        /if (${Location}==${Door}) {
        /nomodkey /keypress USE
        /nomodkey /keypress USE
         }
     }
     /if (${Math.Distance[${LocArray[${Location}]}]}<=4) {
        /nomodkey /keypress forward
        /goto :next
     }
     /goto :MoveLoop
     :next
  /next Location
/return

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Sub Add Components
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
sub AddComp(CompName, PackSlot) 
   /if (${CompName.Equal[NOTFOUND]}) /return 

   /if (!${FindItem[=${CompName}].ID}) { 
      /echo Could not find ${CompName} in inventory 
      /beep 
      /varset EndingVar 2
      /return 
    } 

   :Loop 
      /nomodkey /ctrl /itemnotify ${FindItem[=${CompName}].InvSlot} leftmouseup 

      /if (!${Cursor.ID}) { 
         /delay ${Math.Calc[${DelayMult}*1]}s
         /goto :Loop 
      } 

   /if (${PackCode.Equal[Enviro]}) { 
      /nomodkey /itemnotify ${InvSlot[enviro${PackSlot}]} leftmouseup 
   } else { 
      /nomodkey /itemnotify in ${PackCode} ${PackSlot} leftmouseup 
   }
   /doevents    
   /return 

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Sub Do the Combine
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
sub OldDoCombine 
/combine ${PackCode} 
/doevents 
/delay ${Math.Calc[${DelayMult}*1]}s
/delay 5s ${Cursor.ID} 
/Call ClearCursor
/doevents 
/return 

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Read INI file for Loc array
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sub ReadINIA(FileName,SectionName) 
   /echo Attempting to Read Section "${SectionName}" Zone Information from ${FileName}... 

    
   /if (${Ini[${FileName},${SectionName},-1,NO].Equal[NO]}) { 
      /echo "${SectionName}" is not a Valid Section for FILE:${FileName}, ending macro... 
 
      /return
   } 
   /declare nValues     int local 
   /declare nArray      int local  0 

|~~~~~~~~~~~~~~~~~~~~~~ Locs ~~~~~~~~~~~~~~~~~
   /Varset nValues ${Ini[${FileName},${SectionName},Value]}
   /echo ${nValues} Locs
   :MakeArraylo 
   /if (!${nValues}) /return
   /if (${nValues}) { 
      /echo Declaring Loc Array... 
      /declare LocArray[${nValues}]  string outer
   }
   /for nArray 1 to ${nValues}
      /varset LocArray[${nArray}] ${Ini[${FileName},${SectionName},Loc${nArray}]}
      /echo  loc ${nArray} defined ${LocArray[${nArray}]}

   /next nArray



   /echo "${SectionName}" Zone Information Read Successfully from ${FileName}... 

/return 


|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
|Sub Event Container being used 
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
Sub Event_ContUsed 
/echo Container in use. Waiting 1 Minute, or until no PCs are nearby and trying again. 
/delay 1m ${SpawnCount[pc radius 50]}<=1
/doevents 
/return 

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Sub Event Combine error
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Sub Event_CombineError
    /varset EndingVar 2
/return 

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Sub Event End
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
Sub Event_End
    /call Ending
/return

Sub Ending
    /echo macro ending
    /end 
/return

|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Sub READ Ini File
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
Sub ReadIni(RecipeName) 
    /echo Running ${RecipeName} recipe 
   /if (${Ini["${Filename}","${RecipeName}",-1,"NOTFOUND"].Equal["NOTFOUND"]}) { 
        /echo Recipe ${RecipeName} not found 
        /endmacro 
    } 
   /varset Container ${Ini[${Filename},${RecipeName},Cont,Enviro]} 
   /if (${Container.Equal[Enviro]}) { 
        /varset PackCode Enviro 
   } else { 
        /varset PackCode ${FindItem[=${Container}].InvSlot} 
      /if (${PackCode.Equal[NULL]}) { 
         /echo Could not find container ${Container} 
         /varset EndingVar 2
         /return
      } 
      | pack1 = 22, ... , pack8 = 29 
      /varset PackCode pack${Math.Calc[${PackCode}-21].Int} 
   }  
    /for i 1 to 10 
        /varset Component[${i}] ${Ini[${Filename},${RecipeName},Comp${Math.Calc[${i}-1].Int},NOTFOUND]}
    /next i 
    /return 


|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
|Sub Event Salvage
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
Sub Event_Salvage
        /if (${NewUI}==2) /call cleanPack "${Container}"
/return

Sub Event_LearnedRecipe(Msgtxt, JustLearned)
        /if (${NewUI}==2) /call cleanPack "${Container}"
/return