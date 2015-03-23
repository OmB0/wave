/*

 WAV file appender.

 Copyright (C) 2015 Sergey Kolevatov

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.

 */

// $Revision: 1600 $ $Date:: 2015-03-20 #$ $Author: serge $

#ifndef LIB_WAVE_WAVE_H
#define LIB_WAVE_WAVE_H

#include <string>
#include <stdint.h>     // int32_t, int16_t
#include <exception>    // std::exception

class Wave
{
public:
    Wave();
    Wave( const std::string & filename ) throw( std::exception );
    Wave( const Wave& w );
    virtual ~Wave();

    Wave operator+( const Wave& wave ) const throw( std::exception );
    Wave& operator=( const Wave &w );

    void save( const std::string & filename );

    struct RIFF
    {
        char riffID[4];     //4
        int32_t riffSIZE;   //4
        char riffFORMAT[4]; //4

    };

    struct FMTHDR
    {
        char fmtID[4];      //4
        int32_t fmtSIZE;    //4
    };

    struct FMT
    {
        int16_t wFormatTag; //2
        int16_t nChannels;  //2
        int32_t nSamplesPerSec;     //4
        int32_t nAvgBytesPerSec;    //4
        int16_t nBlockAlign;        //2
        int16_t wBitsPerSample;     //2
    };

    struct DATA
    {
        char dataID[4];     // 4 bytes
        int32_t dataSIZE;   // 4 bytes
    };
    struct FACT
    {
        int32_t samplesNumber;
        int32_t t;
    };

public:
    const int RIFF_SIZE     = 12;
    const int FMTHDR_SIZE   = 8;
    const int DATA_SIZE     = 8;
    const int FACT_SIZE     = 8;

private:

    void init( const Wave& );

private:
    char        * wave_;
    RIFF        riff;
    FMTHDR      fmthdr;
    FMT         *fmt;
    FACT        fact;
    DATA        data;
    int16_t     extra_param_length_;
    char*       extra_param_;

};

#endif // LIB_WAVE_WAVE_H
