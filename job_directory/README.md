# **📌 Job Directory Blockchain**

## **🎯 Project Overview**
The **Job Directory Blockchain** is a secure **blockchain-based job listing system** that ensures **immutable job postings**, **tamper-proof records**, and **verifiable job history** using cryptographic hashing and Proof-of-Work (PoW) mining.

---

## **🚀 Key Features**
✔️ **Decentralized Job Listings on Blockchain**  
✔️ **Tamper-Proof Job Records with SHA-256 Hashing**  
✔️ **Search Jobs by Title, Company, or Location**  
✔️ **Proof-of-Work Mining for Secure Entry Validation**  
✔️ **Blockchain Integrity Verification to Detect Tampering**  
✔️ **Persistent Data Storage with File Backup**  
✔️ **User-Friendly CLI for Job Management**  

---

## **🔹 Installation**

### **1️⃣ Prerequisites**
Ensure you have the following installed:
- **GCC (GNU Compiler Collection)**
- **OpenSSL** (for cryptographic hashing)
- **Valgrind** (for memory leak checks) *(optional)*

---

### **2️⃣ Compilation**
To compile the program, run:
```sh
gcc -o job_directory.out job_directory.c -lcrypto
```

---

### **3️⃣ Running the Program**
To execute the job directory blockchain system:
```sh
./job_directory.out
```

---

## **📜 How to Use (CLI Menu Functions)**

### **1️⃣ Add a Job Listing**
- Enter **Job Title, Company, Location, and Description**
- Job is **stored in the blockchain** and **mined using Proof-of-Work**
- Blockchain is **updated securely**

### **2️⃣ View All Job Listings**
- Displays **all jobs recorded in the blockchain**
- Shows **block index, job details, and cryptographic hash**

### **3️⃣ Search for a Job**
- Find job listings by **Title, Company, or Location**

### **4️⃣ Verify Blockchain Integrity**
- Ensures **job records are untampered**
- Detects if **any block has been altered**

### **5️⃣ Backup & Load Blockchain Data**
- Saves blockchain state for **persistence across sessions**
- Loads previously saved **job data from a file**

---

## **🔒 Security Features**
✅ **SHA-256 Cryptographic Hashing** for job record protection  
✅ **Blockchain Linkage (Previous Block Hash)** ensures tamper detection  
✅ **Proof-of-Work Mining (Nonce Calculation)** prevents spamming of job entries  
✅ **Integrity Verification Mechanism** detects data modifications  

---

## **📂 Project Files**
📌 **job_directory.c** – Main source code  
📌 **job_directory.h** – Header file  
📌 **README.md** – This document  
📌 **output.txt** – Example output file  
📌 **job_directory.out** – Compiled executable file  

