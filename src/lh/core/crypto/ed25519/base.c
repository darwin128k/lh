#include <lh/core/crypto/ed25519/base.h>
#include <lh/core/crypto/ed25519/point/precomp/initializer.h>

/* Monocypher 4's tables (CC0 / BSD-2-Clause, Loup Vaillant), copied by a script
   from lib/monocypher/monocypher.c: b_window, b_comb_low, b_comb_high. */

const lh_crypto_ed25519_point_precomp_t
    m_crypto_ed25519_base_window[LH_CRYPTO_ED25519_BASE_TABLE_SIZE] = {
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(25967493, -14356035, 29566456, 3660896,
                                                   -12694345, 4014787, 27544626, -11754271,
                                                   -6079156, 2047605),
            lh_crypto_curve25519_field_initializer(-12545711, 934262, -2722910, 3049990, -727428,
                                                   9406986, 12720692, 5043384, 19500929, -15469378),
            lh_crypto_curve25519_field_initializer(-8738181, 4489570, 9688441, -14785194, 10184609,
                                                   -12363380, 29287919, 11864899, -24514362,
                                                   -4438546)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(15636291, -9688557, 24204773, -7912398, 616977,
                                                   -16685262, 27787600, -14772189, 28944400,
                                                   -1550024),
            lh_crypto_curve25519_field_initializer(16568933, 4717097, -11556148, -1102322, 15682896,
                                                   -11807043, 16354577, -11775962, 7689662,
                                                   11199574),
            lh_crypto_curve25519_field_initializer(30464156, -5976125, -11779434, -15670865,
                                                   23220365, 15915852, 7512774, 10017326, -17749093,
                                                   -9920357)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(10861363, 11473154, 27284546, 1981175, -30064349,
                                                   12577861, 32867885, 14515107, -15438304,
                                                   10819380),
            lh_crypto_curve25519_field_initializer(4708026, 6336745, 20377586, 9066809, -11272109,
                                                   6594696, -25653668, 12483688, -12668491,
                                                   5581306),
            lh_crypto_curve25519_field_initializer(19563160, 16186464, -29386857, 4097519, 10237984,
                                                   -4348115, 28542350, 13850243, -23678021,
                                                   -15815942)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(5153746, 9909285, 1723747, -2777874, 30523605,
                                                   5516873, 19480852, 5230134, -23952439,
                                                   -15175766),
            lh_crypto_curve25519_field_initializer(-30269007, -3463509, 7665486, 10083793, 28475525,
                                                   1649722, 20654025, 16520125, 30598449, 7715701),
            lh_crypto_curve25519_field_initializer(28881845, 14381568, 9657904, 3680757, -20181635,
                                                   7843316, -31400660, 1370708, 29794553,
                                                   -1409300)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(-22518993, -6692182, 14201702, -8745502,
                                                   -23510406, 8844726, 18474211, -1361450,
                                                   -13062696, 13821877),
            lh_crypto_curve25519_field_initializer(-6455177, -7839871, 3374702, -4740862, -27098617,
                                                   -10571707, 31655028, -7212327, 18853322,
                                                   -14220951),
            lh_crypto_curve25519_field_initializer(4566830, -12963868, -28974889, -12240689,
                                                   -7602672, -2830569, -8514358, -10431137, 2207753,
                                                   -3209784)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(-25154831, -4185821, 29681144, 7868801, -6854661,
                                                   -9423865, -12437364, -663000, -31111463,
                                                   -16132436),
            lh_crypto_curve25519_field_initializer(25576264, -2703214, 7349804, -11814844, 16472782,
                                                   9300885, 3844789, 15725684, 171356, 6466918),
            lh_crypto_curve25519_field_initializer(23103977, 13316479, 9739013, -16149481, 817875,
                                                   -15038942, 8965339, -14088058, -30714912,
                                                   16193877)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(-33521811, 3180713, -2394130, 14003687,
                                                   -16903474, -16270840, 17238398, 4729455,
                                                   -18074513, 9256800),
            lh_crypto_curve25519_field_initializer(-25182317, -4174131, 32336398, 5036987,
                                                   -21236817, 11360617, 22616405, 9761698,
                                                   -19827198, 630305),
            lh_crypto_curve25519_field_initializer(-13720693, 2639453, -24237460, -7406481, 9494427,
                                                   -5774029, -6554551, -15960994, -2449256,
                                                   -14291300)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(-3151181, -5046075, 9282714, 6866145, -31907062,
                                                   -863023, -18940575, 15033784, 25105118,
                                                   -7894876),
            lh_crypto_curve25519_field_initializer(-24326370, 15950226, -31801215, -14592823,
                                                   -11662737, -5090925, 1573892, -2625887, 2198790,
                                                   -15804619),
            lh_crypto_curve25519_field_initializer(-3099351, 10324967, -2241613, 7453183, -5446979,
                                                   -2735503, -13812022, -16236442, -32461234,
                                                   -12290683)),
};

