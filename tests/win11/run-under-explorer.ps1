param(
    [int]$ProbeSeconds = 3
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$exe = Join-Path $root "litestep.exe"
$config = Join-Path $root "step.rc"
$log = Join-Path $root "litestep-win11-trace.log"

if (-not (Test-Path $exe)) {
    throw "litestep.exe not found beside this script."
}

if (-not (Test-Path $config)) {
    throw "step.rc not found beside this script."
}

if (-not (Get-Process explorer -ErrorAction SilentlyContinue)) {
    throw "Explorer is not running. This harness is only for the non-shell diagnostic test."
}

Remove-Item $log -Force -ErrorAction SilentlyContinue
$env:LITESTEP_TRACE_FILE = $log

Write-Host "Starting LiteStep under Explorer (diagnostic mode)..."
$ls = Start-Process -FilePath $exe -ArgumentList @("-nostartup", "`"$config`"") -PassThru

Start-Sleep -Seconds 3
$ls.Refresh()
if ($ls.HasExited) {
    Write-Host "LiteStep exited early with code $($ls.ExitCode)."
    if (Test-Path $log) { Get-Content $log }
    exit 1
}

Write-Host "Generating a create/activate/destroy shell-event probe with Notepad..."
$probe = Start-Process -FilePath "$env:SystemRoot\System32\notepad.exe" -PassThru
Start-Sleep -Seconds $ProbeSeconds

try {
    $probe.Refresh()
    if (-not $probe.HasExited) {
        [void]$probe.CloseMainWindow()
        Start-Sleep -Seconds 2
        $probe.Refresh()
        if (-not $probe.HasExited) {
            Stop-Process -Id $probe.Id -Force
        }
    }
} catch {
    Write-Warning "Could not close Notepad cleanly: $($_.Exception.Message)"
}

Start-Sleep -Seconds 2

Write-Host "Requesting LiteStep !Quit..."
& $exe "!Quit" | Out-Null

try {
    Wait-Process -Id $ls.Id -Timeout 10
} catch {
    Write-Warning "LiteStep did not exit within 10 seconds."
}

Write-Host ""
Write-Host "Trace file: $log"
if (Test-Path $log) {
    Write-Host "---- last 100 trace lines ----"
    Get-Content $log | Select-Object -Last 100
} else {
    throw "No trace file was produced."
}
