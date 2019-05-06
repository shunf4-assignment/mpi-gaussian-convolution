#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <mpi/mpi.h>
#include <string.h>

#define BMP_MAGIC 19778

#define PIX(y,x) bmpdata + (y) * rowbytes + (x) * 3
#define OPIX(y,x) obmpdata + (y) * rowbytes + (x) * 3
#define RED +2
#define BLUE +0
#define GREEN +1

double weight_table[7][256] = {
	{0,0.01441881,0.02883762,0.04325643,0.05767524,0.07209405,0.08651286,0.10093167,0.11535048,0.12976929,0.1441881,0.15860691,0.17302572,0.18744453,0.20186334,0.21628215,0.23070096,0.24511977,0.25953858,0.27395739,0.2883762,0.30279501,0.31721382,0.33163263,0.34605144,0.36047025,0.37488906,0.38930787,0.40372668,0.41814549,0.4325643,0.44698311,0.46140192,0.47582073,0.49023954,0.50465835,0.51907716,0.53349597,0.54791478,0.56233359,0.5767524,0.59117121,0.60559002,0.62000883,0.63442764,0.64884645,0.66326526,0.67768407,0.69210288,0.70652169,0.7209405,0.73535931,0.74977812,0.76419693,0.77861574,0.79303455,0.80745336,0.82187217,0.83629098,0.85070979,0.8651286,0.87954741,0.89396622,0.90838503,0.92280384,0.93722265,0.95164146,0.96606027,0.98047908,0.99489789,1.0093167,1.02373551,1.03815432,1.05257313,1.06699194,1.08141075,1.09582956,1.11024837,1.12466718,1.13908599,1.1535048,1.16792361,1.18234242,1.19676123,1.21118004,1.22559885,1.24001766,1.25443647,1.26885528,1.28327409,1.2976929,1.31211171,1.32653052,1.34094933,1.35536814,1.36978695,1.38420576,1.39862457,1.41304338,1.42746219,1.441881,1.45629981,1.47071862,1.48513743,1.49955624,1.51397505,1.52839386,1.54281267,1.55723148,1.57165029,1.5860691,1.60048791,1.61490672,1.62932553,1.64374434,1.65816315,1.67258196,1.68700077,1.70141958,1.71583839,1.7302572,1.74467601,1.75909482,1.77351363,1.78793244,1.80235125,1.81677006,1.83118887,1.84560768,1.86002649,1.8744453,1.88886411,1.90328292,1.91770173,1.93212054,1.94653935,1.96095816,1.97537697,1.98979578,2.00421459,2.0186334,2.03305221,2.04747102,2.06188983,2.07630864,2.09072745,2.10514626,2.11956507,2.13398388,2.14840269,2.1628215,2.17724031,2.19165912,2.20607793,2.22049674,2.23491555,2.24933436,2.26375317,2.27817198,2.29259079,2.3070096,2.32142841,2.33584722,2.35026603,2.36468484,2.37910365,2.39352246,2.40794127,2.42236008,2.43677889,2.4511977,2.46561651,2.48003532,2.49445413,2.50887294,2.52329175,2.53771056,2.55212937,2.56654818,2.58096699,2.5953858,2.60980461,2.62422342,2.63864223,2.65306104,2.66747985,2.68189866,2.69631747,2.71073628,2.72515509,2.7395739,2.75399271,2.76841152,2.78283033,2.79724914,2.81166795,2.82608676,2.84050557,2.85492438,2.86934319,2.883762,2.89818081,2.91259962,2.92701843,2.94143724,2.95585605,2.97027486,2.98469367,2.99911248,3.01353129,3.0279501,3.04236891,3.05678772,3.07120653,3.08562534,3.10004415,3.11446296,3.12888177,3.14330058,3.15771939,3.1721382,3.18655701,3.20097582,3.21539463,3.22981344,3.24423225,3.25865106,3.27306987,3.28748868,3.30190749,3.3163263,3.33074511,3.34516392,3.35958273,3.37400154,3.38842035,3.40283916,3.41725797,3.43167678,3.44609559,3.4605144,3.47493321,3.48935202,3.50377083,3.51818964,3.53260845,3.54702726,3.56144607,3.57586488,3.59028369,3.6047025,3.61912131,3.63354012,3.64795893,3.66237774,3.67679655,},
	{0,0.02808402,0.05616804,0.08425206,0.11233608,0.1404201,0.16850412,0.19658814,0.22467216,0.25275618,0.2808402,0.30892422,0.33700824,0.36509226,0.39317628,0.4212603,0.44934432,0.47742834,0.50551236,0.53359638,0.5616804,0.58976442,0.61784844,0.64593246,0.67401648,0.7021005,0.73018452,0.75826854,0.78635256,0.81443658,0.8425206,0.87060462,0.89868864,0.92677266,0.95485668,0.9829407,1.01102472,1.03910874,1.06719276,1.09527678,1.1233608,1.15144482,1.17952884,1.20761286,1.23569688,1.2637809,1.29186492,1.31994894,1.34803296,1.37611698,1.404201,1.43228502,1.46036904,1.48845306,1.51653708,1.5446211,1.57270512,1.60078914,1.62887316,1.65695718,1.6850412,1.71312522,1.74120924,1.76929326,1.79737728,1.8254613,1.85354532,1.88162934,1.90971336,1.93779738,1.9658814,1.99396542,2.02204944,2.05013346,2.07821748,2.1063015,2.13438552,2.16246954,2.19055356,2.21863758,2.2467216,2.27480562,2.30288964,2.33097366,2.35905768,2.3871417,2.41522572,2.44330974,2.47139376,2.49947778,2.5275618,2.55564582,2.58372984,2.61181386,2.63989788,2.6679819,2.69606592,2.72414994,2.75223396,2.78031798,2.808402,2.83648602,2.86457004,2.89265406,2.92073808,2.9488221,2.97690612,3.00499014,3.03307416,3.06115818,3.0892422,3.11732622,3.14541024,3.17349426,3.20157828,3.2296623,3.25774632,3.28583034,3.31391436,3.34199838,3.3700824,3.39816642,3.42625044,3.45433446,3.48241848,3.5105025,3.53858652,3.56667054,3.59475456,3.62283858,3.6509226,3.67900662,3.70709064,3.73517466,3.76325868,3.7913427,3.81942672,3.84751074,3.87559476,3.90367878,3.9317628,3.95984682,3.98793084,4.01601486,4.04409888,4.0721829,4.10026692,4.12835094,4.15643496,4.18451898,4.212603,4.24068702,4.26877104,4.29685506,4.32493908,4.3530231,4.38110712,4.40919114,4.43727516,4.46535918,4.4934432,4.52152722,4.54961124,4.57769526,4.60577928,4.6338633,4.66194732,4.69003134,4.71811536,4.74619938,4.7742834,4.80236742,4.83045144,4.85853546,4.88661948,4.9147035,4.94278752,4.97087154,4.99895556,5.02703958,5.0551236,5.08320762,5.11129164,5.13937566,5.16745968,5.1955437,5.22362772,5.25171174,5.27979576,5.30787978,5.3359638,5.36404782,5.39213184,5.42021586,5.44829988,5.4763839,5.50446792,5.53255194,5.56063596,5.58871998,5.616804,5.64488802,5.67297204,5.70105606,5.72914008,5.7572241,5.78530812,5.81339214,5.84147616,5.86956018,5.8976442,5.92572822,5.95381224,5.98189626,6.00998028,6.0380643,6.06614832,6.09423234,6.12231636,6.15040038,6.1784844,6.20656842,6.23465244,6.26273646,6.29082048,6.3189045,6.34698852,6.37507254,6.40315656,6.43124058,6.4593246,6.48740862,6.51549264,6.54357666,6.57166068,6.5997447,6.62782872,6.65591274,6.68399676,6.71208078,6.7401648,6.76824882,6.79633284,6.82441686,6.85250088,6.8805849,6.90866892,6.93675294,6.96483696,6.99292098,7.021005,7.04908902,7.07717304,7.10525706,7.13334108,7.1614251,},
	{0,0.0350727,0.0701454,0.1052181,0.1402908,0.1753635,0.2104362,0.2455089,0.2805816,0.3156543,0.350727,0.3857997,0.4208724,0.4559451,0.4910178,0.5260905,0.5611632,0.5962359,0.6313086,0.6663813,0.701454,0.7365267,0.7715994,0.8066721,0.8417448,0.8768175,0.9118902,0.9469629,0.9820356,1.0171083,1.052181,1.0872537,1.1223264,1.1573991,1.1924718,1.2275445,1.2626172,1.2976899,1.3327626,1.3678353,1.402908,1.4379807,1.4730534,1.5081261,1.5431988,1.5782715,1.6133442,1.6484169,1.6834896,1.7185623,1.753635,1.7887077,1.8237804,1.8588531,1.8939258,1.9289985,1.9640712,1.9991439,2.0342166,2.0692893,2.104362,2.1394347,2.1745074,2.2095801,2.2446528,2.2797255,2.3147982,2.3498709,2.3849436,2.4200163,2.455089,2.4901617,2.5252344,2.5603071,2.5953798,2.6304525,2.6655252,2.7005979,2.7356706,2.7707433,2.805816,2.8408887,2.8759614,2.9110341,2.9461068,2.9811795,3.0162522,3.0513249,3.0863976,3.1214703,3.156543,3.1916157,3.2266884,3.2617611,3.2968338,3.3319065,3.3669792,3.4020519,3.4371246,3.4721973,3.50727,3.5423427,3.5774154,3.6124881,3.6475608,3.6826335,3.7177062,3.7527789,3.7878516,3.8229243,3.857997,3.8930697,3.9281424,3.9632151,3.9982878,4.0333605,4.0684332,4.1035059,4.1385786,4.1736513,4.208724,4.2437967,4.2788694,4.3139421,4.3490148,4.3840875,4.4191602,4.4542329,4.4893056,4.5243783,4.559451,4.5945237,4.6295964,4.6646691,4.6997418,4.7348145,4.7698872,4.8049599,4.8400326,4.8751053,4.910178,4.9452507,4.9803234,5.0153961,5.0504688,5.0855415,5.1206142,5.1556869,5.1907596,5.2258323,5.260905,5.2959777,5.3310504,5.3661231,5.4011958,5.4362685,5.4713412,5.5064139,5.5414866,5.5765593,5.611632,5.6467047,5.6817774,5.7168501,5.7519228,5.7869955,5.8220682,5.8571409,5.8922136,5.9272863,5.962359,5.9974317,6.0325044,6.0675771,6.1026498,6.1377225,6.1727952,6.2078679,6.2429406,6.2780133,6.313086,6.3481587,6.3832314,6.4183041,6.4533768,6.4884495,6.5235222,6.5585949,6.5936676,6.6287403,6.663813,6.6988857,6.7339584,6.7690311,6.8041038,6.8391765,6.8742492,6.9093219,6.9443946,6.9794673,7.01454,7.0496127,7.0846854,7.1197581,7.1548308,7.1899035,7.2249762,7.2600489,7.2951216,7.3301943,7.365267,7.4003397,7.4354124,7.4704851,7.5055578,7.5406305,7.5757032,7.6107759,7.6458486,7.6809213,7.715994,7.7510667,7.7861394,7.8212121,7.8562848,7.8913575,7.9264302,7.9615029,7.9965756,8.0316483,8.066721,8.1017937,8.1368664,8.1719391,8.2070118,8.2420845,8.2771572,8.3122299,8.3473026,8.3823753,8.417448,8.4525207,8.4875934,8.5226661,8.5577388,8.5928115,8.6278842,8.6629569,8.6980296,8.7331023,8.768175,8.8032477,8.8383204,8.8733931,8.9084658,8.9435385,},
	{0,0.0547002,0.1094004,0.1641006,0.2188008,0.273501,0.3282012,0.3829014,0.4376016,0.4923018,0.547002,0.6017022,0.6564024,0.7111026,0.7658028,0.820503,0.8752032,0.9299034,0.9846036,1.0393038,1.094004,1.1487042,1.2034044,1.2581046,1.3128048,1.367505,1.4222052,1.4769054,1.5316056,1.5863058,1.641006,1.6957062,1.7504064,1.8051066,1.8598068,1.914507,1.9692072,2.0239074,2.0786076,2.1333078,2.188008,2.2427082,2.2974084,2.3521086,2.4068088,2.461509,2.5162092,2.5709094,2.6256096,2.6803098,2.73501,2.7897102,2.8444104,2.8991106,2.9538108,3.008511,3.0632112,3.1179114,3.1726116,3.2273118,3.282012,3.3367122,3.3914124,3.4461126,3.5008128,3.555513,3.6102132,3.6649134,3.7196136,3.7743138,3.829014,3.8837142,3.9384144,3.9931146,4.0478148,4.102515,4.1572152,4.2119154,4.2666156,4.3213158,4.376016,4.4307162,4.4854164,4.5401166,4.5948168,4.649517,4.7042172,4.7589174,4.8136176,4.8683178,4.923018,4.9777182,5.0324184,5.0871186,5.1418188,5.196519,5.2512192,5.3059194,5.3606196,5.4153198,5.47002,5.5247202,5.5794204,5.6341206,5.6888208,5.743521,5.7982212,5.8529214,5.9076216,5.9623218,6.017022,6.0717222,6.1264224,6.1811226,6.2358228,6.290523,6.3452232,6.3999234,6.4546236,6.5093238,6.564024,6.6187242,6.6734244,6.7281246,6.7828248,6.837525,6.8922252,6.9469254,7.0016256,7.0563258,7.111026,7.1657262,7.2204264,7.2751266,7.3298268,7.384527,7.4392272,7.4939274,7.5486276,7.6033278,7.658028,7.7127282,7.7674284,7.8221286,7.8768288,7.931529,7.9862292,8.0409294,8.0956296,8.1503298,8.20503,8.2597302,8.3144304,8.3691306,8.4238308,8.478531,8.5332312,8.5879314,8.6426316,8.6973318,8.752032,8.8067322,8.8614324,8.9161326,8.9708328,9.025533,9.0802332,9.1349334,9.1896336,9.2443338,9.299034,9.3537342,9.4084344,9.4631346,9.5178348,9.572535,9.6272352,9.6819354,9.7366356,9.7913358,9.846036,9.9007362,9.9554364,10.0101366,10.0648368,10.119537,10.1742372,10.2289374,10.2836376,10.3383378,10.393038,10.4477382,10.5024384,10.5571386,10.6118388,10.666539,10.7212392,10.7759394,10.8306396,10.8853398,10.94004,10.9947402,11.0494404,11.1041406,11.1588408,11.213541,11.2682412,11.3229414,11.3776416,11.4323418,11.487042,11.5417422,11.5964424,11.6511426,11.7058428,11.760543,11.8152432,11.8699434,11.9246436,11.9793438,12.034044,12.0887442,12.1434444,12.1981446,12.2528448,12.307545,12.3622452,12.4169454,12.4716456,12.5263458,12.581046,12.6357462,12.6904464,12.7451466,12.7998468,12.854547,12.9092472,12.9639474,13.0186476,13.0733478,13.128048,13.1827482,13.2374484,13.2921486,13.3468488,13.401549,13.4562492,13.5109494,13.5656496,13.6203498,13.67505,13.7297502,13.7844504,13.8391506,13.8938508,13.948551,},
	{0,0.06831229,0.13662458,0.20493687,0.27324916,0.34156145,0.40987374,0.47818603,0.54649832,0.61481061,0.6831229,0.75143519,0.81974748,0.88805977,0.95637206,1.02468435,1.09299664,1.16130893,1.22962122,1.29793351,1.3662458,1.43455809,1.50287038,1.57118267,1.63949496,1.70780725,1.77611954,1.84443183,1.91274412,1.98105641,2.0493687,2.11768099,2.18599328,2.25430557,2.32261786,2.39093015,2.45924244,2.52755473,2.59586702,2.66417931,2.7324916,2.80080389,2.86911618,2.93742847,3.00574076,3.07405305,3.14236534,3.21067763,3.27898992,3.34730221,3.4156145,3.48392679,3.55223908,3.62055137,3.68886366,3.75717595,3.82548824,3.89380053,3.96211282,4.03042511,4.0987374,4.16704969,4.23536198,4.30367427,4.37198656,4.44029885,4.50861114,4.57692343,4.64523572,4.71354801,4.7818603,4.85017259,4.91848488,4.98679717,5.05510946,5.12342175,5.19173404,5.26004633,5.32835862,5.39667091,5.4649832,5.53329549,5.60160778,5.66992007,5.73823236,5.80654465,5.87485694,5.94316923,6.01148152,6.07979381,6.1481061,6.21641839,6.28473068,6.35304297,6.42135526,6.48966755,6.55797984,6.62629213,6.69460442,6.76291671,6.831229,6.89954129,6.96785358,7.03616587,7.10447816,7.17279045,7.24110274,7.30941503,7.37772732,7.44603961,7.5143519,7.58266419,7.65097648,7.71928877,7.78760106,7.85591335,7.92422564,7.99253793,8.06085022,8.12916251,8.1974748,8.26578709,8.33409938,8.40241167,8.47072396,8.53903625,8.60734854,8.67566083,8.74397312,8.81228541,8.8805977,8.94890999,9.01722228,9.08553457,9.15384686,9.22215915,9.29047144,9.35878373,9.42709602,9.49540831,9.5637206,9.63203289,9.70034518,9.76865747,9.83696976,9.90528205,9.97359434,10.04190663,10.11021892,10.17853121,10.2468435,10.31515579,10.38346808,10.45178037,10.52009266,10.58840495,10.65671724,10.72502953,10.79334182,10.86165411,10.9299664,10.99827869,11.06659098,11.13490327,11.20321556,11.27152785,11.33984014,11.40815243,11.47646472,11.54477701,11.6130893,11.68140159,11.74971388,11.81802617,11.88633846,11.95465075,12.02296304,12.09127533,12.15958762,12.22789991,12.2962122,12.36452449,12.43283678,12.50114907,12.56946136,12.63777365,12.70608594,12.77439823,12.84271052,12.91102281,12.9793351,13.04764739,13.11595968,13.18427197,13.25258426,13.32089655,13.38920884,13.45752113,13.52583342,13.59414571,13.662458,13.73077029,13.79908258,13.86739487,13.93570716,14.00401945,14.07233174,14.14064403,14.20895632,14.27726861,14.3455809,14.41389319,14.48220548,14.55051777,14.61883006,14.68714235,14.75545464,14.82376693,14.89207922,14.96039151,15.0287038,15.09701609,15.16532838,15.23364067,15.30195296,15.37026525,15.43857754,15.50688983,15.57520212,15.64351441,15.7118267,15.78013899,15.84845128,15.91676357,15.98507586,16.05338815,16.12170044,16.19001273,16.25832502,16.32663731,16.3949496,16.46326189,16.53157418,16.59988647,16.66819876,16.73651105,16.80482334,16.87313563,16.94144792,17.00976021,17.0780725,17.14638479,17.21469708,17.28300937,17.35132166,17.41963395,},
	{0,0.08531173,0.17062346,0.25593519,0.34124692,0.42655865,0.51187038,0.59718211,0.68249384,0.76780557,0.8531173,0.93842903,1.02374076,1.10905249,1.19436422,1.27967595,1.36498768,1.45029941,1.53561114,1.62092287,1.7062346,1.79154633,1.87685806,1.96216979,2.04748152,2.13279325,2.21810498,2.30341671,2.38872844,2.47404017,2.5593519,2.64466363,2.72997536,2.81528709,2.90059882,2.98591055,3.07122228,3.15653401,3.24184574,3.32715747,3.4124692,3.49778093,3.58309266,3.66840439,3.75371612,3.83902785,3.92433958,4.00965131,4.09496304,4.18027477,4.2655865,4.35089823,4.43620996,4.52152169,4.60683342,4.69214515,4.77745688,4.86276861,4.94808034,5.03339207,5.1187038,5.20401553,5.28932726,5.37463899,5.45995072,5.54526245,5.63057418,5.71588591,5.80119764,5.88650937,5.9718211,6.05713283,6.14244456,6.22775629,6.31306802,6.39837975,6.48369148,6.56900321,6.65431494,6.73962667,6.8249384,6.91025013,6.99556186,7.08087359,7.16618532,7.25149705,7.33680878,7.42212051,7.50743224,7.59274397,7.6780557,7.76336743,7.84867916,7.93399089,8.01930262,8.10461435,8.18992608,8.27523781,8.36054954,8.44586127,8.531173,8.61648473,8.70179646,8.78710819,8.87241992,8.95773165,9.04304338,9.12835511,9.21366684,9.29897857,9.3842903,9.46960203,9.55491376,9.64022549,9.72553722,9.81084895,9.89616068,9.98147241,10.06678414,10.15209587,10.2374076,10.32271933,10.40803106,10.49334279,10.57865452,10.66396625,10.74927798,10.83458971,10.91990144,11.00521317,11.0905249,11.17583663,11.26114836,11.34646009,11.43177182,11.51708355,11.60239528,11.68770701,11.77301874,11.85833047,11.9436422,12.02895393,12.11426566,12.19957739,12.28488912,12.37020085,12.45551258,12.54082431,12.62613604,12.71144777,12.7967595,12.88207123,12.96738296,13.05269469,13.13800642,13.22331815,13.30862988,13.39394161,13.47925334,13.56456507,13.6498768,13.73518853,13.82050026,13.90581199,13.99112372,14.07643545,14.16174718,14.24705891,14.33237064,14.41768237,14.5029941,14.58830583,14.67361756,14.75892929,14.84424102,14.92955275,15.01486448,15.10017621,15.18548794,15.27079967,15.3561114,15.44142313,15.52673486,15.61204659,15.69735832,15.78267005,15.86798178,15.95329351,16.03860524,16.12391697,16.2092287,16.29454043,16.37985216,16.46516389,16.55047562,16.63578735,16.72109908,16.80641081,16.89172254,16.97703427,17.062346,17.14765773,17.23296946,17.31828119,17.40359292,17.48890465,17.57421638,17.65952811,17.74483984,17.83015157,17.9154633,18.00077503,18.08608676,18.17139849,18.25671022,18.34202195,18.42733368,18.51264541,18.59795714,18.68326887,18.7685806,18.85389233,18.93920406,19.02451579,19.10982752,19.19513925,19.28045098,19.36576271,19.45107444,19.53638617,19.6216979,19.70700963,19.79232136,19.87763309,19.96294482,20.04825655,20.13356828,20.21888001,20.30419174,20.38950347,20.4748152,20.56012693,20.64543866,20.73075039,20.81606212,20.90137385,20.98668558,21.07199731,21.15730904,21.24262077,21.3279325,21.41324423,21.49855596,21.58386769,21.66917942,21.75449115,},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
}
;

