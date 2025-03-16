# GetItemTime

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
	
GetItemTime outputs the date and/or time when a file's contents were created
For pictures, this is Date Taken. For videos, this is Media Created
It might produce useful results for other file types, but this not been verified
	
GetItemTime was designed to assist creating scripts that organize files by their creation date
	
The default is to provide both date and time. You can request to only get the date (-dateonly or -do)
or to only get the time (-timeonly or -to). (Specify both and you get nothing.)
	
The separators between the date components defaults to "-", but you can change that to anything that you want,
including no separator at all
The separators between the date and the time defaults to a space, but is similarly configurable
The separators between the time components defaults to ":", but is also similarly configurable
	
The order of the components is not configurable, it is always: year month day hour minute seconds
Year is four digits, the other components are two digits, zero padded as needed
	
GetItemTime writes the results to the standard console application output (stdout)
Any error messages are written to the error stream (stderr), so that they are easily separated from the output
	
If the file does not have creation date information or is not found or there is any other error, GetItemTime does
not write anything to standard output (stdout). If the output is empty, you got nothing. If it's not empty,
you got the date and/or time
	
GetItemTime also provides a return code to show success or failure (although just checking for empty output is easier)
Return codes are 0 for success, > 0 for error, < 0 for having no results and also no error (ie. this help message)
