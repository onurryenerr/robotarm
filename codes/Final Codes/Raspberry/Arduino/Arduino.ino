#include <Servo.h>
#include <avr/pgmspace.h>

//byte counter = 0;
byte led = 0;
const byte velocityCoeff = 3; // Enter speed between 1 (fastest)-10(slowest)
//const byte slowingPos = 5; // Decrease speed at specified position (slowingPos) before desired angle
//const byte decreaseCoef = 2; // how many times to decrease speed at specified position
byte slowedFlag[2] = {0};
Servo myservo[3];  // create servo object to control a servo
int16_t pos[2] =  { 0 };    // variable to store the servo position
byte finishFlag[2] =  {2,2}; // flag to save last command execution status. 0 - busy, 1 - done, 2 - ready
int16_t duration[2]; // variable to store delays between position
unsigned long curtime[2]= {0,0}; // variable to store last command execution time
unsigned long j=0;
//const int16_t positions2 [10000] PROGMEM = {251,251,1084,740,252,252,1094,750,1094,754,1099,764,1099,769,1099,774,1104,779,1104,783,1108,788,1108,793,1108,798,1113,803,1113,808,1118,808,1118,813,1118,817,1118,822,1123,827,1123,832,1128,837,1128,842,1128,847,1133,847,1133,851,1133,856,1133,861,1138,861,1138,866,1138,871,1142,876,1142,880,1142,885,1147,890,1147,895,1147,900,1152,905,1152,910,1152,914,1157,919,1157,924,1157,929,1157,934,1162,934,1162,939,1162,944,1162,948,1167,948,1167,953,1167,958,1167,963,1171,968,1171,973,1171,977,1171,982,1176,982,1176,987,1176,992,1176,997,1181,1002,1181,1007,1181,1011,1181,1016,1186,1021,1186,1026,1186,1031,1186,1036,1186,1041,1191,1041,1191,1045,1191,1050,1191,1055,1191,1060,1196,1060,1196,1065,1196,1070,1196,1074,1196,1079,1196,1084,1201,1089,1201,1094,1201,1099,1201,1104,1201,1108,1201,1113,1205,1118,1205,1123,1205,1128,1205,1133,1205,1138,1205,1142,1205,1147,1205,1152,1210,1157,1210,1162,1210,1167,1210,1171,1210,1176,1210,1181,1210,1186,1210,1191,1210,1196,1210,1201,1210,1205,1210,1210,1210,1215,1210,1220,1210,1225,1210,1230,1210,1235,1210,1239,1210,1244,1210,1249,1210,1254,1210,1259,1205,1264,1205,1268,1205,1273,1205,1278,1205,1283,1205,1288,1205,1293,1201,1293,1201,1298,1201,1302,1201,1307,1201,1312,1196,1312,1196,1317,1196,1322,1196,1327,1191,1327,1191,1332,1191,1336,1186,1336,1186,1341,1186,1346,1181,1351,1181,1356,1176,1356,1176,1361,1176,1365,1171,1365,1171,1370,1171,1375,1167,1375,1167,1380,1162,1380,1162,1385,1157,1385,1157,1390,1152,1390,1152,1395,1147,1395,1147,1399,1142,1399,1142,1404,1138,1404,1138,1409,1142,1409,1142,1414,1142,1419,1147,1419,1147,1424,1152,1429,1152,1433,1157,1433,1157,1438,1162,1443,1162,1448,1167,1448,1167,1453,1167,1458,1171,1458,1171,1462,1176,1467,1176,1472,1181,1472,1181,1477,1181,1482,1186,1482,1186,1487,1191,1487,1191,1492,1191,1496,1196,1496,1196,1501,1201,1506,1201,1511,1205,1511,1205,1516,1210,1521,1210,1526,1215,1526,1215,1530,1215,1535,1220,1535,1220,1540,1225,1540,1225,1545,1225,1550,1230,1550,1230,1555,1235,1555,1235,1559,1235,1564,1239,1564,1239,1569,1244,1569,1244,1574,1244,1579,1249,1579,1249,1584,1254,1584,1254,1589,1254,1593,1259,1593,1259,1598,1264,1598,1264,1603,1268,1608,1268,1613,1273,1613,1273,1618,1278,1623,1278,1627,1283,1627,1283,1632,1288,1637,1288,1642,1293,1642,1293,1647,1298,1652,1298,1656,1302,1656,1302,1661,1307,1661,1307,1666,1312,1671,1312,1676,1317,1676,1317,1681,1322,1681,1322,1686,1322,1690,1327,1690,1327,1695,1332,1695,1332,1700,1336,1705,1336,1710,1341,1710,1341,1715,1346,1715,1346,1720,1351,1724,1356,1729,1356,1734,1361,1734,1361,1739,1365,1739,1365,1744,1370,1744,1370,1749,1370,1753,1375,1753,1375,1758,1380,1758,1380,1763,1385,1763,1385,1768,1390,1768,1390,1773,1395,1778,1395,1783,1399,1783,1399,1787,1404,1787,1404,1792,1409,1792,1409,1797,1414,1797,1414,1802,1419,1802,1419,1807,1424,1812,1424,1817,1429,1817,1429,1821,1433,1821,1433,1826,1438,1826,1438,1831,1443,1831,1443,1836,1448,1836,1448,1841,1453,1841,1453,1846,1458,1846,1458,1850,1462,1850,1462,1855,1467,1855,1467,1860,1472,1865,1477,1865,1477,1870,1482,1870,1482,1875,1487,1875,1487,1880,1492,1880,1492,1884,1496,1884,1496,1889,1501,1889,1501,1894,1506,1894,1506,1899,1511,1899,1511,1904,1516,1904,1516,1909,1521,1909,1521,1914,1526,1914,1526,1918,1530,1918,1535,1923,1540,1928,1545,1928,1545,1933,1550,1933,1550,1938,1555,1938,1555,1943,1559,1943,1559,1947,1564,1947,1569,1947,1569,1952,1574,1952,1574,1957,1579,1957,1579,1962,1584,1962,1589,1967,1593,1967,1593,1972,1598,1972,1603,1977,1608,1977,1608,1981,1613,1981,1613,1986,1618,1986,1623,1986,1623,1991,1627,1991,1627,1996,1632,1996,1637,1996,1637,2001,1642,2001,1647,2006,1652,2006,1652,2011,1656,2011,1661,2011,1661,2015,1666,2015,1671,2020,1676,2020,1676,2025,1681,2025,1686,2025,1686,2030,1690,2030,1695,2030,1695,2035,1700,2035,1705,2035,1705,2040,1710,2040,1715,2040,1715,2044,1720,2044,1724,2044,1724,2049,1729,2049,1734,2049,1734,2054,1739,2054,1744,2054,1749,2059,1753,2059,1758,2059,1758,2064,1763,2064,1768,2064,1768,2069,1773,2069,1778,2069,1783,2069,1783,2074,1787,2074,1792,2074,1797,2078,1802,2078,1807,2078,1807,2083,1812,2083,1817,2083,1821,2083,1821,2088,1826,2088,1831,2088,1836,2088,1841,2093,1846,2093,1850,2093,1855,2098,1860,2098,1865,2098,1870,2098,1870,2103,1875,2103,1880,2103,1884,2103,1889,2108,1894,2108,1899,2108,1904,2108,1909,2108,1909,2112,1914,2112,1918,2112,1923,2112,1928,2112,1933,2117,1938,2117,1943,2117,1947,2117,1952,2117,1957,2122,1962,2122,1967,2122,1972,2122,1977,2122,1981,2122,1981,2127,1986,2127,1991,2127,1996,2127,2001,2127,2006,2127,2011,2127,2015,2127,2015,2132,2020,2132,2025,2132,2030,2132,2035,2132,2040,2132,2044,2132,2049,2132,2054,2132,2054,2137,2059,2137,2064,2137,2069,2137,2074,2137,2078,2137,2083,2137,2088,2137,2093,2137,2098,2137,2103,2137,2108,2137,2112,2137,2112,2141,2117,2141,2122,2141,2127,2141,2132,2141,2137,2141,2141,2141,2146,2141,2151,2141,2156,2141,2161,2141,2166,2141,2171,2141,2175,2141,2180,2141,2180,2137,2180,2132,2180,2127,2180,2122,2175,2122,2175,2117,2175,2112,2175,2108,2171,2103,2171,2098,2171,2093,2171,2088,2166,2088,2166,2083,2166,2078,2166,2074,2161,2074,2161,2069,2161,2064,2161,2059,2156,2059,2156,2054,2156,2049,2156,2044,2151,2044,2151,2040,2151,2035,2151,2030,2146,2030,2146,2025,2146,2020,2141,2015,2141,2011,2141,2006,2137,2006,2137,2001,2137,1996,2137,1991,2132,1991,2132,1986,2132,1981,2127,1981,2127,1977,2127,1972,2122,1967,2122,1962,2122,1957,2117,1957,2117,1952,2117,1947,2112,1943,2112,1938,2112,1933,2108,1933,2108,1928,2108,1923,2103,1923,2103,1918,2103,1914,2098,1909,2098,1904,2098,1899,2093,1899,2093,1894,2093,1889,2088,1889,2088,1884,2088,1880,2083,1875,2083,1870,2083,1865,2078,1865,2078,1860,2078,1855,2074,1855,2074,1850,2074,1846,2069,1846,2069,1841,2069,1836,2064,1831,2064,1826,2064,1821,2059,1821,2059,1817,2059,1812,2054,1812,2054,1807,2054,1802,2049,1802,2049,1797,2049,1792,2044,1792,2044,1787,2044,1783,2040,1778,2040,1773,2040,1768,2035,1768,2035,1763,2035,1758,2030,1758,2030,1753,2030,1749,2025,1749,2025,1744,2025,1739,2020,1739,2020,1734,2020,1729,2015,1729,2015,1724,2015,1720,2011,1715,2011,1710,2011,1705,2006,1705,2006,1700,2006,1695,2001,1695,2001,1690,2001,1686,1996,1686,1996,1681,1996,1676,1991,1676,1991,1671,1991,1666,1986,1666,1986,1661,1986,1656,1981,1652,1981,1647,1977,1642,1977,1637,1972,1632,1972,1627,1972,1623,1967,1623,1967,1618,1967,1613,1962,1613,1962,1608,1962,1603,1957,1603,1957,1598,1957,1593,1952,1593,1952,1589,1952,1584,1947,1584,1947,1579,1947,1574,1943,1569,1943,1564,1938,1564,1938,1559,1938,1555,1933,1550,1933,1545,1933,1540,1928,1540,1928,1535,1923,1530,1923,1526,1918,1526,1914,1526,1909,1526,1904,1526,1899,1526,1894,1526,1889,1521,1884,1521,1880,1521,1875,1521,1870,1521,1865,1521,1860,1521,1860,1516,1855,1516,1850,1516,1846,1516,1841,1516,1841,1511,1836,1511,1831,1511,1826,1511,1821,1511,1821,1506,1817,1506,1812,1506,1807,1506,1807,1501,1802,1501,1797,1501,1792,1501,1792,1496,1787,1496,1783,1496,1778,1492,1773,1492,1768,1492,1768,1487,1763,1487,1758,1487,1753,1482,1749,1482,1744,1477,1739,1477,1734,1477,1734,1472,1729,1472,1724,1472,1724,1467,1720,1467,1715,1467,1715,1462,1710,1462,1705,1458,1700,1458,1695,1453,1690,1453,1690,1448,1686,1448,1681,1448,1681,1443,1676,1443,1671,1438,1666,1438,1666,1433,1661,1433,1656,1433,1656,1429,1652,1429,1652,1424,1647,1424,1642,1424,1642,1419,1637,1419,1637,1414,1632,1414,1627,1414,1627,1409,1623,1409,1623,1404,1618,1404,1618,1399,1613,1399,1608,1399,1608,1395,1603,1395,1603,1390,1598,1390,1593,1385,1589,1385,1589,1380,1584,1380,1584,1375,1579,1375,1579,1370,1574,1370,1569,1365,1564,1365,1564,1361,1559,1361,1559,1356,1555,1356,1555,1351,1550,1351,1550,1346,1545,1346,1540,1341,1535,1341,1535,1336,1530,1336,1530,1332,1526,1332,1526,1327,1521,1327,1521,1322,1516,1322,1516,1317,1511,1317,1511,1312,1506,1312,1506,1307,1501,1307,1501,1302,1496,1302,1496,1298,1492,1298,1492,1293,1487,1293,1487,1288,1482,1288,1477,1283,1477,1278,1472,1278,1472,1273,1467,1273,1467,1268,1462,1268,1462,1264,1458,1264,1458,1259,1453,1259,1453,1254,1448,1254,1448,1249,1443,1249,1443,1244,1438,1244,1438,1239,1433,1239,1433,1235,1429,1235,1429,1230,1424,1230,1424,1225,1419,1220,1414,1215,1409,1210,1409,1205,1404,1205,1404,1201,1399,1201,1399,1196,1395,1196,1395,1191,1390,1191,1390,1186,1385,1181,1380,1176,1375,1171,1375,1167,1370,1167,1370,1162,1365,1162,1365,1157,1361,1157,1361,1152,1356,1147,1356,1142,1351,1142,1351,1138,1346,1138,1346,1133,1341,1133,1341,1128,1336,1123,1336,1118,1332,1118,1332,1113,1327,1113,1327,1108,1322,1104,1317,1099,1317,1094,1312,1094,1312,1089,1307,1084,1302,1079,1302,1074,1298,1074,1298,1070,1293,1065,1288,1060,1288,1055,1283,1055,1283,1050,1278,1045,1278,1041,1273,1041,1273,1036,1268,1031,1264,1026,1264,1021,1259,1021,1259,1016,1254,1011,1254,1007,1249,1007,1249,1002,1244,997,1244,992,1239,992,1239,987,1235,982,1235,977,1230,977,1230,973,1225,968,1225,963,1220,963,1220,958,1215,958,1215,953,1210,948,1210,944,1205,939,1205,934,1201,934,1201,929,1196,924,1196,919,1191,919,1191,914,1191,910,1186,910,1186,905,1181,900,1181,895,1176,895,1176,890,1171,885,1167,880,1167,876,1167,871,1162,871,1162,866,1157,861,1157,856,1152,856,1152,851,1147,847,1147,842,1142,842,1142,837,1138,832,1138,827,1133,822,1133,817,1128,813,1128,808,1123,808,1123,803,1118,798,1118,793,1113,783,1108,779,1108,774,1104,769,1099,764,1099,759,1094,750,1089,745,251,251,1264,1293,252,252,1264,1298,1264,1302,1264,1307,1259,1307,1259,1312,1259,1317,1264,1317,1264,1322,1259,1322,1259,1327,1264,1327,1264,1332,1259,1332,1264,1336,1264,1341,1264,1346,1264,1351,1264,1356,1264,1361,1264,1365,1264,1370,1264,1375,1268,1375,1268,1380,1268,1385,1268,1390,1268,1395,1268,1399,1273,1399,1268,1399,1273,1404,1273,1409,1273,1414,1273,1419,1278,1424,1278,1429,1278,1433,1278,1438,1283,1443,1283,1448,1283,1453,1283,1458,1288,1462,1288,1467,1288,1472,1293,1472,1293,1477,1293,1482,1293,1487,1298,1492,1298,1496,1298,1501,1302,1506,1302,1511,1302,1516,1307,1521,1307,1526,1307,1530,1312,1530,1312,1535,1312,1540,1317,1545,1317,1550,1317,1555,1322,1555,1322,1559,1322,1564,1327,1569,1327,1574,1327,1579,1332,1579,1332,1584,1332,1589,1336,1593,1336,1598,1336,1603,1341,1603,1341,1608,1346,1613,1346,1618,1346,1623,1351,1623,1351,1627,1351,1632,1356,1632,1356,1637,1356,1642,1361,1642,1361,1647,1361,1652,1365,1652,1365,1656,1370,1661,1370,1666,1370,1671,1375,1671,1375,1676,1380,1676,1380,1681,1380,1686,1385,1686,1385,1690,1385,1695,1390,1695,1390,1700,1395,1700,1395,1705,1395,1710,1399,1710,1399,1715,1404,1720,1404,1724,1409,1724,1409,1729,1409,1734,1414,1734,1414,1739,1419,1739,1419,1744,1424,1749,1424,1753,1429,1753,1429,1758,1433,1758,1433,1763,1433,1768,1438,1768,1438,1773,1443,1773,1443,1778,1443,1783,1448,1783,1453,1787,1453,1792,1458,1792,1458,1797,1462,1797,1462,1802,1467,1807,1472,1812,1472,1817,1477,1817,1477,1821,1482,1821,1487,1826,1487,1831,1492,1831,1492,1836,1496,1836,1496,1841,1501,1841,1501,1846,1506,1846,1506,1850,1511,1850,1511,1855,1516,1855,1521,1860,1526,1865,1530,1865,1530,1870,1535,1870,1535,1875,1540,1875,1540,1880,1545,1880,1550,1884,1555,1889,1559,1889,1559,1894,1564,1894,1569,1899,1574,1899,1574,1904,1579,1904,1579,1909,1584,1909,1589,1909,1589,1914,1593,1914,1598,1918,1603,1918,1603,1923,1608,1923,1613,1923,1613,1928,1618,1928,1623,1928,1623,1933,1627,1933,1632,1933,1632,1938,1637,1938,1642,1943,1647,1943,1652,1943,1652,1947,1656,1947,1661,1947,1661,1952,1666,1952,1671,1952,1676,1957,1681,1957,1686,1957,1690,1962,1695,1962,1700,1962,1705,1967,1710,1967,1715,1967,1720,1972,1724,1972,1729,1972,1734,1972,1739,1977,1744,1977,1749,1977,1753,1977,1758,1977,1758,1981,1763,1981,1768,1981,1773,1981,1778,1981,1783,1981,1783,1986,1787,1981,1787,1986,1792,1986,1797,1986,1802,1986,1807,1986,1812,1986,1817,1986,1821,1986,1826,1986,1831,1986,1831,1991,1836,1991,1836,1986,1841,1986,1846,1986,1850,1991,1850,1986,1855,1991,1855,1986,1860,1986,1865,1986,1870,1986,1875,1986,1880,1986,1884,1986,1889,1986,1894,1986,1899,1986,1904,1986,1909,1986,1909,1981,1914,1981,1918,1981,1923,1981,1928,1981,1933,1977,1938,1977,1943,1977,1947,1972,1952,1972,1957,1972,1957,1967,1962,1967,1967,1967,1972,1962,1977,1962,1977,1957,1981,1957,1986,1952,1991,1952,1991,1947,1996,1947,1996,1943,1996,1938,2001,1938,2001,1933,2006,1933,2006,1928,2011,1928,2011,1923,2011,1918,2011,1914,2015,1914,2015,1909,2015,1904,2015,1899,2015,1894,2015,1889,2015,1884,2015,1880,2015,1875,2015,1870,2015,1865,2015,1860,2011,1860,2011,1855,2011,1850,2011,1846,2006,1846,2006,1841,2006,1836,2001,1831,2001,1826,1996,1821,1996,1817,1996,1812,1991,1812,1991,1807,1986,1802,1986,1797,1981,1797,1981,1792,1981,1787,1977,1787,1977,1783,1972,1783,1972,1778,1972,1773,1967,1773,1967,1768,1962,1768,1962,1763,1957,1763,1957,1758,1957,1753,1952,1753,1952,1749,1947,1749,1947,1744,1943,1744,1943,1739,1938,1739,1938,1734,1933,1729,1928,1724,1928,1720,1923,1720,1923,1715,1918,1715,1918,1710,1914,1710,1914,1705,1909,1705,1909,1700,1904,1700,1904,1695,1899,1695,1899,1690,1894,1690,1894,1686,1889,1686,1889,1681,1884,1676,1880,1671,1875,1671,1875,1666,1870,1666,1870,1661,1865,1661,1865,1656,1860,1656,1860,1652,1855,1652,1855,1647,1850,1647,1850,1642,1846,1642,1846,1637,1841,1637,1841,1632,1836,1632,1836,1627,1831,1627,1831,1623,1826,1623,1821,1618,1817,1613,1812,1608,1807,1608,1807,1603,1802,1603,1802,1598,1797,1598,1797,1593,1792,1593,1792,1589,1787,1589,1787,1584,1783,1584,1778,1579,1773,1574,1768,1574,1768,1569,1763,1569,1763,1564,1758,1564,1758,1559,1753,1559,1753,1555,1749,1555,1744,1550,1739,1550,1739,1545,1734,1540,1729,1540,1729,1535,1724,1535,1724,1530,1720,1530,1720,1526,1715,1526,1710,1526,1710,1521,1705,1521,1705,1516,1700,1516,1700,1511,1695,1511,1695,1506,1690,1506,1690,1501,1686,1501,1681,1496,1676,1496,1676,1492,1671,1492,1671,1487,1666,1487,1666,1482,1661,1482,1656,1477,1652,1477,1652,1472,1647,1472,1647,1467,1642,1467,1642,1462,1637,1462,1632,1458,1627,1458,1627,1453,1623,1453,1623,1448,1618,1448,1613,1443,1608,1438,1603,1438,1603,1433,1598,1433,1598,1429,1593,1429,1593,1424,1589,1424,1584,1419,1579,1419,1579,1414,1574,1414,1574,1409,1569,1409,1569,1404,1564,1404,1559,1404,1559,1399,1555,1399,1555,1395,1550,1395,1545,1390,1540,1385,1535,1385,1535,1380,1530,1380,1530,1375,1526,1375,1521,1370,1516,1370,1516,1365,1511,1365,1511,1361,1506,1361,1501,1356,1496,1356,1496,1351,1492,1351,1492,1346,1487,1346,1482,1346,1482,1341,1477,1341,1477,1336,1472,1336,1472,1332,1467,1332,1462,1332,1462,1327,1458,1327,1458,1322,1458,1327,1453,1322,1448,1317,1443,1317,1443,1312,1438,1312,1433,1307,1429,1307,1429,1302,1424,1302,1419,1298,1414,1298,1414,1293,1409,1293,1404,1288,1399,1288,1399,1283,1395,1283,1390,1283,1390,1278,1385,1278,1380,1278,1380,1273,1380,1278,1380,1273,1375,1273,1370,1273,1370,1268,1365,1268,1361,1268,1361,1264,1356,1264,1351,1264,1351,1259,1346,1259,1341,1259,1341,1254,1336,1254,1332,1254,1327,1254,1322,1254,1317,1254,1317,1249,1312,1249,1307,1249,1302,1249,1298,1249,1293,1249,1293,1254,1293,1249,1293,1254,1288,1254,1283,1254,1283,1259,1278,1259,1278,1264,1278,1259,1278,1264,1273,1264,1273,1268,1268,1268,1268,1273,1268,1268,1268,1273,1268,1278,1268,1283,1264,1278,1264,1283,1264,1288,1264,1293,251,251,1477,1336,252,252,1477,1341,251,251,1947,1972,252,252,1952,1972,251,251,2044,1778,252,252,2044,1783,251,251,253,253,252,252,251,251
//};
//uint16_t positions [1000][2] = {0};
unsigned int counter = 0;

