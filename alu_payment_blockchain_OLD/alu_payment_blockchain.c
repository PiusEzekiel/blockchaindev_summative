
#include "alu_payment_blockchain.h"

int difficulty = 4;  // Start difficulty at 4

/**
 * generate_hash - Generate SHA-256 hash from input string.
 */
void generate_hash(const char *input, char *output) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input, strlen(input), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = '\0';
}

/**
 * initialize_blockchain - Creates a new blockchain.
 */
Blockchain *initialize_blockchain(void) {
    Blockchain *chain = malloc(sizeof(Blockchain));
    if (!chain) return NULL;
    chain->head = NULL;
    chain->size = 0;

    // Initialize ALU Token Supply
    chain->token.total_supply = 1000000;  // 1 Million ALU Tokens
    chain->token.circulating_supply = 0;
    strcpy(chain->token.token_name, "ALU Token");

    return chain;
}


/**
 * create_wallet - Initializes a new wallet 
 * with user-defined hashed private key
 * ensuring no duplicate wallet IDs.
 */

 /**
 * create_wallet - Initializes a new wallet, ensuring no duplicate wallet IDs.
 */
void create_wallet(Wallet *wallets, int *wallet_count, Blockchain *chain, const char *wallet_id, const char *private_key, double initial_balance) {
    // 🔎 Check if the wallet ID already exists
    for (int i = 0; i < *wallet_count; i++) {
        if (strcmp(wallets[i].wallet_id, wallet_id) == 0) {
            printf(RED "❌ Error: Wallet ID '%s' already exists! Choose a different ID.\n" RESET, wallet_id);
            return;
        }
    }

    // ✅ Proceed with wallet creation
    strcpy(wallets[*wallet_count].wallet_id, wallet_id);
    wallets[*wallet_count].balance = initial_balance;

    // Hash the user-entered private key
    generate_hash(private_key, wallets[*wallet_count].private_key_hash);

    // 🔐 Hash the wallet ID for additional security
    generate_hash(wallet_id, wallets[*wallet_count].wallet_id_hash);

    // 🔹 Update circulating supply
    if (chain->token.circulating_supply + initial_balance <= chain->token.total_supply) {
        chain->token.circulating_supply += initial_balance;
    } else {
        printf(RED "❌ Error: Not enough ALU Tokens available in supply!\n" RESET);
        return;
    }

    printf(GREEN "✅ Wallet Created: %s | Balance: %.2f ALU Tokens\n" RESET, wallet_id, initial_balance);
    printf(YELLOW "🔑 Private Key Hash Saved " RESET);
    printf(RED "(DO NOT SHARE PRIVATE KEY!)\n" RESET);
    printf(YELLOW "🔑 Wallet ID Hash: %s\n" RESET, wallets[*wallet_count].wallet_id_hash);


    // Increase wallet count after successful creation
    (*wallet_count)++;
}


/**
 * approve_transaction - Validates the private key hash for transaction approval.
 */
int approve_transaction(Wallet *wallets, int wallet_count, const char *wallet_id, const char *private_key, const char *receiver_id) {
    char hashed_key[HASH_SIZE];
    char receiver_hashed_id[HASH_SIZE];
    char user_confirmation;

    // Hash the entered private key
    generate_hash(private_key, hashed_key);

    // Find sender wallet and validate private key
    for (int i = 0; i < wallet_count; i++) {
        if (strcmp(wallets[i].wallet_id, wallet_id) == 0) {
            if (strcmp(wallets[i].private_key_hash, hashed_key) == 0) {
                // ✅ Find receiver's hashed ID
                for (int j = 0; j < wallet_count; j++) {
                    if (strcmp(wallets[j].wallet_id, receiver_id) == 0) {
                        strcpy(receiver_hashed_id, wallets[j].wallet_id_hash);
                        printf(YELLOW "\n Confirm Transaction:\n" RESET);
                        printf(RED "   You are about to send funds to: %s\n" RESET, receiver_id);
                        printf(RED "   Receiver's Hashed ID: %s\n" RESET, receiver_hashed_id);
                        printf(YELLOW "Is this the correct receiver? (y/n): " RESET);

                        scanf(" %c", &user_confirmation);
                        if (user_confirmation == 'y' || user_confirmation == 'Y') {
                            return 1;  // ✅ Approved
                        } else {
                            printf(RED "❌ Transaction canceled by sender.\n" RESET);
                            return 0;
                        }
                    }
                }
                printf(RED "❌ Receiver not found.\n" RESET);
                return 0;
            } else {
                printf(RED "❌ Invalid private key! Transaction denied.\n" RESET);
                return 0;
            }
        }
    }
    printf(RED "❌ Wallet not found.\n" RESET);
    return 0;
}


