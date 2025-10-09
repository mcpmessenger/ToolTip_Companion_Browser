param(
  [string]$OutDir = "src\src\out\Default",
  [string]$Target = "chrome",
  [switch]$KeepGoing = $true,
  [switch]$VerboseNinja = $false
)

$ErrorActionPreference = 'Continue'
Set-Location -LiteralPath (Resolve-Path ".")

$LogPath = Join-Path -Path $OutDir -ChildPath "build_chrome.log"
$ConsoleLog = Join-Path -Path $OutDir -ChildPath "build_console.log"
if (!(Test-Path $OutDir)) { Write-Host "Output dir not found: $OutDir"; exit 1 }

"[$(Get-Date -Format s)] start build $Target" | Out-File -FilePath $ConsoleLog -Append -Encoding utf8

$psi = New-Object System.Diagnostics.ProcessStartInfo
$psi.FileName = "cmd.exe"

# Build the autoninja command line with flags
$args = @('autoninja','-C', '"' + $OutDir + '"')
if ($KeepGoing) { $args += @('-k','1000') }
if ($VerboseNinja) { $args += '-v' }
$args += $Target
$cmdLine = ($args -join ' ')

$psi.Arguments = "/c $cmdLine 1>>`"$ConsoleLog`" 2>>&1"
$psi.WorkingDirectory = (Resolve-Path ".")
$psi.UseShellExecute = $false
$psi.RedirectStandardOutput = $false
$psi.RedirectStandardError = $false

$p = [System.Diagnostics.Process]::Start($psi)
if ($null -eq $p) { Write-Host "Failed to start build."; exit 1 }
Write-Host "Build started. PID=$($p.Id). Console log: $ConsoleLog"

exit 0


