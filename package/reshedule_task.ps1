#Requires -RunAsAdministrator
# Force administrator privileges
if (-NOT ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Host "This script must be run with administrator privileges" -ForegroundColor Red
    exit 1
}

$taskName = "D2HS Sync with server"
$exePath = "C:\Program Files\DNS2HostsSyncer\d2hs.exe"
$workingDir = "C:\Program Files\DNS2HostsSyncer\"

# Check if the task already exists
if (Get-ScheduledTask -TaskName $taskName -ErrorAction SilentlyContinue) {
    Write-Host "Task already exists, no need to create" -ForegroundColor Green
    exit 0
}

# Validate executable file path
if (-not (Test-Path $exePath)) {
    Write-Host "Error: Executable file not found at $exePath" -ForegroundColor Red
    exit 2
}

# Create trigger (key modification point)
$triggerParams = @{
    Once              = $true
    At                = Get-Date
    RepetitionInterval  = (New-TimeSpan -Minutes 10)
}
$trigger = New-ScheduledTaskTrigger @triggerParams

# Create task action
$action = New-ScheduledTaskAction -Execute $exePath -WorkingDirectory $workingDir

# Configure task settings
$settings = New-ScheduledTaskSettingsSet `
    -AllowStartIfOnBatteries `
    -DontStopIfGoingOnBatteries `
    -StartWhenAvailable `
    -MultipleInstances IgnoreNew

# Configure run account (use SYSTEM account)
$principal = New-ScheduledTaskPrincipal `
    -UserId "NT AUTHORITY\SYSTEM" `
    -LogonType ServiceAccount `
    -RunLevel Highest

# Create task object
$task = New-ScheduledTask `
    -Action $action `
    -Trigger $trigger `
    -Settings $settings `
    -Principal $principal `
    -Description "Sync DNS and Hosts configuration every 10 minutes"

# Register scheduled task
try {
    Register-ScheduledTask -TaskName $taskName -InputObject $task -Force -ErrorAction Stop
    Write-Host "Task created successfully" -ForegroundColor Green
}
catch {
    Write-Host "Task creation failed: $_" -ForegroundColor Red
    exit 3
}
