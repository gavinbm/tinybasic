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
GET INT m11
GET INT m12
GET INT m13
GET INT m14
GET INT m21
GET INT m22
GET INT m23
GET INT m24
GET INT m31
GET INT m32
GET INT m33
GET INT m34
GET INT m41
GET INT m42
GET INT m43
GET INT m44

LET v1 = 0
LET v2 = 0
LET v3 = 0
LET v4 = 0

PRINT "Enter 4 vector values: "
GET INT v1
GET INT v2
GET INT v3
GET INT v4

LET result1 = m11 * v1 + m12 * v2 + m13 * v3 + m14 * v4
LET result2 = m21 * v1 + m22 * v2 + m23 * v3 + m24 * v4
LET result3 = m31 * v1 + m32 * v2 + m33 * v3 + m34 * v4
LET result4 = m41 * v1 + m42 * v2 + m43 * v3 + m44 * v4

PRINT result1
PRINT result2
PRINT result3
PRINT result4
