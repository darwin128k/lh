# A mutation that no test catches is not a bad test, it is an instrument that is not
# measuring. Each line below puts one of the bugs back, exactly as it was, and the run
# has to go red.
#
# The first version of this script reported six misses out of six and the tree was
# left holding one of the mutations, which is the worst of both: a report that was
# wrong and a source tree that was wrong. So this one checks itself before it
# measures anything:
#
#   1. the file has to **change** when the mutation is applied (the text has to be
#      there, matched literally);
#   2. the test binary has to be **newer than the mutation** (a build that quietly did
#      nothing would otherwise report "no test caught this" for every mutation);
#   3. the file has to be **byte-identical to its backup** after the restore, and the
#      binary has to be rebuilt again before the next round starts.
#
#  usage: powershell -NoProfile -ExecutionPolicy Bypass -File mutation-check.ps1

$ErrorActionPreference = 'Continue'

$src  = 'H:\Projects\paladin\lib\lh\src\lh\net\modbus\link.c'
$core = 'H:\Projects\paladin\lib\lh\src\lh\net\modbus.c'
$hdr  = 'H:\Projects\paladin\lib\lh\include\lh\net\modbus.h'
$hhdr = 'H:\Projects\paladin\lib\lh\include\lh\net\modbus\link.h'
$build = 'H:\Projects\paladin\lib\lh\build'
$exe  = Join-Path $build 'test\lh_test_net.exe'

function Build-Net([string]$Why) {
    Push-Location $build
    & C:\msys64\mingw64\bin\cmake.exe --build . 2>&1 | Out-Null
    $rc = $LASTEXITCODE
    Pop-Location
    if ($rc -ne 0) { throw "build failed ($Why)" }
}

function Run-Net {
    # The exe's output goes through a file, not through the PowerShell pipeline.
    # gtest prints its summary on **stderr**, and with `$ErrorActionPreference`
    # left alone a native command's stderr does not reliably come back through
    # `2>&1 |` -- it arrives as ErrorRecords that the pipeline drops. The first
    # version of this script read the failure count out of an empty string,
    # turned that into zero, and reported all seven mutations as missed while the
    # tests were in fact catching them. An instrument that cannot see the number it
    # exists to read reports "nothing happened" for everything.
    # No --gtest_brief: brief mode prints the *names* of the failed tests but not the
    # count line, and this script's whole job is to read that count. Looking for a line
    # the tool does not print in the mode you asked for is how a run of seven real
    # failures gets reported as seven clean passes.
    $log = Join-Path $env:TEMP 'lh-mutation-run.txt'
    $cmd = '"' + $exe + '" > "' + $log + '" 2>&1'
    & cmd.exe /c $cmd
    $out = Get-Content $log -Raw -Encoding UTF8
    return @{ Failed = [int]([regex]::Match($out, '\[  FAILED  \] (\d+) test').Groups[1].Value)
              Total  = [int]([regex]::Match($out, '\[==========\] (\d+) tests').Groups[1].Value)
              Which  = (([regex]::Matches($out, '\[  FAILED  \] (lh_mb_link\.[^\r\n]+)') |
                         ForEach-Object { $_.Groups[1].Value }) -join ', ')
              Text   = $out }
}

$mutations = @(
    @{ Name = 'frame_length counts the unit byte twice'
       File = $src;   Old = 'return (lh_u16_t)(LH_MB_MBAP_HEAD + length - 1);'
                      New = 'return (lh_u16_t)(LH_MB_MBAP_HEAD + length);' },

    @{ Name = 'decode drops the last byte of every answer'
       File = $src;   Old = '*pdu_length = (lh_u16_t)(length - LH_MB_MBAP_HEAD);'
                      New = '*pdu_length = (lh_u16_t)(length - LH_MB_MBAP_HEAD - 1);' },

    @{ Name = 'encode writes a byte more than it filled in'
       File = $src;   Old = 'return (lh_u16_t)(LH_MB_MBAP_HEAD + pdu_length);'
                      New = 'return (lh_u16_t)(LH_MB_MBAP_HEAD + 1 + pdu_length);' },

    @{ Name = 'the frame buffer is 256, too small for a 125-register answer'
       File = $hdr;   Old = '#define LH_MB_ADU_MAX ((lh_u16_t)260)'
                      New = '#define LH_MB_ADU_MAX ((lh_u16_t)256)' },

    @{ Name = 'the request is built with a constant instead of the code asked for'
       File = $src;   Old = 'sizeof(pdu), self->fc, address, count);'
                      New = 'sizeof(pdu), LH_MB_FC_READ_HOLDING, address, count);' },

    @{ Name = 'the timestamp of a question in flight is 32 bits'
       File = $hhdr;  Old = 'lh_u64_t since_us;'
                      New = 'lh_u32_t since_us;' },

    @{ Name = 'a timeout leaves the link stuck, unable to be asked again'
       File = $src;   Old = "            self->held = 0;`n            self->state = (lh_u8_t)lh_mb_link_idle;`n            self->reason = lh_mb_status_short;`n            self->counters.failed++;"
                      New = "            self->held = 0;`n            self->state = (lh_u8_t)lh_mb_link_failed;`n            self->reason = lh_mb_status_short;`n            self->counters.failed++;" },

    @{ Name = 'a refusal is counted as a failure, so a live device looks dead'
       File = $src;   Old = "            self->counters.refused++;"
                      New = "            self->counters.failed++;" },

    @{ Name = 'the length of a PDU is checked before the refusal is looked for'
       File = $core;  Old = '    lh_return_if(length < 2, lh_mb_status_short);'
                      New = '    lh_return_if(length < 4, lh_mb_status_short);' }
)

