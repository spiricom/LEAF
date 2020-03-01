/*
 * tunings.c
 *
 *  Created on: Dec 18, 2019
 *      Author: josnyder
 */
#include "main.h"
#include "tunings.h"

float centsDeviation[12] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
uint32_t currentTuning = 0;
uint8_t keyCenter = 0;
char tuningNames[NUM_TUNINGS][6]= {
		{"12-TET"},
		{"JUST"},
		{"LMY_P"},
		{"XYL2"},
		{"XYL4"},
		{"LMY_GT"},
		{"ABELL"},
		{"AEOLIC"},
		{"AGRI_P"},
		{"ALVESP"},
		{"ALVESS"},
		{"ANGKLG"},
		{"HUZAM"},
		{"ARCHCH"},
		{"ARCHSY"},
		{"ARCHPT"},
		{"ARCH_7"},
		{"ARIEL3"},
		{"AUGTET"},
		{"AWRAAM"},
		{"BAGPI1"},
		{"BAGPI2"},
		{"BAGPI3"},
		{"BALAF1"},
		{"BALAF2"},
		{"BALAF3"},
		{"BALAF4"},
		{"BELLIN"},
		{"JI_12"},
		{"JOHNST"},
		{"JOHNS6"},
		{"KEENAN"},
		{"KRNBRG"},
		{"KORA1"},
		{"KORA2"},
		{"KORA3"},
		{"KORA4"},
		{"LARA"},
		{"LIGON"},
		{"LYD_D"},
		{"LYD_DI"},
		{"LYD_E"},
		{"LYD_EI"},
		{"MALCOL"},
		{"MABUTI"},
		{"MARIMB"},
		{"MBIRA1"},
		{"MBIRA2"},
		{"MBIRA3"},
		{"MBIRA4"},
		{"MBIRA5"},
		{"MEANQU"},
		{"MET24"},
		{"METALS"},
		{"PARTC1"},
		{"PARTC2"},
		{"PTOLEM"},
		{"RILEYA"},
		{"RILEYR"},
		{"BLUEJ1"},
		{"BLUEJ2"},
		{"DUDON1"},
		{"DUDON2"},
		{"COUNTB"},
		{"COUNTR"},
		{"DUDON3"},
};

		//0 12_TET
		//1 jeff Just 					   12 custom simple overtone just scale
		//2 young-lm_piano.scl             12  LaMonte Young's Well-Tuned Piano
		//3 xylophone2.scl                 10  African Yaswa xylophones (idiophone; calbash resonators with membrane)
		//4 xylophone4.scl                 10  African Bapare xylophone (idiophone; loose log)
		//5 young-lm_guitar.scl            12  LaMonte Young, tuning of For Guitar '58. 1/1 March '92, inv.of Mersenne lute 1
		//6 abell1.scl                     12  Ross Abell's French Baroque Meantone 1, a'=520 Hz
		//7 aeolic.scl                      7  Ancient Greek Aeolic, also tritriadic scale of the 54:64:81 triad
		//8 agricola_p.scl                 12  Agricola's Pythagorean-type Monochord, Musica instrumentalis deudsch (1545)
		//9 alves_pelog.scl                 7  Bill Alves JI Pelog, 1/1 vol.9 no.4, 1997. 1/1=293.33 Hz
		//10 alves_slendro.scl               5  Bill Alves, slendro for Gender Barung, 1/1 vol.9 no.4, 1997. 1/1=282.86 Hz
		//11 angklung.scl                    8  Scale of an anklung set from Tasikmalaya. 1/1=174 Hz
		//12 arabic_huzam_on_e.scl          12  Arabic Huzam with perde segah on E by Dr. Ozan Yarman.
		//13 arch_chrom.scl                  7  Archytas' Chromatic
		//14 archytas12sync.scl             12  Archytas[12] (64/63) hobbit, sync beating
		//15 arch_ptol.scl                  12  Archytas/Ptolemy Hybrid 1
		//16 arch_sept.scl                  12  Archytas Septimal
		//17 ariel3.scl                     12  Ariel's 12-tone JI scale
		//18 augtetj.scl                     6  9/8 C.I. comprised of 11:10:9:8 subharmonic series on 1 and 8:9:10:11 on 16/11
		//19 awraamoff.scl                  12  Awraamoff Septimal Just (1920)

		//21 bagpipe1.scl                   12  Bulgarian bagpipe tuning
		//22 bagpipe2.scl                    9  Highland Bagpipe, from Acustica4: 231 (1954) J.M.A Lenihan and S. McNeill
		//23 bagpipe3.scl                    9  Highland Bagpipe, Allan Chatto, 1991. From Australian Pipe Band College
		//24 balafon.scl                     7  Observed balafon tuning from Patna, Helmholtz/Ellis p. 518, nr.81
		//25 balafon2.scl                    7  Observed balafon tuning from West-Africa, Helmholtz/Ellis p. 518, nr.86
		//26 balafon3.scl                    7  Pitt-River's balafon tuning from West-Africa, Helmholtz/Ellis p. 518, nr.87
		//27 balafon4.scl                    7  Mandinka balafon scale from Gambia
		//28 bellingwolde.scl               12  Current 1/6-P. comma mod.mean of Freytag organ in Bellingwolde. Ortgies,2002
		//29 ji_12.scl                      12  Basic JI with 7-limit tritone. Robert Rich: Geometry
		//30 johnston.scl                   12  Ben Johnston's combined otonal-utonal scale
		//31 johnston_6-qt_row.scl          12  11-limit 'prime row' from Ben Johnston's "6th Quartet"
		//32 keenan_t9.scl                  12  Dave Keenan strange 9-limit temperament TL 19-11-98
		//33 kirnberger.scl                 12  Kirnberger's well-temperament, also called Kirnberger III, letter to Forkel 1779
		//34 kora1.scl                       7  Kora tuning Tomora Ba, also called Silaba, 1/1=F, R. King
		//35 kora2.scl                       7  Kora tuning Tomora Mesengo, also called Tomora, 1/1=F, R. King
		//36 kora3.scl                       7  Kora tuning Hardino, 1/1=F, R.King
		//37 kora4.scl                       7  Kora tuning Sauta (Sawta), 1/1=F, R. King
		//38 lara.scl                       12  Sundanese 'multi-laras' gamelan Ki Barong tuning, Weintraub, TL 15-2-99 1/1=497
		//39 ligon.scl                      12  Jacky Ligon, strictly proper all prime scale, TL 08-09-2000
		//40 lydian_diat2.scl                8  Schlesinger's Lydian Harmonia, a subharmonic series through 13 from 26
		//41 lydian_diat2inv.scl             8  Inverted Schlesinger's Lydian Harmonia, a harmonic series from 13 from 26
		//42 lydian_enh2.scl                 7  Schlesinger's Lydian Harmonia in the enharmonic genus
		//43 lydian_enhinv.scl               7  A harmonic form of Schlesinger's Enharmonic Lydian inverted
		//44 malcolms.scl                   12  Symmetrical version of Malcolm's Monochord and Riley's Albion scale. Also proposed by Hindemith in Unterweisung im Tonsatz
		//45 mambuti.scl                     8  African Mambuti Flutes (aerophone; vertical wooden; one note each)
		//46 marimba3.scl                   10  Marimba from the Yakoma tribe, Zaire. 1/1=185.5 Hz
		//47 mbira_banda.scl                 7  Mubayiwa Bandambira's tuning of keys R2-R9 from Berliner: The soul of mbira.
		//48 mbira_chilimba.scl              7  Mbira chilimba from Bemba. 1/1=228 Hz, Tracey TR-182 B-7
		//49 mbira_chisanzhi2.scl            7  Mbira chisanzhi from Lunda. 1/1=212 Hz, Tracey TR-179 B-5,6
		//50 mbira_kunaka.scl                7  John Kunaka's mbira tuning of keys R2-R9
		//51 mbira_neikembe.scl              7  Mbira neikembe from Medje. 1/1=320 Hz, Tracey TR-120 B-1,2
		//52 meanquar.scl                   12  1/4-comma meantone scale. Pietro Aaron's temp. (1523). 6/5 beats twice 3/2
		//53 met24-chrys_diat-4th_pl.scl     7  Near Chrysanthos 4th Byzantine Liturgical mode, JI
		//54 metals.scl                      9  Gold, silver, titanium - strong metastable intervals between 1 and 2.
		//55 partch-grm.scl                  9  Partch Greek scales from "Two Studies on Ancient Greek Scales" mixed
		//56 partch-greek.scl               12  Partch Greek scales from "Two Studies on Ancient Greek Scales" on black/white
		//57 ptolemy_imix.scl               11  Ptolemy Intense Diatonic mixed with its inverse
		//58 riley_albion.scl               12  Terry Riley's Harp of New Albion scale, inverse Malcolm's Monochord, 1/1 on C#
		//59 riley_rosary.scl               12  Terry Riley, tuning for Cactus Rosary (1993)
		//60 breed-bluesji.scl              12  7-limit JI version of Graham Breed's Blues scale
		//61 blueji-cataclysmic.scl         12  John O'Sullivan's Blueji tempered in 13-limit POTE-tuned cataclysmic
		//62 dudon_bambara.scl              12  Typical pentatonic balafon ceremonial tuning from Mali or Burkina Faso
		//63 dudon_bhairav.scl              12  Bhairav thaat raga, based on 17th harmonic
		//64 dudon_country_blues.scl        12  Differentially-coherent 12 tones country blues scale
		//65 dudon_countrysongs.scl         12  CDEG chords and all transpositions equal-beating meantone sequence
		//66 dudon_didymus.scl              12  Greek-genre scale rich in commas



