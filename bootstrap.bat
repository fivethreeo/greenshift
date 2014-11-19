@echo off
setlocal EnableDelayedExpansion

echo "%PROCESSOR_ARCHITECTURE%" | C:\Windows\System32\findstr "64" > nul 2>&1

if %ERRORLEVEL% == 0 (
    echo Found arch 64
) else if %ERRORLEVEL% == 1 (
    echo Found arch 32
) else (
    echo Found arch 32
)

set /P ARCH="64 or 32 bit build? (1: 64 /2: 32) "

if /I "!ARCH!" EQU "2" (
   goto ARCH32
)
if /I "!ARCH!" EQU "1" (
   goto ARCH64
)
goto :EOF
  
:ARCH32
    if defined VS100COMNTOOLS (
      echo Microsft Visual Studio 2010 found at:
      echo "%VS100COMNTOOLS%"
      echo.
      set /P cmn10="Use Microsft Visual Studio 2010 compile and link? (y/n) "
      
      if /I "!cmn10!" EQU "Y" (
        echo Using Microsft Visual Studio 2010
        echo.
        set "COMNTOOLS=%VS100COMNTOOLS%"
      )  
    )
    if defined VS110COMNTOOLS (
      echo Microsft Visual Studio 2012 found at:
      echo "%VS110COMNTOOLS%"
      echo.
      set /P cmn11="Use Microsft Visual Studio 2012 compile and link? (y/n) "
      
      if /I "!cmn11!" EQU "Y" (
        echo Using Microsft Visual Studio 2012
        echo.
        set "COMNTOOLS=%VS110COMNTOOLS%"
      )   
    )
   
    if defined COMNTOOLS (
      endlocal
      echo "Calling %COMNTOOLS%vsvars32.bat"
      call "%COMNTOOLS%vsvars32.bat" > nul
      setlocal
      echo.
    ) else (
      echo Microsft Visual Studio not found
      echo.
      pause > nul
    )
    set TARGETOS=win32
    set TARGETARCH=i386

goto :ENDOFCONF

:ARCH64
  if not exist "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" (
    echo Windows SDK not found
    echo.
    pause > nul
  )
  echo "Calling C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd /x64"
  endlocal
  call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /x64 > nul
  setlocal
  echo.
  set TARGETOS=win64
  set TARGETARCH=i686

goto :ENDOFCONF

:ENDOFCONF
rem set INCLUDE=%CD%\msinttypes;%CD%\ffmpeg;%INCLUDE%
rem set PATH=%PATH%;%CD%\yasm%ARCH%;%CD%\c99-to-c89

goto :EOF

:UnZipFile <ZipFile> <ExtractTo>
rem Windows has no built-in unzip, so generate a VBS script to do it:
rem -------------------------------------------------------------------------
set UZIP_SCRIPT=unzip.vbs
if exist %UZIP_SCRIPT% del /f /q %UZIP_SCRIPT%
echo Option Explicit                                                     > %UZIP_SCRIPT%
echo Dim args, fso, ExtractTo, FilesInZip, objShell, Zipfile            >> %UZIP_SCRIPT%
echo.                                                                   >> %UZIP_SCRIPT%
echo Set args = Wscript.Arguments                                       >> %UZIP_SCRIPT%
echo Set fso = CreateObject("Scripting.FileSystemObject")               >> %UZIP_SCRIPT%
echo ZipFile = fso.GetAbsolutePathName(args(0))                         >> %UZIP_SCRIPT%
echo ExtractTo = fso.GetAbsolutePathName(args(1))                       >> %UZIP_SCRIPT%
echo If NOT fso.FolderExists(ExtractTo) Then                            >> %UZIP_SCRIPT%
echo    fso.CreateFolder(ExtractTo)                                     >> %UZIP_SCRIPT%
echo End If                                                             >> %UZIP_SCRIPT%
echo Set objShell = CreateObject("Shell.Application")                   >> %UZIP_SCRIPT%
echo Set FilesInZip = objShell.NameSpace(ZipFile).items                 >> %UZIP_SCRIPT%
echo objShell.NameSpace(ExtractTo).CopyHere(FilesInZip)                 >> %UZIP_SCRIPT%
echo Set fso = Nothing                                                  >> %UZIP_SCRIPT%
echo Set objShell = Nothing                                             >> %UZIP_SCRIPT%
rem -------------------------------------------------------------------------

