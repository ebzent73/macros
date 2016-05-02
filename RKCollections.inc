Sub ExecuteArrayOfSubs(string arrayName)
	/if (${DebugMode}) /echo ExecuteArrayOfSubs (${arrayName})
	
	/declare arrayItem string local
	/declare i int local
	/for i 1 to ${${arrayName}.Size}
		/varset arrayItem ${${arrayName}[${i}]}
		/if (!${Bool[${arrayItem}]}) /return FALSE
		/if (${DebugMode}) /echo Executing ${arrayItem}
		/call ${arrayItem}
		/if (${Bool[${Macro.Return}]} && ${Macro.Return}) {
			/if (${DebugMode}) /echo early escape from ${arrayName} on item ${i} ${arrayItem}
			/varset EarlyEscapePreviousTick TRUE
			/return TRUE
		}
	/next i
/return FALSE

Sub AddToIntArray(string arrayName, int value)
	/if (${DebugMode}) /echo AddToArray ${arrayName} ${value}
	/declare i int local
	/for i 1 to ${${arrayName}.Size}
		/if (!${Bool[${${arrayName}[${i}]}]}) {
			/varset ${arrayName}[${i}] ${value}
			/return ${i}
		}
	/next i
/return FALSE

Sub CreateDictionary(string dictionaryName, int size)
	/if (${DebugMode}) /echo CreateDictionary ${dictionaryName} ${size}
	/declare Dictionary_${dictionaryName}_Keys[${size}] string outer
	/declare Dictionary_${dictionaryName}_Values[${size}] string outer
/return 

Sub AddToDictionary(string dictionaryName, string key, string value)
	/if (${DebugMode}) /echo AddToDictionary ${dictionaryName} ${key} ${value}
	/call AddToDistinctStringArray "Dictionary_${dictionaryName}_Keys" "${key}"
	/varset Dictionary_${dictionaryName}_Values[${Macro.Return}] ${value}
/return ${Macro.Return}

Sub GetFromOrAddToDictionary(string dictionaryName, string key, string value)
	/if (${DebugMode}) /echo AddToDictionary ${dictionaryName} ${key} ${value}
	/call GetStringArrayIndex "Dictionary_${dictionaryName}_Keys" "${key}"
	/if (!${Macro.Return}) {
		/if (${DebugMode}) /echo key wasn't present in dictionary ${dictionaryName} - ${key}
		/call AddToDictionary "${dictionaryName}" "${key}" "${value}"
		/return ${value}
	}
/return ${Dictionary_${dictionaryName}_Values[${Macro.Return}]}

Sub GetFromDictionary(string dictionaryName, string key)
	/if (${DebugMode}) /echo GetFromDictionary ${dictionaryName} ${key} ${value}
	/call GetStringArrayIndex "${Dictionary_${dictionaryName}_Keys}" "${key}"
	/if (!${Macro.Return}) {
		/if (${DebugMode}) /echo key ${key} couldn't be found
		/return FALSE
	}
/return ${Dictionary_${dictionaryName}_Values[${Macro.Return}]}

Sub ClearDictionary(string dictionaryName)
	/call ClearArray "Dictionary_${dictionaryName}_Keys"
	/call ClearArray "Dictionary_${dictionaryName}_Values"
/return TRUE

Sub GetStringArrayIndex(string arrayName, string value)
	/if (${DebugMode}) /echo GetStringArrayIndex ${arrayName} ${value}
	/declare arrayItem int local
	/declare i int local
	/for i 1 to ${${arrayName}.Size}
		/if (${Bool[${${arrayName}[${i}]}]}) {
			/if (${${arrayName}[${i}].Equal[${value}]}) {
				/if (${DebugMode}) /echo index in ${arrayName} for value ${value} was ${i}
				/return ${i}
			}
		}
	/next i
	/if (${DebugMode}) /echo item in array ${arrayName} with value ${value} couldn't be found
/return FALSE

|---------- Currently unused ----------
Sub GetIntArrayIndex(string arrayName, int value)
	/if (${DebugMode}) /echo GetIntArrayIndex ${arrayName} ${value}
	/declare arrayItem int local
	/declare i int local
	/for i 1 to ${${arrayName}.Size}
		/if (${${arrayName}[${i}]} == ${value}) /return ${i}
	/next i
/return FALSE

Sub AddToDistinctIntArray(string arrayName, int value)
	/if (${DebugMode}) /echo AddToDistinctIntArray ${arrayName} ${value}
	/declare i int local
	/for i 1 to ${${arrayName}.Size}
		/if (!${Bool[${${arrayName}[${i}]}]}) {
			/varset ${arrayName}[${i}] ${value}
			/return ${i}
		} else {
			/if (${${arrayName}[${i}]} == ${value}) /return ${i}
		}
	/next i
/return FALSE

Sub AddToDistinctStringArray(string arrayName, string value)
	/if (${DebugMode}) /echo AddToDistinctStringArray ${arrayName} ${value}
	/declare i int local
	/for i 1 to ${${arrayName}.Size}
		/if (!${Bool[${${arrayName}[${i}]}]}) {
			/varset ${arrayName}[${i}] ${value}
			/return ${i}
		} else {
			/if (${${arrayName}[${i}]} == ${value}) /return ${i}
		}
	/next i
/return FALSE

Sub AddToArray(string arrayName, string value)
	/if (${DebugMode}) /echo AddToArray ${arrayName} ${value}
	/declare i int local
	/for i 1 to ${${arrayName}.Size}
		/if (!${Bool[${${arrayName}[${i}]}]}) {
			/varset ${arrayName}[${i}] ${value}
			/return ${i}
		}
	/next i
/return FALSE

Sub ClearArray(string arrayName)
	/if (${DebugMode}) /echo ClearArray ${arrayName}
	/declare i int local
	/for i 1 to ${${arrayName}.Size}
		/varset ${arrayName}[${i}]
	/next i
/return TRUE