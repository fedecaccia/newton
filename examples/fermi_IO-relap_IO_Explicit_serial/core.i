*==================================================================
*                        C A R E M  
*==================================================================
*
100 new transnt
0000001 19     * Bestion correlation for drag in the core
*  CPU Time Remaining
105 5. 6.      * Remaining cpu time
* Noncondensable Gas Species
110 air        * Para el modelado de la pileta abierta
* time steps  min dt  max dt      min(grafic)? mj(output)?  restart frec                  
201     100.  .5e-7   0.1    003   1000       10000       100000


*==================================================================
*                    COMPONENTES
*================================================================== 

*------------------------------------------
* Tanque fuente de agua
*------------------------------------------
*       Comp._Name Comp._Type
1110000 inlet  tmdpvol
*       Area Length Volume Azim_Angle incl._Angle Elev._Change
1110101 0.1  1.0    0.00   0.00       0.00        0.00
*       Rough  Hyd._Diam. tlpvbfe
1110102 0.0    0.0        0000000
*       Init._Flag_ebt
1110200 103
*       time  p[MPa]   T[K]
1110201 0.00  12.25e6  555.15

*------------------------------------------
* Bomba
*------------------------------------------
*       Comp._Name  Comp._Type
1120000 JUN_112     tmdpjun
*       From      To        J._Area
1120101 111010002 120010001 0.1
*       cntrl-wrd
1120200 1  *[kg/s]
*       time  liq._vel/flow vap._vel/flow int._velocity
1120201 0.00  410.00         0.00          0.00

*------------------------------------------
* core
*------------------------------------------
*       nombre, tipo
1200000 NucAct pipe
*       num de vol
1200001 5
*       area, hasta el vol
1200101 0.8 5
*       largo, hasta el vol
1200301 0.28 5
*       angulo vertical, hasta el vol
1200601 90. 5
*  roughness - hid diam - vol number, si el hid diam es 0, se computa como 2*sqrt(area/pi)
1200801 1.e-5 0.01 5
* control flags en volumen
1201001 0000100 5  *Bestion correlation interphase drag: la friccion en x no se computa aqui, por eso se pone 1, se usa bestion
* control flags en junctions
1201101 0000000 4
* pipe ebt, t=vol CI: 3 indica las dos siguientes: presion y temperatura en equilibrio para liq/gas
1201201 003 12.25e6 555.15 0. 0. 0. 5
* pipe junct CI: 0:vel, 1:mass flow en 1301
1201300 1
* liq vel o mass flow, gas, interface, hasta junction number
1201301 410.0 0. 0. 4

*------------------------------------------
* Conexión sumidero
*------------------------------------------
*       Comp._Name  Comp._Type
1140000 JUN_114     sngljun
*       From      To        J._Area F._loss R._loss jefvcahs
1140101 120050002 115010001 0.1     0.00    0.00    00000000
*       Init._condition(0: velocidaddes, 1: caudales masicos) liq._vel/flow vap._vel/flow int._velocity 
1140201 1               10.00          0.00          0.00

*------------------------------------------
* Sumidero
*------------------------------------------
*       Comp._Name Comp._Type
1150000 TK-115     tmdpvol
*       Area Length Volume Azim_Angle incl._Angle Elev._Change
1150101 0.1  1.0    0.00   0.0        0.0         0.0
*       Rough  Hyd._Diam. tlpvbfe
1150109 0.0    0.0        0000000
*       Init._Flag_ebt
1150200            103
*       time  p[MPa] T[K]
1150201 0.00 12.25e+06 555.15

