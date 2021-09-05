LET minsofar = 0
LET maxsofar = 0
LET sum = 0

LET num = 0
PRINT "Enter number of inputs: "
GET INT num

LET i = 0
LET c = 0
WHILE i < num REPEAT
    GET INT c
    IF i == 0 THEN
        LET minsofar = c
        LET maxsofar = c
    ENDIF
    IF i != 0 THEN
        IF c < minsofar THEN
            LET minsofar = c
        ENDIF
        IF c > maxsofar THEN
            LET maxsofar = c
        ENDIF
    ENDIF
    LET sum = sum + c
    LET i = i + 1
ENDWHILE

PRINT "Min: "
PRINT minsofar
PRINT "Max: "
PRINT maxsofar
PRINT "Sum: "
PRINT sum
PRINT "Avg: "
PRINT sum / num
