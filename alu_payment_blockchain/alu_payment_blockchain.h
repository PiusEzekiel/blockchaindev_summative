#ifndef ALU_PAYMENT_BLOCKCHAIN_H
#define ALU_PAYMENT_BLOCKCHAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

// ANSI Colors for styling
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

// Constants
#define MAX_WALLET_ID 50
#define MAX_WALLETS 100
#define HASH_LENGTH 64
#define HASH_SIZE 65
#define MINING_REWARD 10  // Reward per mined block
#define MAX_TOKEN_NAME 50 // Define the max token name size
#define MAX_TRANSACTION_DATA 1024  // Define the max transaction data size
// #define DIFFICULTY 4  // Define difficulty for Proof-of-Work
extern int difficulty;  // Declare the global variable (to be defined in the `.c` file)
#define BLOCKCHAIN_FILE "alu_payment_blockchain.dat"
#define WALLETS_FILE "alu_wallets.dat"

// ✅ Define Transaction Struct
typedef struct Transaction {
    char sender_id[MAX_WALLET_ID];
    char receiver_id[MAX_WALLET_ID];
    double amount;
    char payment_type[30];  // NEW: Tuition, Cafeteria, Library Fine, etc.
    time_t timestamp;
    struct Transaction *next;
} Transaction;


// ✅ Define Token Struct
typedef struct {
    char token_name[MAX_TOKEN_NAME];
    unsigned int total_supply;
    unsigned int circulating_supply;
} Token;


// ✅ Define Blockchain Struct (Ensure it includes `pending_transactions`)
typedef struct Blockchain {
    struct Block *head;
    Token token;
    Transaction *pending_transactions;
    int size;
} Blockchain;

// ✅ Define Block Struct
typedef struct Block {
    int index;
    char previous_hash[HASH_LENGTH + 1];
    char current_hash[HASH_LENGTH + 1];
    time_t timestamp;
    int nonce;
    Transaction *transactions;
    struct Block *next;
} Block;

// ✅ Define Wallet Struct
typedef struct Wallet {
    char wallet_id[MAX_WALLET_ID];        // Wallet ID
    char private_key_hash[HASH_SIZE];     // Hashed private key
    char wallet_id_hash[HASH_SIZE];       // ✅ Hashed wallet ID (NEW)
    double balance;                        // Wallet balance 
} Wallet;



// ✅ Function Prototypes
Blockchain *initialize_blockchain(void);
// Update in alu_payment_blockchain.h
int add_transaction(Blockchain *chain, Wallet *wallets, int wallet_count, const char *sender_id, const char *receiver_id, double amount, const char *private_key);
void create_wallet(Wallet *wallets, int *wallet_count, Blockchain *chain, const char *wallet_id, const char *private_key, double initial_balance);

void view_balance(Wallet *wallets, int wallet_count, const char *wallet_id);
Block *create_block(Blockchain *chain, Wallet *wallets, int wallet_count);
// void mine_block(Block *block, Wallet *wallets, int wallet_count, const char *miner_wallet_id);
void mine_block(Blockchain *chain, Block *block, Wallet *wallets, int wallet_count, const char *miner_wallet_id);
void compute_block_hash(Block *block);  // Function prototype to avoid implicit declaration warning

void print_blockchain(Blockchain *chain);
int validate_chain(Blockchain *chain);
void save_blockchain(Blockchain *chain);
void backup_blockchain(Blockchain *chain, Wallet *wallets, int wallet_count);
void load_blockchain(Blockchain *chain);
void save_wallets(Wallet *wallets, int wallet_count);
// void load_wallets(Wallet *wallets, int *wallet_count);
void load_wallets(Wallet *wallets, int *wallet_count, Blockchain *chain);
void get_string_input(const char *prompt, char *buffer, size_t size);
void print_transaction(Transaction *txn);
void list_wallets(Wallet *wallets, int wallet_count);
void view_transaction_history(Blockchain *chain);
void display_mining_success(Block *block, double mining_duration);
void free_blockchain(Blockchain *chain);

#endif // ALU_PAYMENT_BLOCKCHAIN_H