/**
 * view_balance - Displays wallet balance.
 */
void view_balance(Wallet *wallets, int wallet_count, const char *wallet_id) {
    for (int i = 0; i < wallet_count; i++) {
        if (strcmp(wallets[i].wallet_id, wallet_id) == 0) {
            printf(BLUE "💰 Wallet: %s | Balance: %.2f ALU Tokens\n" RESET, wallets[i].wallet_id, wallets[i].balance);
            return;
        }
    }
    printf(RED "❌ Error: Wallet not found.\n" RESET);
}

/**
 * add_transaction - Adds a transaction to the blockchain.
 */
int add_transaction(Blockchain *chain, Wallet *wallets, int wallet_count, const char *sender_id, const char *receiver_id, double amount, const char *private_key)
{
    // 🚨 Prevent self-transfer
    if (strcmp(sender_id, receiver_id) == 0) {
        printf(RED "❌ Error: You cannot send funds to yourself.\n" RESET);
        return 0;
    }

    
    if (!approve_transaction(wallets, wallet_count, sender_id, private_key, receiver_id)) {
        return 0; // ❌ Transaction denied
    }

    Wallet *sender = NULL, *receiver = NULL;

    for (int i = 0; i < wallet_count; i++) {
        if (strcmp(wallets[i].wallet_id, sender_id) == 0) {
            sender = &wallets[i];
        }
        if (strcmp(wallets[i].wallet_id, receiver_id) == 0) {
            receiver = &wallets[i];
        }
    }

    if (!sender || !receiver) {
        printf(RED "❌ Error: Wallet not found.\n" RESET);
        return 0;
    }

    if (sender->balance < amount) {
        printf(RED "❌ Error: Insufficient funds.\n" RESET);
        return 0;
    }

    sender->balance -= amount;
    receiver->balance += amount;

    Transaction *new_txn = malloc(sizeof(Transaction));
if (!new_txn) {
    printf(RED "❌ Memory allocation failed!\n" RESET);
    return 0;
}

// ✅ Initialize transaction properly
memset(new_txn, 0, sizeof(Transaction));

snprintf(new_txn->sender_id, MAX_WALLET_ID, "%s", sender_id);
snprintf(new_txn->receiver_id, MAX_WALLET_ID, "%s", receiver_id);
new_txn->amount = amount;
new_txn->timestamp = time(NULL);
new_txn->next = NULL;  // Important to avoid pointer corruption

// 🔹 Debugging output: Verify transaction details
printf(GREEN "✅ Confirmed and initilizing Transaction..... \n" RESET);
printf(GREEN "Transaction Created -> Sender: '%s', Receiver: '%s', Amount: %.2f\n" RESET,
    new_txn->sender_id, new_txn->receiver_id, new_txn->amount);

// Verify string termination
if (strlen(new_txn->sender_id) == 0 || strlen(new_txn->receiver_id) == 0) {
    printf(RED "⚠️ Error: Transaction sender or receiver ID is empty!\n" RESET);
    return 0;
}

if (new_txn->amount <= 0) {
    printf(RED "⚠️ Error: Invalid transaction amount!\n" RESET);
    return 0;
}


    // 🔹 Prompt user to select payment type
    printf(YELLOW "What is the funds for?\n" RESET);
    printf(GREEN "1. Tuition\n");
    printf("2. Cafeteria\n");
    printf("3. Library Fine\n");
    printf("4. Event Ticket\n" );
    printf("5. Gift\n");
    printf("6. Refund\n" RESET);
    printf(YELLOW "Enter choice (1-4): " RESET);

    int payment_choice;
    scanf("%d", &payment_choice);
    getchar(); // Clear buffer

    switch (payment_choice) {
        case 1:
            strcpy(new_txn->payment_type, "Tuition");
            break;
        case 2:
            strcpy(new_txn->payment_type, "Cafeteria");
            break;
        case 3:
            strcpy(new_txn->payment_type, "Library Fine");
            break;
        case 4:
            strcpy(new_txn->payment_type, "Event Ticket");
            break;
        case 5:
            strcpy(new_txn->payment_type, "Gift");
            break;
            case 6:
            strcpy(new_txn->payment_type, "Refund");
            break;
        default:
            printf(RED "❌ Invalid choice! Defaulting to 'General Payment'.\n" RESET);
            strcpy(new_txn->payment_type, "General Payment");
    }

    // new_txn->next = NULL;

    if (!chain->pending_transactions) {
        chain->pending_transactions = new_txn;
    } else {
        Transaction *temp = chain->pending_transactions;
        while (temp->next) temp = temp->next;
        temp->next = new_txn;
    }


    printf(GREEN "✅ Transaction completed: %s sent %.2f ALU tokens to %s for %s\n" RESET, sender_id, amount, receiver_id, new_txn->payment_type);
    
    // 🔥 Auto-mine after every 2 transactions
    int txn_count = 0;
    Transaction *temp = chain->pending_transactions;
    while (temp) {
        txn_count++;
        temp = temp->next;
    }

    if (txn_count >= 2) {
        printf(YELLOW "⛏️ Auto-mining started due to 2 transactions...\n" RESET);
        create_block(chain, wallets, wallet_count);
    }

    printf("\n");

    return 1;
}