//updater variables
#define bufferSize 20 //number of bytes to store in the serial buffer
#define ramSize 200 //number of times to copy serial buffer to array
int bufferCounter = 0;
int ramCounter = 0;
int rcHigh,rcLow; //received byte.
void updater();
uint16_t index=0;
int16_t positions[bufferSize*ramSize/4][2] = {0};
//


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  
  myservo[0].attach(2);  // attaches the servo on pin 2 to the servo object (alpha)
  myservo[1].attach(4);   // attaches the servo on pin 4 to the servo object (beta)
  myservo[2].attach(6);   // attaches the servo on pin 6 to the servo object (z)
  
  myservo[2].write(89); //initially lift the pen
  delay(2000);
  pos[0]=1200; //zero positions for arms
  myservo[0].writeMicroseconds(pos[0]);
  pos[1]=1200;
  myservo[1].writeMicroseconds(pos[1]);
  Serial.print('r'); //request data
  delay(2000);
  updater();
}
 

void writemotor (short int motor, short int angle) {
  if (finishFlag[motor]==2) {
    
    if (abs(angle - pos[motor])>=2) {   
        duration[motor]=velocityCoeff*max(abs(pos[0]-positions[j][0]),abs(pos[1]-positions[j][1]))/abs(angle-pos[motor]);        
        finishFlag[motor] = 0;
    }
    else 
      finishFlag[motor]= 1;          
  }
    
  if( (finishFlag[motor]==0) && ((millis()-curtime[motor])>duration[motor])){
     
     if (angle-pos[motor]>=2){      
       pos[motor]+=2;       
       myservo[motor].write(pos[motor]);  
       curtime[motor]=millis();
     }
    
     else if (angle-pos[motor]<=-2){          
       pos[motor]-=2;
       myservo[motor].write(pos[motor]); 
       curtime[motor]=millis();       
    }

     else 
      finishFlag[motor]= 1;   
   }
}