//encoded as deviations in semitones from scale position - always a full 12 note scale, repeating notes if there are fewer than 12 in scale.
float tuningPresets[NUM_TUNINGS][12] = {
		{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{0.000000f, 0.110000f, 0.030000f, 0.150000f, -0.140000f, -0.020000f, -0.490000f, 0.010000f, 0.130000f, -0.160000f, -0.320000f, -0.120000f},
		{0.000000f, 0.760000f, 0.030000f, -0.610000f, 0.700000f, -0.570000f, 0.740000f, 0.010000f, -0.630000f, 0.680000f, -0.590000f, 0.720000f},
		{0.000000f, -1.000000f, 0.090000f, 1.160000f, 2.860000f, 4.260000f, 5.440000f, 5.130000f, 5.770000f, 6.300000f, 8.260000f, 7.260000f},
		{0.000000f, -1.000000f, -0.720000f, 0.170000f, 1.020000f, 1.990000f, 2.880000f, 4.410000f, 5.450000f, 5.310000f, 6.040000f, 5.040000f},
		{0.000000f, 0.110000f, -0.180000f, 0.150000f, -0.140000f, -0.020000f, -0.100000f, 0.010000f, 0.130000f, -0.160000f, 0.170000f, -0.120000f},
		{0.000000f, -0.220000f, -0.060000f, -0.290000f, -0.130000f, -0.360000f, -0.190000f, -0.030000f, -0.250000f, -0.090000f, -0.320000f, -0.160000f},
		{0.000000f, -1.000000f, 0.030000f, -0.970000f, -1.060000f, -0.020000f, 1.010000f, 0.010000f, -0.080000f, -1.080000f, -0.040000f, -1.040000f},
		{0.000000f, -0.080000f, 0.030000f, -0.040000f, 0.070000f, -0.020000f, -0.100000f, 0.010000f, -0.060000f, 0.050000f, -0.040000f, 0.090000f},
		{0.000000f, -1.000000f, 0.310000f, -0.690000f, -0.850000f, -0.300000f, 1.010000f, 0.010000f, 0.130000f, -0.870000f, -0.320000f, -1.320000f},
		{0.000000f, -1.000000f, 0.310000f, -0.690000f, -1.690000f, -0.020000f, -1.020000f, 0.640000f, -0.360000f, 0.960000f, -0.040000f, -1.040000f},
		{0.000000f, -1.000000f, 0.060000f, -0.940000f, -0.180000f, 1.100000f, 2.230000f, 1.230000f, 4.340000f, 5.060000f, 4.060000f, 5.330000f},
		{0.000000f, 1.350000f, -0.010000f, 0.010000f, -0.490000f, 0.010000f, -0.320000f, 0.010000f, 0.490000f, 0.010000f, 0.000000f, 0.020000f},
		{0.000000f, -1.000000f, -1.380000f, -2.380000f, -1.970000f, -0.020000f, 1.010000f, 0.010000f, -0.360000f, -1.360000f, -0.950000f, -1.950000f},
		{0.000000f, -0.040000f, 0.220000f, 0.190000f, -0.080000f, -0.120000f, 0.150000f, 0.110000f, 0.070000f, 0.340000f, -0.230000f, 0.030000f},
		{0.000000f, -0.380000f, -0.890000f, -1.180000f, -1.060000f, -0.020000f, -0.390000f, 0.010000f, -0.360000f, -0.870000f, -1.160000f, -1.040000f},
		{0.000000f, -0.380000f, -0.890000f, -0.970000f, -1.060000f, -0.020000f, -0.390000f, 0.010000f, -0.360000f, -0.870000f, -0.950000f, -1.040000f},
		{0.000000f, 0.110000f, -0.180000f, -0.060000f, -0.360000f, -0.020000f, -0.320000f, 0.010000f, 0.130000f, -0.160000f, -0.040000f, -0.340000f},
		{0.000000f, -1.000000f, -0.350000f, -1.350000f, -0.530000f, -1.530000f, -0.490000f, -1.490000f, -1.520000f, -2.520000f, -1.480000f, -2.480000f},
		{0.000000f, 1.030000f, 0.310000f, 0.150000f, -0.140000f, -0.300000f, -1.020000f, 0.010000f, 0.130000f, 0.330000f, -0.320000f, -0.120000f},


		{0.000000f, -3.030000f, -2.000000f, -3.000000f, -1.970000f, -1.140000f, -2.140000f, -1.810000f, -0.990000f, -1.990000f, -1.160000f, -0.830000f},
		{0.000000f, -2.820000f, -2.000000f, -3.000000f, -1.970000f, -1.140000f, -2.140000f, -2.020000f, -0.990000f, -1.990000f, -1.160000f, -0.830000f},
		{0.000000f, -3.310000f, -2.000000f, -3.000000f, -1.970000f, -1.140000f, -2.140000f, -2.020000f, -0.990000f, -1.990000f, -1.160000f, -1.320000f},
		{0.000000f, -1.000000f, -0.130000f, -1.130000f, -0.440000f, 0.260000f, 0.720000f, -0.280000f, 0.560000f, -0.440000f, -0.150000f, -1.150000f},
		{0.000000f, -1.000000f, -0.480000f, -1.480000f, -1.130000f, 0.330000f, 1.240000f, 0.240000f, 0.900000f, -0.100000f, 0.390000f, -0.610000f},
		{0.000000f, -1.000000f, -0.050000f, -1.050000f, -1.110000f, 0.130000f, 0.860000f, -0.140000f, -0.040000f, -1.040000f, 0.080000f, -0.920000f},
		{0.000000f, -1.000000f, -0.490000f, -1.490000f, -0.550000f, 0.260000f, 0.600000f, -0.400000f, 0.610000f, -0.390000f, 0.250000f, -0.750000f},
		{0.000000f, -0.100000f, -0.040000f, 0.010000f, -0.080000f, 0.010000f, -0.120000f, -0.020000f, -0.040000f, -0.060000f, 0.000000f, -0.100000f},
		{0.000000f, 0.117313f, 0.039100f, 0.156413f, -0.136863f, -0.019550f, -0.174878f, 0.019550f, 0.136863f, -0.156413f, 0.175963f, -0.117313f},
		{0.000000f, -0.078213f, 0.039100f, 0.233528f, -0.136863f, 0.513179f, -0.097763f, 0.019550f, -0.591392f, 0.058650f, -0.311741f, -0.117313f},
		{0.000000f, -0.293276f, -0.175963f, -0.254176f, -0.136863f, 0.057565f, -0.312826f, 0.337217f, -0.273726f, -0.156413f, -0.767355f, -0.117313f},
		{0.000000f, 0.060000f, 0.120000f, -0.240000f, -0.180000f, -0.120000f, 0.000000f, 0.060000f, 0.120000f, -0.240000f, -0.180000f, -0.120000f},
		{0.000000f, -0.097750f, -0.068431f, -0.058650f, -0.136863f, -0.019550f, -0.097763f, -0.034216f, -0.078200f, -0.102647f, -0.039100f, -0.117313f},
		{0.000000f, -1.000000f, 0.000000f, -1.000000f, -0.150000f, 0.000000f, 1.000000f, 0.000000f, 1.000000f, 0.000000f, 0.850000f, -0.150000f},
		{0.000000f, -1.000000f, 0.300000f, -0.700000f, -0.750000f, 0.000000f, 1.000000f, 0.000000f, 1.300000f, 0.300000f, 0.250000f, -0.750000f},
		{0.000000f, -1.000000f, -0.150000f, -1.150000f, 0.050000f, 0.000000f, 1.000000f, 0.000000f, 0.850000f, -0.150000f, 1.050000f, 0.050000f},
		{0.000000f, -1.000000f, -0.150000f, -1.150000f, 0.050000f, 1.050000f, 1.000000f, 0.000000f, 0.850000f, -0.150000f, 1.050000f, 0.050000f},
		{0.000000f, 0.570000f, 0.270000f, 0.110000f, 0.590000f, 1.330000f, 1.150000f, 1.200000f, 1.390000f, 1.960000f, 2.000000f, 2.700000f},
		{0.000000f, 0.154584f, -0.074424f, -0.107903f, 0.013028f, -0.355723f, -0.174878f, 0.019550f, -0.175080f, -0.156413f, -0.122533f, -0.282982f},
		{0.000000f, -1.000000f, -0.614273f, -1.614273f, -1.107903f, -0.457861f, -0.569854f, -1.569854f, -1.633823f, -0.594723f, -1.594723f, -0.282982f},
		{0.000000f, -1.000000f, -0.717018f, -1.717018f, -0.405277f, 0.633823f, 0.569854f, -0.430146f, -0.542139f, 0.107903f, -0.892097f, -0.385727f},
		{0.000000f, -1.000000f, -1.663827f, -2.663827f, -3.320998f, -0.457861f, 0.366177f, -0.633823f, -1.146120f, -2.146120f, -2.644277f, -3.644277f},
		{0.000000f, -1.000000f, -1.670231f, -2.670231f, -3.346627f, 0.633823f, 1.457861f, 0.457861f, -0.114653f, -1.114653f, -1.697468f, -2.697468f},
		{0.000000f, 0.117313f, 0.039100f, 0.156413f, -0.136863f, -0.019550f, 0.000000f, 0.019550f, 0.136863f, -0.156413f, -0.039100f, -0.117313f},
		{0.000000f, -1.000000f, 0.040000f, -0.960000f, 0.110000f, 2.100000f, 4.000000f, 3.000000f, 4.060000f, 5.090000f, 4.090000f, 8.180000f},
		{0.000000f, -1.000000f, 0.180000f, 1.950000f, 4.200000f, 5.380000f, 5.850000f, 7.470000f, 8.950000f, 11.200000f, 12.380000f, 11.380000f},
		{0.000000f, -1.000000f, -0.150000f, -1.150000f, -0.110000f, 0.930000f, 1.560000f, 0.560000f, 1.140000f, 0.140000f, 0.510000f, -0.490000f},
		{0.000000f, -1.000000f, -0.270000f, -1.270000f, -0.450000f, 0.200000f, 0.920000f, -0.080000f, 0.660000f, -0.340000f, 0.740000f, -0.260000f},
		{0.000000f, -1.000000f, -0.439274f, -1.439274f, -0.071656f, -0.183649f, 0.240085f, -0.759915f, 0.380132f, -0.619868f, 0.103222f, -0.896778f},
		{0.000000f, -1.000000f, -0.040000f, -1.040000f, -0.230000f, 0.060000f, 0.760000f, -0.240000f, 0.770000f, -0.230000f, 0.500000f, -0.500000f},
		{0.000000f, -1.000000f, 0.040000f, -0.960000f, -0.310000f, 0.200000f, 1.020000f, 0.020000f, 0.670000f, -0.330000f, 0.660000f, -0.340000f},
		{0.000000f, -0.239510f, -0.068431f, 0.102647f, -0.136863f, 0.034216f, -0.205294f, -0.034216f, -0.273726f, -0.102647f, 0.068431f, -0.171079f},
		{0.000000f, -1.000000f, 0.074219f, -0.925781f, -0.425781f, -0.031250f, 1.042969f, 0.042969f, 1.117188f, 0.117188f, 0.617188f, -0.382812f},
		{0.000000f, 2.390000f, 2.220000f, 1.220000f, 1.600000f, 1.070000f, 0.070000f, 0.920000f, 0.330000f, -0.670000f, -0.570000f, -0.980000f},
		{0.000000f, -0.370391f, -0.882687f, -1.882687f, -1.960900f, -1.843587f, -2.843587f, -2.019550f, -0.980450f, -1.980450f, -2.350841f, -2.863137f},
		{0.000000f, -0.370391f, 0.039100f, -1.882687f, 0.980450f, -1.843587f, 1.019550f, 0.019550f, -0.350841f, -0.863137f, -1.863137f, 1.000000f},
		{0.000000f, -1.000000f, -0.882687f, -0.960900f, -0.843587f, -1.136863f, -1.019550f, 0.019550f, 0.136863f, -0.156413f, -0.039100f, -0.117313f},
		{0.000000f, 0.117313f, 0.039100f, 0.156413f, -0.136863f, -0.019550f, 0.097763f, 0.019550f, 0.136863f, -0.156413f, -0.039100f, -0.117313f},
		{0.000000f, -0.643032f, 0.039100f, -0.331291f, -0.136863f, -0.292191f, -0.486821f, 0.019550f, -0.623482f, -0.594723f, -0.311741f, -0.117313f},
		{0.000000f, 0.332376f, -0.175963f, 0.863137f, 0.492746f, -0.019550f, 0.531846f, -0.195513f, 0.351926f, -0.156413f, -0.526804f, 0.512296f},
		{0.000000f, 0.126035f, 0.044326f, 0.170361f, -0.148198f, -0.022163f, -0.103872f, 0.022163f, 0.148198f, -0.170361f, 0.192523f, -0.126035f},
		{0.000000f, 1.229950f, 0.229950f, 1.884990f, 0.884990f, -0.115010f, 1.114487f, 0.114487f, 1.344885f, 0.344885f, 2.000000f, 1.000000f},
		{0.000000f, 0.049554f, -0.882687f, 0.863137f, -0.079251f, -0.019550f, -6.000000f, 0.019550f, 0.069104f, -0.874412f, 0.786239f, -0.117313f},
		{0.000000f, -0.121012f, 0.039100f, 0.276222f, -0.136863f, -0.019550f, -0.226480f, 0.019550f, -0.044420f, -0.156413f, 0.095627f, -0.117313f},
		{0.000000f, -0.184729f, -0.039414f, 0.077899f, -0.101647f, 0.029341f, -0.132614f, -0.019663f, -0.200246f, -0.072405f, 0.053675f, -0.109481f}
};








