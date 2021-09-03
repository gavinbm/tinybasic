LET a = 0
WHILE a < 1 REPEAT
    PRINT "Enter number of scores: "
    INPUT a
ENDWHILE

b = 0
s = 0
PRINT "Enter one value at a time: "
WHILE b < a REPEAT
    INPUT c
    s = s + c
    b = b + 1
ENDWHILE

PRINT "Average: "
PRINT s / a
