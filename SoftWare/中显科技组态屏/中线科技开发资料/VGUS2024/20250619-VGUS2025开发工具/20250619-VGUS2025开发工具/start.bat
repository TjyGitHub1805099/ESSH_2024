@echo off
SETLOCAL

:: 替换下面的路径为您的应用程序实际路径
SET AppPath="C:\\VGUS2024 Development Tool\\VGUS2024开发工具.exe"

:: 添加文件类型关联
reg add "HKEY_CLASSES_ROOT\.vt" /ve /t REG_SZ /d "VGUS.VTFile" /f
reg add "HKEY_CLASSES_ROOT\VGUS.VTFile" /ve /t REG_SZ /d "VT File" /f
reg add "HKEY_CLASSES_ROOT\VGUS.VTFile\shell\open\command" /ve /t REG_SZ /d "\"C:\\VGUS2024 Development Tool\\VGUS2024.exe\" \"%%1\"" /f

echo .vt file association complete.
ENDLOCAL