/**
 * compute_block_hash - Compute hash for a block.
 */
void compute_block_hash(Block *block) {
    char data[MAX_TRANSACTION_DATA + 100];
    sprintf(data, "%d%s%ld%d", block->index, block->previous_hash, block->timestamp, block->nonce);
    generate_hash(data, block->current_hash);
}


/**
 * create_block - Creates a new block in the blockchain.
 */
Block *create_block(Blockchain *chain, Wallet *wallets, int wallet_count) {
    if (!chain) {
        printf(RED "❌ Error: Blockchain is not initialized.\n" RESET);
        return NULL;
    }

    char miner_wallet_id[MAX_WALLET_ID];
    printf(YELLOW "Enter Miner Wallet ID: " RESET);
    scanf("%s", miner_wallet_id);

    Block *new_block = malloc(sizeof(Block));
    if (!new_block) {
        printf(RED "❌ Memory allocation for new block failed!\n" RESET);
        return NULL;
    }

    new_block->index = chain->size;
    new_block->timestamp = time(NULL);
    new_block->nonce = 0;
    new_block->transactions = NULL;
    new_block->next = NULL;

    // ✅ Assign correct previous hash
    if (chain->head) {
        strcpy(new_block->previous_hash, chain->head->current_hash);
    } else {
        strcpy(new_block->previous_hash, "0000000000000000000000000000000000000000000000000000000000000000");
    }

    // ✅ Move pending transactions into the new block
    if (chain->pending_transactions) {
        new_block->transactions = chain->pending_transactions;
        chain->pending_transactions = NULL;
    } else {
        printf(YELLOW "\n⚠️ No transactions found for this block.\n" RESET);
    }

    // ✅ Compute correct hash before adding
    mine_block(chain, new_block, wallets, wallet_count, miner_wallet_id);

    // ✅ Properly link the new block
    new_block->next = chain->head;
    chain->head = new_block;
    chain->size++;

    printf(GREEN "✅ Block %d created successfully! Total Blocks: %d\n" RESET, new_block->index, chain->size);
    backup_blockchain(chain, wallets, wallet_count);
    return new_block;
}


