//
//  DateTimeUtils.cpp
//  MiniScript
//
//  Created by Joe Strout on 2/1/23.
//  Copyright © 2023 Joe Strout. All rights reserved.
//

#include "DateTimeUtils.h"
#include "SplitJoin.h"
#include <time.h>
#include <math.h>

namespace  MiniScript {

static bool Match(const String s, size_t *posB, const String match) {
	size_t matchLenB = match.LengthB();
	if (s.SubstringB(*posB, matchLenB) == match) {
		*posB += matchLenB;
		return true;
	}
	return false;
}

String FormatDate(time_t t, const String formatSpec) {
	tm dateTime;
	localtime_r(&t, &dateTime);
	
	const int BUFSIZE = 128;
	char buffer[BUFSIZE];
	setlocale(LC_ALL, "");
	
	StringList result;
	size_t posB = 0, lenB = formatSpec.LengthB();
	while (posB < lenB) {
		if (Match(formatSpec, &posB, "yyyy")) {			// year
			result.Add(String::Format(dateTime.tm_year + 1900, "%04d"));
		} else if (Match(formatSpec, &posB, "yyy")) {
			result.Add(String::Format(dateTime.tm_year + 1900, "%03d"));
		} else if (Match(formatSpec, &posB, "yy")) {
			result.Add(String::Format(dateTime.tm_year % 100, "%02d"));
		} else if (Match(formatSpec, &posB, "MMMM")) {	// month name
			strftime(buffer, BUFSIZE, "%B", &dateTime);
			result.Add(buffer);
		} else if (Match(formatSpec, &posB, "MMM")) {	// month abbreviation
			strftime(buffer, BUFSIZE, "%b", &dateTime);
			result.Add(buffer);
		} else if (Match(formatSpec, &posB, "MM")) {	// month number
			result.Add(String::Format(dateTime.tm_mon + 1, "%02d"));
		} else if (Match(formatSpec, &posB, "M")) {
			result.Add(String::Format(dateTime.tm_mon + 1, "%d"));
		} else if (Match(formatSpec, &posB, "dddd")) {	// weekday name
			strftime(buffer, BUFSIZE, "%A", &dateTime);
			result.Add(buffer);
		} else if (Match(formatSpec, &posB, "ddd")) {	// weekday abbreviation
			strftime(buffer, BUFSIZE, "%a", &dateTime);
			result.Add(buffer);
		} else if (Match(formatSpec, &posB, "dd")) {	// day (number)
			result.Add(String::Format(dateTime.tm_mday, "%02d"));
		} else if (Match(formatSpec, &posB, "d")) {
			result.Add(String::Format(dateTime.tm_mday, "%d"));
		} else if (Match(formatSpec, &posB, "hh")) {	// 12-hour hour
			int twelveHourHour = (dateTime.tm_hour == 0 ? 12 : (dateTime.tm_hour - 1) % 12+1);
			result.Add(String::Format(twelveHourHour, "%02d"));
		} else if (Match(formatSpec, &posB, "h")) {
			int twelveHourHour = (dateTime.tm_hour == 0 ? 12 : (dateTime.tm_hour - 1) % 12+1);
			result.Add(String::Format(twelveHourHour, "%d"));
		} else if (Match(formatSpec, &posB, "HH")) {	// 24-hour hour
			result.Add(String::Format(dateTime.tm_hour, "%02d"));
		} else if (Match(formatSpec, &posB, "H")) {
			result.Add(String::Format(dateTime.tm_hour, "%d"));
		} else if (Match(formatSpec, &posB, "mm")) {	// minute
			result.Add(String::Format(dateTime.tm_min, "%02d"));
		} else if (Match(formatSpec, &posB, "m")) {
			result.Add(String::Format(dateTime.tm_min, "%d"));
		} else if (Match(formatSpec, &posB, "ss")) {	// second
			result.Add(String::Format(dateTime.tm_sec, "%02d"));
		} else if (Match(formatSpec, &posB, "s")) {
			result.Add(String::Format(dateTime.tm_sec, "%d"));
		} else if (Match(formatSpec, &posB, "ffffff")) {	// fractional part of seconds value
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.6f", d);
			result.Add(&buffer[2]);	// (skipping past "0.")
		} else if (Match(formatSpec, &posB, "fffff")) {
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.5f", d);
			result.Add(&buffer[2]);	// (skipping past "0.")
		} else if (Match(formatSpec, &posB, "ffff")) {
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.4f", d);
			result.Add(&buffer[2]);	// (skipping past "0.")
		} else if (Match(formatSpec, &posB, "fff")) {
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.3f", d);
			result.Add(&buffer[2]);	// (skipping past "0.")
		} else if (Match(formatSpec, &posB, "ff")) {
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.2f", d);
			result.Add(&buffer[2]);	// (skipping past "0.")
		} else if (Match(formatSpec, &posB, "f")) {
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.1f", d);
			result.Add(&buffer[2]);	// (skipping past "0.")
		} else if (Match(formatSpec, &posB, "FFFFFF")) {	// fractional part of seconds value, if nonzero
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.6f", d);
			String s(&buffer[2]);	// (skipping past "0.")
			if (s != "000000") result.Add(s);
		} else if (Match(formatSpec, &posB, "FFFFF")) {
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.5f", d);
			String s(&buffer[2]);	// (skipping past "0.")
			if (s != "00000") result.Add(s);
		} else if (Match(formatSpec, &posB, "FFFF")) {
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.4f", d);
			String s(&buffer[2]);	// (skipping past "0.")
			if (s != "0000") result.Add(s);
		} else if (Match(formatSpec, &posB, "FFF")) {
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.3f", d);
			String s(&buffer[2]);	// (skipping past "0.")
			if (s != "000") result.Add(s);
		} else if (Match(formatSpec, &posB, "FF")) {
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.2f", d);
			String s(&buffer[2]);	// (skipping past "0.")
			if (s != "00") result.Add(s);
		} else if (Match(formatSpec, &posB, "F")) {
			double d, i;
			d = modf(t, &i);
			snprintf(buffer, BUFSIZE, "%.1f", d);
			String s(&buffer[2]);	// (skipping past "0.")
			if (s != "0") result.Add(s);
		} else if (Match(formatSpec, &posB, "tt")) {	// AM/PM
			strftime(buffer, BUFSIZE, "%p", &dateTime);
			result.Add(buffer);
		} else if (Match(formatSpec, &posB, "t")) {
			strftime(buffer, BUFSIZE, "%p", &dateTime);
			buffer[1] = 0;	// (truncate to length 1)
			result.Add(buffer);
		} else if (Match(formatSpec, &posB, "gg") || Match(formatSpec, &posB, "g")) {
			if (dateTime.tm_year + 1900 > 0) result.Add("A.D.");
			else result.Add("B.C.");
		} else if (Match(formatSpec, &posB, ":")) {
			// Ugh.  Getting these in C is hard.  For now we're going to punt.
			result.Add(":");
		} else if (Match(formatSpec, &posB, "/")) {
			// See comment above.
			result.Add("/");
		} else if (formatSpec[posB] == '"') {
			// Find the closing quote, and output the contained string literal
			size_t endPosB = posB + 1;
			while (endPosB < lenB) {
				char c = formatSpec[endPosB];
				if (c == '\\') endPosB++;
				else if (c == '"') break;
			}
			result.Add(formatSpec.Substring(posB + 1, endPosB - endPosB - 1));
			posB = endPosB + 1;
		} else if (Match(formatSpec, &posB, "\\") && posB < lenB) {
			result.Add(formatSpec[posB++]);
		} else {
			result.Add(formatSpec.SubstringB(posB, 1));
			posB++;
		}
	}
	
	return Join("", result);
}


}
