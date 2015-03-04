|------------------------------------------------------------|
|- Uses 'Lessons of the Devoted', on all bots who are able.	-|
|------------------------------------------------------------|
#EVENT Lessons "[#1#] Lesson of the Devoted"
#EVENT Lessons "<#1#> Lesson of the Devoted"
#EVENT Lessons "#1# tells you, 'Lesson of the Devoted'"
#EVENT Lessons "#1# tells the group, 'Lesson of the Devoted'"
#EVENT Lessons "#1# you tell your party, 'Lesson of the Devoted'"
#EVENT Lessons "<#1#> Lesson of the Devoted |/|#*#"
#EVENT Lessons "#1# tells the group, 'Lesson of the Devoted |/|#*#'"
#EVENT Lessons "#1# you tell your party, 'Lesson of the Devoted |/|#*#'"
SUB EVENT_Lessons(line, ChatSender)
/if (${Debug}) /echo |- EVENT_Lessons ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| -Check AA is ready.
		/if (!${Me.AltAbilityReady[Lesson of the Devoted]}) {
			/docommand ${ChatToggle} Lesson of the Devoted is not ready.
		} else {

			|----------------------------------------------------------If I'm a Bard, pause twist
			/if (${Twist.Twisting}) {
				/declare returnTwist bool local TRUE
				/squelch /twist stop
				
			|----------------------------------------------------------If I'm a rogue, stop hiding.
			} else /if (${Me.Class.ShortName.Equal[ROG]} && ${Me.Invis} && !${Me.AbilityReady[Hide]}) {
				/doability Hide
			}
			
			/call e3_Cast ${Me.ID} "Lesson of the Devoted"

			/if (${returnTwist}) {
				/timed 70 /squelch /twist start
				/varset returnTwist 0
			}
		}
	}

/if (${Debug}) /echo <== EVENT_Lessons -|
/RETURN



|----------------------------------------------------------------|
|- Uses 'Infusion of the Faithful', on all bots who are able.	-|
|----------------------------------------------------------------|
#EVENT Infusion "[#1#] Infusion of the Faithful"
#EVENT Infusion "<#1#> Infusion of the Faithful"
#EVENT Infusion "#1# tells you, 'Infusion of the Faithful'"
#EVENT Infusion "#1# tells the group, 'Infusion of the Faithful'"
#EVENT Infusion "#1# you tell your party, 'Infusion of the Faithful'"
#EVENT Infusion "<#1#> Infusion of the Faithful |/|#*#"
#EVENT Infusion "#1# tells the group, 'Infusion of the Faithful |/|#*#'"
#EVENT Infusion "#1# you tell your party, 'Infusion of the Faithful |/|#*#'"
SUB EVENT_Infusion(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_Infusion ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| -Check AA is ready.
		/if (!${Me.AltAbilityReady[Infusion of the Faithful]}) {
			/docommand ${ChatToggle} Infusion of the Faithful is not ready.
		} else {

			|----------------------------------------------------------If I'm a Bard, pause twist
			/if (${Twist.Twisting}) {
				/declare returnTwist bool local TRUE
				/squelch /twist stop
				
			|----------------------------------------------------------If I'm a rogue, stop hiding.
			} else /if (${Me.Class.ShortName.Equal[ROG]} && ${Me.Invis} && !${Me.AbilityReady[Hide]}) {
				/doability Hide
			}
			
			/call e3_Cast ${Me.ID} "Infusion of the Faithful"

			/if (${returnTwist}) {
				/timed 70 /squelch /twist start
				/varset returnTwist 0
			}
		}
	}

/if (${Debug}) /echo <== EVENT_Infusion -|
/RETURN



|--------------------------------------------------------|
|- Uses 'Staunch Recovery', on all bots who are able.	-|
|--------------------------------------------------------|
#EVENT Staunch "[#1#] Staunch Recovery"
#EVENT Staunch "<#1#> Staunch Recovery"
#EVENT Staunch "#1# tells you, 'Staunch Recovery'"
#EVENT Staunch "#1# tells the group, 'Staunch Recovery'"
#EVENT Staunch "#1# you tell your party, 'Staunch Recovery'"
#EVENT Staunch "<#1#> Staunch Recovery |/|#*#"
#EVENT Staunch "#1# tells the group, 'Staunch Recovery |/|#*#'"
#EVENT Staunch "#1# you tell your party, 'Staunch Recovery |/|#*#'"
SUB EVENT_Staunch(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_Staunch ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| -Check AA is ready.
		/if (!${Me.AltAbilityReady[Staunch Recovery]}) {
			/docommand ${ChatToggle} Staunch Recovery is not ready.
		} else {

			|----------------------------------------------------------If I'm a Bard, pause twist
			/if (${Twist.Twisting}) {
				/declare returnTwist bool local TRUE
				/squelch /twist stop
				
			|----------------------------------------------------------If I'm a rogue, stop hiding.
			} else /if (${Me.Class.ShortName.Equal[ROG]} && ${Me.Invis} && !${Me.AbilityReady[Hide]}) {
				/doability Hide
			}
			
			/call e3_Cast ${Me.ID} "Staunch Recovery"

			/if (${returnTwist}) {
				/timed 70 /squelch /twist start
				/varset returnTwist 0
			}
		}
	}

