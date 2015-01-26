|Wait4rez.inc v2.21
Sub Wait4Rez
/echo Awaiting rez: auto-accepting, and looting my corpse
/consent group
/delay 25
/consent raid
/delay 25
/consent guild
:rezmeffs
/doevents
/delay 10s ${Window[ConfirmationDialogBox].Open}
/if (!${Window[ConfirmationDialogBox].Open}) /goto :rezmeffs
/nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
/delay 2s
/if (${Window[RespawnWnd].Open}) {
    /nomodkey /notify RespawnWnd RW_OptionsList listselect 2 
    /delay 1s
    /nomodkey /notify RespawnWnd RW_SelectButton leftmouseup
    /delay 2s 
}
:zoneffs
/delay 5m ${Spawn[${Me}'s corpse].ID}
/delay 5s ${Spawn[${Me}'s corpse].Distance}<100
/if (${Spawn[${Me}'s corpse].ID}) /target mycorpse
/delay 5s ${Target.ID}==${Spawn[${Me}'s corpse].ID}
/if (${Target.CleanName.NotEqual[${Me}'s corpse]}) /goto :zoneffs
:corpsepull
/if (${Target.Distance}>100 || !${Spawn[${Me}'s corpse].ID}) /return
/if (${Target.Distance}>20) /corpse
/delay 5s ${Target.Distance}<20
/loot
/delay 5s ${Me.State.Equal[BIND]}
/if (${Me.State.NotEqual[BIND]}) /goto :corpsepull
/declare loottotal int local
:LootLag
/varset loottotal ${Corpse.Items}
/delay 1s ${loottotal}!=${Corpse.Items}
/if (${loottotal}!=${Corpse.Items}) /goto :LootLag
/declare i int local
/for i 1 to ${loottotal}
:lootagain
/itemnotify loot${i} rightmouseup
/delay 10s !${Corpse.Item[${i}].ID}
/if (${Corpse.Item[${i}].ID}) /goto :lootagain
/next i
/nomodkey /notify LootWnd DoneButton leftmouseup
/return
