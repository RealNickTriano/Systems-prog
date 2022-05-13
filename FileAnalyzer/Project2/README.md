ndt26

shs162

To be compliled using filecompare.c fileQueue.h linkedlist.h strbuf.h dirQueue.h 
    as well as -pthread and -lm flags like so

    gcc -pthread fileCompare.c fileQueue.h linkedlist.h strbuf.h dirQueue.h -lm

We checked scenarios that related to thread saftey such as threads finishing too early and
threads overwriting data. We also tested numeruous text files with both varied and similar text inside
in order to make sure the JSD calculations were accurate. While we did not test for exact numbers we 
ensured that basic calculations worked as if the files were exactly the same or entirely different. 
Also whether the JSD value was closer to one or zero. We tested directory entries to make sure 
our directory traversal was successful, and made sure our threads finished when there were no 
more directory entries left. Our file threads checked whether the directory threads were still
runnning before joining those as well. While we were unable to solve all problems of our program
we believe this is the best we could have done. There are still some issues and you may need to run it
a couple times to get the desired output.