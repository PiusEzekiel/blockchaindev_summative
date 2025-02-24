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
 * generate_keys - Generates an RSA key pair using OpenSSL 3.0+ EVP API.
 */
void generate_keys(char *public_key, char *private_key) {
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

    if (!ctx) {
        printf("Error initializing key generation context.\n");
        return;
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        printf("Error initializing key generation.\n");
        EVP_PKEY_CTX_free(ctx);
        return;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        printf("Error setting key size.\n");
        EVP_PKEY_CTX_free(ctx);
        return;
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        printf("Error generating RSA key pair.\n");
        EVP_PKEY_CTX_free(ctx);
        return;
    }

    EVP_PKEY_CTX_free(ctx);

    // Convert keys to PEM format
    BIO *pri = BIO_new(BIO_s_mem());
    BIO *pub = BIO_new(BIO_s_mem());

    PEM_write_bio_PrivateKey(pri, pkey, NULL, NULL, 0, NULL, NULL);
    PEM_write_bio_PUBKEY(pub, pkey);

    int pri_len = BIO_read(pri, private_key, 4096);
    int pub_len = BIO_read(pub, public_key, 4096);

    private_key[pri_len] = '\0';
    public_key[pub_len] = '\0';

    // Free memory
    EVP_PKEY_free(pkey);
    BIO_free_all(pri);
    BIO_free_all(pub);
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
    // strcpy(wallets[*wallet_count].wallet_id, wallet_id);
    // wallets[*wallet_count].balance = initial_balance;

    Wallet *new_wallet = &wallets[*wallet_count];  // 🔥 FIX: Define new wallet

    // Assign wallet ID and initial balance
    strcpy(new_wallet->wallet_id, wallet_id);
    new_wallet->balance = initial_balance;

    // Generate public/private key pair
    generate_keys(new_wallet->public_key, new_wallet->private_key);

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
    // backup_blockchain(chain, wallets, *wallet_count);
    
}


void create_student(Student *students, int *student_count, Wallet *wallets, int *wallet_count, Blockchain *chain) {
    char student_id[MAX_WALLET_ID], student_name[50], private_key[HASH_SIZE];

    printf(YELLOW "Enter Student ID: " RESET);
    scanf("%s", student_id);
    printf(YELLOW "Enter Student Name: "RESET);
    scanf(" %[^\n]", student_name);
    printf(YELLOW "Enter a Private Key (DO NOT SHARE!): "RESET);
    scanf("%s", private_key);

    // 🔎 Ensure Student ID is unique
    for (int i = 0; i < *student_count; i++) {
        if (strcasecmp(students[i].student_id, student_id) == 0) {  // Case-insensitive check
            printf(RED "❌ Error: Student ID '%s' already exists!\n" RESET, student_id);
            return;
        }
    }

    // 🔎 Ensure Student ID is not used as a Wallet ID
    for (int i = 0; i < *wallet_count; i++) {
        if (strcasecmp(wallets[i].wallet_id, student_id) == 0) {  // Case-insensitive check
            printf(RED "❌ Error: Wallet ID '%s' is already in use!\n" RESET, student_id);
            return;
        }
    }

    // ✅ Create student wallet
    create_wallet(wallets, wallet_count, chain, student_id, private_key, 1000); // Default balance: 1000 ALU

    // ✅ Assign student details
    strcpy(students[*student_count].student_id, student_id);
    strcpy(students[*student_count].name, student_name);
    students[*student_count].wallet = wallets[*wallet_count - 1]; // Assign wallet to student

    printf(GREEN "✅ Student '%s' (ID: %s) successfully created with wallet balance of 1000 ALU Tokens.\n" RESET, student_name, student_id);

    (*student_count)++;
}


