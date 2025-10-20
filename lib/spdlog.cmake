include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY	https://github.com/Bannirui/spdlog.git
        GIT_TAG 	    4397dac510274c7ccf79b5f40b1b747033c12dda
)
FetchContent_MakeAvailable(spdlog)
