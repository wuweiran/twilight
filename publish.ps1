# Publishes the Twilight installer via Inno Setup.
# Usage:
#   .\publish.ps1                 # Release | x64 (default)
#   .\publish.ps1 -Platform ARM64
#   .\publish.ps1 -Platform Win32

param(
	[ValidateSet('x64', 'ARM64', 'Win32')]
	[string]$Platform = 'x64'
)

$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
	throw "vswhere.exe not found. Is Visual Studio installed?"
}
$vsPath = & $vswhere -latest -prerelease -property installationPath
$msbuild = Join-Path $vsPath 'MSBuild\Current\Bin\MSBuild.exe'
if (-not (Test-Path $msbuild)) {
	throw "MSBuild.exe not found under $vsPath"
}

Write-Host "Publishing Twilight ($Platform) ..." -ForegroundColor Cyan
& $msbuild "$root\twilight.sln" /t:Rebuild /p:Configuration=Release /p:Platform=$Platform /p:Publish=true /v:minimal /nologo
if ($LASTEXITCODE -ne 0) {
	throw "Publish failed (MSBuild exit code $LASTEXITCODE)."
}

$installer = Join-Path $root "dist\twilight_setup_$Platform.exe"
if (Test-Path $installer) {
	Write-Host "Installer created: $installer" -ForegroundColor Green
}
else {
	Write-Warning "Build succeeded but installer not found at $installer."
}
