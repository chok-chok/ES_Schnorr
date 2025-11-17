# ES_Schnorr
Implementation of the Schnorr Signature Scheme in C

The Schnorr signature scheme is a digital signature algorithm based on the discrete logarithm problem in cyclic groups, known for its simplicity, efficiency, and provable security.

To generate a key pair, two prime numbers, p and q, are first selected such that q is a divisor of p-1, i.e., q | (p - 1). Then, a number a is chosen such that a ≡ 1 (mod p). All these numbers can be freely published and used within a group of users.

To generate a specific key pair, a random number s, less than q, is selected. This number serves as the private key. The public key v is then calculated as v = a^(-s) (mod p).

Next, a one-way hash function H(M) is chosen.

1. Alice selects a random number r, less than q, and computes x = a^r (mod p). This stage constitutes preliminary computations.
2. Alice concatenates M and x, and hashes the result:
   e = H(M, x)
3. Alice computes y = (r + se) mod q. The values e and y constitute the signature, which she sends to Bob.
4. Bob computes x' = a^y * v^e (mod p). Then he verifies that the hash value for the concatenation of M and x' equals e:
   e = H(M, x')
5. If this holds true, he considers the signature valid.
