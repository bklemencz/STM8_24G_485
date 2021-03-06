EESchema Schematic File Version 4
LIBS:STM8_24G_485-cache
EELAYER 26 0
EELAYER END
$Descr User 8268 5827
encoding utf-8
Sheet 1 1
Title "STM8S103 Min Board 2.4G to RS485"
Date "2021-01-22"
Rev "V1.0"
Comp "BK"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L BK_Common:STM8S103_Brd M1
U 1 1 6000565E
P 2650 1425
F 0 "M1" H 2675 2190 50  0000 C CNN
F 1 "STM8S103_Brd" H 2675 2099 50  0000 C CNN
F 2 "BK_Common:STM8S103_Minimum_Board" H 2650 1025 50  0001 C CNN
F 3 "" H 2650 1025 50  0001 C CNN
	1    2650 1425
	1    0    0    -1  
$EndComp
$Comp
L BK_Common:MAX485 U2
U 1 1 60005729
P 5450 2400
F 0 "U2" H 5450 2837 60  0000 C CNN
F 1 "MAX485" H 5450 2731 60  0000 C CNN
F 2 "Housings_SOIC:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 5150 2050 60  0001 C CNN
F 3 "" H 5450 2400 60  0000 C CNN
	1    5450 2400
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:L78L05_SOT89 U1
U 1 1 600058C2
P 5275 875
F 0 "U1" H 5275 1117 50  0000 C CNN
F 1 "L78L05_SOT89" H 5275 1026 50  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-89-3" H 5275 1075 50  0001 C CIN
F 3 "http://www.st.com/content/ccc/resource/technical/document/datasheet/15/55/e5/aa/23/5b/43/fd/CD00000446.pdf/files/CD00000446.pdf/jcr:content/translations/en.CD00000446.pdf" H 5275 825 50  0001 C CNN
	1    5275 875 
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C2
U 1 1 600059BF
P 5875 1100
F 0 "C2" H 5967 1146 50  0000 L CNN
F 1 "1u" H 5967 1055 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 5875 1100 50  0001 C CNN
F 3 "~" H 5875 1100 50  0001 C CNN
	1    5875 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C1
U 1 1 60005A09
P 4700 1100
F 0 "C1" H 4792 1146 50  0000 L CNN
F 1 "1u" H 4792 1055 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 4700 1100 50  0001 C CNN
F 3 "~" H 4700 1100 50  0001 C CNN
	1    4700 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4975 875  4700 875 
Wire Wire Line
	4700 875  4700 1000
Connection ~ 4700 875 
Wire Wire Line
	4700 875  4400 875 
Wire Wire Line
	5575 875  5875 875 
Wire Wire Line
	5875 875  5875 1000
Wire Wire Line
	5875 875  6100 875 
Connection ~ 5875 875 
Wire Wire Line
	4700 1325 5275 1325
Wire Wire Line
	5875 1325 5875 1200
Wire Wire Line
	4700 1200 4700 1325
Wire Wire Line
	5275 1175 5275 1325
Connection ~ 5275 1325
Wire Wire Line
	5275 1325 5875 1325
$Comp
L power:GND #PWR03
U 1 1 60005B6A
P 5275 1400
F 0 "#PWR03" H 5275 1150 50  0001 C CNN
F 1 "GND" H 5280 1227 50  0000 C CNN
F 2 "" H 5275 1400 50  0001 C CNN
F 3 "" H 5275 1400 50  0001 C CNN
	1    5275 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5275 1325 5275 1400
$Comp
L power:VCC #PWR01
U 1 1 60005CCB
P 4400 875
F 0 "#PWR01" H 4400 725 50  0001 C CNN
F 1 "VCC" H 4417 1048 50  0000 C CNN
F 2 "" H 4400 875 50  0001 C CNN
F 3 "" H 4400 875 50  0001 C CNN
	1    4400 875 
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR02
U 1 1 60005D38
P 6100 875
F 0 "#PWR02" H 6100 725 50  0001 C CNN
F 1 "+5V" H 6115 1048 50  0000 C CNN
F 2 "" H 6100 875 50  0001 C CNN
F 3 "" H 6100 875 50  0001 C CNN
	1    6100 875 
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR08
U 1 1 60005E32
P 6425 2175
F 0 "#PWR08" H 6425 2025 50  0001 C CNN
F 1 "VCC" H 6442 2348 50  0000 C CNN
F 2 "" H 6425 2175 50  0001 C CNN
F 3 "" H 6425 2175 50  0001 C CNN
	1    6425 2175
	1    0    0    -1  
$EndComp
Wire Wire Line
	6425 2175 6425 2250
Wire Wire Line
	5850 2450 5975 2450
Wire Wire Line
	5850 2350 6350 2350
$Comp
L power:+5V #PWR07
U 1 1 60006222
P 5700 2000
F 0 "#PWR07" H 5700 1850 50  0001 C CNN
F 1 "+5V" H 5715 2173 50  0000 C CNN
F 2 "" H 5700 2000 50  0001 C CNN
F 3 "" H 5700 2000 50  0001 C CNN
	1    5700 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 2000 5700 2075
