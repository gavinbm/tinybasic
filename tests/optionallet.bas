PRINT "How many fibonacci numbers do you want?"
INPUT nums
PRINT ""

LET a = 0
LET b = 1
LET c = a + b
WHILE nums > 0 REPEAT
    PRINT a
    c = a + b
    a = b
    b = c
    nums = nums - 1
ENDWHILE
