PRINT "Transform a vector by a matrix."

LET m11 = 0
LET m12 = 0
LET m13 = 0
LET m14 = 0
LET m21 = 0
LET m22 = 0
LET m23 = 0
LET m24 = 0
LET m31 = 0
LET m32 = 0
LET m33 = 0
LET m34 = 0
LET m41 = 0
LET m42 = 0
LET m43 = 0
LET m44 = 0

PRINT "Enter 16 matrix values: "
GET m11 AS INT
GET m12 AS INT
GET m13 AS INT
GET m14 AS INT
GET m21 AS INT
GET m22 AS INT
GET m23 AS INT
GET m24 AS INT
GET m31 AS INT
GET m32 AS INT
GET m33 AS INT
GET m34 AS INT
GET m41 AS INT
GET m42 AS INT
GET m43 AS INT
GET m44 AS INT

LET v1 = 0
LET v2 = 0
LET v3 = 0
LET v4 = 0

PRINT "Enter 4 vector values: "
GET v1 AS INT
GET v2 AS INT
GET v3 AS INT
GET v4 AS INT

LET result1 = m11 * v1 + m12 * v2 + m13 * v3 + m14 * v4
LET result2 = m21 * v1 + m22 * v2 + m23 * v3 + m24 * v4
LET result3 = m31 * v1 + m32 * v2 + m33 * v3 + m34 * v4
LET result4 = m41 * v1 + m42 * v2 + m43 * v3 + m44 * v4

PRINT result1
PRINT result2
PRINT result3
PRINT result4
