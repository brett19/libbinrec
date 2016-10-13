#include "benchmarks/blobs.h"
static const uint8_t ppc32_dhry_noopt_bin[] = {
148,33,255,40,124,8,2,166,144,1,0,220,147,225,0,212,
124,63,11,120,144,127,0,200,61,32,0,17,57,95,0,96,
145,73,14,236,61,32,0,17,57,95,0,144,145,73,14,248,
61,32,0,17,129,41,14,248,61,64,0,17,129,74,14,236,
145,73,0,0,61,32,0,17,129,41,14,248,57,64,0,0,
145,73,0,4,61,32,0,17,129,41,14,248,57,64,0,2,
145,73,0,8,61,32,0,17,129,41,14,248,57,64,0,40,
145,73,0,12,61,32,0,17,129,41,14,248,57,41,0,16,
61,64,0,16,57,74,14,88,128,170,0,0,128,202,0,4,
128,234,0,8,129,10,0,12,144,169,0,0,144,201,0,4,
144,233,0,8,145,9,0,12,128,170,0,16,128,202,0,20,
128,234,0,24,161,10,0,28,144,169,0,16,144,201,0,20,
144,233,0,24,177,9,0,28,137,74,0,30,153,73,0,30,
61,32,0,16,57,41,14,120,128,201,0,0,128,233,0,4,
129,9,0,8,129,73,0,12,144,223,0,32,144,255,0,36,
145,31,0,40,145,95,0,44,128,201,0,16,128,233,0,20,
129,9,0,24,161,73,0,28,144,223,0,48,144,255,0,52,
145,31,0,56,177,95,0,60,137,41,0,30,153,63,0,62,
61,32,0,17,57,41,15,0,57,64,0,10,145,73,6,92,
57,32,0,1,145,63,0,16,72,0,2,84,72,0,6,253,
72,0,6,149,57,32,0,2,145,63,0,20,57,32,0,3,
145,63,0,8,61,32,0,16,57,41,14,152,128,201,0,0,
128,233,0,4,129,9,0,8,129,73,0,12,144,223,0,64,
144,255,0,68,145,31,0,72,145,95,0,76,128,201,0,16,
128,233,0,20,129,9,0,24,161,73,0,28,144,223,0,80,
144,255,0,84,145,31,0,88,177,95,0,92,137,41,0,30,
153,63,0,94,57,32,0,1,145,63,0,28,57,95,0,64,
57,63,0,32,125,68,83,120,125,35,75,120,76,198,49,130,
72,0,9,245,124,105,27,120,125,41,0,52,85,41,217,126,
85,41,6,62,125,42,75,120,61,32,0,17,145,73,14,252,
72,0,0,64,129,63,0,20,29,73,0,5,129,63,0,8,
125,41,80,80,145,63,0,24,129,63,0,20,57,95,0,24,
125,69,83,120,128,159,0,8,125,35,75,120,76,198,49,130,
72,0,7,109,129,63,0,20,57,41,0,1,145,63,0,20,
129,95,0,20,129,63,0,8,127,138,72,0,65,156,255,184,
129,63,0,20,129,95,0,24,125,70,83,120,125,37,75,120,
61,32,0,17,56,137,15,0,61,32,0,17,56,105,54,16,
76,198,49,130,72,0,7,121,61,32,0,17,129,41,14,248,
125,35,75,120,76,198,49,130,72,0,3,37,57,32,0,65,
153,63,0,12,72,0,0,172,137,63,0,12,56,128,0,67,
125,35,75,120,76,198,49,130,72,0,8,197,124,106,27,120,
129,63,0,28,127,138,72,64,64,158,0,124,57,63,0,28,
125,36,75,120,56,96,0,0,76,198,49,130,72,0,5,201,
61,32,0,16,57,41,14,184,128,201,0,0,128,233,0,4,
129,9,0,8,129,73,0,12,144,223,0,64,144,255,0,68,
145,31,0,72,145,95,0,76,128,201,0,16,128,233,0,20,
129,9,0,24,161,73,0,28,144,223,0,80,144,255,0,84,
145,31,0,88,177,95,0,92,137,41,0,30,153,63,0,94,
129,63,0,16,145,63,0,8,61,32,0,17,129,95,0,16,
145,73,14,240,137,63,0,12,57,41,0,1,153,63,0,12,
61,32,0,17,137,41,14,245,137,95,0,12,127,138,72,64,
64,157,255,72,129,63,0,20,129,95,0,8,125,42,73,214,
145,63,0,8,129,63,0,24,129,95,0,8,125,42,75,214,
145,63,0,20,129,63,0,24,129,95,0,8,125,41,80,80,
29,73,0,7,129,63,0,20,125,41,80,80,145,63,0,8,
57,63,0,20,125,35,75,120,76,198,49,130,72,0,3,85,
129,63,0,16,57,41,0,1,145,63,0,16,129,95,0,16,
129,63,0,200,127,138,72,0,64,157,253,164,61,32,0,17,
129,41,14,240,47,137,0,5,64,158,1,196,61,32,0,17,
129,41,14,252,47,137,0,1,64,158,1,180,61,32,0,17,
137,41,14,244,43,137,0,65,64,158,1,164,61,32,0,17,
137,41,14,245,43,137,0,66,64,158,1,148,61,32,0,17,
57,41,54,16,129,41,0,32,47,137,0,7,64,158,1,128,
61,32,0,17,57,41,15,0,129,73,6,92,129,63,0,200,
57,41,0,10,127,138,72,0,64,158,1,100,61,32,0,17,
129,41,14,248,129,41,0,4,47,137,0,0,64,158,1,80,
61,32,0,17,129,41,14,248,129,41,0,8,43,137,0,2,
64,158,1,60,61,32,0,17,129,41,14,248,129,41,0,12,
47,137,0,17,64,158,1,40,61,32,0,17,129,41,14,248,
57,73,0,16,61,32,0,16,56,137,14,88,125,67,83,120,
72,0,9,137,124,105,27,120,47,137,0,0,64,158,1,0,
61,32,0,17,129,41,14,236,129,73,0,0,61,32,0,17,
129,41,14,248,129,41,0,0,127,138,72,64,64,158,0,224,
61,32,0,17,129,41,14,236,129,41,0,4,47,137,0,0,
64,158,0,204,61,32,0,17,129,41,14,236,129,41,0,8,
43,137,0,1,64,158,0,184,61,32,0,17,129,41,14,236,
129,41,0,12,47,137,0,18,64,158,0,164,61,32,0,17,
129,41,14,236,57,73,0,16,61,32,0,16,56,137,14,88,
125,67,83,120,72,0,9,5,124,105,27,120,47,137,0,0,
64,158,0,124,129,63,0,20,47,137,0,5,64,158,0,112,
129,63,0,8,47,137,0,13,64,158,0,100,129,63,0,24,
47,137,0,7,64,158,0,88,129,63,0,28,43,137,0,1,
64,158,0,76,57,95,0,32,61,32,0,16,56,137,14,120,
125,67,83,120,72,0,8,181,124,105,27,120,47,137,0,0,
64,158,0,44,57,95,0,64,61,32,0,16,56,137,14,152,
125,67,83,120,72,0,8,149,124,105,27,120,47,137,0,0,
64,158,0,12,57,32,0,1,72,0,0,8,57,32,0,0,
125,35,75,120,57,127,0,216,128,11,0,4,124,8,3,166,
131,235,255,252,125,97,91,120,78,128,0,32,148,33,255,216,
124,8,2,166,144,1,0,44,147,225,0,36,124,63,11,120,
144,127,0,24,129,63,0,24,129,41,0,0,145,63,0,8,
129,63,0,24,129,73,0,0,61,32,0,17,129,41,14,248,
125,40,75,120,57,32,0,48,125,37,75,120,125,4,67,120,
125,67,83,120,72,0,7,61,129,63,0,24,57,64,0,5,
145,73,0,12,129,63,0,24,129,73,0,12,129,63,0,8,
145,73,0,12,129,63,0,24,129,73,0,0,129,63,0,8,
145,73,0,0,129,63,0,8,125,35,75,120,76,198,49,130,
72,0,1,65,129,63,0,8,129,41,0,4,47,137,0,0,
64,158,0,108,129,63,0,8,57,64,0,6,145,73,0,12,
129,63,0,24,129,73,0,8,129,63,0,8,57,41,0,8,
125,36,75,120,125,67,83,120,76,198,49,130,72,0,2,41,
61,32,0,17,129,41,14,248,129,73,0,0,129,63,0,8,
145,73,0,0,129,63,0,8,129,73,0,12,129,63,0,8,
57,41,0,12,125,37,75,120,56,128,0,10,125,67,83,120,
76,198,49,130,72,0,2,249,72,0,0,40,129,63,0,24,
129,41,0,0,129,95,0,24,125,40,75,120,57,32,0,48,
125,37,75,120,125,4,67,120,125,67,83,120,72,0,6,101,
96,0,0,0,125,35,75,120,57,127,0,40,128,11,0,4,
124,8,3,166,131,235,255,252,125,97,91,120,78,128,0,32,
148,33,255,216,147,225,0,36,124,63,11,120,144,127,0,24,
129,63,0,24,129,41,0,0,57,41,0,10,145,63,0,8,
61,32,0,17,137,41,14,244,43,137,0,65,64,158,0,48,
129,63,0,8,57,41,255,255,145,63,0,8,61,32,0,17,
129,41,14,240,129,95,0,8,125,73,80,80,129,63,0,24,
145,73,0,0,57,32,0,0,145,63,0,12,129,63,0,12,
47,137,0,0,64,158,255,188,96,0,0,0,125,35,75,120,
57,127,0,40,131,235,255,252,125,97,91,120,78,128,0,32,
148,33,255,232,124,8,2,166,144,1,0,28,147,225,0,20,
124,63,11,120,144,127,0,8,61,32,0,17,129,41,14,248,
47,137,0,0,65,158,0,24,61,32,0,17,129,41,14,248,
129,73,0,0,129,63,0,8,145,73,0,0,61,32,0,17,
129,73,14,240,61,32,0,17,129,41,14,248,57,41,0,12,
125,37,75,120,125,68,83,120,56,96,0,10,76,198,49,130,
72,0,1,205,96,0,0,0,125,35,75,120,57,127,0,24,
128,11,0,4,124,8,3,166,131,235,255,252,125,97,91,120,
78,128,0,32,148,33,255,224,147,225,0,28,124,63,11,120,
61,32,0,17,137,41,14,244,105,41,0,65,125,41,0,52,
85,41,217,126,85,41,6,62,145,63,0,8,61,32,0,17,
129,73,14,252,129,63,0,8,125,74,75,120,61,32,0,17,
145,73,14,252,61,32,0,17,57,64,0,66,153,73,14,245,
96,0,0,0,125,35,75,120,57,127,0,32,131,235,255,252,
125,97,91,120,78,128,0,32,148,33,255,240,147,225,0,12,
124,63,11,120,61,32,0,17,57,64,0,65,153,73,14,244,
61,32,0,17,57,64,0,0,145,73,14,252,96,0,0,0,
125,35,75,120,57,127,0,16,131,235,255,252,125,97,91,120,
78,128,0,32,148,33,255,232,124,8,2,166,144,1,0,28,
147,225,0,20,124,63,11,120,144,127,0,8,144,159,0,12,
129,63,0,12,129,95,0,8,145,73,0,0,128,127,0,8,
76,198,49,130,72,0,4,41,124,105,27,120,47,137,0,0,
64,158,0,16,129,63,0,12,57,64,0,3,145,73,0,0,
129,63,0,8,43,137,0,4,65,157,0,148,129,63,0,8,
85,42,16,58,61,32,0,16,57,41,14,216,125,42,74,20,
129,73,0,0,61,32,0,16,57,41,14,216,125,42,74,20,
125,41,3,166,78,128,4,32,129,63,0,12,57,64,0,0,
145,73,0,0,72,0,0,88,61,32,0,17,129,41,14,240,
47,137,0,100,64,157,0,20,129,63,0,12,57,64,0,0,
145,73,0,0,72,0,0,56,129,63,0,12,57,64,0,3,
145,73,0,0,72,0,0,40,129,63,0,12,57,64,0,1,
145,73,0,0,72,0,0,24,129,63,0,12,57,64,0,2,
145,73,0,0,72,0,0,8,96,0,0,0,96,0,0,0,
125,35,75,120,57,127,0,24,128,11,0,4,124,8,3,166,
131,235,255,252,125,97,91,120,78,128,0,32,148,33,255,208,
147,225,0,44,124,63,11,120,144,127,0,24,144,159,0,28,
144,191,0,32,129,63,0,24,57,41,0,2,145,63,0,8,
129,95,0,28,129,63,0,8,125,74,74,20,129,63,0,32,
145,73,0,0,96,0,0,0,125,35,75,120,57,127,0,48,
131,235,255,252,125,97,91,120,78,128,0,32,148,33,255,208,
147,225,0,44,124,63,11,120,144,127,0,24,144,159,0,28,
144,191,0,32,144,223,0,36,129,63,0,32,57,41,0,5,
145,63,0,12,129,63,0,12,85,41,16,58,129,95,0,24,
125,42,74,20,129,95,0,36,145,73,0,0,129,63,0,12,
57,41,0,1,85,41,16,58,129,95,0,24,125,42,74,20,
129,95,0,12,85,74,16,58,129,31,0,24,125,72,82,20,
129,74,0,0,145,73,0,0,129,63,0,12,57,41,0,30,
85,41,16,58,129,95,0,24,125,42,74,20,129,95,0,12,
145,73,0,0,129,63,0,12,145,63,0,8,72,0,0,52,
129,63,0,12,29,41,0,200,129,95,0,28,125,74,74,20,
129,63,0,8,85,41,16,58,125,42,74,20,129,95,0,12,
145,73,0,0,129,63,0,8,57,41,0,1,145,63,0,8,
129,63,0,12,57,73,0,1,129,63,0,8,127,138,72,0,
64,156,255,192,129,63,0,12,29,41,0,200,129,95,0,28,
125,10,74,20,129,63,0,12,57,41,255,255,129,95,0,12,
29,74,0,200,128,255,0,28,124,231,82,20,129,95,0,12,
57,74,255,255,85,74,16,58,125,71,82,20,129,74,0,0,
57,74,0,1,85,41,16,58,125,40,74,20,145,73,0,0,
129,63,0,12,29,41,0,200,57,41,15,160,129,95,0,28,
125,10,74,20,129,63,0,12,85,41,16,58,129,95,0,24,
125,42,74,20,129,73,0,0,129,63,0,12,85,41,16,58,
125,40,74,20,145,73,0,0,61,32,0,17,57,64,0,5,
145,73,14,240,96,0,0,0,125,35,75,120,57,127,0,48,
131,235,255,252,125,97,91,120,78,128,0,32,148,33,255,216,
147,225,0,36,124,63,11,120,124,106,27,120,124,137,35,120,
153,95,0,24,153,63,0,28,137,63,0,24,153,63,0,8,
137,63,0,8,153,63,0,9,137,95,0,9,137,63,0,28,
127,138,72,0,65,158,0,12,57,32,0,0,72,0,0,20,
61,32,0,17,137,95,0,8,153,73,14,244,57,32,0,1,
125,35,75,120,57,127,0,40,131,235,255,252,125,97,91,120,
78,128,0,32,148,33,255,216,124,8,2,166,144,1,0,44,
147,225,0,36,124,63,11,120,144,127,0,24,144,159,0,28,
57,32,0,2,145,63,0,8,72,0,0,92,129,63,0,8,
129,95,0,24,125,42,74,20,137,41,0,0,125,40,75,120,
129,63,0,8,57,41,0,1,129,95,0,28,125,42,74,20,
137,41,0,0,125,36,75,120,125,3,67,120,76,198,49,130,
75,255,255,61,124,105,27,120,47,137,0,0,64,158,0,24,
57,32,0,65,153,63,0,12,129,63,0,8,57,41,0,1,
145,63,0,8,129,63,0,8,47,137,0,2,64,157,255,160,
137,63,0,12,43,137,0,86,64,157,0,24,137,63,0,12,
43,137,0,89,65,157,0,12,57,32,0,7,145,63,0,8,
137,63,0,12,47,137,0,82,64,158,0,12,57,32,0,1,
72,0,0,64,128,159,0,28,128,127,0,24,72,0,1,109,
124,105,27,120,47,137,0,0,64,157,0,36,129,63,0,8,
57,41,0,7,145,63,0,8,61,32,0,17,129,95,0,8,
145,73,14,240,57,32,0,1,72,0,0,8,57,32,0,0,
125,35,75,120,57,127,0,40,128,11,0,4,124,8,3,166,
131,235,255,252,125,97,91,120,78,128,0,32,148,33,255,216,
147,225,0,36,124,63,11,120,144,127,0,24,129,63,0,24,
145,63,0,8,129,63,0,8,47,137,0,2,64,158,0,12,
57,32,0,1,72,0,0,8,57,32,0,0,125,35,75,120,
57,127,0,40,131,235,255,252,125,97,91,120,78,128,0,32,
44,5,0,0,77,130,0,32,57,67,0,4,125,74,32,16,
57,68,0,4,124,231,57,16,125,74,24,16,125,74,81,16,
33,5,0,9,57,74,0,1,56,199,0,1,125,8,65,16,
124,199,83,120,125,72,0,208,125,74,56,56,85,74,6,62,
47,138,0,0,65,158,0,116,124,138,27,120,113,73,0,3,
64,130,0,104,57,69,255,252,56,196,255,252,85,74,240,190,
56,227,255,252,57,74,0,1,125,73,3,166,85,72,16,58,
133,70,0,4,149,71,0,4,66,0,255,248,127,133,64,64,
77,158,0,32,57,72,0,1,124,228,64,174,127,133,80,64,
124,227,65,174,76,157,0,32,57,8,0,2,125,36,80,174,
127,133,64,64,125,35,81,174,64,157,0,44,125,36,64,174,
125,35,65,174,78,128,0,32,124,169,3,166,57,67,255,255,
56,132,255,255,141,36,0,1,157,42,0,1,66,0,255,248,
78,128,0,32,78,128,0,32,137,35,0,0,47,137,0,0,
65,158,0,68,137,68,0,0,127,137,80,64,65,190,0,20,
72,0,0,40,141,68,0,1,127,137,80,64,64,158,0,28,
141,35,0,1,47,137,0,0,64,158,255,236,136,100,0,1,
124,99,72,80,78,128,0,32,125,67,83,120,124,99,72,80,
78,128,0,32,136,100,0,0,75,255,255,232,56,132,255,255,
57,67,255,255,141,36,0,1,47,137,0,0,157,42,0,1,
64,158,255,244,78,128,0,32,68,72,82,89,83,84,79,78,
69,32,80,82,79,71,82,65,77,44,32,83,79,77,69,32,
83,84,82,73,78,71,0,0,68,72,82,89,83,84,79,78,
69,32,80,82,79,71,82,65,77,44,32,49,39,83,84,32,
83,84,82,73,78,71,0,0,68,72,82,89,83,84,79,78,
69,32,80,82,79,71,82,65,77,44,32,50,39,78,68,32,
83,84,82,73,78,71,0,0,68,72,82,89,83,84,79,78,
69,32,80,82,79,71,82,65,77,44,32,51,39,82,68,32,
83,84,82,73,78,71,0,0,255,255,250,32,255,255,250,48,
255,255,250,96,255,255,250,128,255,255,250,112,
};
const Blob ppc32_dhry_noopt = {
    .data = ppc32_dhry_noopt_bin,
    .size = sizeof(ppc32_dhry_noopt_bin),
    .reserve = 0x1136D8,
    .base = 0x100000,
    .entry = 0x0,
};