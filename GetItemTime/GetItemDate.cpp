// ----------------------------------------------------------------------------
//
//								GetItemTime
//
// GetItemTime is a Windows command line utility that outputs the date and/or 
// time when a file's contents were created. 
// 
// For pictures, this is Date Taken. For videos, this is Media Created. It might 
// produce useful results for other file types, but this not been verified.
// 
// GetItemTime was designed to assist creating scripts that organize files by 
// their creation date.
// 
// For syntax and usage, run: GetItemTime /?
// 
// ----------------------------------------------------------------------------
// 
// MIT License
//
// Copyright(c) 2025 Daniel B.Konigsbach
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
// ----------------------------------------------------------------------------

#include <iostream>
#include <filesystem>
#include <Objbase.h>
#include <propsys.h>
#include <shobjidl.h>
#include <propvarutil.h>

struct Parameters
{
    std::wstring strFilename;
	struct DateFormat
	{
		std::wstring strDateSeparator = L"/";
		std::wstring strTimeSeparator = L":";
		std::wstring strDateTimeSeparator = L" ";
		bool bDateOnly = false;
		bool bTimeOnly = false;
	} dateFormat;

} params;

const auto strItemDateName = L"System.ItemDate";
const FILETIME ftNull = { 0 };
bool operator==(const FILETIME& ft1, const FILETIME& ft2)
{
	return ft1.dwHighDateTime == ft2.dwHighDateTime && ft1.dwLowDateTime == ft2.dwLowDateTime;
}

void ShowUsage()
{
	std::wcerr << L"GetItemTime [-dateonly] [-timeonly] [-iso] [-datesep <sep>] [-timesep <sep>] [-dtsep <sep>] <filename>" << std::endl;
	std::wcerr << L"  -dateonly, -do : Only output the date part" << std::endl;
	std::wcerr << L"  -timeonly, -to : Only output the time part" << std::endl;
	std::wcerr << L"  -iso           : Use ISO 8601 date and time separators" << std::endl;
	std::wcerr << L"                   It is an abbreviation for: -datesep \"-\" -dtsep \"T\"  -timesep \":\"" << std::endl;
	std::wcerr << L"                   which yields: YYYY-MM-DDThh:mm:ss" << std::endl;
	std::wcerr << L"  -datesep, -ds  : Specify the date separator (Default: \"-\")" << std::endl;
	std::wcerr << L"  -dtsep, -dts   : Specify the date-time separator (Default: \" \")" << std::endl;
	std::wcerr << L"  -timesep, -ts  : Specify the time separator (Default: \":\")" << std::endl;
	std::wcerr << L"  <filename>     : The file to get the date from" << std::endl;
	std::wcerr << L"                   Absolute paths, relative paths, and environment variable replacement are all supported." << std::endl;
	std::wcerr << L"                   Only a single filename is supported. Trying to specify more than one filename results in an error." << std::endl;
	std::wcerr << std::endl;
	std::wcerr << L"GetItemTime outputs the date and/or time when a file's contents were created." << std::endl;
	std::wcerr << L"For pictures, this is Date Taken. For videos, this is Media Created." << std::endl;
	std::wcerr << L"It might produce useful results for other file types, but this not been verified." << std::endl;
	std::wcerr << std::endl;
	std::wcerr << L"GetItemTime was designed to assist creating scripts that organize files by their creation date." << std::endl;
	std::wcerr << std::endl;
	std::wcerr << L"The default is to provide both date and time. You can request to only get the date (-dateonly or -do)" << std::endl;
	std::wcerr << L"or to only get the time (-timeonly or -to). (Specify both and you get nothing.)" << std::endl;
	std::wcerr << std::endl;
	std::wcerr << L"The separators between the date components defaults to \"-\", but you can change that to anything that you want," << std::endl;
	std::wcerr << L"including no separator at all." << std::endl;
	std::wcerr << L"The separators between the date and the time defaults to a space, but is similarly configurable." << std::endl;
	std::wcerr << L"The separators between the time components defaults to \":\", but is also similarly configurable." << std::endl;
	std::wcerr << std::endl;
	std::wcerr << L"The order of the components is not configurable, it is always: year month day hour minute seconds." << std::endl;
	std::wcerr << L"Year is four digits, the other components are two digits, zero padded as needed." << std::endl;
	std::wcerr << std::endl;
	std::wcerr << L"GetItemTime writes the results to the standard console application output (stdout)." << std::endl;
	std::wcerr << L"Any error messages are written to the error stream (stderr), so that they are easily separated from the output." << std::endl;
	std::wcerr << std::endl;
	std::wcerr << L"If the file does not have creation date information or is not found or there is any other error, GetItemTime does" << std::endl;
	std::wcerr << L"not write anything to standard output (stdout). If the output is empty, you got nothing. If it's not empty," << std::endl;
	std::wcerr << L"you got the date and/or time." << std::endl;
	std::wcerr << std::endl;
	std::wcerr << L"GetItemTime also provides a return code to show success or failure (although just checking for empty output is easier)." << std::endl;
	std::wcerr << L"Return codes are 0 for success, > 0 for error, < 0 for having no results and also no error (ie. this help message)." << std::endl;
}