/**
 * view_token_supply - Displays the ALU Token supply in a formatted table.
 */
void view_token_supply(Blockchain *chain) {
    if (!chain) {
        printf(RED "❌ Error: Blockchain is not initialized.\n" RESET);
        return;
    }

    printf(BLUE "\n╔════════════════════════════════════════════╗\n" RESET);
    printf(BLUE   "║             🏦 ALU Token Supply            ║\n" RESET);
    printf(BLUE   "╠════════════════════════════════════════════╣\n" RESET);
    printf(BLUE   "║  🔹 Token Name         │ %-16s  ║\n" RESET, chain->token.token_name);
    printf(YELLOW   "║  🏦 Total Supply       │ %-10u        ║\n" RESET, chain->token.total_supply);
    printf(BLUE   "║  💸 Circulating Supply │ %-10u        ║\n" RESET, chain->token.circulating_supply);
    printf(BLUE   "╚════════════════════════════════════════════╝\n" RESET);
}



/**
 * mine_block - Simulates Proof-of-Work mining.
 */
void mine_block(Blockchain *chain, Block *block, Wallet *wallets, int wallet_count, const char *miner_wallet_id) {

    clock_t start_time = clock();
    clock_t last_update = clock();  // Track time for progress display

    while (1) {
        compute_block_hash(block);
        
        if (strncmp(block->current_hash, "0000", difficulty) == 0) {
            break;
        }

        block->nonce++;

        // Print progress every 3 seconds
        if (((clock() - last_update) / CLOCKS_PER_SEC) >= 3) {
            printf(YELLOW "⏳ Mining in progress... Nonce: %d | Time Elapsed: %.2f seconds\n" RESET, 
                   block->nonce, (double)(clock() - start_time) / CLOCKS_PER_SEC);
            last_update = clock();
        }
    }

    clock_t end_time = clock();
    double mining_duration = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // 🔧 Adaptive difficulty scaling
    if (mining_duration < 2.0) {
        difficulty += 0.1;  // ✅ Increase difficulty gradually
    } else if (mining_duration > 10.0 && difficulty > 1) {
        difficulty -= 0.1;  // ✅ Decrease difficulty gradually
    }

    // 🔥 Find miner and update balance
    Wallet *miner = NULL;
    for (int i = 0; i < wallet_count; i++) {
        if (strcmp(wallets[i].wallet_id, miner_wallet_id) == 0) {
            miner = &wallets[i];
            miner->balance += MINING_REWARD;
            chain->token.circulating_supply += MINING_REWARD; // ✅ Update supply
            printf(GREEN "🎉 Miner Reward: %s received %d ALU Tokens!\n" RESET, miner_wallet_id, MINING_REWARD);
            break;
        }
    }

    if (!miner) {
        printf(RED "❌ Error: Miner wallet not found!\n" RESET);
        return;
    }

    // ✅ Create a reward transaction
    Transaction *reward_txn = malloc(sizeof(Transaction));
    if (!reward_txn) {
        printf(RED "❌ Memory allocation for reward transaction failed!\n" RESET);
        return;
    }

    snprintf(reward_txn->sender_id, MAX_WALLET_ID, "Blockchain");
    snprintf(reward_txn->receiver_id, MAX_WALLET_ID, "%s", miner_wallet_id);
    reward_txn->amount = MINING_REWARD;
    reward_txn->timestamp = time(NULL);
    strcpy(reward_txn->payment_type, "Mining Reward");
    reward_txn->next = NULL;

    // ✅ Append reward transaction to the block
    if (!block->transactions) {
        block->transactions = reward_txn;
    } else {
        Transaction *temp = block->transactions;
        while (temp->next) temp = temp->next;
        temp->next = reward_txn;
    }

   

    // Display success message
    display_mining_success(block, mining_duration);
     // ✅ Show in Blockchain transactions
     printf(GREEN "✅ Mining reward transaction added to block!\n" RESET);
}


