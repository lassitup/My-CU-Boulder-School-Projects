import random

def RSA_program():    
    
    ''' The below code related to the Miller-Rabin Primality Testing (lines 11 - 104) was sourced from the Geeks for Geeks Website
    Link: https://www.geeksforgeeks.org/primality-test-set-3-miller-rabin/ - This code was added in to provide user
    the ability to test for or auto generate a prime number. I've made a few changes to the original code to work appropriately with 
    my RSA program.
    '''
    
    # Python3 program Miller-Rabin primality test
    # This code is contributed by mits
    
    # Utility function to do
    # modular exponentiation.
    # It returns (x^y) % p
    def power(x, y, p):

        # Initialize result
        res = 1; 

        # Update x if it is more than or
        # equal to p
        x = x % p; 
        while (y > 0):

            # If y is odd, multiply
            # x with result
            if (y & 1):
                res = (res * x) % p;

            # y must be even now
            y = y>>1; # y = y/2
            x = (x * x) % p;

        return res;

    # This function is called
    # for all k trials. It returns
    # false if n is composite and 
    # returns false if n is
    # probably prime. d is an odd 
    # number such that d*2<sup>r</sup> = n-1
    # for some r >= 1
    def miillerTest(d, n):

        # Pick a random number in [2..n-2]
        # Corner cases make sure that n > 4
        a = 2 + random.randint(1, n - 4);

        # Compute a^d % n
        x = power(a, d, n);

        if (x == 1 or x == n - 1):
            return True;

        # Keep squaring x while one 
        # of the following doesn't 
        # happen
        # (i) d does not reach n-1
        # (ii) (x^2) % n is not 1
        # (iii) (x^2) % n is not n-1
        while (d != n - 1):
            x = (x * x) % n;
            d *= 2;

            if (x == 1):
                return False;
            if (x == n - 1):
                return True;

        # Return composite
        return False;

    # It returns false if n is 
    # composite and returns true if n
    # is probably prime. k is an 
    # input parameter that determines
    # accuracy level. Higher value of 
    # k indicates more accuracy.
    def isPrime(n):
        # original function from G4G allowed this as an argument
        #in the function, left as contstant for simplicity
        k = 4 
        
            
        # Corner cases
        if (n <= 1 or n == 4):
            return False;
        if (n <= 3):
            return True;

        # Find r such that n = 
        # 2^d * r + 1 for some r >= 1
        d = n - 1;
        while (d % 2 == 0):
            d //= 2;

        # Iterate given number of 'k' times
        for i in range(k):
            if (miillerTest(d, n) == False):
                return False;

        return True;



    
    #Code Below This point is my original code written for the course project
    def FME(b, n, m):
        """
        This function performs Professor Sriram's FME Algorithm 
        provided to the class in the course lecture videos

        Parameter b: This parameter is the integer value to be used as the base
        Parameter n: This parameter is the integer value to be used as the exponent
        Parameter m: This parameter is the integer value to be used as the modulo value

        """
        result = 1     #This variable holds the accumulated product of modulo values - changes only if we have binary of 1 in the while loop
        square = b     #This variable holds the current squared value with modulo applied - used to find next 'return' value
        n = n          # Holds the current value of n - floor division of 2 applied at the end of each cycle to reduce
        while (n > 0): # While loop will continue until n // 2 results in 0
            k = n % 2  # determine if current value of _int mod 2 is 0 or 1 in the binary exapnsion
            if k == 1: # Algorithm requires the squaring and modding of values to be applied if binary value is 1 in the expansion
                result = (result * square) % m  # Square and fronm previous cycle and finds the remainder
            square = (square * square) % m # move on to the next squared value, regardless if previously applied (prepare for next iteration)
            n = n // 2 # reduce n for next iteration for purposes of binary expansion

        return result # ending value in result contains the remainder of the overall expression



    def Euclidean_Alg(a, b):
        """
        This function performs the Euclidean Algorithm and returns the greatest
        common divisor (gcd) of two numbers.

        Parameter a: This parameter is an integer value
        Parameter b: This parameter is an integer value

        """
        # For the Euclid's Algorithm, the larger value needs to be first (in the 'a' position)
        # swap values of a and b if b is larger
        if b > a:
            temp = a
            a = b
            b = temp

        # continuosly take the mod b of a
        # b becomes 'new' and the remainder becomes new 'b'
        # continues until b is 0 and the remaining a is the Greatest Common Divisor (gcd)
        while b > 0:
            k = a % b
            a = b
            b = k

        return a #return the gcd



    def EEA(a, b):
        """
        This function performs Sriram's Extended Euclidean Algorithm 
        provided to the class in the course lecture videos

        Parameter a: This parameter is an integer value. Value must be greater than or equal to b.
        Parameter b: This parameter is an integer value. Value must be less than or equal to a and
                     must be greater than or equal to 0.
        """

        s1, t1 = 1, 0    # Bezout Coefficients for variable m - Set initial coefficient to 1 and 0 to represent the input
        s2, t2 = 0, 1    # Bezout Coefficients for variable n - Set initial coefficient to 0 and 1 to represent the input
        while( b > 0) :  #Loop continues until there is no longer a remainder performing Euclid's Algorithm (b of 0 indicates we've reached the gcd)
            k = a % b    
            q = a // b   # Determine quotient for use in calculation of the Bezout Coeff. for the new n.
            a = b        # Assign the smaller of the two integers in previous step to m for next step in Euclid's Alg.
            b = k        # Assign the remainder to the smaller of the two integers for the next step in Euclid's Alg.
            s1_hat, t1_hat = s2, t2   # assign Bezout coefficients of n into m (temporarily) avoids overwriting
            s2_hat, t2_hat = s1 - q * s2, t1 - q * t2  #Calculate the bezout coefficients for the new n / the remainder k
            s1, t1 = s1_hat, t1_hat  # reassigning bezout coeff. from temporary / loop variables for use in next cycle
            s2, t2 = s2_hat, t2_hat  # reassigning bezout coeff. from temporary / loop variables for use in next cycle


        return a, (s1, t1) #returns the gcd and a tuple for the necessary bezout coefficients to have the linear combination for the gcd  
    
    def Find_Public_Key_e(p, q):
        """
        This function will find a public key 'e' that is relatively prime with (p-1) * (q-1)

        Parameter p: This parameter is a prime number used in the calculation of the product (p-1) * (q-1)
        Parameter q: This parameter is a prime number used in the calculation of the product (p-1) * (q-1)
        """

        #Initialize the status that will, in part, control the loop below (e and (p-1) * (q-1) must be relatively prime)
        relative_prime_status = False

        #calculate the modulo value to be used within both private and public keys
        n = p * q

        assert n > 256, "Product of the two prime numbers entered is not greater than 256."


        # calculate modulo value for test of relative primality with e (property of Euler's Totient Function)
        m = (p - 1) * (q - 1) 

        #Continue the loop while e and n are not relatively prime
        #The loop will also continue if e and m are the same
        #both conditions must be false for loop to end
        while relative_prime_status == False or e == m:
            #obtain an random number to assign to e
            #limited to 31 for demonstration purposes
            e = random.randint(3, 31)

            # Call Euclid's Algorithm Function to determine GCD of e and m
            if e != m:
                gcd_result = Euclidean_Alg(m, e)
            else:
                #  continue to the next iteration as e and m can't be the same
                continue   
            # Test if the gcd of e and m is 1, if so they are relatively prime
            # update status so flow breaks out of loop
            if gcd_result == 1:
                 relative_prime_status = True

        return e, n


    def Find_Private_Key_d(e, p, q):
        """
        This function finds the modular inverse of the public key 
        'e' mod (p-1) * (q-1) or the private key 'd'

        Parameter p: This parameter is a prime number used in the calculation of the product (p-1) * (q-1)
        Parameter q: This parameter is a prime number used in the calculation of the product (p-1) * (q-1)
        """

        # calculate modulo value to be used to determine modular inverse of e in EEA
        m = (p - 1) * (q - 1) 


        # Determine which of the values e and m are larger
        # as the Extended Eucl. Alg requires the larger of the two to be the first argument passed
        if e > m:
            gcd, bezout_coefficients = EEA(e, m)
            d = bezout_coefficients[0] #extract the coeffiecient that represents the modular inverse

        # Based on the EA function used to determine e,
        # e cannot be the same - no additional check needed
        else:
            gcd, bezout_coefficients = EEA(m, e)
            d = bezout_coefficients[1] #extract the coeffiecient that represents the modular inverse

        while d < 0:
            d += m   #Add the Modulo value used in finding modular inverse until we get a positive value

        return d


    def Convert_Text(_string):
        """
        This function converts a string passed as an argument 
        to a list of it's associated ASCII decimal values.

        Parameter _string: This parameter is string of text.
        """
        integer_list = [] # List to hold the ASCII code values of each character passed
        for character in _string:
            integer_list.append(ord(character)) #convert the passed character to the ASCII decimal value
        
        return integer_list


    def Convert_Num(_list):
        """
        This function converts a list of ASCII decimal values passed as an argument 
        to a concatenated string of it's associated ASCII character values.

        Parameter _list This parameter is a list containing ASCII decimal values.
        """
        _string = '' # 'accumulate' the converted characters which will ultimately be returned as the original text
        for i in _list:
            _string += chr(i) #convert the passed ASCII decimal value to it's related character value and concatenate to string

        return _string


    def Encode(n, e, message):
        """
        This function converts an string message passed to an encrypted
        value using Fast Modular Exponentiation.

        Parameter n: This parameter is the modular value used as the message key.
        Parameter e: This parameter is the public key.
        Parameter message: This parameter is the string / text to be encrypted.
        """
        
        converted_message = Convert_Text(message) # convert the passed message argument to its ASCII decimal values

        cipher_text = []

        for code in converted_message:
            FME_result = FME(code, e, n) # encrypt each ASCII value via Fast Modular Exponentiation using public key 'e' and modulo value 'n'
            cipher_text.append(FME_result)

        return cipher_text


    def Decode(n, d, cipher_text):
        """
        This function converts an encoded message passed to a decrypted value
        using Fast Modular Exponentiation.

        Parameter n: This parameter is the modular value used as the message key.
        Parameter d: This parameter is the private key.
        Parameter cipher_text: this is the list of encrypted values to be decrypted.
        """

        decipher_list = []

        for code in cipher_text:
            FME_result = FME(code, d, n) # 'reverse' each encrypted value to it's related ASCII decimal code using private key 'd' and modulo 'n'
            decipher_list.append(FME_result)

        message = Convert_Num(decipher_list) # convert each ASCII decimal value to it's related character and get the original message

        return message

    
    def factorize(n):
        """
        This function runs through all numbers between 2 and (n-1) and looks
        for the first number that divides n.

        Parameter n: This parameter is the modular value used as the message key.
        """
            
        for num in range(2, n): #Loop through all integers between 2 and (n -1)
            if n % num == 0:    
                return num      # If a number is divisible by num, a factor has been identified
            
        return False
    
    
    def convert_string_list(user_message):
        """
        This function takes in a string value containing the list of values in the format
        [1, 2, 3, 4] and converts it to a python list object. 

        Parameter user_message: a list of of numeric values entered by user as a string.
        """
        # create list splitting it on ',' - clear divide between each number in string
        converted_message = user_message.split(",") 
        int_list = []
        for num in converted_message:
            num = num.strip() #remove extra whitespace/newline chacters
            # treat numbers with '[' and ']' different to extract just the number
            #otherwise convert the numnber to an integer and append to the list
            if '[' in num:
                int_list.append(int(num[1:]))
            elif ']' in num:
                int_list.append(int(num[:-1]))
            else:
                int_list.append(int(num))
                
        return int_list
                          
 
    def code_break_time(message, code_break_e, code_break_n):
            """
            This function uses the known value of 'e' and 'n' to brute force
            find the related value of 'd' or the private key and decrypt the 
            message passed to the function.

            Parameter message: The message to decrypt
            Parameter code_break_e: The public key
            Parameter code_break_n: The modulo value key value
            """
            
            prime_1 = factorize(code_break_n) # find the first prime number
            print("\nPrime Factor 1 used for 'n' is:", prime_1)
            prime_2 = code_break_n // prime_1 # find the second prime number
            print("\nPrime Factor 2 used for 'n' is:", prime_2)
            
            code_break_d = Find_Private_Key_d(code_break_e, prime_1, prime_2) # 'find the private key / modular inverse of e
            print("\nThe private key 'd' is:", code_break_d)
            
            deciphered_message = Decode(code_break_n, code_break_d, message) # read the message
            print("\nThe decrypted message is:", deciphered_message)  


    def decrypt_file():
        """
        This function opens a text file, reads the contents of 
        various code break messages from piazza, breaks each code
        and displays the results
        """
        
        with open('2824 Piazza Messages.txt', 'r') as ciphers:
            text_line = ciphers.readlines() # break out contents of text file with each line as a list
            n, e = 0, 0  
            cipher = []
            name = ""
            for line in text_line:
                if 'n, e' in line:
                    second_comma_pos = line.rindex(',') # find position of the right most ',' determine where n and e are located in string
                    n = int(line[line.index('=') + 2:second_comma_pos])  # n value begins 2 postions to the right of '=' and end before 2nd comma
                    e = int(line[second_comma_pos+2:])    # e value begins exactly two positions to the right of the right most comma, top the end of the 
                elif 'Cipher' in line: # identifies where the message line begins
                    message = line[9:]
                    cipher = convert_string_list(message)
                elif "Message" in line: # identifies where the name line begins
                    name = line[13:].strip() #postion of where name begins is consistent
                elif name != "":
                    print("\nThe message from " + name + " contains the following:")
                    code_break_time(cipher, e, n)
                    name = "" # reset name to empty string to reset the cycle
    
    def find_prime(begin, end):
        """
        This function takes in two values representing the range of numbers
        from which a random number will be generated. The Rabin Miller 
        Primality Test function is called to check if the number is prime.
        If it is not, it will continue to generate a random number and reperform
        the test.

        Parameter begin: start of random number range
        Parameter end: end of random number range
        """
        
        prime_status = False
        while not prime_status:
            rand_num = random.randint(begin, end)
            prime_status = isPrime(rand_num)
        
        return rand_num
    
    #Begin Menu Loop
    
    exit_variable = '' #set initial loop control variable to empty string, triggering while loop
    
    
    print("\nWelcome to my RSA Encryption / Decryption Program!\n")    
   
    while exit_variable != 'q':
        print("\n***************** RSA Project *****************")
        print("************* By Justin Lassiter **************")
        print("*********** CSPB 2824 Summer 2024 *************\n")
        print("Select From the Following Options:\n")
        print("(K) Key Generation\n"
              "(E) Encrypt\n"
              "(D) Decrypt\n"
              "(B) Break Code\n"
              "(R) Decipher Messages From a Text File\n"
              "(Q) Quit Program\n")
        
        
        user_selection = input("Please Select an Option: ")
        if user_selection.lower() == 'q':    
            exit_variable = 'q'
            continue
        
        elif user_selection.lower() == 'k':
            print("\nYou have selected 'Key Generation'\n")
            while True: 
                print("Would you like to select two prime numbers or automatically generate them?")
                user_selection = input("\nPlease choose (S) Self Select or (A) Auto generate: ")
                      
                if user_selection.lower() == "s":
                    while True:
                        try:  # implement try / except to ensure user enters an integer value
                            prime_1 = int(input("\nPlease enter a prime number: ")) 
                            prime_2 = int(input("\nPlease enter a second prime number: "))
                            is_prime1 = isPrime(prime_1)
                            is_prime2 = isPrime(prime_2)
                            
                            while prime_1 == prime_2 or not (is_prime1 and is_prime2):
                                if prime_1 == prime_2:
                                    print("\nThe two numbers cannot be the same. Please select new numbers.\n")
                                    prime_1 = int(input("Please enter a prime number: ")) 
                                    prime_2 = int(input("\nPlease enter a second prime number: "))
                                elif not(is_prime1 and is_prime2):
                                    print("\nYou have entered a non-prime (composite) number. Please select new numbers.\n")
                                    prime_1 = int(input("Please enter a prime number: ")) 
                                    prime_2 = int(input("\nPlease enter a second prime number: "))

                            e, n = Find_Public_Key_e(prime_1, prime_2)  # generate a public key
                            d = Find_Private_Key_d(e, prime_1, prime_2) # generate a private key
                            print("\nYour Public Key ('e') is:", e)
                            print("\nYour Public Key ('n') is:", n)
                            print("\nYour Private Key ('d') is:", d, '\n')
                            break # exit the loop
                        except: # if user enters an invalid value causing an exception, loop will continuously prompt until satisfied.
                            print("\nYou've entered an invalid value - please enter an integer value.")
                    break # exit the main loop
                    
                elif user_selection.lower() == "a":
                    prime_1 = 0
                    prime_2 = 0                    
                    while prime_1 == prime_2:
                        prime_1 = find_prime(100, 200)
                        prime_2 = find_prime(100, 200)
                    e, n = Find_Public_Key_e(prime_1, prime_2)  # generate a public key
                    d = Find_Private_Key_d(e, prime_1, prime_2) # generate a private key
                    print("\nYour first prime number is:", prime_1)
                    print("\nYour second prime number is:", prime_2)
                    print("\nYour Public Key ('e') is:", e)
                    print("\nYour Public Key ('n') is:", n)
                    print("\nYour Private Key ('d') is:", d, '\n')                      
                    break # exit the main loop
                else:
                      print("\nPlease select a valid choice\n")
                      
                      
                       
                    
        elif user_selection.lower() == 'e':
            print("\nYou have selected 'Encrypt'\n")
            user_message = input("Please enter the message you would like to encode: ")
            
            while True:
                try: # implement try / except to ensure user enters an integer value
                    user_public_key_e = int(input("\nPlease enter the public key ('e'): "))
                    user_public_key_n = int(input("\nPlease enter the public key ('n'): "))
                    encoded_message = Encode(user_public_key_n, user_public_key_e, user_message) # generate ciphered message
                    print("\nYour encrypted messaged is:", encoded_message, "\n")
                    break
                except: # if user enters an invalid value causing an exception, loop will continuously prompt until satisfied.
                    print("\nYou've entered an invalid value - please enter an integer value.")
        
        elif user_selection.lower() == 'd':
            print("\nYou have selected 'Decrypt'\n")
            
            while True:
                try: # implement try / except to ensure user enters an integer value
                    user_message = input("Please enter the message you would like to decode: ")
                    converted_message_list = convert_string_list(user_message) #convert the string numeric message to a list of integers
                    break # exit the loop
                except: # if user enters an invalid value causing an exception, loop will continuously prompt until satisfied.
                    print("\nPlease enter a valid input. Input must be in the following list form: '[1, 2, 3, 4]' containing only integer values.\n")
                    
            while True:
                try: # implement try / except to ensure user enters an integer value
                    user_private_key_d = int(input("\nPlease enter your private key ('d'): "))
                    user_public_key_n = int(input("\nPlease enter the public key ('n'): "))
                    decoded_message = Decode(user_public_key_n, user_private_key_d, converted_message_list) # generate decrypted message
                    print("\nYour decrypted messaged is:", decoded_message, '\n')
                    break # exit the loop
                except: # if user enters an invalid value causing an exception, loop will continuously prompt until satisfied.
                    print("\nYou've entered an invalid value - please enter an integer value.")
            
        elif user_selection.lower() == 'b':
            print("\nYou have selected 'Break Code'\n")
            
            while True:
                    try: # implement try / except to ensure user enters an integer value
                        cipher = input("Please enter the message you would like to decode: ")
                        converted_cipher_list = convert_string_list(cipher) #convert the string numeric message to a list of integers
                        break # exit the loop
                    except: # if user enters an invalid value causing an exception, loop will continuously prompt until satisfied.
                        print("\nPlease enter a valid input. Input must be in the following list form: '[1, 2, 3, 4]' containing only integer values.\n")
            while True:
                try: # implement try / except to ensure user enters an integer value
                    code_break_e = int(input("\nPlease enter the Public Key 'e' of the code you are trying to break: "))
                    code_break_n = int(input("\nPlease enter the Public Key 'n' of the code you are trying to break: "))

                    prime_1 = factorize(code_break_n)
                    print("\nPrime Factor 1 used for 'n' is:", prime_1)
                    prime_2 = code_break_n // prime_1
                    print("\nPrime Factor 2 used for 'n' is:", prime_2)

                    code_break_d = Find_Private_Key_d(code_break_e, prime_1, prime_2)
                    print("\nThe private key 'd' is:", code_break_d)

                    deciphered_message = Decode(code_break_n, code_break_d, converted_cipher_list)
                    print("\nThe decrypted message is:", deciphered_message, '\n')
                    break # exit the loop
                except: # if user enters an invalid value causing an exception, loop will continuously prompt until satisfied.
                    print("\nYou've entered an invalid value - please enter an integer value.")
            
        elif user_selection.lower() == 'r':
            print("\nYou have selected 'Decipher Messages From a Text File'")
            decrypt_file()
        
        else: # If user did not select one of the menu items, loop will continuously prompt until satisfied.
            print("\nThe input entered is not recognized!\n\n"
                  "Please select a valid input from the menu!\n")
              
        
    print("\nThank you for using my program!\n")       
                

def main():
    RSA_program()
        
if __name__ == "__main__":
    main()