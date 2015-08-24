# taiji
Small cryptographic program.

What is taiji?
  -A small commandline tool for adding a bit of extra security to encrypted files.
  
What is taiji not?
  -An all encompassing and perfect encryption program.
  
Taiji is a program that splits one file into two by dividing half of each byte into each and can then recombine them later.
It can add an extra layer of certainty to your encrypted files by allowing you to store sensitive information in multiple
physical locations. If someone steals a device with only half of the information of a file, they will have a harder time 
decrypting it even if they find your passwords or keys.

Please note:
  -This does no encryption itself so another method should be used first.
  -Encryption done beforehand should be in chain coding to remove patterns as this program does not obsfucate them itself.
  -Keep backups! If half of the information is stolen and you can't get it back then you will have just as much trouble 
    recovering the the information as the theif!

To Developers:
  -The SynthStream class is the real meat of the process, main just makes it usable. The class can be extracted and given a
    much prettier interface or implemented into larger projects.
    
