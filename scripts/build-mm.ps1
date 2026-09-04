# Build the self-contained `mm` engine-core demo (Track A) on Windows.
# Requires: CMake, a C++20 toolchain (Visual Studio 2022 or Ninja+LLVM).
param(
    [string]$Generator = ""   # e.g. "Ninja" or Visual Studio generator string
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root

if (-not $Generator) {
    if (Get-Command ninja -ErrorAction SilentlyContinue) {
        $Generator = "Ninja"
    } else {
        $Generator = "Visual Studio 17 2022"
    }
}

Write-Host "==> Configuring ($Generator)"
cmake -S . -B build -G $Generator -A x64 -DCMAKE_BUILD_TYPE=Release

Write-Host "==> Building"
cmake --build build --config Release

Write-Host "==> Running self-test (mm --core)"
ctest --test-dir build --output-on-failure -C Release

# Visual Studio generator puts binaries in a Release/ subfolder; Ninja in build/.
$Bin = Get-ChildItem -Recurse -Filter mm.exe build | Select-Object -First 1
if ($Bin) {
    Write-Host "==> Engine-core self test"
    & $Bin.FullName --core
    Write-Host "`nDone. Binary: $($Bin.FullName)"
} else {
    Write-Host "mm.exe not found - check the build output."
}