// Return values:
// > 0	: Success
// == 0 : Error encountered, do not process further
// < 0	: Non-error termination, do not process further
int ParseArgs(Parameters& params, int argc, wchar_t* argv[])
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' || argv[i][0] == '/')
		{
			if (0 == _wcsicmp(&argv[i][1], L"dateonly") ||
				0 == _wcsicmp(&argv[i][1], L"do"))
			{
				params.dateFormat.bDateOnly = true;
			}
			else if (0 == _wcsicmp(&argv[i][1], L"timeonly") ||
				     0 == _wcsicmp(&argv[i][1], L"to"))
			{
				params.dateFormat.bTimeOnly = true;
			}
			else if (0 == _wcsicmp(&argv[i][1], L"iso"))
			{
				params.dateFormat.strDateSeparator = L"-";
				params.dateFormat.strDateTimeSeparator = L"T";
				params.dateFormat.strTimeSeparator = L":";
			}
			else if (0 == _wcsicmp(&argv[i][1], L"h") ||
				     0 == _wcsicmp(&argv[i][1], L"help") ||
				     0 == _wcsicmp(&argv[i][1], L"?"))
			{
				ShowUsage();
				return -1;
			}
			else if (0 == _wcsicmp(&argv[i][1], L"datesep") ||
				     0 == _wcsicmp(&argv[i][1], L"ds"))
			{
				++i;
				if (i >= argc)
				{
					std::wcerr << L"Missing argument for " << argv[i - 1] << std::endl;
					return 0;
				}
				params.dateFormat.strDateSeparator = argv[i];
			}
			else if (0 == _wcsicmp(&argv[i][1], L"timesep") ||
				     0 == _wcsicmp(&argv[i][1], L"ts"))
			{
				++i;
				if (i >= argc)
				{
					std::wcerr << L"Missing argument for " << argv[i - 1] << std::endl;
					return 0;
				}
				params.dateFormat.strTimeSeparator = argv[i];
			}
			else if (0 == _wcsicmp(&argv[i][1], L"dtsep") ||
				     0 == _wcsicmp(&argv[i][1], L"dts"))
			{
				++i;
				if (i >= argc)
				{
					std::wcerr << L"Missing argument for " << argv[i - 1] << std::endl;
					return 0;
				}
				params.dateFormat.strDateTimeSeparator = argv[i];
			}
			else
			{
				std::wcerr << L"Unknown option ignored: " << argv[i] << std::endl;
			}
		} 
		else
		{
			if (params.strFilename.empty())
			{
				std::wstring strExpandedFilename(MAX_PATH + 2, L'\0');
				if (ExpandEnvironmentStrings(argv[i], &strExpandedFilename[0], MAX_PATH + 1) == 0)
				{
					std::wcerr << "Failed to expand environment strings in \"" << argv[i] << "\"." << std::endl;
					return 0;
				}
				params.strFilename = strExpandedFilename;
			}
			else
			{
				std::wcerr << "Only one filename allowed. In parameters, found \"" << params.strFilename << "\" and also \"" << argv[i] << "\"." << std::endl;
				return 0;
			}
		}
	}
	return 1;
}

