LET a = 1

WHILE a > 0 REPEAT
    READ 64 FROM "stdin" INTO buffer
    IF buffer == "exit" THEN
        BREAK
    ENDIF
    PRINT buffer
ENDWHILE