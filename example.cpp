#include "wave.h"   // Wave

int main( int argc, char* argv[] )
{
    Wave wave1( "a.wav" );
    Wave wave2( "b.wav" );
    Wave wave3 = wave1 + wave2;
    wave3.save( "c.wav" );
    return 0;
}
