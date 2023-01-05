### Zanficu Madalina 333CA

## Homework2 - APD - BONUS FUNCTIONALITY
In order to divide orders.txt as equal as possible, I computed
the total size of the file in bytes and divided it to P threads.
So each thread has a chunk of bytes between startIndex and 
endIndex to read from orders.txt.

The main problem is when the start index of a thread is not 
located at the start of a line. So start indexes 
will be adjusted in stage2, while the file is being read.

Each thread will execute .run() method, which firstly
reads the bytes from startIndex and endIndex. In case
startIndex is not at the beginning of the line, I move the start index
on the next line. Same situtation, in case the endIndex is not at the
end of a line, I move it at the end of the previous line.

Once the startIndex and endIndex have adjusted positions,
I place my cursor at the start Index, and start reading
the orders between startIndex and endIndex (startByte && endByte).