#ifndef GAMERTAG_H
#define GAMERTAG_H

#define GAMERTAG_MAX_LEN 15
#define GAMERTAG_LIMIT_HDD 8
#define GAMERTAG_LIMIT_MEM 1

int XboxLiveProfile_GetFirstGamertag(char *outGamertag, unsigned int outSize);

#endif