void create_vendor(Vendor *vendors, int *vendor_count, Wallet *wallets, int *wallet_count, Blockchain *chain) {
    char vendor_id[MAX_WALLET_ID], vendor_name[50], private_key[HASH_SIZE];

    printf(YELLOW "Enter Vendor ID: " RESET);
    scanf("%s", vendor_id);
    printf(YELLOW"Enter Vendor Name: " RESET);
    scanf(" %[^\n]", vendor_name);
    printf(YELLOW "Enter a Private Key (DO NOT SHARE!): "RESET);
    scanf("%s", private_key);

    // Ensure vendor ID is unique
    for (int i = 0; i < *vendor_count; i++) {
        if (strcmp(vendors[i].vendor_id, vendor_id) == 0) {
            printf(RED "❌ Error: Vendor ID already exists!\n" RESET);
            return;
        }
    }

    // Create vendor wallet
    create_wallet(wallets, wallet_count, chain, vendor_id, private_key, 0); // Default balance: 0 ALU

    // Assign vendor details
    strcpy(vendors[*vendor_count].vendor_id, vendor_id);
    strcpy(vendors[*vendor_count].name, vendor_name);
    vendors[*vendor_count].wallet = wallets[*wallet_count - 1]; // Assign wallet to vendor

    printf(GREEN "✅ Vendor '%s' (ID: %s) successfully created with wallet balance of 0 ALU Tokens.\n" RESET, vendor_name, vendor_id);

    (*vendor_count)++;
}

/**
 * /* update_student_vendor_wallets - Updates the wallet balances of students and vendors based on the provided wallets.
 */
void update_student_vendor_wallets(Student *students, int student_count, Vendor *vendors, int vendor_count, Wallet *wallets, int wallet_count) {
    // 🔹 Update student wallet balances
    for (int i = 0; i < student_count; i++) {
        for (int j = 0; j < wallet_count; j++) {
            if (strcmp(students[i].student_id, wallets[j].wallet_id) == 0) {
                students[i].wallet.balance = wallets[j].balance; // 🔥 Update student balance
                break;
            }
        }
    }

    // 🔹 Update vendor wallet balances
    for (int i = 0; i < vendor_count; i++) {
        for (int j = 0; j < wallet_count; j++) {
            if (strcmp(vendors[i].vendor_id, wallets[j].wallet_id) == 0) {
                vendors[i].wallet.balance = wallets[j].balance; // 🔥 Update vendor balance
                break;
            }
        }
    }
}


/**
 * list_students - Displays all students in a formatted table.
 */
void list_students(Student *students, int student_count) {
    if (student_count == 0) {
        printf(RED "❌ No students registered.\n" RESET);
        return;
    }

    printf(BLUE "\n╔══════════════════════════════════════════════════════════════════╗\n" RESET);
    printf(BLUE   "║                          🎓 Students List                        ║\n" RESET);
    printf(BLUE   "╠══════════════════════════════════════════════════════════════════╣\n" RESET);
    printf(YELLOW   "║  No.  │  Student Name        │  Student ID     │  Balance (ALU)  ║\n" RESET);
    printf(BLUE   "╠══════════════════════════════════════════════════════════════════╣\n" RESET);

    for (int i = 0; i < student_count; i++) {
        printf(BLUE "║  %-4d │ %-20s │ %-15s │  %9.2f      ║\n" RESET,
               i + 1, students[i].name, students[i].student_id, students[i].wallet.balance);
    }

    printf(BLUE   "╚══════════════════════════════════════════════════════════════════╝\n" RESET);
}


/**
 * list_vendors - Displays all vendors in a formatted table.
 */
