add_executable(test_mem test_storage.cpp)
target_link_system_libraries(
            test_mem
            PRIVATE
            default_warnings
            sh
            Catch2::Catch2WithMain)
