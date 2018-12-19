/*************************************************
 * Public Constants
 *************************************************/


// Notes and frequencies, see after this for an array of bytes that reference these values
// although this may seem wasteful in memory, when it comes to adding background music we would
// have to have arrays of 2 bytes for the frequencies, but instead we can have an array of single
// bytes that represent the notes as there are less than 256 of them.


#define FNOTE_B0  31
#define FNOTE_C1  33
#define FNOTE_CS1 35
#define FNOTE_D1  37
#define FNOTE_DS1 39
#define FNOTE_E1  41
#define FNOTE_F1  44
#define FNOTE_FS1 46
#define FNOTE_G1  49
#define FNOTE_GS1 52
#define FNOTE_A1  55
#define FNOTE_AS1 58
#define FNOTE_B1  62
#define FNOTE_C2  65
#define FNOTE_CS2 69
#define FNOTE_D2  73
#define FNOTE_DS2 78
#define FNOTE_E2  82
#define FNOTE_F2  87
#define FNOTE_FS2 93
#define FNOTE_G2  98
#define FNOTE_GS2 104
#define FNOTE_A2  110
#define FNOTE_AS2 117
#define FNOTE_B2  123
#define FNOTE_C3  131
#define FNOTE_CS3 139
#define FNOTE_D3  147
#define FNOTE_DS3 156
#define FNOTE_E3  165
#define FNOTE_F3  175
#define FNOTE_FS3 185
#define FNOTE_G3  196
#define FNOTE_GS3 208
#define FNOTE_A3  220
#define FNOTE_AS3 233
#define FNOTE_B3  247
#define FNOTE_C4  262
#define FNOTE_CS4 277
#define FNOTE_D4  294
#define FNOTE_DS4 311
#define FNOTE_E4  330
#define FNOTE_F4  349
#define FNOTE_FS4 370
#define FNOTE_G4  392
#define FNOTE_GS4 415
#define FNOTE_A4  440
#define FNOTE_AS4 466
#define FNOTE_B4  494
#define FNOTE_C5  523
#define FNOTE_CS5 554
#define FNOTE_D5  587
#define FNOTE_DS5 622
#define FNOTE_E5  659
#define FNOTE_F5  698
#define FNOTE_FS5 740
#define FNOTE_G5  784
#define FNOTE_GS5 831
#define FNOTE_A5  880
#define FNOTE_AS5 932
#define FNOTE_B5  988
#define FNOTE_C6  1047
#define FNOTE_CS6 1109
#define FNOTE_D6  1175
#define FNOTE_DS6 1245
#define FNOTE_E6  1319
#define FNOTE_F6  1397
#define FNOTE_FS6 1480
#define FNOTE_G6  1568
#define FNOTE_GS6 1661
#define FNOTE_A6  1760
#define FNOTE_AS6 1865
#define FNOTE_B6  1976
#define FNOTE_C7  2093
#define FNOTE_CS7 2217
#define FNOTE_D7  2349
#define FNOTE_DS7 2489
#define FNOTE_E7  2637
#define FNOTE_F7  2794
#define FNOTE_FS7 2960
#define FNOTE_G7  3136
#define FNOTE_GS7 3322
#define FNOTE_A7  3520
#define FNOTE_AS7 3729
#define FNOTE_B7  3951
#define FNOTE_C8  4186
#define FNOTE_CS8 4435
#define FNOTE_D8  4699
#define FNOTE_DS8 4978


