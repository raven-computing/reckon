[Tasks]
Name: "addtopath"; Description: "Add scount to PATH";

[Registry]
; Append {app}\bin to the current user's PATH when the task is selected
Root: HKCU; Subkey: "Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}\bin"; Tasks: addtopath; Flags: preservestringtype
