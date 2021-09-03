PRINT "How many fibonacci numbers do you want?"
INPUT nums
PRINT ""

a = 0
b = 1
c = a + b
WHILE nums > 0 REPEAT
    PRINT a
    c = a + b
    a = b
    b = c
    nums = nums - 1
ENDWHILE