// These notes index the above in the array (after removal of negative sign)
#define NOTE_SILENCE 0
#define NOTE_B0  -1
#define NOTE_C1  -2
#define NOTE_CS1 -3
#define NOTE_D1  -4
#define NOTE_DS1 -5
#define NOTE_E1  -6
#define NOTE_F1  -7
#define NOTE_FS1 -8
#define NOTE_G1  -9
#define NOTE_GS1 -10
#define NOTE_A1  -11
#define NOTE_AS1 -12
#define NOTE_B1  -13
#define NOTE_C2  -14
#define NOTE_CS2 -15
#define NOTE_D2  -16
#define NOTE_DS2 -17
#define NOTE_E2  -18
#define NOTE_F2  -19
#define NOTE_FS2 -20
#define NOTE_G2  -21
#define NOTE_GS2 -22
#define NOTE_A2  -23
#define NOTE_AS2 -24
#define NOTE_B2  -25
#define NOTE_C3  -26
#define NOTE_CS3 -27
#define NOTE_D3  -28
#define NOTE_DS3 -29
#define NOTE_E3  -30
#define NOTE_F3  -31
#define NOTE_FS3 -32
#define NOTE_G3  -33
#define NOTE_GS3 -34
#define NOTE_A3  -35
#define NOTE_AS3 -36
#define NOTE_B3  -37
#define NOTE_C4  -38
#define NOTE_CS4 -39
#define NOTE_D4  -40
#define NOTE_DS4 -41
#define NOTE_E4  -42
#define NOTE_F4  -43
#define NOTE_FS4 -44
#define NOTE_G4  -45
#define NOTE_GS4 -46
#define NOTE_A4  -47
#define NOTE_AS4 -48
#define NOTE_B4  -49
#define NOTE_C5  -50
#define NOTE_CS5 -51
#define NOTE_D5  -52
#define NOTE_DS5 -53
#define NOTE_E5  -54
#define NOTE_F5  -55
#define NOTE_FS5 -56
#define NOTE_G5  -57
#define NOTE_GS5 -58
#define NOTE_A5  -59
#define NOTE_AS5 -60
#define NOTE_B5  -61
#define NOTE_C6  -62
#define NOTE_CS6 -63
#define NOTE_D6  -64
#define NOTE_DS6 -65
#define NOTE_E6  -66
#define NOTE_F6  -67
#define NOTE_FS6 -68
#define NOTE_G6  -69
#define NOTE_GS6 -70
#define NOTE_A6  -71
#define NOTE_AS6 -72
#define NOTE_B6  -73
#define NOTE_C7  -74
#define NOTE_CS7 -75
#define NOTE_D7  -76
#define NOTE_DS7 -77
#define NOTE_E7  -78
#define NOTE_F7  -79
#define NOTE_FS7 -80
#define NOTE_G7  -81
#define NOTE_GS7 -82
#define NOTE_A7  -83
#define NOTE_AS7 -84
#define NOTE_B7  -85
#define NOTE_C8  -86
#define NOTE_CS8 -87
#define NOTE_D8  -88
#define NOTE_DS8 -89
#define SCORE_END -127


// Tempos in BPM
#define TEMPO_LARGHISSIMO 20
#define TEMPO_GRAVE 30
#define TEMPO_LARGO 40
#define TEMPO_LENTO 50
#define TEMPO_LARGHETTO 60
#define TEMPO_ADAGIO  70
#define TEMPO_ADAGIETTO 77
#define TEMPO_ADENTE 84
#define TEMPO_ANDANTINO 92
#define TEMPO_MARCIA_MODERATO 100
#define TEMPO_ANDANTE_MODERATO 110
#define TEMPO_MODERATO 120
#define TEMPO_ALLEGRO 156
#define TEMPO_PRESTO 200
#define TEMPO_PRESTISSIMO 300

// Beats for a notes
#define BEAT_025 1
#define BEAT_05 2
#define BEAT_075 3
#define BEAT_1 4
#define BEAT_125 5
#define BEAT_15 6
#define BEAT_175 7
#define BEAT_2 8
#define BEAT_25 10
#define BEAT_3 12
#define BEAT_4 16
#define BEAT_5 20


// Wave forms
#define WAVE_SQUARE 0
#define WAVE_SAWTOOTH 1
#define WAVE_TRIANGLE 2
#define WAVE_SINE 3


// Instrument ID's
#define INSTRUMENT_NONE 0
#define INSTRUMENT_PIANO 1
#define INSTRUMENT_HARPSICHORD 2
#define INSTRUMENT_ORGAN 3
#define INSTRUMENT_SAXOPHONE 4