/**
 * display_mining_success - Displays a well-formatted mining success message.
 */
void display_mining_success(Block *block, double mining_duration) {
    if (!block) {
        printf(RED "❌ Error: Block is NULL.\n" RESET);
        return;
    }

    // Extract first 7 and last 7 characters from the hash
    char prev_hash_short[20], curr_hash_short[20];  // Increase buffer size
    snprintf(prev_hash_short, sizeof(prev_hash_short), "%.7s...%.6s", 
         block->previous_hash, block->previous_hash + strlen(block->previous_hash) - 6);

snprintf(curr_hash_short, sizeof(curr_hash_short), "%.7s...%.6s", 
         block->current_hash, block->current_hash + strlen(block->current_hash) - 6);


    printf(BLUE   "╔════════════════════════════════════════════════════════╗\n" RESET);
    printf(BLUE   "║                 ⛏️  Block Mined Successfully!           ║\n" RESET);
    printf(BLUE   "╠════════════════════════════════════════════════════════╣\n" RESET);
    printf(BLUE   "║  🔢 Block Index       │ %-4d                           ║\n" RESET, block->index);
    printf(BLUE   "║  🔗 Previous Hash     │ %-15s               ║\n" RESET, prev_hash_short);
    printf(BLUE   "║  🔗 Current Hash      │ %-15s               ║\n" RESET, curr_hash_short);
    printf(BLUE   "║  🔢 Nonce             │ %-10d                     ║\n" RESET, block->nonce);
    printf(BLUE   "║  ⏳ Mining Time       │ %.2f seconds                   ║\n" RESET, mining_duration);
    printf(BLUE   "║  🔥 Difficulty Level  │ %-4d                           ║\n" RESET, difficulty);
    printf(BLUE   "╚════════════════════════════════════════════════════════╝\n" RESET);
}



/**
 * save_blockchain - Saves the blockchain along with transactions.
 */
void save_blockchain(Blockchain *chain) {
    FILE *file = fopen(BLOCKCHAIN_FILE, "wb");
    if (!file) {
        printf(RED "❌ Error: Unable to save blockchain.\n" RESET);
        return;
    }

    Block *current = chain->head;
    while (current) {
        fwrite(current, sizeof(Block), 1, file);  // ✅ Write block first

        // ✅ Save transaction count
        int txn_count = 0;
        Transaction *txn = current->transactions;
        while (txn) {
            txn_count++;
            txn = txn->next;
        }
        fwrite(&txn_count, sizeof(int), 1, file);  // ✅ Write transaction count

        // ✅ Write transactions
        txn = current->transactions;
        while (txn) {
            fwrite(txn, sizeof(Transaction), 1, file);
            txn = txn->next;
        }

        current = current->next;
    }

    fclose(file);
    // printf(GREEN "✅ Blockchain saved successfully!\n" RESET);
}





/**
 * load_blockchain - Loads blockchain from a file while properly restoring transactions.
 */
