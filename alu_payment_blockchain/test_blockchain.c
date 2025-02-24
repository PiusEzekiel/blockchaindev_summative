
#include "alu_payment_blockchain.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include "unity.h"


// Define global variables for tests
Blockchain *test_chain;
Wallet test_wallets[MAX_WALLETS];
Student test_students[MAX_WALLETS];
Vendor test_vendors[MAX_WALLETS];
int test_wallet_count = 0;
int test_student_count = 0;
int test_vendor_count = 0;


// Setup & Teardown Functions- These run before and after each test to ensure a clean environment.
void setUp(void) {
    test_chain = initialize_blockchain();
    test_wallet_count = 0;
    test_student_count = 0;
    test_vendor_count = 0;
}

void tearDown(void) {
    free(test_chain);
}

// Test the create_wallet function
void test_create_wallet(void) {
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet1", "private_key", 500.0);
    
    TEST_ASSERT_EQUAL(1, test_wallet_count);
    TEST_ASSERT_EQUAL_STRING("wallet1", test_wallets[0].wallet_id);
    TEST_ASSERT_EQUAL_FLOAT(500.0, test_wallets[0].balance);
}

// Test Preventing Duplicate Wallets
void test_create_duplicate_wallet(void) {
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet1", "private_key", 500.0);
    create_wallet(test_wallets, &test_wallet_count, test_chain, "wallet1", "private_key", 1000.0);
    
    TEST_ASSERT_EQUAL(1, test_wallet_count); // Should still be 1
}

// Test Student Creation
void test_create_student(void) {
    create_student(test_students, &test_student_count, test_wallets, &test_wallet_count, test_chain);
    
    TEST_ASSERT_EQUAL(1, test_student_count);
    TEST_ASSERT_NOT_EQUAL(0, test_wallet_count);
}

// Test Vendor Creation
void test_create_vendor(void) {
    create_vendor(test_vendors, &test_vendor_count, test_wallets, &test_wallet_count, test_chain);
    
    TEST_ASSERT_EQUAL(1, test_vendor_count);
    TEST_ASSERT_NOT_EQUAL(0, test_wallet_count);
}

//  Test Adding Transactions
void test_add_transaction(void) {
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


// Test Preventing Transactions with Insufficient Balance

void test_transaction_insufficient_funds(void) {
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


// Test Blockchain Integrity

void test_verify_blockchain(void) {
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


int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_create_wallet);
    RUN_TEST(test_create_duplicate_wallet);
    RUN_TEST(test_create_student);
    RUN_TEST(test_create_vendor);
    RUN_TEST(test_add_transaction);
    RUN_TEST(test_transaction_insufficient_funds);
    RUN_TEST(test_verify_blockchain);
    
    return UNITY_END();
}
