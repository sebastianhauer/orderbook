add_executable(test test_level.cpp test_page.cpp test_book.cpp)
target_link_system_libraries(
            test
            PRIVATE
            ob
            Catch2::Catch2WithMain)
