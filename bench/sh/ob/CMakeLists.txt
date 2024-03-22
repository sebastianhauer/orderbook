add_executable(bench bench_book.cpp)
target_link_system_libraries(
            bench
            PRIVATE
            default_warnings
            sh
            Catch2::Catch2WithMain)
