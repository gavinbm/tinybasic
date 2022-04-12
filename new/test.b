PRINT "How many fibonacci numbers do you want?"
INPUT v

LET a = 0
LET b = 1
WHILE v > 0 DO
    PRINT a
    LET c = a + b
    LET a = b
    LET b = c
    LET v = v - 1
END