*=======================================================================
*            Heat Structures
*=======================================================================
*
* core active zone structure 9 nodos
*1CCCG0000 vol axiales, radial mesh points, geom type(1:rect, 2:cil), CI (0: provided, 1: a calcular), coord borde izq
11200000 5 11 2 1 0.0
* mesh data as input, formato en 10X (1: pares de intervalos en región y coord borde derecho)
11200100 0 1
* 5 pares entre 0 y 0.0038, ..., 2 entre 0.003875 y 0.0045: la suma de todos estos intervalos es radial mesh points-1
* Es el radio de un solo pin
11200101  5  0.0038
11200102  2  0.003875
11200103  3  0.0045
* composition number, hasta el interval number. Negativo excluye del promedio (3:UO2, 4:zircaloy, 5:gap)
* vaina y gap se excluyen del promedio. Igual no sirve porque hay que usar temp doppler.
11200201  3 5
11200202 -5 7
11200203 -4 10
* valor de fuente, en sentido relativo, hasta el mesh intervalo
11200301 1.0 5
11200302 0. 10
* init temp flag: 0 con el input data
11200400 0
* init temp: K, hasta el mesh point
11200401 557. 11
* left bound condition: vol asociado al left bound, increment en vol al que se conecta, tipo (0:adiab, 1:convective, otros ...), surface area code for W5 (1:cylinder height), cyl height: de todos los pines juntos, HS number
* Altura de 102 pines * 61 combustibles: XXX*102*61=1316, XXX~0.2? m (zona activa de cada pin)
11200501 0             0     0     1  1316.  5
* right bound condition: vol asociado a derecha(CCCNN000F: comp, cara, 0, coord), incremento en vol al que se conecta(+10000 en tarjeta), tipo: concevtive vertical bundle without crossflow, altura hasta el HS number
11200601 120010000   10000   110   1  1316.  5
* source type, (0: no, 1-999: from table, 1000: form kynetics), internal source multiplier (axial peaking factor, multiplica la potencia calculada en W1), .., .., hasta el HS number
*NEWTON_POWER_CARD
11200701 001  0.2    0.  0. 1
11200702 001  0.2    0.  0. 2
11200703 001  0.2    0.  0. 3
11200704 001  0.2    0.  0. 4
11200705 001  0.2    0.  0. 5
*END_NEWTON_POWER_CARD
* Aditional left boundary: W1: hid diam heat transfer, ..., w8: local boiling (correleación para calcular flujo crítico), W9: hasta el vol
11200801 0.      100. 100. 0.  0.  0.1 0.1 1.0  5
* Aditional right boundary:
* W1: diam hidráulico, debería corresponderse con el de arriba, es el que se lee para esta correlación
11200901 0.01723 100. 100. 0.  0.  0.1 0.1 1.0  5

*======================================================================
*   Tablas
*======================================================================

* 202TTT00 tipo
20200100 power
* tiempo valor (W)
20200101 0. 100.0e6

*======================================================================
*   thermal  property  data
*======================================================================
* Se preveen 3 tipos de material: MMM
* 003: UO2
* 004: Zircaloy
* 005: Gap en barras 

* UO2
* 201MMM00: from table, temp-conductivity, temp-vol heat cap, 
20100300       tbl/fctn       1       1     * zirc.
* Constant thermal conductivity (w/m/k)
20100301   293.  8.361
20100302   366.  7.27
20100303   373.  7.18
20100304   473.  6.10
20100305   533.  5.6
20100306   573.  5.31
20100307   623.  4.99
20100308   673.  4.70
20100309   723.  4.45
20100310   773.  4.22
20100311   823.  4.02
20100312   873.  3.84
20100313   923.  3.67
20100314   973.  3.52
20100315  1023.  3.38
20100316  1073.  3.26
20100317  1123.  3.14
20100318  1223.  2.94
20100319  1273.  2.85
20100320  1323.  2.76
20100321  1373.  2.69
20100322  1423.  2.62
20100323  1473.  2.55
20100324  1523.  2.5
20100325  1573.  2.44
20100326  1623.  2.39
20100327  1673.  2.35
20100328  1723.  2.31
20100329  1823.  2.25
20100330  1873.  2.22
20100331  1923.  2.20
20100332  1973.  2.22
20100333  2023.  2.25
20100334  2073.  2.29
20100335  2123.  2.33
20100336  2173.  2.37
20100337  2223.  2.42
20100338  2273.  2.47
20100339  2323.  2.52
20100340  2423.  2.65
20100341  2473.  2.73
20100342  2523.  2.81
20100343  2573.  2.90
20100344  2623.  2.99
20100345  2673.  3.10
20100346  2773.  3.35
20100347  2823.  3.49
20100348  2873.  3.65
20100349  3600.  5.29
* u02 heat capacity (j/m3/kg)
20100351   273.  2.31e6
20100352   323.  2.57e6
20100353   373.  2.75e6
20100354   473.  2.92e6
20100355   673.  3.13e6
20100356  1373.  3.44e6
20100357  4700.  6.80e6

* Zr
* zr conductivity (w/m/k)
20100400       tbl/fctn       1       1     * zirc.
20100401   273.15   7.
20100402   473.15   1.200438e1
20100403   673.15   1.400510e1
20100404   873.15   1.700793e1
20100405  1073.15   1.900866e1
20100406  1273.15   2.000975e1
20100407  1473.15   2.501085e1
20100408  1673.15   3.001267e1
20100409  1873.15   3.601486e1
20100410  2073.15   4.401777e1
20100411  2273.15   5.502352e1
20100412  3000.15   6.802826e1
* zr heat capacity (j/m3/kg)
20100451   255.3722   1.904141e6
20100452  1077.594    2.312171e6
20100453  1185.928    5.712422e6
20100454  1248.428    2.311769e6
20100455  2999.817    2.312171e6

*GAP
* GAP conductivity (w/m/k)
20100500       tbl/fctn       1       1    
20100501   273.   0.56
20100502   5000. 0.56
* GAP heat capacity (j/m3/kg)
20100551   3.  5.40
20100552   5000. 5.40

.  end of case  