cscript //nologo %UZIP_SCRIPT% %1 %2
if exist %UZIP_SCRIPT% del /f /q %UZIP_SCRIPT%
goto :EOF

:DownloadFile <DownloadFrom> <SaveTo> <HexBase>
rem Windows has no built-in wget or curl, so generate a VBS script to do it:
rem -------------------------------------------------------------------------
set DLOAD_SCRIPT=download.vbs
if exist %DLOAD_SCRIPT% del /f /q %DLOAD_SCRIPT%
echo Option Explicit                                                     > %DLOAD_SCRIPT%
echo Dim args, http, fileSystem, adoStream, url, target, status         >> %DLOAD_SCRIPT%
echo.                                                                   >> %DLOAD_SCRIPT%
echo Set args = Wscript.Arguments                                       >> %DLOAD_SCRIPT%
echo Set http = CreateObject("WinHttp.WinHttpRequest.5.1")              >> %DLOAD_SCRIPT%
echo If args.Count=3 Then                                               >> %DLOAD_SCRIPT%
echo    url = ToAscii(args(0))                                          >> %DLOAD_SCRIPT%
echo Else                                                               >> %DLOAD_SCRIPT%
echo    url = args(0)                                                   >> %DLOAD_SCRIPT%
echo End If                                                             >> %DLOAD_SCRIPT%  
echo target = args(1)                                                   >> %DLOAD_SCRIPT%
echo WScript.Echo "Getting '" ^& target ^& "' from '" ^& url ^& "'..."  >> %DLOAD_SCRIPT%
echo.                                                                   >> %DLOAD_SCRIPT%
echo http.Open "GET", url, False                                        >> %DLOAD_SCRIPT%
echo http.Send                                                          >> %DLOAD_SCRIPT%
echo status = http.Status                                               >> %DLOAD_SCRIPT%
echo.                                                                   >> %DLOAD_SCRIPT%
echo If status ^<^> 200 Then                                            >> %DLOAD_SCRIPT%
echo 	WScript.Echo "FAILED to download: HTTP Status " ^& status         >> %DLOAD_SCRIPT%
echo 	WScript.Quit 1                                                    >> %DLOAD_SCRIPT%
echo End If                                                             >> %DLOAD_SCRIPT%
echo.                                                                   >> %DLOAD_SCRIPT%
echo Set adoStream = CreateObject("ADODB.Stream")                       >> %DLOAD_SCRIPT%
echo adoStream.Open                                                     >> %DLOAD_SCRIPT%
echo adoStream.Type = 1                                                 >> %DLOAD_SCRIPT%
echo adoStream.Write http.ResponseBody                                  >> %DLOAD_SCRIPT%
echo adoStream.Position = 0                                             >> %DLOAD_SCRIPT%
echo.                                                                   >> %DLOAD_SCRIPT%
echo Set fileSystem = CreateObject("Scripting.FileSystemObject")        >> %DLOAD_SCRIPT%
echo If fileSystem.FileExists(target) Then fileSystem.DeleteFile target >> %DLOAD_SCRIPT%
echo adoStream.SaveToFile target                                        >> %DLOAD_SCRIPT%
echo adoStream.Close                                                    >> %DLOAD_SCRIPT%
echo.                                                                   >> %DLOAD_SCRIPT%
echo Function ToAscii(hextext)                                          >> %DLOAD_SCRIPT%
echo   Dim y, asciiOUT                                                  >> %DLOAD_SCRIPT%
echo   asciiOUT = ""                                                    >> %DLOAD_SCRIPT%
echo   For y = 1 To Len(hextext)                                        >> %DLOAD_SCRIPT%
echo     asciiOUT = asciiOUT ^& Chr("&H" ^& Mid(hextext, y, 2))         >> %DLOAD_SCRIPT%
echo     y = y + 1                                                      >> %DLOAD_SCRIPT%
echo   Next                                                             >> %DLOAD_SCRIPT%
echo   ToAscii = asciiOUT                                               >> %DLOAD_SCRIPT%
echo End Function                                                       >> %DLOAD_SCRIPT%
rem -------------------------------------------------------------------------

cscript //nologo %DLOAD_SCRIPT% %1 %2 %3
if exist %DLOAD_SCRIPT% del /f /q %DLOAD_SCRIPT%
goto :EOF
