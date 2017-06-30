lc=100;
nn=10;
r =10;
Mesh.CharacteristicLengthMin = 7.00;

Point(1) = {0, 0, 0, 1};
Point(2) = {lc, 0, 0, 1};
Point(3) = {0, lc, 0, 1};
Point(4) = {lc, lc, 0, 1};

Point(5) = {0+lc/2, lc/2, 0, 1};
Point(6) = {0+lc/2+r, lc/2, 0, 1};
Point(7) = {0+lc/2, lc/2+r, 0, 1};
Point(8) = {0+lc/2-r, lc/2, 0, 1};
Point(9) = {0+lc/2, lc/2-r, 0, 1};

Line(1) = {1, 2};
Line(2) = {2, 4};
Line(3) = {4, 3};
Line(4) = {3, 1};

Circle(5) = {6, 5, 7};
Circle(6) = {7, 5, 8};
Circle(7) = {8, 5, 9};
Circle(8) = {9, 5, 6};
Line Loop(9) = {7, 8, 5, 6};
Plane Surface(10) = {9};

Translate {35, 0, 0} {
  Duplicata { Surface{10}; }
}
Translate {-35, 0, 0} {
  Duplicata { Surface{10}; }
}
Translate {-35, 35, 0} {
  Duplicata { Surface{10}; }
}
Translate {0, 35, 0} {
  Duplicata { Surface{10}; }
}
Translate {35, 35, 0} {
  Duplicata { Surface{10}; }
}
Translate {-35, -35, 0} {
  Duplicata { Surface{10}; }
}
Translate {0, -35, 0} {
  Duplicata { Surface{10}; }
}
Translate {35, -35, 0} {
  Duplicata { Surface{10}; }
}


Line Loop(51) = {4, 1, 2, 3};
Line Loop(52) = {22, 23, 24, 25};
Line Loop(53) = {19, 20, 17, 18};
Line Loop(54) = {27, 28, 29, 30};
Line Loop(55) = {45, 42, 43, 44};
Line Loop(56) = {47, 48, 49, 50};
Line Loop(57) = {12, 13, 14, 15};
Line Loop(58) = {32, 33, 34, 35};
Line Loop(59) = {39, 40, 37, 38};
Plane Surface(60) = {9, 51, 52, 53, 54, 55, 56, 57, 58, 59};
Extrude {0, 0, 20} {
  Surface{60, 10, 11, 16, 21, 26, 31, 36, 41, 46};
}
Extrude {0, 0, 20} {
  Surface{262, 284, 306, 328, 350, 372, 394, 416, 438, 460};
}
Extrude {0, 0, 20} {
  Surface{662, 816, 838, 860, 728, 684, 706, 750, 772, 794};
}
Extrude {0, 0, 20} {
  Surface{1062, 1084, 1106, 1128, 1150, 1172, 1194, 1216, 1238, 1260};
}
Extrude {0, 0, 20} {
  Surface{1462, 1484, 1506, 1528, 1550, 1572, 1594, 1616, 1638, 1660};
}

Physical Volume("fuel_z1") = {5, 6, 4, 2, 8, 7, 3, 9, 10};
Physical Volume("fuel_z2") = {19, 20, 13, 18, 12, 17, 16, 14, 15};
Physical Volume("fuel_z3") = {22, 25, 28, 29, 26, 23, 24, 27, 30};
Physical Volume("fuel_z4") = {38, 35, 32, 33, 36, 39, 40, 37, 34};
Physical Volume("fuel_z5") = {44, 47, 50, 43, 46, 49, 42, 45, 48};
Physical Volume("fluid_z1") = {1};
Physical Volume("fluid_z2") = {11};
Physical Volume("fluid_z3") = {21};
Physical Volume("fluid_z4") = {31};
Physical Volume("fluid_z5") = {41};
Physical Surface("lateral") = {1721, 1733, 1729, 1725, 1325, 1329, 1333, 1321, 933, 921, 929, 925, 525, 529, 533, 521, 121, 133, 129, 125};
Physical Surface("top") = {577, 1950, 2016, 1884, 2038, 1972, 1906, 2060, 1862, 1928, 1994};
Physical Surface("bottom") = {36, 41, 46, 11, 31, 10, 16, 21, 26, 60};
