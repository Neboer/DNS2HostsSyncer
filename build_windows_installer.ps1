param(
    [string]$certlocation,
    [securestring]$certpassword,
    [string]$timestampUrl = "http://timestamp.digicert.com"
)

# Set up build paths
$buildDir = "build"
$installerName = "DNS2HostsSyncer_Installer.exe"
$packageDir = "build/package_install"
$nsisScript = "package/windows/installer.nsi"

# Clean up and create build directory
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# Execute NSIS compilation
Write-Host "Generating installer package..." -ForegroundColor Cyan
makensis -DINSTDIR=$packageDir $nsisScript

# Check NSIS compilation result
if (-not (Test-Path "$buildDir\$installerName")) {
    Write-Host "Installer package generation failed!" -ForegroundColor Red
    exit 1
}

# Signing process logic
if ($certlocation) {
    Write-Host "Signing using certificate..." -ForegroundColor Cyan
    
    # Verify that certificate file exists
    if (-not (Test-Path $certlocation)) {
        Write-Host "Certificate file not found: $certlocation" -ForegroundColor Red
        exit 2
    }

    # Execute signing command
    try {
        & signtool sign /fd SHA256 `
                        /f $certlocation `
                        /tr $timestampUrl `
                        /td SHA256 `
                        /p $certpassword `
                        "$buildDir\$installerName"
        Write-Host "Signature completed successfully" -ForegroundColor Green
    }
    catch {
        Write-Host "Error occurred during signing: $_" -ForegroundColor Red
        exit 3
    }
}
else {
    Write-Host "No certificate path provided, skipping signing step" -ForegroundColor Yellow
}

Write-Host "Build process completed" -ForegroundColor Green
