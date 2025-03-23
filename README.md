# GetItemTime
GetItemTime outputs the date and/or time when a file's contents were created
For pictures, this is Date Taken. For videos, this is Media Created
It might produce useful results for other file types, but this not been verified
	
GetItemTime was designed to assist creating scripts that organize files by their creation date
	
## Syntax
```
GetItemTime [-dateonly] [-timeonly] [-iso] [-datesep <sep>] [-timesep <sep>] [-dtsep <sep>] <filename>
	-dateonly, -do : Only output the date part
	-timeonly, -to : Only output the time part
	-iso           : Use ISO 8601 date and time separators
	                It is an abbreviation for: -datesep "-" -dtsep "T"  -timesep ":"
	                which yields: YYYY-MM-DDThh:mm:ss
	-datesep, -ds  : Specify the date separator (Default: "-")
	-dtsep, -dts   : Specify the date-time separator (Default: " ")
	-timesep, -ts  : Specify the time separator (Default: ":")
	<filename>     : The file to get the date from
	                Absolute paths, relative paths, and environment variable replacement are all supported
	                Only a single filename is supported. Trying to specify more than one filename results in an error
```	

GetItemTIme recognizes either forward slash or hyphen to indicate a parameter. So, `-timesep` and `/timesep` are equally good and mean the same thing.

GetItemTime supports environment varable replacement in the filename.

GetItemTime operates on a single file per invocation. Trying to specify more than one file produces an error message.

## Output to Standard Output (STDOUT)
GetItemTime writes the results to the standard console application output (stdout).

The default is to provide both date and time. You can request to only get the date (-dateonly or -do) or to only get the time (-timeonly or -to). (Specify both and you get nothing.)
	
The separators between the date components defaults to "-", but you can change that to anything that you want, including no separator at all
The separators between the date and the time defaults to a space, but is similarly configurable.
The separators between the time components defaults to ":", but is also similarly configurable.
	
The order of the components is not configurable, it is always: year month day hour minute seconds

Year is four digits, the other components are two digits, zero padded as needed
	
If the file does not have creation date information or is not found or there is any other error, GetItemTime does
not write anything to standard output (stdout).
## Output to Standard Error (STDERR)
Any error messages are written to the error stream (stderr), so that they are easily separated from the output. Errors would be parameter errors or if the specified file is not found.

Note that GetItemTime does NOT consider it an error if the file is found but does not contain Item Time information. In that case, nothing is written to either STDOUT or STDERR. 
## Return code (a/k/a %ERRORLEVEL%,  $LASTEXITCODE, ...)	
GetItemTime also provides a return code to show success or failure.
Return codes are 0 for success, > 0 for error, < 0 for having no results and also no error (ie. the help message).

## Examples
These examples use a fictitious image file `IMAGE.JPG` containing a picture that was taken on 6 January 2021 at 9:07:05 AM. (Month, day, hour, minute, and seconds were chosen to demonstrate zero padding for numbers < 10.)

### Default output

The default output format from GetItemTime is:  `YYYY-MM-DD hh:mm:ss`
<table>
	<tr>
		<td>Command Line</td>
		<td>
			<code>GetItemTime IMAGE.JPG</code>
		</td>
	<tr>
		<td>STDOUT</td>
		<td><samp>2021-01-06 09:07:05</samp></td>
	</tr>
	<tr>
		<td>STDERR</td>
		<td></td>
	</tr>
	<tr>
		<td>Return code</td>
		<td>0</td>
	</tr>
</table>

### Date only

The parameter to output only the date is either `/dateonly` or `/do` (or `-dateonly` or `-do`).
<table>
	<tr>
		<td>Command Line</td>
		<td>
			<code>GetItemTime IMAGE.JPG /do</code>
		</td>
	<tr>
		<td>STDOUT</td>
		<td><samp>2021-01-06</samp></td>
	</tr>
	<tr>
		<td>STDERR</td>
		<td></td>
	</tr>
	<tr>
		<td>Return code</td>
		<td>0</td>
	</tr>
</table>

### Time only

The parameter to output only the date is either `/timeonly` or `/to` (or `-timeonly` or `-to`).
<table>
	<tr>
		<td>Command Line</td>
		<td>
			<code>GetItemTime IMAGE.JPG -to</code>
		</td>
	<tr>
		<td>STDOUT</td>
		<td><samp>09:07:05</samp></td>
	</tr>
	<tr>
		<td>STDERR</td>
		<td></td>
	</tr>
	<tr>
		<td>Return code</td>
		<td>0</td>
	</tr>
</table>

### No separators at all

This is how you would get date and time into a single string with no separators (`YYYYMMDDhhmmss`).
<table>
	<tr>
		<td>Command Line</td>
		<td>
			<code>GetItemTime IMAGE.JPG /ds "" /dts "" /ts ""</code>
		</td>
	<tr>
		<td>STDOUT</td>
		<td><samp>20210106090705</samp></td>
	</tr>
	<tr>
		<td>STDERR</td>
		<td></td>
	</tr>
	<tr>
		<td>Return code</td>
		<td>0</td>
	</tr>
</table>

### Separate each field with a colon

This will give you a colon between each field (`YYYY:MM:DD:hh:mm:ss`). Note that time already uses colon, so we didn't need to specify `/timesep ":"` (although we could have if we wanted to).
<table>
	<tr>
		<td>Command Line</td>
		<td>
			<code>GetItemTime IMAGE.JPG /ds ":" /dts ":"</code>
		</td>
	<tr>
		<td>STDOUT</td>
		<td><samp>2021:01:06:09:07:05</samp></td>
	</tr>
	<tr>
		<td>STDERR</td>
		<td></td>
	</tr>
	<tr>
		<td>Return code</td>
		<td>0</td>
	</tr>
</table>

### ISO 8601 format

Return the date and time in ISO 8601 format (`YYYY-MM-DDThh:mm:ss`). This is the same as specifying `/datesep "-" /dtsep "T" /timesep ":"`.

Note that GetItemTime does not support time zone offset or fractions of a second.
<table>
	<tr>
		<td>Command Line</td>
		<td>
			<code>GetItemTime IMAGE.JPG /iso</code>
		</td>
	<tr>
		<td>STDOUT</td>
		<td><samp>2021-01-06T09:07:05</samp></td>
	</tr>
	<tr>
		<td>STDERR</td>
		<td></td>
	</tr>
	<tr>
		<td>Return code</td>
		<td>0</td>
	</tr>
</table>

### Missing parameter error

The parameters `/datesep`, `/dtsep`, and `/timesep` must be followed by another parameter giving the separator value. If not, then GetItemTime reports this as an error.

Note that this parameter is processed by the quoting rules are used by whatever invokes GetItemTime.  So, if this is coming from the command prompt and your separator value is an empty string or contains a space, then you will need quotation marks around the value.
<table>
	<tr>
		<td>Command Line</td>
		<td>
			<code>GetItemTime IMAGE.JPG /ds </code>
		</td>
	<tr>
		<td>STDOUT</td>
		<td></td>
	</tr>
	<tr>
		<td>STDERR</td>
		<td><samp>Missing argument for /ds</same></td>
	</tr>
	<tr>
		<td>Return code</td>
		<td>1</td>
	</tr>
</table>