/if (${Debug}) /echo <== EVENT_Staunch -|
/RETURN



|--------------------------------------------------------|
|- Uses 'Steadfast Servant', on all bots who are able.	-|
|--------------------------------------------------------|
#EVENT Servant "[#1#] Steadfast Servant"
#EVENT Servant "<#1#> Steadfast Servant"
#EVENT Servant "#1# tells you, 'Steadfast Servant'"
#EVENT Servant "#1# tells the group, 'Steadfast Servant'"
#EVENT Servant "#1# you tell your party, 'Steadfast Servant'"
#EVENT Servant "<#1#> Steadfast Servant |/|#*#"
#EVENT Servant "#1# tells the group, 'Steadfast Servant |/|#*#'"
#EVENT Servant "#1# you tell your party, 'Steadfast Servant |/|#*#'"
SUB EVENT_Servant(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_Servant ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| -Check AA is ready.
		/if (!${Me.AltAbilityReady[Steadfast Servant]}) {
			/docommand ${ChatToggle} Steadfast Servant is not ready.
		} else {

			|----------------------------------------------------------If I'm a Bard, pause twist
			/if (${Twist.Twisting}) {
				/declare returnTwist bool local TRUE
				/squelch /twist stop
				
			|----------------------------------------------------------If I'm a rogue, stop hiding.
			} else /if (${Me.Class.ShortName.Equal[ROG]} && ${Me.Invis} && !${Me.AbilityReady[Hide]}) {
				/doability Hide
			}
			
			/call e3_Cast ${Me.ID} "Steadfast Servant"

			/if (${returnTwist}) {
				/timed 70 /squelch /twist start
				/varset returnTwist 0
			}
		}
	}

/if (${Debug}) /echo <== EVENT_Servant -|
/RETURN



|----------------------------------------------------------------|
|- Uses 'Intensity of the Resolute', on all bots who are able.	-|
|----------------------------------------------------------------|
#EVENT Intensity "[#1#] Intensity of the Resolute"
#EVENT Intensity "<#1#> Intensity of the Resolute"
#EVENT Intensity "#1# tells you, 'Intensity of the Resolute'"
#EVENT Intensity "#1# tells the group, 'Intensity of the Resolute'"
#EVENT Intensity "#1# you tell your party, 'Intensity of the Resolute'"
#EVENT Intensity "<#1#> Intensity of the Resolute |/|#*#"
#EVENT Intensity "#1# tells the group, 'Intensity of the Resolute |/|#*#'"
#EVENT Intensity "#1# you tell your party, 'Intensity of the Resolute |/|#*#'"
SUB EVENT_Intensity(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_Intensity ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| -Check AA is ready.
		/if (!${Me.AltAbilityReady[Intensity of the Resolute]}) {
			/docommand ${ChatToggle} Intensity of the Resolute is not ready.
		} else {

			|----------------------------------------------------------If I'm a Bard, pause twist
			/if (${Twist.Twisting}) {
				/declare returnTwist bool local TRUE
				/squelch /twist stop
				
			|----------------------------------------------------------If I'm a rogue, stop hiding.
			} else /if (${Me.Class.ShortName.Equal[ROG]} && ${Me.Invis} && !${Me.AbilityReady[Hide]}) {
				/doability Hide
			}
			
			/call e3_Cast ${Me.ID} "Intensity of the Resolute"

			/if (${returnTwist}) {
				/timed 70 /squelch /twist start
				/varset returnTwist 0
			}
		}
	}

/if (${Debug}) /echo <== EVENT_Intensity -|
/RETURN



|--------------------------------------------------------|
|- Uses 'Expedient Recovery', on all bots who are able.	-|
|--------------------------------------------------------|
#EVENT RecoverCorpses "[#1#] Expedient Recovery"
#EVENT RecoverCorpses "<#1#> Expedient Recovery"
#EVENT RecoverCorpses "#1# tells you, 'Expedient Recovery'"
#EVENT RecoverCorpses "#1# tells the group, 'Expedient Recovery'"
#EVENT RecoverCorpses "#1# you tell your party, 'Expedient Recovery'"
#EVENT RecoverCorpses "<#1#> Expedient Recovery |/|#*#"
#EVENT RecoverCorpses "#1# tells the group, 'Expedient Recovery |/|#*#'"
#EVENT RecoverCorpses "#1# you tell your party, 'Expedient Recovery |/|#*#'"
SUB EVENT_RecoverCorpses(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_RecoverCorpses ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| -Check AA is ready.
		/if (!${Me.AltAbilityReady[Expedient Recovery]}) {
			/docommand ${ChatToggle} Expedient Recovery is not ready.
		} else {

			|----------------------------------------------------------If I'm a Bard, pause twist
			/if (${Twist.Twisting}) {
				/declare returnTwist bool local TRUE
				/squelch /twist stop
				
			|----------------------------------------------------------If I'm a rogue, stop hiding.
			} else /if (${Me.Class.ShortName.Equal[ROG]} && ${Me.Invis} && !${Me.AbilityReady[Hide]}) {
				/doability Hide
			}
			
			/call e3_Cast ${Me.ID} "Expedient Recovery"

			/if (${returnTwist}) {
				/timed 70 /squelch /twist start
				/varset returnTwist 0
			}
		}
	}

