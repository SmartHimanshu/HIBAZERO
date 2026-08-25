.code32

.file "int.S"

.section .text


/*
You wanna go fast? let's go fast!
I am gonna use regparm to pass the struct in edx and int no. in al for speed.
*/
bios_interrupt:
    // We will have to be careful and mindful about the way by which we will fill registers.
    // First off, we know that we will have to load ds es eax and flags with utmost
    // precision and care. 

