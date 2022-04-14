$name = Read-Host "Enter dir name"
Copy-Item 'tutorial/Game' -Destination "tutorial/$($name)"
Add-Content './tutorial/CmakeLists.txt' "`nadd_subdirectory($($name))"
CD tutorial/$name

Copy-Item '../Game/game.cpp' -Destination "$($name).cpp"
Copy-Item '../Game/game.h' -Destination "$($name).h"
Copy-Item '../Game/main.cpp' -Destination "main.cpp"
Copy-Item '../Game/InputManager.h' -Destination "InputManager.h"
Copy-Item '../Game/CmakeLists.txt' -Destination "CmakeLists.txt"

((Get-Content -path './CmakeLists.txt' -Raw) -replace 'Game', $name) | Set-Content -Path './CmakeLists.txt'
((Get-Content -path './InputManager.h' -Raw) -replace 'Game', $name) | Set-Content -Path './InputManager.h'
((Get-Content -path "./$($name).cpp" -Raw) -replace 'Game', $name) | Set-Content -Path "./$($name).cpp"
((Get-Content -path "./$($name).h" -Raw) -replace 'Game', $name) | Set-Content -Path "./$($name).h"
((Get-Content -path './main.cpp' -Raw) -replace 'Game', $name) | Set-Content -Path './main.cpp'





