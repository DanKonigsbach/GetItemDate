# Running Tests
## Important!
Because you likely downloaded the source code from GitHub, the test script Test.ps1 is "blocked" for your protection. In order to run these tests, the script needs to be "unblocked".
This is a security feature of Windows. You should only unblock this script if you understand PowerShell well enough to examine the script and see what it does. 

This script is neither inherently safe nor unsafe. Used as intended, it is safe. However, it runs whatever program is passed to it. 
Make sure you are clear on how the script is invoked, and that it is actually being passed the GetItemDate.exe program (or whatever
you have renamed it to) that you intend to test.

To unblock this script, right-click on the script file, select Properties, and then click the Unblock button.

## Building the Tests
There is no need to build the tests. They are PowerShell scripts that run the GetItemDate program and check the output.

Building the Test project copies the test into a subdirectory of where you build GetItemDate.exe, and creates a batch file that 
makes it easy to run the tests outside of Visual Studio.

## Running the Tests within Visual Studio
Within Visual Studio, you run the tests by Debugging the Test project. This will run the tests and show you the results.

You may get a warning "This project is out of date: [...] Would you like to build it?" That's okay. Just click the "No" button to continue.

The tests are not integrated with the Visual Studio Test Explorer.

## Running the Tests outside of Visual Studio
If you build the Test project, it will create a Test subdirectory below the directory where GetItemDate.exe is built. 
In that Test subdirectory, it will have created a batch file called RunTests.bat. Change to that directory and run the batch file.

The batch file will run the tests and show you the results.

## Test Results
The test results appear as command line output. Note that one test (or more, if added in the future) is testing the way that the
test script itself handles failures, and is expected to fail.

## Adding Tests
To add tests, you can add them to the Test.ps1 script. The script is designed to be easy to read and modify.