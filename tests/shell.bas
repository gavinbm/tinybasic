LET a = 1

WHILE a > 0 REPEAT
    READ 64 FROM "stdin" INTO buffer
    IF buffer == "exit" THEN
        a = 0
    ENDIF
    IF buffer == "add" THEN
        PRINT "Enter two numbers"
        GET x AS INT
        GET y AS INT
        PRINT "Sum:"
        PRINT y + x
    ENDIF
    IF buffer == "sub" THEN
        PRINT "Enter two numbers"
        GET x AS INT
        GET y AS INT
        PRINT "Difference:"
        PRINT x - y
ENDWHILE