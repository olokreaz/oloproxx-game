#pragma once

#define APP_LOG_DIR  "logs"
#define APP_GLOBAL_LOGGER_NAME = "global"
#define APP_LOGGER_PATTERN "[ %Y:%m:%d - %H:%M:%S:%F ] [ pid %P ] [ thread %t ] [ %^==== %l ====%$ ] <%n> %v"

#define APP_CONFIG_FILE_NAME = "config.local"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