void liftPen() {                //lifting the pen with 3rd motor
   myservo[2].write(89);
   delay(70);
}

void lowerPen() {               //lowering the pen with 3rd motor
   delay(20);
   myservo[2].write(14);
   delay(150);
}


void updater(){ 
 if(j%5==0){
  static byte flag=0;
  j=0;
  index=0;
  ramCounter=0;
  //Serial.print('r'); //request data
  
    while(Serial.available()<bufferSize) //wait until buffer is full
    {}
      
    for (bufferCounter=0;bufferCounter<bufferSize;bufferCounter+=2){
         rcHigh = Serial.read();
         rcHigh = rcHigh << 8;
         rcLow = Serial.read();
         rcHigh = rcHigh + rcLow;
         positions[index][flag]= rcHigh;           
         flag=!flag;
         if(!flag)
           index++;         
     }
      
     if (positions[index-1][!flag] == 253) //end of the file
           return; //exit loop
      
     Serial.print('r');    
 }
  /*
  if(j%1000==0){
      j=0;
      int i=0;
      for (i=0;i<1000;i++){
         positions[i][0] = (uint16_t)(pgm_read_word_near(positions2 + 2*(counter*1000 + i)));
         positions[i][1] = (uint16_t)(pgm_read_word_near(positions2 + 2*(counter*1000 + i) + 1));
      }
    counter++;    
    }
    */
}

void loop() {
    while(positions[j][0]>250 && positions[j][0]<=255) {
      if(positions[j][0]==251){               //lift code is 251, alpha and beta should be 251 
        liftPen();
        j++;
        updater();
      }
      else if(positions[j][0]==252){          //lower code is 252, alpha and beta should be 252 
        lowerPen();
        j++;
        updater();
      }
      else if(positions[j][0]==253){          //end code is 253, alpha and beta should be 253
        liftPen();
        while(1){};
      }
    }
    
writemotor(0,positions[j][0]);
writemotor(1,positions[j][1]);

  if (finishFlag[0] && finishFlag[1]) {
    j++;
    updater();
    finishFlag[0] = 2;
    finishFlag[1] = 2;    
    }
}