Wire Wire Line
	5700 2075 5850 2075
Wire Wire Line
	5850 2075 5850 2250
$Comp
L Device:R_Small R1
U 1 1 600063DF
P 6175 2150
F 0 "R1" V 5979 2150 50  0000 C CNN
F 1 "120" V 6070 2150 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" H 6175 2150 50  0001 C CNN
F 3 "~" H 6175 2150 50  0001 C CNN
	1    6175 2150
	0    1    1    0   
$EndComp
Wire Wire Line
	6075 2150 5975 2150
Wire Wire Line
	5975 2150 5975 2450
Connection ~ 5975 2450
Wire Wire Line
	6275 2150 6350 2150
Wire Wire Line
	6350 2150 6350 2350
Connection ~ 6350 2350
Wire Wire Line
	5050 2350 4975 2350
Wire Wire Line
	4975 2350 4975 2450
Wire Wire Line
	4975 2450 5050 2450
Wire Wire Line
	4975 2350 4650 2350
Connection ~ 4975 2350
Wire Wire Line
	5050 2550 4650 2550
Text Label 3975 2600 0    50   ~ 0
RX
Text Label 4650 2550 0    50   ~ 0
TX
Text Label 4650 2350 0    50   ~ 0
DE
Wire Wire Line
	2050 1025 1625 1025
Wire Wire Line
	2050 1125 1625 1125
Wire Wire Line
	2050 925  1625 925 
Text Label 1625 925  0    50   ~ 0
DE
Text Label 1625 1025 0    50   ~ 0
TX
Text Label 1625 1125 0    50   ~ 0
RX
Wire Wire Line
	2050 1825 1650 1825
Text Label 1650 1825 0    50   ~ 0
NSS
Wire Wire Line
	3300 1225 3675 1225
Wire Wire Line
	3300 1325 3675 1325
Wire Wire Line
	3300 1425 3675 1425
Text Label 3475 1225 0    50   ~ 0
MISO
Text Label 3475 1325 0    50   ~ 0
MOSI
Text Label 3475 1425 0    50   ~ 0
SCK
Text Notes 3150 1900 0    50   ~ 0
led
$Comp
L BK_Common:LT8920_Mini U3
U 1 1 6000AAD2
P 2025 3425
F 0 "U3" H 1675 3800 60  0000 C CNN
F 1 "LT8920_Mini" H 2350 3800 60  0000 C CNN
F 2 "BK_Common:XY-WB_LT8920_Nosilk" H 2025 3425 60  0001 C CNN
F 3 "" H 2025 3425 60  0000 C CNN
	1    2025 3425
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 1525 1450 1525
Wire Wire Line
	1450 1525 1450 1225
Wire Wire Line
	1450 1225 1125 1225
Wire Wire Line
	1125 1225 1125 1650
$Comp
L power:GND #PWR05
U 1 1 6000B4DF
P 1125 1650
F 0 "#PWR05" H 1125 1400 50  0001 C CNN
F 1 "GND" H 1130 1477 50  0000 C CNN
F 2 "" H 1125 1650 50  0001 C CNN
F 3 "" H 1125 1650 50  0001 C CNN
	1    1125 1650
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR04
U 1 1 6000B4FA
P 1275 1500
F 0 "#PWR04" H 1275 1350 50  0001 C CNN
F 1 "+5V" H 1290 1673 50  0000 C CNN
F 2 "" H 1275 1500 50  0001 C CNN
F 3 "" H 1275 1500 50  0001 C CNN
	1    1275 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1275 1500 1275 1625
Wire Wire Line
	1275 1625 2050 1625
Wire Wire Line
	2050 1725 1525 1725
Wire Wire Line
	1525 1725 1525 1900
Wire Wire Line
	1525 1900 1325 1900
Wire Wire Line
	1325 1900 1325 1850
$Comp
L power:+3.3V #PWR06
U 1 1 6000CB8A
P 1325 1850
F 0 "#PWR06" H 1325 1700 50  0001 C CNN
F 1 "+3.3V" H 1340 2023 50  0000 C CNN
F 2 "" H 1325 1850 50  0001 C CNN
F 3 "" H 1325 1850 50  0001 C CNN
	1    1325 1850
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR09
U 1 1 6000CCD6
P 2025 2775
F 0 "#PWR09" H 2025 2625 50  0001 C CNN
F 1 "+3.3V" H 2040 2948 50  0000 C CNN
F 2 "" H 2025 2775 50  0001 C CNN
F 3 "" H 2025 2775 50  0001 C CNN
	1    2025 2775
	1    0    0    -1  
$EndComp
Wire Wire Line
	2025 2775 2025 2825
NoConn ~ 2725 3525
$Comp
L power:GND #PWR010
U 1 1 6000E5F5
P 2025 4100
F 0 "#PWR010" H 2025 3850 50  0001 C CNN
F 1 "GND" H 2030 3927 50  0000 C CNN
F 2 "" H 2025 4100 50  0001 C CNN
F 3 "" H 2025 4100 50  0001 C CNN
	1    2025 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2025 4025 2025 4100