void load_blockchain(Blockchain *chain) {
    if (!chain) {
        printf(RED "❌ Error: Blockchain is NULL.\n" RESET);
        return;
    }

    FILE *file = fopen(BLOCKCHAIN_FILE, "rb");
    if (!file) {
        printf(YELLOW "⚠️ No existing blockchain found. Starting fresh.\n" RESET);
        return;
    }

    // 🛑 Clear existing blockchain before loading
    while (chain->head) {
        Block *temp = chain->head;
        chain->head = chain->head->next;
        free(temp);
    }
    chain->size = 0;

    Block *last_block = NULL;
    while (1) {
        // ✅ Allocate memory for new block
        Block *block = (Block *)malloc(sizeof(Block));
        if (!block) {
            printf(RED "❌ Memory allocation failed while loading blockchain!\n" RESET);
            break;
        }

        // ✅ Read block data
        if (fread(block, sizeof(Block), 1, file) != 1) {
            free(block);
            break;
        }

        // ✅ Read transaction count
        int txn_count = 0;
        if (fread(&txn_count, sizeof(int), 1, file) != 1) {
            printf(RED "⚠️ Error reading transaction count!\n" RESET);
            free(block);
            break;
        }

        block->transactions = NULL;
        Transaction *last_txn = NULL;

        // ✅ Read transactions and link them properly
        for (int i = 0; i < txn_count; i++) {
            Transaction *new_txn = (Transaction *)malloc(sizeof(Transaction));
            if (!new_txn || fread(new_txn, sizeof(Transaction), 1, file) != 1) {
                printf(RED "⚠️ Skipping corrupted transaction\n" RESET);
                free(new_txn);
                continue;
            }

            new_txn->next = NULL;
            if (!block->transactions) {
                block->transactions = new_txn;
            } else {
                last_txn->next = new_txn;
            }
            last_txn = new_txn;

           // ✅ Restore previous hash links
        if (last_block) {
            strcpy(block->previous_hash, last_block->current_hash);
        }
        }

        // ✅ Append block to blockchain
        if (chain->head == NULL) {
            chain->head = block;
        } else {
            last_block->next = block;
        }
        last_block = block;
        chain->size++;
    }

    fclose(file);

    
    printf(GREEN "✅ Blockchain loaded successfully! Total Blocks: %d\n" RESET, chain->size);
}


/**
 * save_wallets - Saves the wallets to a file.
 */
void save_wallets(Wallet *wallets, int wallet_count) {
    FILE *file = fopen(WALLETS_FILE, "wb");
    if (!file) {
        printf(RED "❌ Error: Unable to save wallets.\n" RESET);
        return;
    }

    fwrite(wallets, sizeof(Wallet), wallet_count, file);
    fclose(file);
    // printf(GREEN "✅ Wallets saved successfully!\n" RESET);
}

/**
 * load_wallets - Loads wallets from a file.
 */
void load_wallets(Wallet *wallets, int *wallet_count, Blockchain *chain) {
    FILE *file = fopen(WALLETS_FILE, "rb");
    if (!file) {
        printf(YELLOW "⚠️ No existing wallets found. Starting fresh.\n" RESET);
        return;
    }

    *wallet_count = fread(wallets, sizeof(Wallet), MAX_WALLETS, file);
    fclose(file);

    // ✅ Recalculate circulating supply
    chain->token.circulating_supply = 0;
    for (int i = 0; i < *wallet_count; i++) {
        chain->token.circulating_supply += wallets[i].balance;
    }
    printf(GREEN "✅ Wallets loaded successfully! Total wallets: %d\n" RESET, *wallet_count);
}


/**
 * backup_blockchain - Automatically saves blockchain and wallets periodically.
 */
void backup_blockchain(Blockchain *chain, Wallet *wallets, int wallet_count) {
    save_blockchain(chain);
    save_wallets(wallets, wallet_count);
    printf(YELLOW "💾 Auto-backup completed successfully!\n" RESET);
}


/**
 * print_blockchain - Displays the blockchain while preventing segmentation faults.
 */