typedef struct _bmp_header
{
	unsigned int   bfSize;
	unsigned int   bfReserved;
	unsigned int   bfOffBits;
	
	unsigned int   biSize;
	int            biWidth;
	int            biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int   biCompression;
	unsigned int   biSizeImage;
	int            biXPelsPerMeter;
	int            biYPelsPerMeter;
	unsigned int   biClrUsed;
	unsigned int   biClrImportant;
} bmp_header;

typedef struct _bmp_pixel
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
} bmp_pixel;

// This is faster than a function call
#define BMP_PIXEL(r,g,b) ((bmp_pixel){(b),(g),(r)})

// 约束:
//      卷积核为 5x5 且对称(只有六种不同权值)
//      这样可以不把倒 BMP 倒过来, 直接查表进行高斯

// mmap 的方法本质上是各进程在读取文件时, 直接把内存中的页缓存拿出来给应用程序用
void process_bmp(const char *filename, const char *output) {
    double stime = 0;
    //double stime = MPI_Wtime();

    int sz;
	int mr;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &mr);

    double stime_aftermpiinit = 0;
    //double stime_aftermpiinit = MPI_Wtime();

    int fd = open(filename, O_RDWR, (mode_t)0600);
    int ofd = open(output, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0660);
    int res;
    bmp_header header;

    if (fd == -1 || ofd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    

    unsigned short magic;
    res = read(fd, &magic, sizeof(magic));
    if (res <= 0 || magic != BMP_MAGIC) {
        fprintf(stderr, "%hu\n", magic);
        perror("Error reading magic || magic incorrect");
        exit(EXIT_FAILURE);
    }

    res = read(fd, &header, sizeof(header));
    if (res < sizeof(header)) {
        perror("error");
        exit(EXIT_FAILURE);
    }

#if 0
    printf("%u\n", header.bfSize);
    printf("%u\n", header.bfOffBits);
    printf("%u\n", header.biSize);
    printf("%d\n", header.biWidth);
    printf("%d\n", header.biHeight);
    printf("%hu\n", header.biPlanes);
    printf("%hu\n", header.biBitCount);
    printf("%u\n", header.biCompression);
    printf("%u\n", header.biSizeImage);
    printf("%d\n", header.biXPelsPerMeter);
    printf("%d\n", header.biYPelsPerMeter);
    printf("%u\n", header.biClrUsed);
    printf("%u\n", header.biClrImportant);
#endif

    if (header.biWidth < 0 || header.biBitCount != 24 || header.biCompression != 0 || header.biClrUsed != 0) {
        fprintf(stderr, "BMP format does not meet the req of this program\n");
        exit(EXIT_FAILURE);
    }

    // mmap 输入文件
    unsigned char *map = mmap(NULL, header.bfSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    unsigned char *bmpdata = map + sizeof(short) + sizeof(bmp_header);

    if (map == MAP_FAILED) {
        close(fd);
        perror("Error mmapping in file");
        exit(EXIT_FAILURE);
    }

    // 扩充输出文件的大小, 然后 mmap
    lseek(ofd, header.bfSize - 1, SEEK_SET);
    write(ofd, "", 1);
    unsigned char *omap = mmap(NULL, header.bfSize, PROT_READ | PROT_WRITE, MAP_SHARED, ofd, 0);
    if (omap == MAP_FAILED) {
        close(fd);
        perror("Error mmapping out file");
        exit(EXIT_FAILURE);
    }

    unsigned char *obmpdata = omap + sizeof(short) + sizeof(bmp_header);

    unsigned rowbytes = header.biWidth * 3 + /* padding = 4 - header.biWidth * 3 % 4 = */ header.biWidth % 4;

    
    int width = header.biWidth;
    int height = header.biHeight;


    int start = (height + sz - 1) / sz * mr;
    int end = start + (height + sz - 1) / sz - 1;
    if (end >= height)
        end = height - 1;
    int size = (end - start + 1) * width;
    const int INVALID = 6;

    for (int y = start; y <= end; y++) {
        int x00 = INVALID, x01 = INVALID, x03 = 1, x04 = 0;
        int x10 = INVALID, x11 = INVALID, x13 = 3, x14 = 1;
        int x20 = INVALID, x21 = INVALID, x23 = 4, x24 = 2;
        for (int x = 0; x < width; x++) {
            double red = 0.5;
            double green = 0.5;
            double blue = 0.5;

            if (x == 1) {
                x01 = 1;
                x11 = 3;
                x21 = 4;
            }

            if (x == 2) {
                x00 = 0;
                x10 = 1;
                x20 = 2;
            }

            if (x == width - 2) {
                x04 = INVALID;
                x14 = INVALID;
                x24 = INVALID;
            }

            if (x == width - 1) {
                x03 = INVALID;
                x13 = INVALID;
                x23 = INVALID;
            }

            // [0, 1, 2, 1, 0],
            // [1, 3, 4, 3, 1],
            // [2, 4, 5, 4, 2],
            // [1, 3, 4, 3, 1],
            // [0, 1, 2, 1, 0],

            if (y < 1)
                goto ylessthan1;
            if (y < 2)
                goto ylessthan2;

            

            red +=      weight_table[x00][*(PIX(y - 2, x - 2) RED)];
            green +=    weight_table[x00][*(PIX(y - 2, x - 2) GREEN)];
            blue +=     weight_table[x00][*(PIX(y - 2, x - 2) BLUE)];

            red +=      weight_table[x01][*(PIX(y - 2, x - 1) RED)];
            green +=    weight_table[x01][*(PIX(y - 2, x - 1) GREEN)];
            blue +=     weight_table[x01][*(PIX(y - 2, x - 1) BLUE)];

            red +=      weight_table[2][*(PIX(y - 2, x) RED)];
            green +=    weight_table[2][*(PIX(y - 2, x) GREEN)];
            blue +=     weight_table[2][*(PIX(y - 2, x) BLUE)];

            red +=      weight_table[x03][*(PIX(y - 2, x + 1) RED)];
            green +=    weight_table[x03][*(PIX(y - 2, x + 1) GREEN)];
            blue +=     weight_table[x03][*(PIX(y - 2, x + 1) BLUE)];

            red +=      weight_table[x04][*(PIX(y - 2, x + 2) RED)];
            green +=    weight_table[x04][*(PIX(y - 2, x + 2) GREEN)];
            blue +=     weight_table[x04][*(PIX(y - 2, x + 2) BLUE)];

ylessthan2:

            red +=      weight_table[x10][*(PIX(y - 1, x - 2) RED)];
            green +=    weight_table[x10][*(PIX(y - 1, x - 2) GREEN)];
            blue +=     weight_table[x10][*(PIX(y - 1, x - 2) BLUE)];

            red +=      weight_table[x11][*(PIX(y - 1, x - 1) RED)];
            green +=    weight_table[x11][*(PIX(y - 1, x - 1) GREEN)];
            blue +=     weight_table[x11][*(PIX(y - 1, x - 1) BLUE)];

            red +=      weight_table[4][*(PIX(y - 1, x) RED)];
            green +=    weight_table[4][*(PIX(y - 1, x) GREEN)];
            blue +=     weight_table[4][*(PIX(y - 1, x) BLUE)];

            red +=      weight_table[x13][*(PIX(y - 1, x + 1) RED)];
            green +=    weight_table[x13][*(PIX(y - 1, x + 1) GREEN)];
            blue +=     weight_table[x13][*(PIX(y - 1, x + 1) BLUE)];

            red +=      weight_table[x14][*(PIX(y - 1, x + 2) RED)];
            green +=    weight_table[x14][*(PIX(y - 1, x + 2) GREEN)];
            blue +=     weight_table[x14][*(PIX(y - 1, x + 2) BLUE)];

ylessthan1:

            red +=      weight_table[x20][*(PIX(y, x - 2) RED)];
            green +=    weight_table[x20][*(PIX(y, x - 2) GREEN)];
            blue +=     weight_table[x20][*(PIX(y, x - 2) BLUE)];

            red +=      weight_table[x21][*(PIX(y, x - 1) RED)];
            green +=    weight_table[x21][*(PIX(y, x - 1) GREEN)];
            blue +=     weight_table[x21][*(PIX(y, x - 1) BLUE)];

            red +=      weight_table[5][*(PIX(y, x) RED)];
            green +=    weight_table[5][*(PIX(y, x) GREEN)];
            blue +=     weight_table[5][*(PIX(y, x) BLUE)];

            red +=      weight_table[x23][*(PIX(y, x + 1) RED)];
            green +=    weight_table[x23][*(PIX(y, x + 1) GREEN)];
            blue +=     weight_table[x23][*(PIX(y, x + 1) BLUE)];

            red +=      weight_table[x24][*(PIX(y, x + 2) RED)];
            green +=    weight_table[x24][*(PIX(y, x + 2) GREEN)];
            blue +=     weight_table[x24][*(PIX(y, x + 2) BLUE)];

            if (y >= height - 1)
                goto done;
ygreaterthan1:

            red +=      weight_table[x10][*(PIX(y + 1, x - 2) RED)];
            green +=    weight_table[x10][*(PIX(y + 1, x - 2) GREEN)];
            blue +=     weight_table[x10][*(PIX(y + 1, x - 2) BLUE)];

            red +=      weight_table[x11][*(PIX(y + 1, x - 1) RED)];
            green +=    weight_table[x11][*(PIX(y + 1, x - 1) GREEN)];
            blue +=     weight_table[x11][*(PIX(y + 1, x - 1) BLUE)];

            red +=      weight_table[4][*(PIX(y + 1, x) RED)];
            green +=    weight_table[4][*(PIX(y + 1, x) GREEN)];
            blue +=     weight_table[4][*(PIX(y + 1, x) BLUE)];

            red +=      weight_table[x13][*(PIX(y + 1, x + 1) RED)];
            green +=    weight_table[x13][*(PIX(y + 1, x + 1) GREEN)];
            blue +=     weight_table[x13][*(PIX(y + 1, x + 1) BLUE)];

            red +=      weight_table[x14][*(PIX(y + 1, x + 2) RED)];
            green +=    weight_table[x14][*(PIX(y + 1, x + 2) GREEN)];
            blue +=     weight_table[x14][*(PIX(y + 1, x + 2) BLUE)];

            if (y >= height - 2)
                goto done;

ygreaterthan2:

            red +=      weight_table[x00][*(PIX(y + 2, x - 2) RED)];
            green +=    weight_table[x00][*(PIX(y + 2, x - 2) GREEN)];
            blue +=     weight_table[x00][*(PIX(y + 2, x - 2) BLUE)];

            red +=      weight_table[x01][*(PIX(y + 2, x - 1) RED)];
            green +=    weight_table[x01][*(PIX(y + 2, x - 1) GREEN)];
            blue +=     weight_table[x01][*(PIX(y + 2, x - 1) BLUE)];

            red +=      weight_table[2][*(PIX(y + 2, x) RED)];
            green +=    weight_table[2][*(PIX(y + 2, x) GREEN)];
            blue +=     weight_table[2][*(PIX(y + 2, x) BLUE)];

            red +=      weight_table[x03][*(PIX(y + 2, x + 1) RED)];
            green +=    weight_table[x03][*(PIX(y + 2, x + 1) GREEN)];
            blue +=     weight_table[x03][*(PIX(y + 2, x + 1) BLUE)];

            red +=      weight_table[x04][*(PIX(y + 2, x + 2) RED)];
            green +=    weight_table[x04][*(PIX(y + 2, x + 2) GREEN)];
            blue +=     weight_table[x04][*(PIX(y + 2, x + 2) BLUE)];

done:
            *(OPIX(y, x) RED) = (char)red;
            *(OPIX(y, x) GREEN) = (char)green;
            *(OPIX(y, x) BLUE) = (char)blue;
        }
    }

    if (mr != 0) {
        int send = 1;
        MPI_Send(&send, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
		int recv;

        memcpy(omap, map, sizeof(short) + sizeof(bmp_header));

		for (int i = 1; i < sz; i++) {
			MPI_Recv(&recv, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		
        // 如果是应用到真的独立内存机器上, 须做如下改进:
        // 每一个页必须只由一个进程占有; 或每个进程只同步自己负责的段, 
        // 而这段可能跨页, 因此需要妥善处理.
		if (msync(omap, header.bfSize, MS_SYNC) == -1)
        {
            perror("Could not sync the file to disk");
        }
	}

    if (munmap(map, header.bfSize) == -1)
    {
        close(fd);
        close(ofd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    if (munmap(omap, header.bfSize) == -1)
    {
        close(fd);
        close(ofd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    close(fd);
    close(ofd);
    
    double etime = 0;
    //double etime = MPI_Wtime();
	printf("Process %d time: %lg\n", mr, etime - stime);
    printf("Process %d time (except MPI initialization): %lg\n", mr, etime - stime_aftermpiinit);

    MPI_Finalize();
}

int main(int argc, const char *const *argv) {
    if (argc != 3) {
		printf("Usage: %s <input_bmp_file> <output_bmp_file>\n", argv[0]);
		return 0;
	}

    process_bmp(argv[1], argv[2]);
    return 0;
}