const lh_crypto_ed25519_point_precomp_t
    m_crypto_ed25519_base_comb_low[LH_CRYPTO_ED25519_BASE_TABLE_SIZE] = {
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(-6816601, -2324159, -22559413, 124364, 18015490,
                                                   8373481, 19993724, 1979872, -18549925, 9085059),
            lh_crypto_curve25519_field_initializer(10306321, 403248, 14839893, 9633706, 8463310,
                                                   -8354981, -14305673, 14668847, 26301366,
                                                   2818560),
            lh_crypto_curve25519_field_initializer(-22701500, -3210264, -13831292, -2927732,
                                                   -16326337, -14016360, 12940910, 177905, 12165515,
                                                   -2397893)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(-12282262, -7022066, 9920413, -3064358,
                                                   -32147467, 2927790, 22392436, -14852487, 2719975,
                                                   16402117),
            lh_crypto_curve25519_field_initializer(-7236961, -4729776, 2685954, -6525055, -24242706,
                                                   -15940211, -6238521, 14082855, 10047669,
                                                   12228189),
            lh_crypto_curve25519_field_initializer(-30495588, -12893761, -11161261, 3539405,
                                                   -11502464, 16491580, -27286798, -15030530,
                                                   -7272871, -15934455)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(17650926, 582297, -860412, -187745, -12072900,
                                                   -10683391, -20352381, 15557840, -31072141,
                                                   -5019061),
            lh_crypto_curve25519_field_initializer(-6283632, -2259834, -4674247, -4598977, -4089240,
                                                   12435688, -31278303, 1060251, 6256175, 10480726),
            lh_crypto_curve25519_field_initializer(-13871026, 2026300, -21928428, -2741605,
                                                   -2406664, -8034988, 7355518, 15733500, -23379862,
                                                   7489131)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(6883359, 695140, 23196907, 9644202, -33430614,
                                                   11354760, -20134606, 6388313, -8263585,
                                                   -8491918),
            lh_crypto_curve25519_field_initializer(-7716174, -13605463, -13646110, 14757414,
                                                   -19430591, -14967316, 10359532, -11059670,
                                                   -21935259, 12082603),
            lh_crypto_curve25519_field_initializer(-11253345, -15943946, 10046784, 5414629,
                                                   24840771, 8086951, -6694742, 9868723, 15842692,
                                                   -16224787)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(9639399, 11810955, -24007778, -9320054, 3912937,
                                                   -9856959, 996125, -8727907, -8919186, -14097242),
            lh_crypto_curve25519_field_initializer(7248867, 14468564, 25228636, -8795035, 14346339,
                                                   8224790, 6388427, -7181107, 6468218, -8720783),
            lh_crypto_curve25519_field_initializer(15513115, 15439095, 7342322, -10157390, 18005294,
                                                   -7265713, 2186239, 4884640, 10826567, 7135781)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(-14204238, 5297536, -5862318, -6004934, 28095835,
                                                   4236101, -14203318, 1958636, -16816875, 3837147),
            lh_crypto_curve25519_field_initializer(-5511166, -13176782, -29588215, 12339465,
                                                   15325758, -15945770, -8813185, 11075932,
                                                   -19608050, -3776283),
            lh_crypto_curve25519_field_initializer(11728032, 9603156, -4637821, -5304487, -7827751,
                                                   2724948, 31236191, -16760175, -7268616,
                                                   14799772)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(-28842672, 4840636, -12047946, -9101456,
                                                   -1445464, 381905, -30977094, -16523389, 1290540,
                                                   12798615),
            lh_crypto_curve25519_field_initializer(27246947, -10320914, 14792098, -14518944,
                                                   5302070, -8746152, -3403974, -4149637, -27061213,
                                                   10749585),
            lh_crypto_curve25519_field_initializer(25572375, -6270368, -15353037, 16037944, 1146292,
                                                   32198, 23487090, 9585613, 24714571, -1418265)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(19844825, 282124, -17583147, 11004019, -32004269,
                                                   -2716035, 6105106, -1711007, -21010044,
                                                   14338445),
            lh_crypto_curve25519_field_initializer(8027505, 8191102, -18504907, -12335737, 25173494,
                                                   -5923905, 15446145, 7483684, -30440441,
                                                   10009108),
            lh_crypto_curve25519_field_initializer(-14134701, -4174411, 10246585, -14677495,
                                                   33553567, -14012935, 23366126, 15080531,
                                                   -7969992, 7663473)),
};