Write-Output '--- the tree as it stands, before any mutation ---'
Build-Net 'baseline'
$base = Run-Net
Write-Output ("{0} / {1} tests pass" -f ($base.Total - $base.Failed), $base.Total)
if ($base.Failed -ne 0) {
    Write-Output "STOP: the suite is not green to begin with, so nothing below means anything:"
    Write-Output $base.Text
    exit 1
}

$results = @()
foreach ($m in $mutations) {
    $backup = $m.File + '.mutbak'
    $before = (Get-FileHash $m.File -Algorithm SHA256).Hash
    Copy-Item $m.File $backup -Force
    $text = [System.IO.File]::ReadAllText($m.File)

    $at = $text.IndexOf($m.Old, [System.StringComparison]::Ordinal)
    if ($at -lt 0) {
        Write-Output ("SKIP    {0} -- the text to mutate is not in the file" -f $m.Name)
        Move-Item $backup $m.File -Force
        (Get-Item $m.File).LastWriteTime = Get-Date
        $results += "SKIP   $($m.Name)"
        continue
    }
    $text = $text.Remove($at, $m.Old.Length).Insert($at, $m.New)
    [System.IO.File]::WriteAllText($m.File, $text, (New-Object System.Text.UTF8Encoding $false))

    $mutated = (Get-FileHash $m.File -Algorithm SHA256).Hash
    if ($mutated -eq $before) {
        Write-Output ("BROKEN  {0} -- the mutation produced no change, so the file did not move" -f $m.Name)
        Move-Item $backup $m.File -Force
        (Get-Item $m.File).LastWriteTime = Get-Date
        Build-Net 'restore'
        $results += "BROKEN $($m.Name)"
        continue
    }

    $stamp = (Get-Item $m.File).LastWriteTime
    Build-Net $m.Name
    $exeStamp = (Get-Item $exe).LastWriteTime
    if ($exeStamp -lt $stamp) {
        Write-Output ("BROKEN  {0} -- the binary is OLDER than the mutation: nothing was rebuilt" -f $m.Name)
        Move-Item $backup $m.File -Force
        (Get-Item $m.File).LastWriteTime = Get-Date
        Build-Net 'restore'
        $results += "BROKEN $($m.Name)"
        continue
    }

    $r = Run-Net
    if ($r.Total -eq 0) {
        Write-Output ("BROKEN  {0} -- the run produced no readable summary, so 'no failures' and" -f $m.Name)
        Write-Output  "        'the output was never read' look identical. The instrument is broken,"
        Write-Output  "        not the tests."
        $results += "BROKEN $($m.Name) (no summary)"
    }
    elseif ($r.Failed -eq 0) {
        Write-Output ("MISSED  {0}  ({1} tests, none red) -- the tests do not see this" -f $m.Name, $r.Total)
        $results += "MISSED $($m.Name)"
    }
    else {
        Write-Output ("CAUGHT  {0}  -> {1} red: {2}" -f $m.Name, $r.Failed, $r.Which)
        $results += "CAUGHT $($m.Name)"
    }

    Move-Item $backup $m.File -Force
    # Move-Item keeps the backup's own timestamp, so a restored file looks *older*
    # than every object file built from it and ninja rebuilds nothing. The result is
    # a test binary compiled against the mutated layout of a struct, reading
    # `counters` out of the middle of the frame buffer: tests fail for reasons that
    # have nothing to do with the code under test. Restore, then make the tree look
    # as though it had just been edited.
    (Get-Item $m.File).LastWriteTime = Get-Date
    $after = (Get-FileHash $m.File -Algorithm SHA256).Hash
    if ($after -ne $before) {
        Write-Output ("BROKEN  {0} -- the restore did not put the file back" -f $m.Name)
        $results += "BROKEN $($m.Name) (restore)"
    }
    Build-Net 'restore'
}

Write-Output ''
Write-Output '--- the tree after every mutation was put back ---'
$clean = Run-Net
Write-Output ("{0} / {1} tests pass" -f ($clean.Total - $clean.Failed), $clean.Total)
if ($clean.Failed -ne 0) { Write-Output $clean.Text }
Write-Output ''
$results | ForEach-Object { Write-Output $_ }

# The tally, with its denominator, and an exit code that means something.
#
# A SKIP is not a catch. It is the instrument admitting it did not look, and the run
# that found this had six CAUGHT and one SKIP on the same page: the reader's eye goes
# to "6/7 caught" and the missing seventh is a line that looks like a result. A
# mutation whose text is no longer in the file is a mutation this script has stopped
# being able to check -- the code moved and the instrument did not -- so it has to be
# the loudest thing in the output and it has to fail the run.
$caught  = @($results | Where-Object { $_ -like 'CAUGHT*' }).Count
$missed  = @($results | Where-Object { $_ -like 'MISSED*' }).Count
$broken  = @($results | Where-Object { $_ -like 'BROKEN*' }).Count
$skipped = @($results | Where-Object { $_ -like 'SKIP*' }).Count
$total   = $mutations.Count
Write-Output ''
Write-Output ("CAUGHT {0} of {1} mutations   (missed {2}, broken {3}, skipped {4})" -f $caught, $total, $missed, $broken, $skipped)
if ($caught -ne $total) {
    Write-Output 'THE INSTRUMENT DID NOT MEASURE ALL OF THEM. A mutation it cannot apply is not a'
    Write-Output 'mutation it caught, and the code has moved on without it.'
    exit 1
}
exit 0