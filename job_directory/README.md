# Job Directory Blockchain

## Overview
This project implements a blockchain-based job directory that ensures job listings are securely stored, immutable, and verifiable. The blockchain structure uses cryptographic hashing and proof-of-work (PoW) mining to maintain data integrity and prevent unauthorized modifications.

## Features
- **Add Job Listings:** Stores job details as blockchain blocks.
- **Search Jobs:** Allows searching for job listings by title, company, or location.
- **Blockchain Integrity Verification:** Ensures data has not been tampered with.
- **Proof-of-Work Mining:** Uses a hashing mechanism to mine jobs securely.
- **Data Persistence:** Saves and loads blockchain data from a file.

---

## Compilation and Execution
### **Compilation**
To compile the program, run:
```sh
gcc job_directory.c -o job_directory.out -lcrypto
```

### **Execution**
To run the program:
```sh
./job_directory
```

---

## Blockchain Principles Used
- **Immutable Job Entries:** Each job listing is stored as a block with a cryptographic hash linking to the previous job.
- **SHA-256 Hashing:** Ensures data integrity by generating unique block hashes.
- **Proof-of-Work (Mining):** Each job listing requires computational work (nonce calculation) to validate its entry.
- **Integrity Checks:** Prevents data tampering by verifying hashes in the blockchain.

---

## Hashing Mechanism
Each block's hash is generated using **SHA-256** by combining:
- **Index**
- **Job Title**
- **Company**
- **Location**
- **Description**
- **Timestamp**
- **Previous Hash**
- **Nonce** (computed for mining)

Hashing ensures that even a minor modification in any field will change the hash, making tampering easily detectable.

---

## Example Output
### **Adding a Job Listing**
```plaintext
✅ Job added successfully and mined as Block 1!
```

### **Searching for a Job**
```plaintext
📜 Job Listing (Block 0)
📍 Frontend Developer at ALU (Kigali)
📝 Description: Seeking a frontend developer for our team.
🔗 Hash: 0000abcd1234...
```

### **Verifying Blockchain Integrity**
```plaintext
✅ Blockchain integrity verified. No issues detected.
```

### **Tampering Detection**
```plaintext
⚠️ Blockchain broken between Blocks 2 and 1!
```

---

## Documentation
For a detailed explanation of the implementation, cryptographic security, and challenges faced, refer to `documentation.pdf`.

---

## Code Quality Checks
### **Memory Leak Detection**
Run the following command to check for memory leaks:
```sh
valgrind --leak-check=full ./job_directory
```

### **Best Practices Followed**
- Modular code structure
- Consistent function naming
- Proper memory allocation and deallocation
- Commented and well-documented functions

---

## Submission Contents
Ensure the following files are included:
- `job_directory.c` (Main source code)
- `blockchain_utils.c`, `input_utils.c`, `search_utils.c` (Modular components)
- `job_directory.h` (Header file)
- `README.md` (This document)
- `output.txt` (Example output file)
- `documentation.pdf` (Detailed explanation of blockchain usage)
- `job_directory.out` (Compiled executable file)

---

## Conclusion
This job directory blockchain provides a **secure**, **tamper-proof**, and **verifiable** way to store job listings. It applies blockchain principles such as **cryptographic hashing**, **proof-of-work mining**, and **data integrity checks** to ensure the reliability of job listings.

