
PRINT "How long is your name?"
GET INT len
LET count = 0

WHILE count < len REPEAT
    GET CHAR letter
    PRINT letter
    LET count = count + 1
ENDWHILE
