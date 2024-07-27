
Create README.md
This repository contains a walkthrough of the RSA Encryption Method. RSA is an acronym for the names of the individuals that first publicly revealed the encryption scheme - Ron Rivest, Adi Shamir and Leonard Adleman of the Massachusetts Institute of Technology, in 1977. 

As explained within the Jupyter Notebook, the overall effectiveness of this encryption scheme is based on the inability to factor large integers and the use of two different keys, a public key and a private key - in cryptology this is called Asymmetric Cryptography. 

Throughout the document, I will walk you through the steps of generating a public and private key as well as the process of encoding and decoding messages as well as the functions that make this possible. Additionally, I will walk you through how we can potentially break an encoded message using just a public key, but I will also explain how this is not always easily accomplished with today's computing power. 

I also provide herein an independent, working version of the RSA Encryption Scheme allowing user interaction and an additional feature that imports the contents of a text file containing multiple encrypted messages. I've additionally added here a function to determine the the primality of a provided number via the Rabin-Miller test with code borrowed from the Geeks for Geeks website.

This python file was created using Python version 3.10.8

Files Included:

RSA_Project.ipynb - Jupyter Notebook containing my code demonstration

RSA_Complete.py - Final version of my RSA encryption project
