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

function RunTest {
    param (
        [string] $TestName = '',
        [string] $TestParams = '',
        [int] $ExpectedErrorLevel = 0,
        [string] $ExpectedReturn = ''
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

RunTest -TestName 'Default output' -TestParams "$PSScriptRoot\Image1.jpg" -ExpectedErrorLevel 0 -ExpectedReturn '2017/07/23 17:31:50'
RunTest -TestName 'Date only' -TestParams "$PSScriptRoot\Image1.jpg -do" -ExpectedErrorLevel 0 -ExpectedReturn '2017/07/23'
RunTest -TestName 'Time only' -TestParams "$PSScriptRoot\Image1.jpg -to" -ExpectedErrorLevel 0 -ExpectedReturn '17:31:50'
RunTest -TestName 'Date separator' -TestParams "$PSScriptRoot\Image1.jpg -ds :" -ExpectedErrorLevel 0 -ExpectedReturn '2017:07:23 17:31:50'
RunTest -TestName 'ISO format' -TestParams "$PSScriptRoot\Image1.jpg -iso" -ExpectedErrorLevel 0 -ExpectedReturn '2017-07-23T17:31:50'
RunTest -TestName 'Param failure, two file names' -TestParams "$PSScriptRoot\Image1.jpg $PSScriptRoot\Image2.jpg" -ExpectedErrorLevel 1 -ExpectedReturn ''
RunTest -TestName 'Exercise test failure logic.  Expect FAILED.' -TestParams "$PSScriptRoot\Image1.jpg $PSScriptRoot\Image2.jpg" -ExpectedErrorLevel 0 -ExpectedReturn ''
