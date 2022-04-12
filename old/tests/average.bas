LET a = 0
WHILE a < 1 REPEAT
    PRINT "Enter number of scores: "
    GET a AS INT
ENDWHILE

LET b = 0
LET s = 0
PRINT "Enter one value at a time: "
WHILE b < a REPEAT
    GET c AS INT
    LET s = s + c
    LET b = b + 1
ENDWHILE

PRINT "Average: "
PRINT s / a