/if (${Debug}) /echo <== EVENT_RecoverCorpses -|
/RETURN



|----------------------------------------|
|- Arms MGB on all bots who are able.	-|
|----------------------------------------|
|- Use, /bc MGB, or /tell 'Name' [MGB]	-|
|----------------------------------------|
#EVENT MGB "[#1#] Mass Group Buff"
#EVENT MGB "<#1#> Mass Group Buff"
#EVENT MGB "#1# tells you, 'Mass Group Buff'"
#EVENT MGB "#1# tells the group, 'Mass Group Buff'"
#EVENT MGB "#1# you tell your party, 'Mass Group Buff'"
#EVENT MGB "<#1#> Mass Group Buff |/|#*#"
#EVENT MGB "#1# tells the group, 'Mass Group Buff |/|#*#'"
#EVENT MGB "#1# you tell your party, 'Mass Group Buff |/|#*#'"
SUB EVENT_MGB(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_MGB ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		| -Check AA is ready.
		/if (${Me.AltAbility[Mass Group Buff]}) {
			/if (!${Me.AltAbilityReady[Mass Group Buff]}) {
				/docommand ${ChatToggle} Mass Group Buff is not ready.
			} else {

				|----------------------------------------------------------If I'm a Bard, pause twist
				/if (${Twist.Twisting}) {
					/declare returnTwist bool local TRUE
					/squelch /twist stop
					
				|----------------------------------------------------------If I'm a rogue, stop hiding.
				} else /if (${Me.Class.ShortName.Equal[ROG]} && ${Me.Invis} && !${Me.AbilityReady[Hide]}) {
					/doability Hide
				}
				
				/call e3_Cast ${Me.ID} "Mass Group Buff"

				/if (${returnTwist}) {
					/squelch /twist start
					/varset returnTwist 0
				}
			}
		}
	}

/if (${Debug}) /echo <== EVENT_GateToLobby -|
/RETURN



|--------------------------------------------------------|
|- Uses 'Throne of Heroes', on all bots who are able.	-|
|--------------------------------------------------------|
#EVENT GateToLobby "[#1#] Throne of Heroes"
#EVENT GateToLobby "<#1#> Throne of Heroes"
#EVENT GateToLobby "#1# tells you, 'Throne of Heroes'"
#EVENT GateToLobby "#1# tells the group, 'Throne of Heroes'"
#EVENT GateToLobby "#1# you tell your party, 'Throne of Heroes'"
#EVENT GateToLobby "<#1#> Throne of Heroes |/|#*#"
#EVENT GateToLobby "#1# tells the group, 'Throne of Heroes |/|#*#'"
#EVENT GateToLobby "#1# you tell your party, 'Throne of Heroes |/|#*#'"
SUB EVENT_GateToLobby(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_GateToLobby ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Defined[ChatSender]}) /varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {
	
		| -Check AA is ready.
		/if (!${Me.AltAbilityReady[Throne of Heroes]}) {
			/docommand ${ChatToggle} Throne of Heroes is not ready.
		} else {

			|----------------------------------------------------------If I'm a Bard, pause twist
			/if (${Twist.Twisting}) {
				/declare restartTwist bool local TRUE
				/squelch /twist stop
				
			|----------------------------------------------------------If I'm a rogue, stop hiding.
			} else /if (${Me.Class.ShortName.Equal[ROG]} && ${Me.Invis} && !${Me.AbilityReady[Hide]}) {
				/doability Hide
			}
			
			/call e3_Cast 0 "Throne of Heroes"

			/if (${restartTwist}) {
				/timed 180 /squelch /twist start
				/varset restartTwist 0
			}
		}
	}

/if (${Debug}) /echo <== EVENT_GateToLobby -|
/RETURN



Sub VetAAs_Aliases
/if (${Debug}) /echo |- VetAAs_Aliases ==>

	/squelch /alias /Lessons /bc Lesson of the Devoted
	/squelch /alias /Lesson /bc Lesson of the Devoted
	/squelch /alias /Infusions /bc Infusion of the Faithful
	/squelch /alias /Infusion /bc Infusion of the Faithful
	/squelch /alias /Staunch /bc Staunch Recovery
	/squelch /alias /Servants /bc Steadfast Servant
	/squelch /alias /Servant /bc Steadfast Servant
	/squelch /alias /Intensify /bc Intensity of the Resolute
	/squelch /alias /Intensity /bc Intensity of the Resolute
	/squelch /alias /Expedient /bc Expedient Recovery
	/squelch /alias /Corpses /bc Expedient Recovery
	/squelch /alias /MGB /bc Mass Group Buff
	/squelch /alias /throne /bc Throne of Heroes

/if (${Debug}) /echo <== VetAAs_Aliases -|
/return