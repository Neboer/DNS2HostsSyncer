#Requires -RunAsAdministrator

param(
    [Parameter(Mandatory = $true)]
    [string]$BinaryPath,  # Accepts the full path to the binary file

    [Parameter(Mandatory = $true)]
    [string]$LogFilePath # Accepts the full path to the log file
)

# https://stackoverflow.com/a/58723508
function Show-MessageBox {
    [CmdletBinding(PositionalBinding = $false)]
    param(
        [Parameter(Mandatory, Position = 0)]
        [string] $Message,
        [Parameter(Position = 1)]
        [string] $Title,
        [Parameter(Position = 2)]
        [ValidateSet('OK', 'OKCancel', 'AbortRetryIgnore', 'YesNoCancel', 'YesNo', 'RetryCancel')]
        [string] $Buttons = 'OK',
        [ValidateSet('Information', 'Warning', 'Stop')]
        [string] $Icon = 'Information',
        [ValidateSet(0, 1, 2)]
        [int] $DefaultButtonIndex
    )
  
    # So that the $IsLinux and $IsMacOS PS Core-only
    # variables can safely be accessed in WinPS.
    Set-StrictMode -Off
  
    $buttonMap = @{ 
        'OK'               = @{ buttonList = 'OK'; defaultButtonIndex = 0 }
        'OKCancel'         = @{ buttonList = 'OK', 'Cancel'; defaultButtonIndex = 0; cancelButtonIndex = 1 }
        'AbortRetryIgnore' = @{ buttonList = 'Abort', 'Retry', 'Ignore'; defaultButtonIndex = 2; ; cancelButtonIndex = 0 }; 
        'YesNoCancel'      = @{ buttonList = 'Yes', 'No', 'Cancel'; defaultButtonIndex = 2; cancelButtonIndex = 2 };
        'YesNo'            = @{ buttonList = 'Yes', 'No'; defaultButtonIndex = 0; cancelButtonIndex = 1 }
        'RetryCancel'      = @{ buttonList = 'Retry', 'Cancel'; defaultButtonIndex = 0; cancelButtonIndex = 1 }
    }
  
    $numButtons = $buttonMap[$Buttons].buttonList.Count
    $defaultIndex = [math]::Min($numButtons - 1, ($buttonMap[$Buttons].defaultButtonIndex, $DefaultButtonIndex)[$PSBoundParameters.ContainsKey('DefaultButtonIndex')])
    $cancelIndex = $buttonMap[$Buttons].cancelButtonIndex
  
    if ($IsLinux) { 
        Throw "Not supported on Linux." 
    }
    elseif ($IsMacOS) {
  
        $iconClause = if ($Icon -ne 'Information') { 'as ' + $Icon -replace 'Stop', 'critical' }
        $buttonClause = "buttons { $($buttonMap[$Buttons].buttonList -replace '^', '"' -replace '$', '"' -join ',') }"
  
        $defaultButtonClause = 'default button ' + (1 + $defaultIndex)
        if ($null -ne $cancelIndex -and $cancelIndex -ne $defaultIndex) {
            $cancelButtonClause = 'cancel button ' + (1 + $cancelIndex)
        }
  
        $appleScript = "display alert `"$Title`" message `"$Message`" $iconClause $buttonClause $defaultButtonClause $cancelButtonClause"            #"
  
        Write-Verbose "AppleScript command: $appleScript"
  
        # Show the dialog.
        # Note that if a cancel button is assigned, pressing Esc results in an
        # error message indicating that the user canceled.
        $result = $appleScript | osascript 2>$null
  
        # Output the name of the button chosen (string):
        # The name of the cancel button, if the dialog was canceled with ESC, or the
        # name of the clicked button, which is reported as "button:<name>"
        if (-not $result) { $buttonMap[$Buttons].buttonList[$buttonMap[$Buttons].cancelButtonIndex] } else { $result -replace '.+:' }
    }
    else {
        # Windows
        Add-Type -Assembly System.Windows.Forms        
        # Show the dialog.
        # Output the chosen button as a stringified [System.Windows.Forms.DialogResult] enum value,
        # for consistency with the macOS behavior.
        [System.Windows.Forms.MessageBox]::Show($Message, $Title, $Buttons, $Icon, $defaultIndex * 256).ToString()
    }
  
}

# Automatically determine the working directory
$exePath = $BinaryPath
$workingDir = [System.IO.Path]::GetDirectoryName($BinaryPath)
# Validate the existence of the path
if (-not (Test-Path $exePath)) {
    Write-Host "cannot find executable $exePath" -ForegroundColor Red
    Show-MessageBox -Message "cannot find executable $exePath" -Title "Error" -Icon Stop -Buttons OK
    exit 2
}
Write-Output "will create task for $exePath"

# Force administrator privileges
if (-NOT ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Host "This script must be run with administrator privileges" -ForegroundColor Red
    Show-MessageBox -Message "This script must be run with administrator privileges" -Title "Error" -Icon Stop -Buttons OK
    exit 1
}

$taskName = "D2HSAutoSync"
# $exePath = "C:\Program Files\DNS2HostsSyncer\d2hs.exe"
# $workingDir = "C:\Program Files\DNS2HostsSyncer\"

# Check if the task already exists
if (Get-ScheduledTask -TaskName $taskName -ErrorAction SilentlyContinue) {
    Show-MessageBox -Message "Task $taskName already exists, delete and recreate." -Title "Task already exists" -Icon Information -Buttons OK
    Write-Host "Task $taskName already exists, no need to create" -ForegroundColor Green
    Unregister-ScheduledTask -TaskName $taskName -Confirm:$false
    Write-Host "Deleted existing task $taskName" -ForegroundColor Yellow
}

# Create trigger (key modification point)
$triggerParams = @{
    Once               = $true
    At                 = Get-Date
    RepetitionInterval = (New-TimeSpan -Minutes 10)
}
$trigger = New-ScheduledTaskTrigger @triggerParams

# Create task action
$action = New-ScheduledTaskAction -Execute $exePath -WorkingDirectory $workingDir -Argument "--log-file-location `"$LogFilePath`""

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
    Write-Host "Task $taskName created successfully" -ForegroundColor Green
    Show-MessageBox -Message "Task $taskName created successfully" -Title "Task created" -Icon Information -Buttons OK
}
catch {
    Write-Host "Task $taskName creation failed: $_" -ForegroundColor Red
    Show-MessageBox -Message "Task $taskName creation failed: $_" -Title "Error" -Icon Stop -Buttons OK
    exit 3
}