Wire Wire Line
	1325 3225 900  3225
Wire Wire Line
	1325 3325 900  3325
Wire Wire Line
	1325 3525 900  3525
Wire Wire Line
	1325 3625 900  3625
Text Label 900  3225 0    50   ~ 0
MISO
Text Label 900  3325 0    50   ~ 0
MOSI
Text Label 900  3625 0    50   ~ 0
SCK
Text Label 900  3525 0    50   ~ 0
NSS
$Comp
L Device:R_Small R2
U 1 1 60012D69
P 2975 3025
F 0 "R2" V 2779 3025 50  0000 C CNN
F 1 "1K" V 2870 3025 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" H 2975 3025 50  0001 C CNN
F 3 "~" H 2975 3025 50  0001 C CNN
	1    2975 3025
	-1   0    0    1   
$EndComp
Wire Wire Line
	2025 2825 2975 2825
Wire Wire Line
	2975 2825 2975 2925
Connection ~ 2025 2825
$Comp
L Device:C_Small C3
U 1 1 60013F44
P 2975 3475
F 0 "C3" H 3067 3521 50  0000 L CNN
F 1 "100n" H 3067 3430 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 2975 3475 50  0001 C CNN
F 3 "~" H 2975 3475 50  0001 C CNN
	1    2975 3475
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 6001403E
P 2975 4100
F 0 "#PWR011" H 2975 3850 50  0001 C CNN
F 1 "GND" H 2980 3927 50  0000 C CNN
F 2 "" H 2975 4100 50  0001 C CNN
F 3 "" H 2975 4100 50  0001 C CNN
	1    2975 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2975 3575 2975 4100
Wire Wire Line
	2975 3375 2975 3325
Wire Wire Line
	2725 3325 2975 3325
Connection ~ 2975 3325
Wire Wire Line
	2975 3325 2975 3125
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 6001A328
P 6725 2250
F 0 "J1" H 6805 2242 50  0000 L CNN
F 1 "485_1" H 6805 2151 50  0000 L CNN
F 2 "BK_Common:TerminalBlock_Phoenix_PT-1,5-2-3.5-H_1x02_P3.50mm_Horizontal" H 6725 2250 50  0001 C CNN
F 3 "~" H 6725 2250 50  0001 C CNN
	1    6725 2250
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 6001A3A0
P 6725 2450
F 0 "J2" H 6805 2442 50  0000 L CNN
F 1 "485_2" H 6805 2351 50  0000 L CNN
F 2 "BK_Common:TerminalBlock_Phoenix_PT-1,5-2-3.5-H_1x02_P3.50mm_Horizontal" H 6725 2450 50  0001 C CNN
F 3 "~" H 6725 2450 50  0001 C CNN
	1    6725 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6425 2250 6525 2250
Wire Wire Line
	6350 2350 6525 2350
Wire Wire Line
	5975 2450 6525 2450
Wire Wire Line
	5850 2550 6250 2550
$Comp
L power:GND #PWR0101
U 1 1 6001F4F1
P 6250 2600
F 0 "#PWR0101" H 6250 2350 50  0001 C CNN
F 1 "GND" H 6255 2427 50  0000 C CNN
F 2 "" H 6250 2600 50  0001 C CNN
F 3 "" H 6250 2600 50  0001 C CNN
	1    6250 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 2600 6250 2550
Connection ~ 6250 2550
Wire Wire Line
	6250 2550 6525 2550
Text Label 6100 2350 0    50   ~ 0
B
Text Label 6100 2450 0    50   ~ 0
A
$Comp
L Device:R_Small R3
U 1 1 60020CAC
P 4250 2425
F 0 "R3" V 4054 2425 50  0000 C CNN
F 1 "1K" V 4145 2425 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" H 4250 2425 50  0001 C CNN
F 3 "~" H 4250 2425 50  0001 C CNN
	1    4250 2425
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R4
U 1 1 60020D0C
P 4250 2750
F 0 "R4" V 4054 2750 50  0000 C CNN
F 1 "2K" V 4145 2750 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" H 4250 2750 50  0001 C CNN
F 3 "~" H 4250 2750 50  0001 C CNN
	1    4250 2750
	-1   0    0    1   
$EndComp
Wire Wire Line
	4250 2525 4250 2600
$Comp
L power:GND #PWR0102
U 1 1 6002354E
P 4250 2925
F 0 "#PWR0102" H 4250 2675 50  0001 C CNN
F 1 "GND" H 4255 2752 50  0000 C CNN
F 2 "" H 4250 2925 50  0001 C CNN
F 3 "" H 4250 2925 50  0001 C CNN
	1    4250 2925
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 2850 4250 2925
Wire Wire Line
	4250 2250 4250 2325
Wire Wire Line
	4250 2250 5050 2250
Wire Wire Line
	4250 2600 3975 2600
Connection ~ 4250 2600
Wire Wire Line
	4250 2600 4250 2650
$EndSCHEMATC
