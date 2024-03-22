add_executable(test_ob test_level.cpp test_page.cpp test_book.cpp)
target_link_system_libraries(
            test_ob
            PRIVATE
            default_warnings
            sh
            Catch2::Catch2WithMain)