FILETIME GetItemDate(std::wstring strFilename)
{
	FILETIME ftOut = ftNull;
	PROPERTYKEY key;
	HRESULT hr = PSGetPropertyKeyFromName(strItemDateName, &key);
	if (SUCCEEDED(hr))
	{
		// Note that as long as you have the property store, you are keeping the file open
		// So always release it once you are done.
		IPropertyStore* pps = NULL;
		HRESULT hr = SHGetPropertyStoreFromParsingName(strFilename.c_str(), NULL, GPS_DEFAULT, IID_PPV_ARGS(&pps));
		if (SUCCEEDED(hr))
		{
			PROPVARIANT propvarValue;
			PropVariantInit(&propvarValue);
			HRESULT hr = pps->GetValue(key, &propvarValue);
			if (SUCCEEDED(hr))
			{
				FILETIME ft;
				hr = PropVariantToFileTime(propvarValue, PSTF_UTC, &ft);
				if (SUCCEEDED(hr))
				{
					ftOut = ft;
				}
				PropVariantClear(&propvarValue);
			}
			pps->Release();
		}
	}
	return ftOut;
}

std::wstring FormatDate(const FILETIME& ft, const Parameters::DateFormat& dateFormat)
{
	std::wostringstream oss;

	if (ft != ftNull)
	{
		// Note: FileTimeToLocalFileTime is not used because it does not handle daylight saving time.
		SYSTEMTIME st, tzspecific_st;
		FileTimeToSystemTime(&ft, &st);
		SystemTimeToTzSpecificLocalTime(NULL, &st, &tzspecific_st);


		oss << std::setfill(L'0');
		if (!dateFormat.bTimeOnly)
		{
			oss << std::setw(4) << tzspecific_st.wYear << dateFormat.strDateSeparator 
				<< std::setw(2) << tzspecific_st.wMonth << dateFormat.strDateSeparator 
				<< std::setw(2) << tzspecific_st.wDay;
		}

		if (!dateFormat.bDateOnly && !dateFormat.bTimeOnly)
		{
			oss << dateFormat.strDateTimeSeparator;
		}

		if (!dateFormat.bDateOnly)
		{
			oss << std::setw(2) << tzspecific_st.wHour << dateFormat.strTimeSeparator 
				<< std::setw(2) << tzspecific_st.wMinute << dateFormat.strTimeSeparator 
				<< std::setw(2) << tzspecific_st.wSecond;
		}
	}
	return oss.str();
}

int wmain(int argc, wchar_t* argv[])
{
	int retcode = 0;

	if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
	{
		switch (ParseArgs(params, argc, argv))
		{
		case 0:	
			{
				retcode = 1;
				break;
			}
		case 1:
			{
				// Expand and normalize the filename
				auto pathFilename = std::filesystem::absolute(params.strFilename).wstring();

				auto itemft = GetItemDate(params.strFilename);
				if (itemft == ftNull)
				{
					std::wcerr << L"Failed to get item date for \"" << pathFilename << "\"." << std::endl;
					retcode = 3;
				}
				else
				{
					std::wcout << FormatDate(itemft, params.dateFormat) << std::endl;
				}
				break;
			}
		default:
			{
				retcode = -1;
				break;
			}
		}

		CoUninitialize();
	}
	else
	{
		std::wcerr << L"Failed to initialize COM." << std::endl;
		retcode = 2;
	}

	return retcode;
}
