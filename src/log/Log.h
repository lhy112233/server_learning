#ifndef HY_LOG_H_
#define HY_LOG_H_
/*Options*/
#define HY_SPDLOG


/*Verify spdlog*/
#ifdef HY_SPDLOG
#include "spdlog/spdlog.h"
#define LOG(level) ::spdlog::level
#endif  //HYSPDLOG





#endif  //HY_LOG_H_