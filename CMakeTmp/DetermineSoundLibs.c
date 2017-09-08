#include "SDL_sound.h"
    #include "SDL.h"
    int main(int argc, char* argv[])
    {
        Sound_AudioInfo desired;
        Sound_Sample* sample;

        SDL_Init(0);
        Sound_Init();
        
        /* This doesn't actually have to work, but Init() is a no-op
         * for some of the decoders, so this should force more symbols
         * to be pulled in.
         */
        sample = Sound_NewSampleFromFile(argv[1], &desired, 4096);
        
        Sound_Quit();
        SDL_Quit();
        return 0;
     }