<#
.SYNOPSIS
Configures and builds lh on Windows with CMake.

.EXAMPLE
.\scripts\build-windows.ps1 -Config Release -RunTests

.EXAMPLE
.\scripts\build-windows.ps1 -Generator "MinGW Makefiles" -CCompiler "C:\SysGCC\mingw64\bin\gcc.exe" -Docs OFF -Tests OFF
#>

param(
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$Config = "Release",

    [string]$BuildDir = "build/windows",
    [string]$Generator = "",
    [string]$CCompiler = "",
    [string]$CXXCompiler = "",
    [string]$Target = "",

    [ValidateSet("ON", "OFF")]
    [string]$Shared = "ON",

    [ValidateSet("ON", "OFF")]
    [string]$Docs = "ON",

    [ValidateSet("ON", "OFF")]
    [string]$Tests = "ON",

    [switch]$Clean,
    [switch]$RunTests,
    [int]$Parallel = [Math]::Max(1, [Environment]::ProcessorCount)
)

$ErrorActionPreference = "Stop"

function Invoke-Native {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Command,

        [Parameter(ValueFromRemainingArguments = $true)]
        [string[]]$Arguments
    )

    & $Command @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$Command failed with exit code $LASTEXITCODE."
    }
}

$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$BuildPath = [System.IO.Path]::GetFullPath((Join-Path $RepoRoot $BuildDir))
$RepoRootPath = [System.IO.Path]::GetFullPath($RepoRoot)

if (-not $BuildPath.StartsWith($RepoRootPath, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "BuildDir must be inside the repository: $BuildPath"
}

if ($BuildPath -eq $RepoRootPath) {
    throw "BuildDir cannot be the repository root."
}

if ($Clean -and (Test-Path -LiteralPath $BuildPath)) {
    Write-Host "Removing $BuildPath"
    Remove-Item -LiteralPath $BuildPath -Recurse -Force
}

$ConfigureArgs = @(
    "-S", $RepoRootPath,
    "-B", $BuildPath,
    "-DCMAKE_BUILD_TYPE=$Config",
    "-DLH_BUILD_SHARED=$Shared",
    "-DLH_BUILD_DOCS=$Docs",
    "-DLH_BUILD_TESTS=$Tests"
)

if ($Generator -ne "") {
    $ConfigureArgs += @("-G", $Generator)
}

if ($CCompiler -ne "") {
    $ConfigureArgs += "-DCMAKE_C_COMPILER=$CCompiler"
}

if ($CXXCompiler -ne "") {
    $ConfigureArgs += "-DCMAKE_CXX_COMPILER=$CXXCompiler"
}

Write-Host "Configuring lh ($Config)"
Invoke-Native cmake @ConfigureArgs

$BuildArgs = @("--build", $BuildPath, "--config", $Config, "--parallel", "$Parallel")
if ($Target -ne "") {
    $BuildArgs += @("--target", $Target)
}

Write-Host "Building lh ($Config)"
Invoke-Native cmake @BuildArgs

if ($RunTests) {
    if ($Tests -ne "ON") {
        throw "RunTests requires -Tests ON."
    }

    Write-Host "Running tests"
    Invoke-Native ctest --test-dir $BuildPath -C $Config --output-on-failure
}
