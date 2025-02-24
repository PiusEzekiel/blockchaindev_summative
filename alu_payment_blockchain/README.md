# **ALU Payment Blockchain**  
**A Secure, Decentralized Payment System for ALU Transactions**

---

## **📌 Project Overview**  
The **ALU Payment Blockchain** is a **custom-built blockchain system** designed for secure, transparent, and decentralized transactions within **African Leadership University (ALU)**. It enables **students and vendors** to securely transfer **ALU Tokens**, mine blocks, and verify blockchain integrity using cryptographic security.

---

## **🎯 Key Features**  

✔️ **Decentralized ALU Token Economy** (1,000,000 ALU Tokens in supply)  
✔️ **Secure Wallet Creation for Students & Vendors**  
✔️ **Cryptographically Signed Transactions** with RSA Encryption  
✔️ **Proof-of-Work (PoW) Mining Mechanism** for Block Validation  
✔️ **Automatic Blockchain & Wallet Backup & Recovery**  
✔️ **Adaptive Mining Difficulty Adjustment for Efficiency**  
✔️ **Tamper-Proof Blockchain Validation & Integrity Checks**  
✔️ **Comprehensive Command-Line Interface (CLI) for Interaction**  

---

## **🚀 Getting Started**

### **🔹 Prerequisites**  
Ensure you have the following installed before proceeding:  
- **GCC (GNU Compiler)**  
- **OpenSSL**  
- **Make (if using a Makefile for compilation)**  

---

### **🔹 Installation & Running the Program**

#### **🛠️ Compile the Blockchain System**
```bash
gcc -o alu_payment_blockchain.out alu_payment_blockchain.c -lcrypto
```

#### **🚀 Run the Blockchain**
```bash
./alu_payment_blockchain.out
```

---

## **📌 How to Use**  

The **ALU Payment Blockchain** operates via a **Command-Line Interface (CLI)**, allowing users to interact with the system efficiently.

### **🔹 CLI Menu Options & Their Functions**  

| Option | Functionality |
|--------|--------------|
| **1. Create Student Wallet** | Registers a student with a secure wallet & ALU Tokens. |
| **2. Create Vendor Wallet** | Registers a vendor with a secure wallet for receiving payments. |
| **3. View Wallet Balance** | Displays the balance of a specified wallet. |
| **4. List All Wallets** | Shows all registered wallets and their balances. |
| **5. Make Payment (Transaction)** | Transfers ALU Tokens from one wallet to another. |
| **6. Mine a Block** | Performs Proof-of-Work mining, validates transactions, and rewards miners. |
| **7. View Blockchain** | Displays the full blockchain with transaction history. |
| **8. List All Students** | Shows all registered students along with their wallet balances. |
| **9. List All Vendors** | Shows all registered vendors and their wallet balances. |
| **10. View Transaction History** | Displays past transactions stored on the blockchain. |
| **11. View ALU Token Supply** | Shows details about token supply and circulating ALU Tokens. |
| **12. Backup Blockchain & Wallets** | Saves the blockchain and wallets to a file for recovery. |
| **13. Load Blockchain & Wallets** | Restores blockchain and wallets from a saved backup. |
| **14. Verify Blockchain Integrity** | Ensures the blockchain has not been tampered with. |
| **15. Exit** | Exits the program safely. |

---

## **🔐 Security Features**
The ALU Payment Blockchain includes **multiple security layers** to ensure safe transactions and prevent fraud.

- **🔹 SHA-256 Encryption** for hashing transactions.  
- **🔹 RSA Key Pairs** for secure authentication & digital signatures.  
- **🔹 Private Key Hashing** to prevent unauthorized wallet access.  
- **🔹 Auto-Backup of Blockchain & Wallets** to prevent data loss.  
- **🔹 Anti-Fraud Mechanism** to prevent duplicate IDs & transactions.  

---

## **✅ Unit Tests**
To ensure reliability, unit tests are implemented using the **Unity Testing Framework**.  
The test suite checks critical functions such as **wallet creation, transactions, mining, and blockchain verification**.

### **🔹 Running Unit Tests**
```bash
gcc -DTESTING test_blockchain.c alu_payment_blockchain.c unity.c -o test_blockchain.out -lcrypto
./test_blockchain.out
```

### **🔹 Test Cases & Expected Outcomes**
| Test Case | Expected Outcome |
|-----------|-----------------|
| **Create Wallet** | Wallet is successfully created with a unique ID. |
| **Prevent Duplicate Wallets** | Duplicate wallet IDs are not allowed. |
| **Create Student & Vendor** | Students & vendors are correctly registered. |
| **Add Transaction** | ALU Tokens are transferred between wallets. |
| **Prevent Insufficient Funds Transactions** | Transactions fail if the sender has low balance. |
| **Mine Block & Verify Blockchain** | Blockchain remains valid after mining new blocks. |

---

## **💡 Future Improvements**


🔹 **Web-Based Dashboard for Blockchain Interaction**  
🔹 **Mobile App for Wallet Management & Payments**  
🔹 **DeFi Features (Lending & Staking ALU Tokens)**  
🔹 **Smart Contract Implementation for Automating Transactions**  


