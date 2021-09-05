PRINT "How many fibonacci numbers do you want?"
GET INT nums
PRINT ""

LET a = 0
LET b = 1
WHILE nums > 0 REPEAT
    PRINT a
    LET c = a + b
    LET a = b
    LET b = c
    LET nums = nums - 1
ENDWHILE
