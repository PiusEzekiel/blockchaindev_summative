#include "alu_payment_blockchain.h"
#include "unity.h"

// ANSI Colors for Pretty Output
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

// Define global variables for tests
Blockchain *test_chain;
Wallet test_wallets[MAX_WALLETS];
Student test_students[MAX_WALLETS];
Vendor test_vendors[MAX_WALLETS];
int test_wallet_count = 0;
int test_student_count = 0;
int test_vendor_count = 0;

// Setup & Teardown Functions - Run before & after each test
void setUp(void) {
    test_chain = initialize_blockchain();
    test_wallet_count = 0;
    test_student_count = 0;
    test_vendor_count = 0;
}

void tearDown(void) {
    free(test_chain);
}

// ✅ Test Wallet Creation
void test_create_wallet(void) {
    printf(BLUE "\n🛠 Testing Wallet Creation...\n" RESET);
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet1", "private_key", 500.0);
    
    TEST_ASSERT_EQUAL(1, test_wallet_count);
    TEST_ASSERT_EQUAL_STRING("wallet1", test_wallets[0].wallet_id);
    TEST_ASSERT_EQUAL_FLOAT(500.0, test_wallets[0].balance);
}

// ✅ Test Duplicate Wallet Prevention
void test_create_duplicate_wallet(void) {
    printf(BLUE "\n🛠 Testing Duplicate Wallet Prevention...\n" RESET);
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet1", "private_key", 500.0);
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet1", "private_key", 1000.0);
    
    TEST_ASSERT_EQUAL(1, test_wallet_count); // Should still be 1
}

// ✅ Test Creating Wallet with Zero Balance
void test_create_wallet_zero_balance(void) {
    printf(BLUE "\n🛠 Testing Wallet Creation with Zero Balance...\n" RESET);
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet2", "private_key", 0.0);
    
    TEST_ASSERT_EQUAL(1, test_wallet_count);
    TEST_ASSERT_EQUAL_FLOAT(0.0, test_wallets[0].balance);
}

// ✅ Test Student Creation
void test_create_student(void) {
    printf(BLUE "\n🛠 Testing Student Creation...\n" RESET);
    create_student(test_students, &test_student_count, test_wallets, &test_wallet_count, test_chain);
    
    TEST_ASSERT_EQUAL(1, test_student_count);
    TEST_ASSERT_NOT_EQUAL(0, test_wallet_count);
}

// ✅ Test Vendor Creation
void test_create_vendor(void) {
    printf(BLUE "\n🛠 Testing Vendor Creation...\n" RESET);
    create_vendor(test_vendors, &test_vendor_count, test_wallets, &test_wallet_count, test_chain);
    
    TEST_ASSERT_EQUAL(1, test_vendor_count);
    TEST_ASSERT_NOT_EQUAL(0, test_wallet_count);
}

// ✅ Test Adding Transactions
void test_add_transaction(void) {
    printf(BLUE "\n🛠 Testing Transaction Processing...\n" RESET);
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet1", "private_key", 1000.0);
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet2", "private_key", 500.0);
    
    int success = add_transaction(test_chain, test_wallets, test_wallet_count, 
                                  test_students, test_student_count, 
                                  test_vendors, test_vendor_count, 
                                  "wallet1", "wallet2", 200.0, "private_key");
    
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_FLOAT(800.0, test_wallets[0].balance);
    TEST_ASSERT_EQUAL_FLOAT(700.0, test_wallets[1].balance);
}

// ❌ Test Preventing Transactions with Insufficient Balance
void test_transaction_insufficient_funds(void) {
    printf(BLUE "\n🛠 Testing Insufficient Funds Handling...\n" RESET);
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet1", "private_key", 100.0);
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet2", "private_key", 500.0);
    
    int success = add_transaction(test_chain, test_wallets, test_wallet_count, 
                                  test_students, test_student_count, 
                                  test_vendors, test_vendor_count, 
                                  "wallet1", "wallet2", 500.0, "private_key");
    
    TEST_ASSERT_FALSE(success);
    TEST_ASSERT_EQUAL_FLOAT(100.0, test_wallets[0].balance);
    TEST_ASSERT_EQUAL_FLOAT(500.0, test_wallets[1].balance);
}

// ❌ Test Preventing Transactions with Invalid Sender ID
void test_transaction_invalid_sender(void) {
    printf(BLUE "\n🛠 Testing Invalid Sender ID Handling...\n" RESET);
    
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet1", "private_key", 500.0);
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet2", "private_key", 500.0);

    int success = add_transaction(test_chain, test_wallets, test_wallet_count, 
                                  test_students, test_student_count, 
                                  test_vendors, test_vendor_count, 
                                  "wallet3", "wallet2", 200.0, "private_key"); // Invalid sender

    TEST_ASSERT_FALSE(success);
}

// ✅ Test Blockchain Integrity
void test_verify_blockchain(void) {
    printf(BLUE "\n🛠 Testing Blockchain Integrity...\n" RESET);
    
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet1", "private_key", 1000.0);
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet2", "private_key", 500.0);
    
    add_transaction(test_chain, test_wallets, test_wallet_count, 
                    test_students, test_student_count, 
                    test_vendors, test_vendor_count, 
                    "wallet1", "wallet2", 200.0, "private_key");
    
    create_block(test_chain, test_wallets, test_wallet_count, 
                 test_students, test_student_count, 
                 test_vendors, test_vendor_count);

    int is_valid = validate_chain(test_chain);
    TEST_ASSERT_TRUE(is_valid);
}

// 🏆 Main Test Runner
int main(void) {
    UNITY_BEGIN();
    
    printf(YELLOW "\n🏆 Starting ALU Blockchain Tests...\n" RESET);

    RUN_TEST(test_create_wallet);
    RUN_TEST(test_create_duplicate_wallet);
    RUN_TEST(test_create_wallet_zero_balance);
    RUN_TEST(test_create_student);
    RUN_TEST(test_create_vendor);
    RUN_TEST(test_add_transaction);
    RUN_TEST(test_transaction_insufficient_funds);
    RUN_TEST(test_transaction_invalid_sender);
    RUN_TEST(test_verify_blockchain);

    printf(GREEN "\n🎉 ALL TESTS COMPLETED SUCCESSFULLY! 🎉\n" RESET);
    
    return UNITY_END();
}