const lh_crypto_ed25519_point_precomp_t
    m_crypto_ed25519_base_comb_high[LH_CRYPTO_ED25519_BASE_TABLE_SIZE] = {
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(33055887, -4431773, -521787, 6654165, 951411,
                                                   -6266464, -5158124, 6995613, -5397442, -6985227),
            lh_crypto_curve25519_field_initializer(4014062, 6967095, -11977872, 3960002, 8001989,
                                                   5130302, -2154812, -1899602, -31954493,
                                                   -16173976),
            lh_crypto_curve25519_field_initializer(16271757, -9212948, 23792794, 731486, -25808309,
                                                   -3546396, 6964344, -4767590, 10976593,
                                                   10050757)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(2533007, -4288439, -24467768, -12387405,
                                                   -13450051, 14542280, 12876301, 13893535,
                                                   15067764, 8594792),
            lh_crypto_curve25519_field_initializer(20073501, -11623621, 3165391, -13119866,
                                                   13188608, -11540496, -10751437, -13482671,
                                                   29588810, 2197295),
            lh_crypto_curve25519_field_initializer(-1084082, 11831693, 6031797, 14062724, 14748428,
                                                   -8159962, -20721760, 11742548, 31368706,
                                                   13161200)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(2050412, -6457589, 15321215, 5273360, 25484180,
                                                   124590, -18187548, -7097255, -6691621,
                                                   -14604792),
            lh_crypto_curve25519_field_initializer(9938196, 2162889, -6158074, -1711248, 4278932,
                                                   -2598531, -22865792, -7168500, -24323168,
                                                   11746309),
            lh_crypto_curve25519_field_initializer(-22691768, -14268164, 5965485, 9383325, 20443693,
                                                   5854192, 28250679, -1381811, -10837134,
                                                   13717818)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(-8495530, 16382250, 9548884, -4971523, -4491811,
                                                   -3902147, 6182256, -12832479, 26628081,
                                                   10395408),
            lh_crypto_curve25519_field_initializer(27329048, -15853735, 7715764, 8717446, -9215518,
                                                   -14633480, 28982250, -5668414, 4227628, 242148),
            lh_crypto_curve25519_field_initializer(-13279943, -7986904, -7100016, 8764468,
                                                   -27276630, 3096719, 29678419, -9141299, 3906709,
                                                   11265498)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(11918285, 15686328, -17757323, -11217300,
                                                   -27548967, 4853165, -27168827, 6807359, 6871949,
                                                   -1075745),
            lh_crypto_curve25519_field_initializer(-29002610, 13984323, -27111812, -2713442,
                                                   28107359, -13266203, 6155126, 15104658, 3538727,
                                                   -7513788),
            lh_crypto_curve25519_field_initializer(14103158, 11233913, -33165269, 9279850, 31014152,
                                                   4335090, -1827936, 4590951, 13960841, 12787712)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(1469134, -16738009, 33411928, 13942824, 8092558,
                                                   -8778224, -11165065, 1437842, 22521552,
                                                   -2792954),
            lh_crypto_curve25519_field_initializer(31352705, -4807352, -25327300, 3962447, 12541566,
                                                   -9399651, -27425693, 7964818, -23829869,
                                                   5541287),
            lh_crypto_curve25519_field_initializer(-25732021, -6864887, 23848984, 3039395, -9147354,
                                                   6022816, -27421653, 10590137, 25309915,
                                                   -1584678)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(-22951376, 5048948, 31139401, -190316, -19542447,
                                                   -626310, -17486305, -16511925, -18851313,
                                                   -12985140),
            lh_crypto_curve25519_field_initializer(-9684890, 14681754, 30487568, 7717771, -10829709,
                                                   9630497, 30290549, -10531496, -27798994,
                                                   -13812825),
            lh_crypto_curve25519_field_initializer(5827835, 16097107, -24501327, 12094619, 7413972,
                                                   11447087, 28057551, -1793987, -14056981,
                                                   4359312)),
        lh_crypto_ed25519_point_precomp_initializer(
            lh_crypto_curve25519_field_initializer(26323183, 2342588, -21887793, -1623758, -6062284,
                                                   2107090, -28724907, 9036464, -19618351,
                                                   -13055189),
            lh_crypto_curve25519_field_initializer(-29697200, 14829398, -4596333, 14220089,
                                                   -30022969, 2955645, 12094100, -13693652,
                                                   -5941445, 7047569),
            lh_crypto_curve25519_field_initializer(-3201977, 14413268, -12058324, -16417589,
                                                   -9035655, -7224648, 9258160, 1399236, 30397584,
                                                   -5684634)),
};

const lh_crypto_ed25519_point_precomp_t *
lh_crypto_ed25519_base_get_window(void)
{
    return m_crypto_ed25519_base_window;
}

const lh_crypto_ed25519_point_precomp_t *
lh_crypto_ed25519_base_get_comb_low(void)
{
    return m_crypto_ed25519_base_comb_low;
}

const lh_crypto_ed25519_point_precomp_t *
lh_crypto_ed25519_base_get_comb_high(void)
{
    return m_crypto_ed25519_base_comb_high;
}
