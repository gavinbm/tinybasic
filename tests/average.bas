LET a = 0
WHILE a < 1 REPEAT
    PRINT "Enter number of scores: "
    GET INT a
ENDWHILE

LET b = 0
LET s = 0
PRINT "Enter one value at a time: "
WHILE b < a REPEAT
    GET INT c
    LET s = s + c
    LET b = b + 1
ENDWHILE

PRINT "Average: "
PRINT s / a
