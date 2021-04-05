Shihab Siddique shs162
Nicholas Triano ndt26

To be compiled with strbuf.c and strbuf.h

We went to text generators to find decent sized sample texts and then went in and made random paragraph spacing and word spacing.
Then, we examined the output manually using a '|' character to tell where the width ends. We encountered numerous types of issues
during this proccess so we feel a good amount of confidence in our programs capapbilities.

We tested the following cases:
    - words that are bigger than the width
    - multiple new line characters in a row
    - reading from file, directory, or stdin
    - writing to file/stdout
    - wrapping a file that is already wrapped
    - words split between calls to read()
    - multiple spaces between words

Problems we encountered:
    - some lines overextending the width (we were not resetting the amount of line width we had left properly)
    - too many spaces between paragraphs (again not resetting width left properly)
    - at the end of the file our buffer would write more than it should have and we would get repetition of characters
        -> we were looping one too many times so the last loop would print stuff that was not overwritten from the previous buffer

