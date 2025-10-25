# Author: HarutoHiroki
# Date: 2025-09-11
# Version: 1.0.0
# Frigg you Windows and your stupid ass updates
# This script downloads my set of applications for Windows
# because windows gets shitty every update and breaks everything and I have to reinstall everything

# Define the directory to save installers
$downloadDir = "$PSScriptRoot\Installers"
if (-Not (Test-Path -Path $downloadDir)) {
    New-Item -ItemType Directory -Force -Path $downloadDir | Out-Null
}
Set-Location -Path $downloadDir

# Tell the user its doing the thing
Write-Host "Downloading installers to $downloadDir..."

# Helper function for GitHub latest release downloads
function Get-GitHubLatest {
  param (
    [string]$repo,
    [string]$pattern
  )
  $release = Invoke-RestMethod -Uri "https://api.github.com/repos/$repo/releases/latest"
  $asset = $release.assets | Where-Object { $_.name -like $pattern } | Select-Object -First 1
  if ($asset) {
    Write-Host "Downloading $($asset.name) from $repo"
    Invoke-WebRequest -Uri $asset.browser_download_url -OutFile $asset.name
  }
} else {
  Write-Host "No matching asset found in $repo"
}

# --- 7-Zip ---
Get-GitHubLatest -repo "ip7z/7zip" -pattern "*x64.exe"

# --- Git ---
Get-GitHubLatest -repo "git-for-windows/git" -pattern "*64-bit.exe"

# --- GitKraken ---
Write-Host "Downloading GitKraken"
Invoke-WebRequest -Uri "https://release.gitkraken.com/windows/GitKrakenSetup.exe" -OutFile "GitKrakenSetup.exe"

# --- Termius ---
Write-Host "Downloading Termius"
Invoke-WebRequest -Uri "https://termi.us/win" -OutFile "TermiusSetup.exe"

# --- Google Cloud SDK ---
Write-Host "Downloading Google Cloud SDK"
Invoke-WebRequest -Uri "https://dl.google.com/dl/cloudsdk/channels/rapid/GoogleCloudSDKInstaller.exe" -OutFile "GoogleCloudSDKInstaller.exe"

# --- OpenVPN ---
Write-Host "Downloading OpenVPN"
Invoke-WebRequest -Uri "https://swupdate.openvpn.org/community/releases/openvpn-install-latest-stable.exe" -OutFile "OpenVPNSetup.exe"

# --- Steam ---
Write-Host "Downloading Steam"
# Invoke-WebRequest -Uri "https://steamcdn-a.akamaihd.net/client/installer/SteamSetup.exe" -OutFile "SteamSetup.exe"
Invoke-WebRequest -Uri "https://cdn.fastly.steamstatic.com/client/installer/SteamSetup.exe" -OutFile "SteamSetup.exe"

# --- Discord ---
Write-Host "Downloading Discord"
Invoke-WebRequest -Uri "https://discord.com/api/download?platform=win" -OutFile "DiscordSetup.exe"

# --- Spotify ---
Write-Host "Downloading Spotify"
Invoke-WebRequest -Uri "https://download.scdn.co/SpotifySetup.exe" -OutFile "SpotifySetup.exe"

# --- VSCode Insiders ---
Write-Host "Downloading VSCode Insiders"
Invoke-WebRequest -Uri "https://update.code.visualstudio.com/latest/win32-x64/insider" -OutFile "VSCodeInsidersSetup.exe"

# --- BalenaEtcher ---
Get-GitHubLatest -repo "balena-io/etcher" -pattern "*Setup.exe"

# --- ShareX ---
Get-GitHubLatest -repo "ShareX/ShareX" -pattern "*setup.exe"

# --- OBS Studio ---
Get-GitHubLatest -repo "obsproject/obs-studio" -pattern "*Installer-x64.exe"

# -- Rufus ---
Get-GitHubLatest -repo "pbatard/rufus" -pattern "*p.exe"

# --- Yubikey Manager ---
Write-Host "Downloading Yubikey Manager"
Invoke-WebRequest -Uri "https://developers.yubico.com/yubikey-manager-qt/Releases/yubikey-manager-qt-latest-win64.exe" -OutFile "YubikeyManagerSetup.exe"

# --- Yubikey Minidriver ---
Write-Host "Downloading Yubikey Minidriver"
Invoke-WebRequest -Uri "https://downloads.yubico.com/support/YubiKey-Minidriver-latest-x64.msi" -OutFile "YubiKeyMinidriver.msi"

# --- KiCAD ---
Get-GitHubLatest -repo "KiCad/kicad-source-mirror" -pattern "*x86_64.exe"

# --- Jellyfin Server ---
Write-Host "Downloading Jellyfin Server"
Invoke-WebRequest -Uri "https://repo.jellyfin.org/?path=/server/windows/latest-stable/amd64/jellyfin-windows-x64.exe" -OutFile "JellyfinServerSetup.exe"about:blank#blocked