void list_vendors(Vendor *vendors, int vendor_count) {
    if (vendor_count == 0) {
        printf(RED "❌ No vendors registered.\n" RESET);
        return;
    }

    printf(BLUE "\n╔══════════════════════════════════════════════════════════════════╗\n" RESET);
    printf(BLUE   "║                           🏢 Vendors List                        ║\n" RESET);
    printf(YELLOW   "╠══════════════════════════════════════════════════════════════════╣\n" RESET);
    printf(BLUE   "║  No.  │  Vendor Name         │  Vendor ID      │  Balance (ALU)  ║\n" RESET);
    printf(BLUE   "╠══════════════════════════════════════════════════════════════════╣\n" RESET);

    for (int i = 0; i < vendor_count; i++) {
        printf(BLUE "║  %-4d │ %-20s │ %-15s │  %9.2f      ║\n" RESET, 
               i + 1, vendors[i].name, vendors[i].vendor_id, vendors[i].wallet.balance);
    }

    printf(BLUE   "╚══════════════════════════════════════════════════════════════════╝\n" RESET);
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
 * verify_transaction - Checks if the private key matches the wallet.
 */
int verify_transaction(Wallet *wallets, int wallet_count, const char *wallet_id, const char *private_key) {
    for (int i = 0; i < wallet_count; i++) {
        if (strcmp(wallets[i].wallet_id, wallet_id) == 0) {
            // Compare stored private key with input
            if (strcmp(wallets[i].private_key, private_key) == 0) {
                return 1; // ✅ Authentication Successful
            }
        }
    }
    return 0; // ❌ Authentication Failed
}


/**
 * add_transaction - Adds a transaction to the blockchain.
 */
int add_transaction(Blockchain *chain, Wallet *wallets, int wallet_count,
    Student *students, int student_count, Vendor *vendors, int vendor_count,
    const char *sender_id, const char *receiver_id, double amount, const char *private_key)

{
    // 🚨 Prevent self-transfer
    if (strcmp(sender_id, receiver_id) == 0) {
        printf(RED "❌ Error: You cannot send funds to yourself.\n" RESET);
        return 0;
    }

    // // ✅ Verify sender's private key
    // if (!verify_transaction(wallets, wallet_count, sender_id, private_key)) {
    //     printf(RED "❌ Error: Private key verification failed! Transaction denied.\n" RESET);
    //     return 0;
    // }
    
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
    // 🔥 Update student and vendor balances to match wallets
update_student_vendor_wallets(students, student_count, vendors, vendor_count, wallets, wallet_count);
    
// 🔥 Auto-mine after every 2 transactions
    int txn_count = 0;
    Transaction *temp = chain->pending_transactions;
    while (temp) {
        txn_count++;
        temp = temp->next;
    }

    if (txn_count >= 2) {
        printf(YELLOW "⛏️ Auto-mining started due to 2 transactions...\n" RESET);
        create_block(chain, wallets, wallet_count, students, student_count, vendors, vendor_count);

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
Block *create_block(Blockchain *chain, Wallet *wallets, int wallet_count, 
    Student *students, int student_count, Vendor *vendors, int vendor_count) {

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
    // backup_blockchain(chain, wallets, wallet_count);
    backup_blockchain(chain, wallets, students, student_count, vendors, vendor_count, wallet_count);
    // 🔥 Update student and vendor balances to match wallets
update_student_vendor_wallets(students, student_count, vendors, vendor_count, wallets, wallet_count);
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
    int block_count = 0;

    // Count the number of blocks
    while (current) {
        block_count++;
        current = current->next;
    }

    // Create an array to store blocks in the correct order
    Block *blocks[block_count];
    current = chain->head;
    int i = block_count - 1;

    // Store blocks in reverse order
    while (current) {
        blocks[i--] = current;
        current = current->next;
    }

    // Save in the correct order (oldest to newest)
    for (i = 0; i < block_count; i++) {
        fwrite(blocks[i], sizeof(Block), 1, file);

        // ✅ Save transaction count
        int txn_count = 0;
        Transaction *txn = blocks[i]->transactions;
        while (txn) {
            txn_count++;
            txn = txn->next;
        }
        fwrite(&txn_count, sizeof(int), 1, file);

        // ✅ Write transactions
        txn = blocks[i]->transactions;
        while (txn) {
            fwrite(txn, sizeof(Transaction), 1, file);
            txn = txn->next;
        }
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

    // 🛑 Clear existing blockchain
    while (chain->head) {
        Block *temp = chain->head;
        chain->head = chain->head->next;
        free(temp);
    }
    chain->size = 0;

    Block *last_block = NULL;
    int block_count = 0;
    Block *blocks[100]; // Temporary storage for ordered loading

    while (1) {
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
        }

        blocks[block_count++] = block;
    }
    fclose(file);

    // ✅ Link blocks correctly from oldest to newest
    for (int i = 0; i < block_count; i++) {
        if (i > 0) {
            blocks[i]->next = blocks[i - 1];
            strcpy(blocks[i]->previous_hash, blocks[i - 1]->current_hash);
        }
        compute_block_hash(blocks[i]); // ✅ Recompute hash
    }

    chain->head = blocks[block_count - 1]; // ✅ Oldest block becomes the head
    chain->size = block_count;

    printf(GREEN "✅ Blockchain loaded successfully! Total Blocks: %d\n" RESET, chain->size);
}




/**
 * save_wallets - Saves the wallets to a file, including students & vendors names.
 */
void save_wallets(Wallet *wallets, Student *students, int student_count, Vendor *vendors, int vendor_count, int wallet_count) {
    FILE *file = fopen(WALLETS_FILE, "wb");
    if (!file) {
        printf(RED "❌ Error: Unable to save wallets.\n" RESET);
        return;
    }

    // Save wallet count
    fwrite(&wallet_count, sizeof(int), 1, file);
    fwrite(wallets, sizeof(Wallet), wallet_count, file);

    // Save student count and student data
    fwrite(&student_count, sizeof(int), 1, file);
    fwrite(students, sizeof(Student), student_count, file);

    // Save vendor count and vendor data
    fwrite(&vendor_count, sizeof(int), 1, file);
    fwrite(vendors, sizeof(Vendor), vendor_count, file);

    fclose(file);
    // printf(GREEN "✅ Wallets, students, and vendors saved successfully!\n" RESET);
}

/**
 * load_wallets - Loads wallets, students, and vendors from a file.
 */
void load_wallets(Wallet *wallets, Student *students, int *student_count, Vendor *vendors, int *vendor_count, int *wallet_count, Blockchain *chain) {
    FILE *file = fopen(WALLETS_FILE, "rb");
    if (!file) {
        printf(YELLOW "⚠️ No existing wallets found. Starting fresh.\n" RESET);
        return;
    }

    // Load wallet count and wallets
    fread(wallet_count, sizeof(int), 1, file);
    fread(wallets, sizeof(Wallet), *wallet_count, file);

    // Load student count and student data
    fread(student_count, sizeof(int), 1, file);
    fread(students, sizeof(Student), *student_count, file);

    // Load vendor count and vendor data
    fread(vendor_count, sizeof(int), 1, file);
    fread(vendors, sizeof(Vendor), *vendor_count, file);

    fclose(file);

    // ✅ Recalculate circulating supply
    chain->token.circulating_supply = 0;
    for (int i = 0; i < *wallet_count; i++) {
        chain->token.circulating_supply += wallets[i].balance;
    }
    // 🔥 Update student and vendor balances to match wallets
    update_student_vendor_wallets(students, *student_count, vendors, *vendor_count, wallets, *wallet_count);

    printf(GREEN "✅ Wallets, students, and vendors loaded successfully! \n" RESET);
}



/**
 * backup_blockchain - Saves blockchain, wallets, students, and vendors.
 */
void backup_blockchain(Blockchain *chain, Wallet *wallets, Student *students, int student_count, Vendor *vendors, int vendor_count, int wallet_count) {
    save_blockchain(chain);
    save_wallets(wallets, students, student_count, vendors, vendor_count, wallet_count);
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

    printf(YELLOW "\n                              === Transaction History ===\n" RESET);
    printf(YELLOW "+----------------------------------------------------------------------------------------+\n" RESET);
        
    Block *current = chain->head;

    while (current) {
        Transaction *txn = current->transactions;
        while (txn) {
            printf(GREEN"🔹 %s sent %.2f ALU tokens to %s for %s on %s", 
                   txn->sender_id, txn->amount, txn->receiver_id, txn->payment_type, ctime(&txn->timestamp));
            txn = txn->next;
        }
        printf(YELLOW "+----------------------------------------------------------------------------------------+\n" RESET);
        current = current->next;
    }
}



/**
 * verify blockchain - Ensures blockchain integrity.
 */


int validate_chain(Blockchain *chain) {
    Block *current = chain->head;
    char temp_hash[HASH_LENGTH + 1];

    while (current && current->next) {
        compute_block_hash(current);
        // compute_hash(current);
        strcpy(temp_hash, current->current_hash);

        if (strcmp(temp_hash, current->current_hash) != 0) {
            printf(RED "⚠️ Integrity violation detected at Block %d!\n" RESET, current->index);
            return 0;
        }

        if (strcmp(current->previous_hash, current->next->current_hash) != 0) {
            printf(RED "⚠️ Blockchain broken between Blocks %d and %d!\n" RESET, current->index, current->next->index);
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
    printf(GREEN "1. Create Student Wallet\n");
    printf("2. Create Vendor Wallet\n");
    printf("3. View Wallet Balance\n");
    printf("4. View ALL Wallets\n");
    printf("5. Make Payment (Transaction)\n" RESET);
    printf(YELLOW "+-----------------------------+\n" RESET);

    printf(BLUE"6. Mine Block\n");
    printf("7. View Blockchain\n");
    printf("8. List Students\n");
    printf("9. List Vendors\n");
    printf("10. View Transaction History\n");
    printf("11. View ALU Token Supply\n" RESET);
    printf(YELLOW "+-----------------------------+\n" RESET);
    printf(GREEN"12. Backup Blockchain & Wallets\n");
    printf("13. Load Blockchain & Wallets\n");
    printf("14. Verify Blockchain Integrity\n");
    printf("15. Exit\n" RESET);
    printf(YELLOW "Enter your choice: " RESET);
}


#ifndef TESTING
int main() {
    Blockchain *chain = initialize_blockchain();
    Wallet wallets[MAX_WALLETS];
    int wallet_count = 0;
    Student students[MAX_WALLETS];  // Array to store students
Vendor vendors[MAX_WALLETS];    // Array to store vendors
int student_count = 0, vendor_count = 0;  // Track count


    // Load existing blockchain and wallets
    load_blockchain(chain);
    // load_wallets(wallets, &wallet_count, chain);
    load_wallets(wallets, students, &student_count, vendors, &vendor_count, &wallet_count, chain);


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
                create_student(students, &student_count, wallets, &wallet_count, chain);
                backup_blockchain(chain, wallets, students, student_count, vendors, vendor_count, wallet_count);
                break;
            case 2:
                create_vendor(vendors, &vendor_count, wallets, &wallet_count, chain);
                backup_blockchain(chain, wallets, students, student_count, vendors, vendor_count, wallet_count);
                break;
            case 3:
                printf(YELLOW "Enter wallet ID: " RESET);
                char wallet_id[MAX_WALLET_ID];
                scanf("%s", wallet_id);
                view_balance(wallets, wallet_count, wallet_id);
                break;
            case 4:
                list_wallets(wallets, wallet_count);
                
                break;
            

            case 5:
                printf(YELLOW "Sender Wallet ID: " RESET);
                scanf("%s", sender_id);

                printf(YELLOW "Receiver Wallet ID: " RESET);
                scanf("%s", receiver_id);

                printf(YELLOW "Enter amount: " RESET);
                scanf("%lf", &amount);
                getchar();

                printf(YELLOW "Enter your private key: " RESET);
                scanf("%64s", private_key);

                if (add_transaction(chain, wallets, wallet_count, students, student_count, vendors, vendor_count, sender_id, receiver_id, amount, private_key)) {

                    // printf(GREEN "✅ Transaction confirmed and added!\n" RESET);
                    printf(GREEN "\n" RESET);
                } else {
                    printf(RED "❌ Transaction failed.\n" RESET);
                }
                break;

            case 6:
            create_block(chain, wallets, wallet_count, students, student_count, vendors, vendor_count);

                break;
            

            case 7:
                print_blockchain(chain);
                break;

            case 8:
                list_students(students, student_count);
                break;
            case 9:
                list_vendors(vendors, vendor_count);
                break;

            case 10:
                view_transaction_history(chain);
                break;

            case 11:
                view_token_supply(chain);
                break;

            case 12:
                // backup_blockchain(chain, wallets, wallet_count);
                backup_blockchain(chain, wallets, students, student_count, vendors, vendor_count, wallet_count);
                break;

            case 13:
                load_blockchain(chain);
                // load_wallets(wallets, &wallet_count, chain);
                load_wallets(wallets, students, &student_count, vendors, &vendor_count, &wallet_count, chain);

                break;

            case 14:
                // validate_chain(chain);
                validate_chain(chain);
                break;

            case 15:
            backup_blockchain(chain, wallets, students, student_count, vendors, vendor_count, wallet_count);
                free(chain);
                return 0;
        }
    }
}


#endif // Testing