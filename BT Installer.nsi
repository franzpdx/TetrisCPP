; Modern Interface Blue Tetris Installer

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Blue Tetris v2.0"
  OutFile "Blue Tetris 2.0 Installation.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\Blue Tetris"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Blue Tetris" ""

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Blue Tetris" SecMain

  SetOutPath "$INSTDIR\Bitmaps\"
  File Bitmaps\classici.bmp		; Bitmaps Directory
  File Bitmaps\classicj.bmp
  File Bitmaps\classicl.bmp
  File Bitmaps\classico.bmp
  File Bitmaps\classics.bmp
  File Bitmaps\classict.bmp
  File Bitmaps\classicz.bmp
  File Bitmaps\credits.bmp
  File Bitmaps\gameboyi.bmp
  File Bitmaps\gameboyj.bmp
  File Bitmaps\gameboyl.bmp
  File Bitmaps\gameboyo.bmp
  File Bitmaps\gameboys.bmp
  File Bitmaps\gameboyi.bmp
  File Bitmaps\gameboyz.bmp
  File Bitmaps\moderni.bmp
  File Bitmaps\modernj.bmp
  File Bitmaps\modernl.bmp
  File Bitmaps\moderno.bmp
  File Bitmaps\moderns.bmp
  File Bitmaps\modernt.bmp
  File Bitmaps\modernz.bmp
  File Bitmaps\mariner10.bmp

  CreateDirectory "$INSTDIR\Data\"	; Data Directory (empty)

  SetOutPath "$INSTDIR\Music"
  File "Music\Dubmood - ST-Style.xm"	; Music Directory
  File "Music\Floppi - Jttimarsu Edit.xm"
  File "Music\Gammis - Saturn.xm"
  File "Music\Tetris - Korobeiniki.mp3"


  SetOutPath "$INSTDIR\Sound"
  File Sound\Lock.wav			; Sound Directory
  File Sound\Bump.wav
  File Sound\Rotate.wav
  File Sound\Impact.wav

  SetOutPath "$INSTDIR"
  File "Release\Blue Tetris.exe"		; Main directory
;  File "Release\Blue Tetris Server.exe"
;  File "Initialize Database.sql"
  File Release\fmodex.dll
  File Release\fmodex_vc.lib
  File Release\GlAux.Lib
  File Release\glu32.dll
  File Release\glut32.dll
  File Release\glut32.lib
  File /a credits.doc
  
  ;Store installation folder
  WriteRegStr HKCU "Software\Blue Tetris" "" $INSTDIR
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Blue Tetris" "DisplayName" "Blue Tetris"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Blue Tetris" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Blue Tetris" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Blue Tetris" "NoRepair" 1
  WriteUninstaller "Uninstall.exe"

SectionEnd

Section "Start Menu Shortcuts" SecShortcuts

  CreateDirectory "$SMPROGRAMS\Blue Tetris"
  CreateShortCut "$SMPROGRAMS\Blue Tetris\Blue Tetris.lnk" "$INSTDIR\Blue Tetris.exe" "" "" 0
;  CreateShortCut "$SMPROGRAMS\Blue Tetris\Blue Tetris Server.lnk" "$INSTDIR\Blue Tetris Server.exe" "" "" 0
  CreateShortCut "$SMPROGRAMS\Blue Tetris\Credits.lnk" $INSTDIR\Credits.doc" "" "" 0
;  CreateShortCut "$SMPROGRAMS\Blue Tetris\DB Init Script.lnk" "$INSTDIR\Initialize Database.sql" "" "" 0
  CreateShortCut "$SMPROGRAMS\Blue Tetris\Uninstall BT.lnk" "$INSTDIR\Uninstall.exe" "" "" 0

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecMain ${LANG_ENGLISH} "Blue Tetris Game Files"
  LangString DESC_SecShortcuts ${LANG_ENGLISH} "Start Menu Shortcuts"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} $(DESC_SecMain)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecShortcuts} $(DESC_SecShortcuts)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section


Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Blue Tetris"
  DeleteRegKey HKLM "Software\Blue Tetris"

  ; Remove files and uninstaller
  Delete "$INSTDIR\Blue Tetris.exe"		; Main directory
  Delete "$INSTDIR\Blue Tetris Server.exe"
  Delete "$INSTDIR\Initialize Database.sql"
  Delete $INSTDIR\fmodex.dll
  Delete $INSTDIR\fmodex_vc.lib
  Delete $INSTDIR\GlAux.Lib
  Delete $INSTDIR\glu32.dll
  Delete $INSTDIR\glut32.dll
  Delete $INSTDIR\glut32.lib
  Delete $INSTDIR\Credits.doc

  Delete $INSTDIR\Bitmaps\classici.bmp		; Bitmaps Directory
  Delete $INSTDIR\Bitmaps\classicj.bmp
  Delete $INSTDIR\Bitmaps\classicl.bmp
  Delete $INSTDIR\Bitmaps\classico.bmp
  Delete $INSTDIR\Bitmaps\classics.bmp
  Delete $INSTDIR\Bitmaps\classict.bmp
  Delete $INSTDIR\Bitmaps\classicz.bmp
  Delete $INSTDIR\Bitmaps\credits.bmp
  Delete $INSTDIR\Bitmaps\gameboyi.bmp
  Delete $INSTDIR\Bitmaps\gameboyj.bmp
  Delete $INSTDIR\Bitmaps\gameboyl.bmp
  Delete $INSTDIR\Bitmaps\gameboyo.bmp
  Delete $INSTDIR\Bitmaps\gameboys.bmp
  Delete $INSTDIR\Bitmaps\gameboyi.bmp
  Delete $INSTDIR\Bitmaps\gameboyz.bmp
  Delete $INSTDIR\Bitmaps\moderni.bmp
  Delete $INSTDIR\Bitmaps\modernj.bmp
  Delete $INSTDIR\Bitmaps\modernl.bmp
  Delete $INSTDIR\Bitmaps\moderno.bmp
  Delete $INSTDIR\Bitmaps\moderns.bmp
  Delete $INSTDIR\Bitmaps\modernt.bmp
  Delete $INSTDIR\Bitmaps\modernz.bmp
  Delete $INSTDIR\Bitmaps\mariner10.bmp

  Delete "$INSTDIR\Music\Dubmood - ST-Style.xm"	; Music Directory
  Delete "$INSTDIR\Music\Floppi - Jttimarsu Edit.xm"
  Delete "$INSTDIR\Music\Gammis - Saturn.xm"
  Delete "$INSTDIR\Music\Tetris - Korobeiniki.mp3"

  Delete $INSTDIR\Sound\Lock.wav			; Sound Directory
  Delete $INSTDIR\Sound\Bump.wav
  Delete $INSTDIR\Sound\Rotate.wav
  Delete $INSTDIR\Sound\Impact.wav

  Delete $INSTDIR\Data\keymap.dat
  Delete $INSTDIR\Data\scores.dat
  Delete $INSTDIR\Data\serverscores.dat
  Delete $INSTDIR\Data\settings.xml

  Delete $INSTDIR\Uninstall.exe

  ; Remove shortcuts
  Delete "$SMPROGRAMS\Blue Tetris\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Blue Tetris"
  RMDir "$INSTDIR\Sound"
  RMDir "$INSTDIR\Bitmaps"
  RMDir "$INSTDIR\Data"
  RMDir "$INSTDIR\Music"
  RMDir "$INSTDIR"

SectionEnd