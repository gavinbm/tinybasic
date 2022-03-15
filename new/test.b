PRINT "How many fibonacci numbers do you want?"
INPUT c
PRINT ""

LET a = 0
LET b = 1
WHILE c > 0 DO
    PRINT a
    LET e = a + b
    LET a = b
    LET b = e
    LET c = c - 1
END
