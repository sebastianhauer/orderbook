add_executable(bench bench_book.cpp)
target_link_system_libraries(
            bench
            PRIVATE
            sh
            Catch2::Catch2WithMain)
