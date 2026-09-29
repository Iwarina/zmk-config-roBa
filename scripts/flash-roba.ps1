#requires -Version 5.1

[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^[0-9a-fA-F]{7,40}$')]
    [string]$Commit,

    [string]$LeftDrive,
    [string]$RightDrive,
    [switch]$ResetSettings,
    [switch]$Yes,
    [switch]$ValidateOnly,
    [ValidateRange(5, 60)]
    [int]$UnmountTimeoutSeconds = 15
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-Uf2Image {
    param([string]$Directory, [string]$Prefix)

    $files = @(Get-ChildItem -LiteralPath $Directory -File -Filter "$Prefix*.uf2")
    if ($files.Count -ne 1) {
        throw "Expected exactly one $Prefix*.uf2 in $Directory; found $($files.Count)."
    }

    $stream = [System.IO.File]::OpenRead($files[0].FullName)
    try {
        if ($stream.Length -lt 512 -or ($stream.Length % 512) -ne 0) {
            throw "Invalid UF2 length: $($files[0].FullName)"
        }
        $blockCount = [int]($stream.Length / 512)
        $block = New-Object byte[] 512
        for ($index = 0; $index -lt $blockCount; $index++) {
            if ($stream.Read($block, 0, 512) -ne 512 -or
                [BitConverter]::ToUInt32($block, 0) -ne 0x0A324655 -or
                [BitConverter]::ToUInt32($block, 4) -ne 2656915799 -or
                [BitConverter]::ToUInt32($block, 20) -ne $index -or
                [BitConverter]::ToUInt32($block, 24) -ne $blockCount -or
                [BitConverter]::ToUInt32($block, 28) -ne 2913282112 -or
                [BitConverter]::ToUInt32($block, 508) -ne 0x0AB16F30) {
                throw "Invalid UF2 block $index in $($files[0].FullName)"
            }
        }
    }
    finally {
        $stream.Dispose()
    }

    return $files[0]
}

function ConvertTo-DriveRoot {
    param([string]$Letter)

    if ($Letter -notmatch '^[A-Za-z]:?$') {
        throw "Use a single drive letter, for example O or O: (received '$Letter')."
    }
    return $Letter.Substring(0, 1).ToUpperInvariant() + ':\'
}

function Assert-Bootloader {
    param([string]$Root, [string]$Side)

    $drive = [System.IO.DriveInfo]::new($Root)
    if (-not $drive.IsReady -or $drive.DriveType -ne [System.IO.DriveType]::Removable) {
        throw "$Side drive $Root is not a ready removable drive."
    }
    if ($drive.VolumeLabel -ne 'XIAO-SENSE') {
        throw "$Side drive $Root has unexpected label '$($drive.VolumeLabel)'."
    }

    $infoPath = Join-Path $Root 'INFO_UF2.TXT'
    if (-not (Test-Path -LiteralPath $infoPath -PathType Leaf)) {
        throw "$Side drive $Root has no INFO_UF2.TXT. Enter bootloader mode first."
    }
    $info = Get-Content -LiteralPath $infoPath -Raw
    if ($info -notmatch '(?m)^Model: Seeed XIAO nRF52840\r?$' -or
        $info -notmatch '(?m)^Board-ID: Seeed_XIAO_nRF52840_Sense\r?$') {
        throw "$Side drive $Root is not the expected XIAO nRF52840 Sense bootloader."
    }
}

function Wait-ForUnmount {
    param([string]$Root)

    $infoPath = Join-Path $Root 'INFO_UF2.TXT'
    $deadline = [DateTime]::UtcNow.AddSeconds($UnmountTimeoutSeconds)
    while ([DateTime]::UtcNow -lt $deadline) {
        if (-not (Test-Path -LiteralPath $infoPath)) {
            return
        }
        Start-Sleep -Milliseconds 250
    }
    throw "The bootloader at $Root did not unmount after the UF2 copy. Stop and check the device."
}

function Write-Uf2 {
    param([System.IO.FileInfo]$Image, [string]$Root, [string]$Side)

    Assert-Bootloader -Root $Root -Side $Side
    Write-Host "Flashing $Side at $Root with $($Image.Name)"
    Copy-Item -LiteralPath $Image.FullName -Destination $Root -ErrorAction Stop
    Wait-ForUnmount -Root $Root
    Write-Host "$Side bootloader disconnected after flashing."
}

$firmwareRoot = Join-Path $PSScriptRoot '..\..\firmware'
$firmwareDirectory = (Resolve-Path -LiteralPath (Join-Path $firmwareRoot $Commit)).Path
$leftImage = Get-Uf2Image -Directory $firmwareDirectory -Prefix 'roBa_L-'
$rightImage = Get-Uf2Image -Directory $firmwareDirectory -Prefix 'roBa_R-'
$resetImage = Get-Uf2Image -Directory $firmwareDirectory -Prefix 'settings_reset-'

Write-Host "Firmware: $firmwareDirectory"
Write-Host "Left:     $($leftImage.Name)"
Write-Host "Right:    $($rightImage.Name)"
if ($ValidateOnly) {
    Write-Host 'UF2 preflight passed. No device was written.'
    return
}

if (-not $LeftDrive -or -not $RightDrive) {
    throw 'Specify both -LeftDrive and -RightDrive. The bootloader volume cannot identify the keyboard half.'
}
$leftRoot = ConvertTo-DriveRoot $LeftDrive
$rightRoot = ConvertTo-DriveRoot $RightDrive
if ($leftRoot -eq $rightRoot) {
    throw 'Left and right must use different drive letters.'
}

Assert-Bootloader -Root $leftRoot -Side 'left'
Assert-Bootloader -Root $rightRoot -Side 'right'
Write-Host "LEFT=$leftRoot RIGHT=$rightRoot"
if ($ResetSettings) {
    Write-Warning 'Settings reset erases Bluetooth bonds and ZMK Studio keymap/settings on BOTH halves.'
}
if (-not $Yes) {
    $confirmation = Read-Host 'Type FLASH to continue'
    if ($confirmation -cne 'FLASH') {
        throw 'Cancelled before writing any device.'
    }
}

if ($ResetSettings) {
    Write-Uf2 -Image $resetImage -Root $leftRoot -Side 'left settings reset'
    Write-Uf2 -Image $resetImage -Root $rightRoot -Side 'right settings reset'

    Write-Host 'Double-tap reset on BOTH halves to re-enter their bootloaders.'
    Write-Host 'The drive letters may change. Check which letter belongs to each half.'
    $newLeft = Read-Host "Left drive letter [${leftRoot}]"
    $newRight = Read-Host "Right drive letter [${rightRoot}]"
    if ($newLeft) { $leftRoot = ConvertTo-DriveRoot $newLeft }
    if ($newRight) { $rightRoot = ConvertTo-DriveRoot $newRight }
    if ($leftRoot -eq $rightRoot) {
        throw 'Left and right must use different drive letters.'
    }
    Assert-Bootloader -Root $leftRoot -Side 'left after settings reset'
    Assert-Bootloader -Root $rightRoot -Side 'right after settings reset'
}

Write-Uf2 -Image $leftImage -Root $leftRoot -Side 'left'
Write-Uf2 -Image $rightImage -Root $rightRoot -Side 'right'
Write-Host 'Both halves flashed. Re-pair Bluetooth and restore Studio settings if settings reset was used.'
