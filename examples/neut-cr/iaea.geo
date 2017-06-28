lc = 0;
le = 100;
nz=20;

Mesh.CharacteristicLengthMin = 7.00;
Mesh.CharacteristicLengthMax = 10.00;

Point(1) = {0, 0, 0, lc};
Point(2) = {10, 0, 0, lc};
Point(3) = {0, 10, 0, lc};
Point(4) = {10, 10, 0, lc};
Point(5) = {70, 0, 0, lc};
Point(6) = {70, 10, 0, lc};
Point(7) = {90, 10, 0, lc};
Point(8) = {90, 0, 0, lc};
Point(9) = {0, 70, 0, lc};
Point(10) = {10, 70, 0, lc};
Point(11) = {10, 90, 0, lc};
Point(12) = {0, 90, 0, lc};
Point(13) = {130, 0, 0, lc};
Point(14) = {130, 30, 0, lc};
Point(15) = {110, 30, 0, lc};
Point(16) = {110, 70, 0, lc};
Point(17) = {90, 70, 0, lc};
Point(18) = {90, 90, 0, lc};
Point(19) = {70, 90, 0, lc};
Point(20) = {70, 70, 0, lc};
Point(21) = {70, 110, 0, lc};
Point(22) = {30, 110, 0, lc};
Point(23) = {30, 130, 0, lc};
Point(24) = {0, 130, 0, lc};
Point(25) = {0, 150, 0, lc};
Point(26) = {50, 150, 0, lc};
Point(27) = {50, 130, 0, lc};
Point(28) = {90, 130, 0, lc};
Point(29) = {90, 110, 0, lc};
Point(30) = {110, 110, 0, lc};
Point(31) = {110, 90, 0, lc};
Point(32) = {130, 90, 0, lc};
Point(33) = {130, 50, 0, lc};
Point(34) = {150, 50, 0, lc};
Point(35) = {150, 0, 0, lc};
Point(36) = {170, 0, 0, lc};
Point(37) = {170, 70, 0, lc};
Point(38) = {150, 70, 0, lc};
Point(39) = {150, 110, 0, lc};
Point(40) = {130, 110, 0, lc};
Point(41) = {130, 130, 0, lc};
Point(42) = {110, 130, 0, lc};
Point(43) = {110, 150, 0, lc};
Point(44) = {70, 150, 0, lc};
Point(45) = {70, 170, 0, lc};
Point(46) = {0, 170, 0, lc};

Line(1) = {1, 2};
Line(2) = {2, 4};
Line(3) = {4, 3};
Line(4) = {1, 3};
Line(5) = {5, 8};
Line(6) = {8, 7};
Line(7) = {7, 6};
Line(8) = {6, 5};
Line(9) = {9, 10};
Line(10) = {10, 11};
Line(11) = {11, 12};
Line(12) = {12, 9};
Line(13) = {19, 20};
Line(14) = {20, 17};
Line(15) = {17, 18};
Line(16) = {18, 19};
Line(17) = {9, 3};
Line(18) = {2, 5};
Line(19) = {8, 13};
Line(20) = {13, 14};
Line(21) = {14, 15};
Line(22) = {15, 16};
Line(23) = {16, 17};
Line(24) = {19, 21};
Line(25) = {21, 22};
Line(26) = {22, 23};
Line(27) = {23, 24};
Line(28) = {24, 12};
Line(29) = {13, 35};
Line(30) = {35, 34};
Line(31) = {34, 33};
Line(32) = {33, 32};
Line(33) = {32, 31};
Line(34) = {31, 30};
Line(35) = {30, 29};
Line(36) = {29, 28};
Line(37) = {28, 27};
Line(38) = {27, 26};
Line(39) = {26, 25};
Line(40) = {25, 24};
Line(41) = {35, 36};
Line(42) = {36, 37};
Line(43) = {37, 38};
Line(44) = {38, 39};
Line(45) = {39, 40};
Line(46) = {40, 41};
Line(47) = {41, 42};
Line(48) = {42, 43};
Line(49) = {43, 44};
Line(50) = {44, 45};
Line(51) = {45, 46};
Line(52) = {46, 25};

Line Loop(53) = {29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, -27, -26, -25, -24, -16, -15, -23, -22, -21, -20};
Plane Surface(54) = {53};
//Recombine Surface {54};
Line Loop(55) = {19, 20, 21, 22, 23, -14, -13, 24, 25, 26, 27, 28, -11, -10, -9, 17, -3, -2, 18, -8, -7, -6};
Plane Surface(56) = {55};
//Recombine Surface {56};
Line Loop(57) = {11, 12, 9, 10};
Plane Surface(58) = {57};
//Recombine Surface {58};
Line Loop(59) = {16, 13, 14, 15};
Plane Surface(60) = {59};
//Recombine Surface {59};
Line Loop(61) = {7, 8, 5, 6};
Plane Surface(62) = {61};
//Recombine Surface {62};
Line Loop(63) = {3, -4, 1, 2};
Plane Surface(64) = {63};
//Recombine Surface {64};
Line Loop(65) = {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, -39, -38, -37, -36, -35, -34, -33, -32, -31, -30};
Plane Surface(66) = {65};
//Recombine Surface {66};

Color Goldenrod {Surface {54};}
Color LimeGreen {Surface {56};}
Color Firebrick {Surface {60, 58, 64, 62};}
Color BlueViolet {Surface {66};}

Mesh.Light = 0;
General.SmallAxes = 0;
v1[]=Extrude {0, 0, le} {
  Surface{58, 64, 60, 62};
  Layers{nz};
  Recombine;
};
v2[]=Extrude {0, 0, le} {
  Surface{56};
  Layers{nz};
  Recombine;
};
v3[]=Extrude {0, 0, le} {
  Surface{54};
  Layers{nz};
  Recombine;
};
v4[]=Extrude {0, 0, le} {
  Surface{66};
  Layers{nz};
  Recombine;
};

Physical Volume("ROD1") = {1, 3, 4, 2};
Physical Volume("MAT1") = {v2[1]};
Physical Volume("MAT2") = {v3[1]};
Physical Volume("MAT3") = {v4[1]};

Physical Surface("EXT") = {445, 441, 437, 429, 433, 425, 421, 417, 413, 409};
Physical Surface("INT") = {449, 337, 225, 79, 241, 101, 105, 253, 149, 181, 293, 405};
Physical Surface("TOP") = {154, 266, 110, 88, 132, 378, 490};
Physical Surface("BOT") = {58, 64, 56, 60, 54, 66, 62};
Physical Point("PTO1") = {62};
