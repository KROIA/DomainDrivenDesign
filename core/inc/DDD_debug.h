#pragma once
#include "DDD_global.h"

/// USER_SECTION_START 14

/// USER_SECTION_END

// The Logger library is automaticly included if the logger dependency .cmake file is available
#if LOGGER_LIBRARY_AVAILABLE == 1
	#include "Logger.h"
#endif

/// USER_SECTION_START 1

/// USER_SECTION_END

// Debugging
#ifdef NDEBUG
	#define DDD_CONSOLE(msg)
	#define DDD_CONSOLE_FUNCTION(msg)
#else
	#include <iostream>

	#define DDD_DEBUG
	#define DDD_CONSOLE_STREAM std::cout

	#define DDD_CONSOLE(msg) DDD_CONSOLE_STREAM << msg;
	#define DDD_CONSOLE_FUNCTION(msg) DDD_CONSOLE_STREAM << __PRETTY_FUNCTION__ << " " << msg;
#endif

/// USER_SECTION_START 2

/// USER_SECTION_END

#ifdef DDD_PROFILING
	#include "easy/profiler.h"
	#include <easy/arbitrary_value.h> // EASY_VALUE, EASY_ARRAY are defined here

	#define DDD_PROFILING_BLOCK_C(text, color) EASY_BLOCK(text, color)
	#define DDD_PROFILING_NONSCOPED_BLOCK_C(text, color) EASY_NONSCOPED_BLOCK(text, color)
	#define DDD_PROFILING_END_BLOCK EASY_END_BLOCK
	#define DDD_PROFILING_FUNCTION_C(color) EASY_FUNCTION(color)
	#define DDD_PROFILING_BLOCK(text, colorStage) DDD_PROFILING_BLOCK_C(text,profiler::colors::  colorStage)
	#define DDD_PROFILING_NONSCOPED_BLOCK(text, colorStage) DDD_PROFILING_NONSCOPED_BLOCK_C(text,profiler::colors::  colorStage)
	#define DDD_PROFILING_FUNCTION(colorStage) DDD_PROFILING_FUNCTION_C(profiler::colors:: colorStage)
	#define DDD_PROFILING_THREAD(name) EASY_THREAD(name)

	#define DDD_PROFILING_VALUE(name, value) EASY_VALUE(name, value)
	#define DDD_PROFILING_TEXT(name, value) EASY_TEXT(name, value)

#else
	#define DDD_PROFILING_BLOCK_C(text, color)
	#define DDD_PROFILING_NONSCOPED_BLOCK_C(text, color)
	#define DDD_PROFILING_END_BLOCK
	#define DDD_PROFILING_FUNCTION_C(color)
	#define DDD_PROFILING_BLOCK(text, colorStage)
	#define DDD_PROFILING_NONSCOPED_BLOCK(text, colorStage)
	#define DDD_PROFILING_FUNCTION(colorStage)
	#define DDD_PROFILING_THREAD(name)

	#define DDD_PROFILING_VALUE(name, value)
	#define DDD_PROFILING_TEXT(name, value)
#endif

// Special expantion tecniques are required to combine the color name
#define CONCAT_SYMBOLS_IMPL(x, y) x##y
#define CONCAT_SYMBOLS(x, y) CONCAT_SYMBOLS_IMPL(x, y)



// Different color stages
#define DDD_COLOR_STAGE_1 50
#define DDD_COLOR_STAGE_2 100
#define DDD_COLOR_STAGE_3 200
#define DDD_COLOR_STAGE_4 300
#define DDD_COLOR_STAGE_5 400
#define DDD_COLOR_STAGE_6 500
#define DDD_COLOR_STAGE_7 600
#define DDD_COLOR_STAGE_8 700
#define DDD_COLOR_STAGE_9 800
#define DDD_COLOR_STAGE_10 900
#define DDD_COLOR_STAGE_11 A100 
#define DDD_COLOR_STAGE_12 A200 
#define DDD_COLOR_STAGE_13 A400 
#define DDD_COLOR_STAGE_14 A700 

namespace DDD
{
/// USER_SECTION_START 4

/// USER_SECTION_END
	class DDD_EXPORT Profiler
	{
	public:
		/// USER_SECTION_START 5

		/// USER_SECTION_END

		// Implementation defined in LibraryName_info.cpp to save files.
		static void start();
		static void stop();
		static void stop(const char* profilerOutputFile);

		/// USER_SECTION_START 6

		/// USER_SECTION_END
	};

/// USER_SECTION_START 7

/// USER_SECTION_END


#if LOGGER_LIBRARY_AVAILABLE == 1
	class DDD_EXPORT Logger 
	{
		/// USER_SECTION_START 8

		/// USER_SECTION_END
		Logger();
		static Logger& instance();
		public:
		/// USER_SECTION_START 9

		/// USER_SECTION_END

		static void setEnabled(bool enable);
		static bool isEnabled();
		static void setName(const std::string& name);
		static std::string getName();
		static void setColor(const Log::Color& col);
		static Log::Color getColor();
		static Log::DateTime getCreationDateTime();
		static Log::LoggerID getID();
		static void setParentID(Log::LoggerID parentID);
		static Log::LoggerID getParentID();



		static void log(const Log::Message& msg);

		static void log(const std::string& msg);
		static void log(const std::string& msg, Log::Level level);
		static void log(const std::string& msg, Log::Level level, const Log::Color& col);

		static void logTrace(const std::string& msg);
		static void logDebug(const std::string& msg);
		static void logInfo(const std::string& msg);
		static void logWarning(const std::string& msg);
		static void logError(const std::string& msg);
		static void logCustom(const std::string& msg);

		/// USER_SECTION_START 10

		/// USER_SECTION_END

		private:
		Log::LogObject m_logObject;

		/// USER_SECTION_START 11

		/// USER_SECTION_END
	};
/// USER_SECTION_START 12

/// USER_SECTION_END
#endif
/// USER_SECTION_START 13

/// USER_SECTION_END
}


// General
#define DDD_GENERAL_PROFILING_COLORBASE Cyan
#define DDD_GENERAL_PROFILING_BLOCK_C(text, color) DDD_PROFILING_BLOCK_C(text, color)
#define DDD_GENERAL_PROFILING_NONSCOPED_BLOCK_C(text, color) DDD_PROFILING_NONSCOPED_BLOCK_C(text, color)
#define DDD_GENERAL_PROFILING_END_BLOCK DDD_PROFILING_END_BLOCK;
#define DDD_GENERAL_PROFILING_FUNCTION_C(color) DDD_PROFILING_FUNCTION_C(color)
#define DDD_GENERAL_PROFILING_BLOCK(text, colorStage) DDD_PROFILING_BLOCK(text, CONCAT_SYMBOLS(DDD_GENERAL_PROFILING_COLORBASE, colorStage))
#define DDD_GENERAL_PROFILING_NONSCOPED_BLOCK(text, colorStage) DDD_PROFILING_NONSCOPED_BLOCK(text, CONCAT_SYMBOLS(DDD_GENERAL_PROFILING_COLORBASE, colorStage))
#define DDD_GENERAL_PROFILING_FUNCTION(colorStage) DDD_PROFILING_FUNCTION(CONCAT_SYMBOLS(DDD_GENERAL_PROFILING_COLORBASE, colorStage))
#define DDD_GENERAL_PROFILING_VALUE(name, value) DDD_PROFILING_VALUE(name, value)
#define DDD_GENERAL_PROFILING_TEXT(name, value) DDD_PROFILING_TEXT(name, value)


/// USER_SECTION_START 3

/// USER_SECTION_END