A modding library for Kenshi that allows Lua scripts to be loaded and influence game events.
New hooks will (eventually) have to be defined in a dll file alongside the lua, but for now existing hooks can be used.

Hooks can:
* Change input function parameters
* Prevent the original function from executing
* Change the return value (both with and without the existing function executing)

## TODO:
* Provide a way for lua to ask C++ for only filtered hook hits for speed?

## Installation:
* Clone the repo
* Rename `CMakeUserPresets.json.template` to `CMakeUserPresets.json` and change variables in angle brackets.
* Run `vcpkg install`
* Run `cmake --preset x64-debug`
* Run `cmake --build --preset local-debug`
* Run the game. A terminal window should open and contain logs regarding hook installation.

## Lua Scripting

Lua scripts can use any of the hooks defined by declaring a function of the same name. They have access to any of the classes/properties configured in LuaBridgeSetupImplementation.

## Example Lua Scripts:

### Double prices (silently)
```
function Ownerships_addMoney(ownerships, amount)
	amount = amount * 2
	return { args = { [2] = amount } }
end
```

### Everything free (silently)
```
function Ownerships_addMoney(ownerships, amount)
	return { continueOriginal = false }
end
```

### Everything costs 1 (visual as well)
```
function Item_getValue_ukn(item, quantity)
	return { returnValue = 1, continueOriginal = false }
end
```