#include <gtest/gtest.h>
#include <windows.h>
#include <iostream>
#include <sstream>
#include <vector>

extern "C" {
    // Declare the functions we want to test
    DWORD WINAPI workerFunction(LPVOID lpParam);
    struct structure {
        char* str;
        int size;
    };
}

// Test fixture for worker function tests
class WorkerFunctionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test data
        testStr = new char[100];
    }

    void TearDown() override {
        delete[] testStr;
        if (s) {
            delete s;
        }
    }

    char* testStr;
    structure* s = nullptr;
    DWORD result;
};

// Test for empty string
TEST_F(WorkerFunctionTest, HandlesEmptyString) {
    strcpy(testStr, "");
    s = new structure{testStr, 0};
    
    result = workerFunction(s);
    
    EXPECT_EQ(result, 0);
}

// Test for string with no non-ASCII characters
TEST_F(WorkerFunctionTest, HandlesNoNonAsciiChars) {
    strcpy(testStr, "Hello World");
    s = new structure{testStr, 11};
    
    testing::internal::CaptureStdout();
    result = workerFunction(s);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("Nolatin: ") != std::string::npos);
    EXPECT_TRUE(output.find("Hello") == std::string::npos); // Shouldn't print ASCII chars
}

// Test for string with non-ASCII characters
TEST_F(WorkerFunctionTest, HandlesNonAsciiChars) {
    // Using some non-ASCII characters (like Russian letters)
    strcpy(testStr, "Привет Hello");
    s = new structure{testStr, 12};
    
    testing::internal::CaptureStdout();
    result = workerFunction(s);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("Nolatin: ") != std::string::npos);
    // Should print the non-ASCII characters
    EXPECT_TRUE(output.find("П") != std::string::npos);
    EXPECT_TRUE(output.find("р") != std::string::npos);
    EXPECT_TRUE(output.find("и") != std::string::npos);
    EXPECT_TRUE(output.find("в") != std::string::npos);
    EXPECT_TRUE(output.find("е") != std::string::npos);
    EXPECT_TRUE(output.find("т") != std::string::npos);
    EXPECT_TRUE(output.find("Hello") == std::string::npos); // Shouldn't print ASCII chars
}

// Test for thread creation (mocked)
TEST(ThreadTest, ThreadCreation) {
    structure s;
    char str[] = "Test";
    s.str = str;
    s.size = 4;
    
    HANDLE worker;
    DWORD IDThread;
    worker = CreateThread(NULL, 0, workerFunction, (LPVOID)&s, CREATE_SUSPENDED, &IDThread);
    
    EXPECT_NE(worker, NULL);
    
    if (worker != NULL) {
        DWORD resumeResult = ResumeThread(worker);
        EXPECT_EQ(resumeResult, 1);
        
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
    }
}

// Test for main function logic (mocked input/output)
TEST(MainFunctionTest, HandlesInputCorrectly) {
    // Mock cin input
    std::istringstream input("5\nabcde\n1000\n");
    std::streambuf* origCin = std::cin.rdbuf(input.rdbuf());
    
    // Capture cout output
    std::ostringstream output;
    std::streambuf* origCout = std::cout.rdbuf(output.rdbuf());
    
    // Call main (you might need to rename your main to something else and call it here)
    // extern int my_main();
    // int result = my_main();
    
    // Restore cin/cout
    std::cin.rdbuf(origCin);
    std::cout.rdbuf(origCout);
    
    // Check output
    std::string outputStr = output.str();
    EXPECT_TRUE(outputStr.find("Size: ") != std::string::npos);
    EXPECT_TRUE(outputStr.find("stroka: ") != std::string::npos);
    EXPECT_TRUE(outputStr.find("time for sleep(ms): ") != std::string::npos);
    EXPECT_TRUE(outputStr.find("tread slepping...") != std::string::npos);
    EXPECT_TRUE(outputStr.find("threed is working") != std::string::npos);
    EXPECT_TRUE(outputStr.find("thread zavershon") != std::string::npos);
}
