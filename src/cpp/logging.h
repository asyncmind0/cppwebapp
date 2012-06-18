#ifndef _LOGGER
#define _LOGGER
#include <pantheios/pantheios.hpp>
#include <pantheios/frontends/stock.h>
#include <pantheios/inserters/args.hpp>
#include <pantheios/inserters.hpp>  // Pantheios inserter classes
//Specify process identity
const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = "cppwebapp";
#define ENDCOLOR "\e[m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define BLUE "\e[0;34m"
#define CYAN "\e[0;36m"
#define PURPLE "\e[0;35m"
#define BROWN "\e[0;33m"

#define LIGHTRED "\e[1;31m"
#define LIGHTGREEN "\e[1;32m"
#define LIGHTBLUE "\e[1;34m"
#define LIGHTCYAN "\e[1;36m"
#define LIGHTPURPLE "\e[1;35m"
#define LIGHTBROWN "\e[1;33m"

#ifdef __DEBUG__
#define log_DEBUG(...) pantheios::log_DEBUG(GREEN,__func__,"(): ", __VA_ARGS__,ENDCOLOR)
#else
#define log_DEBUG(...) 
#endif

#define LOG_PREFIX __func__, "(): "

#define log_WARNING(...) pantheios::log_WARNING(RED,LOG_PREFIX, __VA_ARGS__,ENDCOLOR)
#define log_INFORMATIONAL(...) pantheios::log_INFORMATIONAL(LIGHTGREEN,LOG_PREFIX, __VA_ARGS__,ENDCOLOR)
#define log_NOTICE(...) pantheios::log_NOTICE(LIGHTBLUE,LOG_PREFIX, __VA_ARGS__,ENDCOLOR)
#define log_ERROR(...) pantheios::log_ERROR(LIGHTRED,LOG_PREFIX, __VA_ARGS__,ENDCOLOR)
#define log_CRITICAL(...) pantheios::log_CRITICAL(LIGHTRED,LOG_PREFIX, __VA_ARGS__,ENDCOLOR)
#define log_ALERT(...) pantheios::log_ALERT(LIGHTRED,LOG_PREFIX, __VA_ARGS__,ENDCOLOR)
#define log_EMERGENCY(...) pantheios::log_EMERGENCY(LIGHTRED,LOG_PREFIX, __VA_ARGS__,ENDCOLOR)

#endif
