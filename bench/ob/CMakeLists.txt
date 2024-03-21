add_executable(bench bench_book.cpp)
target_link_system_libraries(
            bench
            PRIVATE
            ob
            Catch2::Catch2WithMain)