void print_blockchain(Blockchain *chain) {
    if (!chain || !chain->head) {
        printf(RED "❌ No blocks in the blockchain.\n" RESET);
        return;
    }

    Block *current = chain->head;
    while (current != NULL) {

        // Extract first 7 and last 7 characters from the hash
    char prev_hash_short[20], curr_hash_short[20];  // Increase buffer size
    snprintf(prev_hash_short, sizeof(prev_hash_short), "%.7s...%.6s", 
        
        current->previous_hash, current->previous_hash + strlen(current->previous_hash) - 6);

snprintf(curr_hash_short, sizeof(curr_hash_short), "%.7s...%.6s", 
         current->current_hash, current->current_hash + strlen(current->current_hash) - 6);


        printf(BLUE "\n📜 Block %d\n" RESET, current->index);
        printf("🔗 Previous Hash: %s\n", prev_hash_short);
        printf("🔗 Hash: %s\n", curr_hash_short);
        printf("⏳ Timestamp: %s", ctime(&current->timestamp));
        printf("💸 Transactions:\n");

        Transaction *txn = current->transactions;


        while (txn) {
            if (strlen(txn->sender_id) == 0 || strlen(txn->receiver_id) == 0) { 
                printf("   ❌ Invalid transaction detected. Skipping...\n");
                txn = txn->next;
                continue;
            }
            printf("   🔹 %s sent %.2f ALU tokens to %s for %s\n", 
                   txn->sender_id, txn->amount, txn->receiver_id, txn->payment_type);
            txn = txn->next;
        }

        if (!current->next) {
            printf(GREEN "\n✅ End of Blockchain.\n" RESET);
            break;
        }

        printf(BLUE "╠═══════════════════════════════════════════════════════╣\n" RESET);
        current = current->next;
    }

    // printf(GREEN "\n✅ Blockchain printed successfully.\n" RESET);
}


/**
 * list_wallets - Displays all wallets with proper formatting.
 */
void list_wallets(Wallet *wallets, int wallet_count) {
    if (wallet_count == 0) {
        printf(RED "❌ No wallets found.\n" RESET);
        return;
    }

    printf(BLUE "\n╔════════════════════════════════════════════════════╗\n" RESET);
    printf(BLUE   "║                  💰 Wallets List                   ║\n" RESET);
    printf(BLUE   "╠════════════════════════════════════════════════════╣\n" RESET);
    printf(BLUE   "║  No.  │  Wallet ID        │  Balance (ALU Tokens)  ║\n" RESET);
    printf(BLUE   "╠════════════════════════════════════════════════════╣\n" RESET);

    for (int i = 0; i < wallet_count; i++) {
        printf(BLUE "║  %-4d │ %-17s │  %9.2f ALU Tokens  ║\n" RESET, 
               i + 1, wallets[i].wallet_id, wallets[i].balance);
    }

    printf(BLUE   "╚════════════════════════════════════════════════════╝\n" RESET);
}



/**
 * view_transaction_history - Displays all transactions in the blockchain.
 */
void view_transaction_history(Blockchain *chain) {
    if (!chain || !chain->head) {
        printf(RED "❌ No transaction history available.\n" RESET);
        return;
    }

    printf(BLUE "\n=== Transaction History ===\n" RESET);
    Block *current = chain->head;

    while (current) {
        Transaction *txn = current->transactions;
        while (txn) {
            printf("🔹 %s sent %.2f ALU tokens to %s for %s on %s", 
                   txn->sender_id, txn->amount, txn->receiver_id, txn->payment_type, ctime(&txn->timestamp));
            txn = txn->next;
        }
        current = current->next;
    }
}



/**
 * validate_chain - Ensures blockchain integrity.
 */

int validate_chain(Blockchain *chain) {
    if (!chain || !chain->head) {
        printf(RED "❌ Error: Blockchain is empty.\n" RESET);
        return 0;
    }

    Block *current = chain->head;
    char computed_hash[HASH_SIZE];

    while (current->next) {  // Loop through blocks
        compute_block_hash(current);  // Recalculate hash for comparison
        strcpy(computed_hash, current->current_hash);

        if (strcmp(current->current_hash, computed_hash) != 0) {
            printf(RED "⚠️ Block %d's hash is invalid!\n" RESET, current->index);
            return 0;
        }

        if (strcmp(current->next->previous_hash, current->current_hash) != 0) {
            printf(RED "⚠️ Blockchain broken between Block %d and Block %d!\n" RESET, current->index, current->next->index);
            return 0;
        }

        current = current->next;
    }

    printf(GREEN "✅ Blockchain integrity verified. No issues detected.\n" RESET);
    return 1;
}


