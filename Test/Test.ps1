# In order to be used, this script needs to be "unblocked". Do this ONLY if you understand PowerShell
# well enough to be confortable you know what this script does.
# 
# You can do this through the file's Properties dialog. Intentionally being vague, so that anyone who
# does this has the knowledge or has done the research to protect themself.
#
param (
    [Parameter(Mandatory,
	           HelpMessage='Enter the path and name of the executable to test (typically <path>\GetItemDate.exe')]
    [string] $testSubject
)

<#
.DESCRIPTION
This script tests the GetItemDate executable. It runs the executable with various parameters and compares the output 
and exit code to expected values. If the output or exit code does not match the expected values, the test is considered
to fail.

.SYNOPSIS
Test suite for GetItemDate.exe

.LINK
../README.md
#>
function RunTest {
    param (
        [string] 
        # A name that describes this test. This is displayed in the output when the test is run.
        $TestName = '',

        [string] 
        # The command line parameters to use when running the executable. This must include
        # the path and filename of the file to use and any formatting parameters being tested.
        # See README.md or run GetItemDate /? to see the parameter descriptions.
        # $TestParams does NOT include the name of the executable itself (it's everything AFTER
        # GetItemDate in the parameter description.)
        $TestParams = '',

        [int]
        # This is what you expect back as the exit code (also known as ErrorLevel) from running
        # the program. If this does not match what RunTest sees, it will report a test failure.
        $ExpectedErrorLevel = 0,

        [string] 
        # This is the expected output (to Standard Output, or STDOUT) from running the program.
        # If this does not match what RunTest sees, it will report a test failure.
        $ExpectedReturn = ''
    )

    $pass = $true
    $messages = [System.Collections.Generic.List[string]]::new()
    $process = $null

    try {
        Write-Host '========================================'
        Write-Host "Test:  $TestName" -NoNewLine

        $StartInfo = [System.Diagnostics.ProcessStartInfo]::new() 
        $StartInfo.FileName = $testSubject
        $StartInfo.Arguments = $TestParams
        $StartInfo.CreateNoWindow = $true
        $StartInfo.UseShellExecute = $false
        $StartInfo.RedirectStandardOutput = $true
        $StartInfo.RedirectStandardError = $true

        $process = [System.Diagnostics.Process]::Start($StartInfo)
        $process.WaitForExit();

        $ExitCode = $Process.ExitCode;
        $Output = $Process.StandardOutput.ReadToEnd().Trim();
        $ErrorOutput = $Process.StandardError.ReadToEnd().Trim();

        if ($ExitCode -ne $ExpectedErrorLevel) {
            $pass = $false
            $messages.Add("Expected Error Level $ExpectedErrorLevel, instead received $ExitCode")
            $messages.Add("    $ErrorOutput")
        }

        if ($Output -ne $ExpectedReturn) {
            $pass = $false
            $messages.Add("Expected output ""$ExpectedReturn"", instead received ""$Output""")
        }

        if ($pass) {
            Write-Host " - Pass"
        } else {
            Write-Host " - FAILED ***"
            foreach ( $message in $messages ) {
                Write-Host "    $message"
            }
        }
    } catch {
        Write-Host $error
    } finally {
        if ($null -ne $process) {
            $process.Dispose()
            $process = $null
        }
    }
}

if (-not (Test-Path -Path $testSubject -PathType Leaf)) {
    throw (New-Object System.IO.FileNotFoundException("Executable to test not found: $testSubject", $testSubject))
} else {
    # Make sure path to test executable is absolute. During testing, it got expanded in a different current
    # directory, presumably because .NET was picking up the editor's working directory rather than the
    # current directory when this script was launched. 
    $testSubject = Convert-Path -Path $testSubject
}

get-help RunTest

# Run the tests
# - Choice of Image1.jpg vs Image2.jpg for each test is arbitrary. Just confirming get different, correct results.
# - Choice of -param format vs /param format is arbitrary. Just confirming both work.
# - Choice of short vs long parameter name is arbitrary. Just confirming both work.
RunTest -TestName 'Default output' -TestParams "$PSScriptRoot\Image1.jpg"       -ExpectedErrorLevel 0 -ExpectedReturn '2017/07/23 17:31:50'
RunTest -TestName 'Date only'      -TestParams "$PSScriptRoot\Image1.jpg -do"   -ExpectedErrorLevel 0 -ExpectedReturn '2017/07/23'
RunTest -TestName 'Time only'      -TestParams "$PSScriptRoot\Image1.jpg /to"   -ExpectedErrorLevel 0 -ExpectedReturn '17:31:50'
RunTest -TestName 'Date separator' -TestParams "$PSScriptRoot\Image1.jpg -ds :" -ExpectedErrorLevel 0 -ExpectedReturn '2017:07:23 17:31:50'
RunTest -TestName 'ISO format'     -TestParams "$PSScriptRoot\Image1.jpg /iso"  -ExpectedErrorLevel 0 -ExpectedReturn '2017-07-23T17:31:50'
RunTest -TestName 'No separators'  -TestParams "$PSScriptRoot\Image2.jpg -ds """" -dts """" -ts """""  -ExpectedErrorLevel 0 -ExpectedReturn '20200810073627'
RunTest -TestName 'No extraneous date-time separator, even when specified (date only)' -TestParams "$PSScriptRoot\Image2.jpg /dateonly -dtsep foo" -ExpectedErrorLevel 0 -ExpectedReturn '2020/08/10'
RunTest -TestName 'No extraneous date-time separator, even when specified (time only)' -TestParams "$PSScriptRoot\Image2.jpg /timeonly /dts bar"   -ExpectedErrorLevel 0 -ExpectedReturn '07:36:27'
RunTest -TestName 'Param failure, two file names'                -TestParams "$PSScriptRoot\Image1.jpg $PSScriptRoot\Image2.jpg" -ExpectedErrorLevel 1 -ExpectedReturn ''
RunTest -TestName 'Exercise test failure logic.  Expect FAILED.' -TestParams "$PSScriptRoot\Image1.jpg $PSScriptRoot\Image2.jpg" -ExpectedErrorLevel 0 -ExpectedReturn ''
