# LiteStep 0.25 Windows 11 diagnostic run

This package is a **non-shell** runtime test. It is designed to run while
Explorer is still the Windows shell.

## Safety

Use a disposable Windows user account, Windows Sandbox, or a VM for the first
run. The included step.rc explicitly avoids closing Explorer or setting
LiteStep as the shell, and disables LiteStep's tray service for this stage.

## Run

Right-click **run-under-explorer.ps1** and run it with PowerShell, or from a
PowerShell prompt:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\run-under-explorer.ps1
```

The script:

1. verifies Explorer is running;
2. starts LiteStep with startup applications disabled;
3. launches and closes Notepad to generate shell events;
4. sends LiteStep `!Quit`;
5. writes and prints `litestep-win11-trace.log`.

Send that trace log back for analysis.

Do not configure LiteStep as the Windows shell yet.