/**
 * CLI for Blockchain Interaction
 */
void display_menu() {

    printf("\n" BLUE "=== ALU Payment Blockchain ===\n" RESET);
    printf(GREEN "1. Create Wallet\n");
    printf("2. View Wallet Balance\n");
    printf("3. List All Wallets\n");
    printf("4. Make Payment (Transaction)\n");
    printf(YELLOW "+-----------------------------+\n" RESET);
    printf(BLUE"5. Mine Block\n");
    printf("6. View Blockchain\n");
    printf("7. View Transaction History\n");
    printf("8. View ALU Token Supply\n" RESET);
    printf(YELLOW "+-----------------------------+\n" RESET);
    printf(GREEN"9. Backup Blockchain & Wallets\n");
    printf("10. Load Blockchain & Wallets\n");
    printf("11. Verify Blockchain Integrity\n");
    printf("12. Exit\n" RESET);

    printf(YELLOW "Enter your choice: " RESET);
}


int main() {
    Blockchain *chain = initialize_blockchain();
    Wallet wallets[MAX_WALLETS];
    int wallet_count = 0;

    // Load existing blockchain and wallets
    load_blockchain(chain);
    load_wallets(wallets, &wallet_count, chain);


    int choice;
    char wallet_id[MAX_WALLET_ID];
    char private_key[HASH_SIZE]; // Unhashed input from the user
    double amount;
    char sender_id[MAX_WALLET_ID], receiver_id[MAX_WALLET_ID];

    while (1) {
        display_menu();
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Enter wallet ID: ");
                scanf("%s", wallet_id);
                getchar();

                printf("Enter a private key (DO NOT SHARE!): ");
                scanf("%s", private_key);
                getchar();

                create_wallet(wallets, &wallet_count, chain, wallet_id, private_key, 1000);


                save_wallets(wallets, wallet_count); // ✅ Auto-save after creation
                break;

            case 2:
                printf("Enter wallet ID: ");
                scanf("%s", wallet_id);
                view_balance(wallets, wallet_count, wallet_id);
                break;

            case 3: 
                list_wallets(wallets, wallet_count);
                break;

            case 4:
                printf(YELLOW "Sender Wallet ID: " RESET);
                scanf("%s", sender_id);

                printf(YELLOW "Receiver Wallet ID: " RESET);
                scanf("%s", receiver_id);

                printf(YELLOW "Enter amount: " RESET);
                scanf("%lf", &amount);
                getchar();

                printf(YELLOW "Enter your private key: " RESET);
                scanf("%64s", private_key);

                if (add_transaction(chain, wallets, wallet_count, sender_id, receiver_id, amount, private_key)) {
                    // printf(GREEN "✅ Transaction confirmed and added!\n" RESET);
                    printf(GREEN "\n" RESET);
                } else {
                    printf(RED "❌ Transaction failed.\n" RESET);
                }
                break;

            case 5:
                create_block(chain, wallets, wallet_count);
                break;
            

            case 6:
                print_blockchain(chain);
                break;

            case 7:
                view_transaction_history(chain);
                break;

            case 8:
                view_token_supply(chain);
                break;

            case 9:
                backup_blockchain(chain, wallets, wallet_count);
                break;

            case 10:
                load_blockchain(chain);
                load_wallets(wallets, &wallet_count, chain);

                break;

            case 11:
                validate_chain(chain);
                break;

            case 12:
                
                free(chain);
                return 0;
        }
    }
}