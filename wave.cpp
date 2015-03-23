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

#include "wave.h"           // self

#include <stdexcept>        // std::runtime_error
#include <fstream>          // std::ofstream
#include <errno.h>          // errno
#include <cstring>          // strerror

Wave::Wave( const std::string & filename ) throw( std::exception )
{
    extra_param_    = nullptr;
    wave_           = nullptr;
    fmt             = nullptr;
    extra_param_length_ = 0;
    fact.samplesNumber  = -1;

    std::ifstream file( filename.c_str(), std::ios_base::binary | std::ios_base::in );
    if( file.is_open() == false )
    {
        throw std::runtime_error( strerror( errno ) );
    }

    file.read( reinterpret_cast<char*>( &riff ), RIFF_SIZE );
    file.read( reinterpret_cast<char*>( &fmthdr ), FMTHDR_SIZE );

    fmt = reinterpret_cast<FMT*>( new char[ fmthdr.fmtSIZE ] );

    file.read( reinterpret_cast<char*>( fmt ), fmthdr.fmtSIZE );

    if( fmt->wFormatTag != 1 )
    {
        file.read( reinterpret_cast<char*>( &extra_param_length_ ), 2 ); //2 bytes
        if( extra_param_length_ > 0 )
        {
            extra_param_ = new char[extra_param_length_];
            file.read( reinterpret_cast<char*>( extra_param_ ), extra_param_length_ );
        }
    }

    file.read( reinterpret_cast<char*>( &data.dataID ), 4 );

    if( data.dataID[0] == 'f' && data.dataID[1] == 'a' && data.dataID[2] == 'c' && data.dataID[3] == 't' )
    {
        file.read( reinterpret_cast<char*>( &fact ), FACT_SIZE );
        file.read( reinterpret_cast<char*>( &data ), DATA_SIZE );
    }
    else
        file.read( reinterpret_cast<char*>( &data.dataSIZE ), 4 );

    wave_ = new char[data.dataSIZE];

    file.read( reinterpret_cast<char*>( wave_ ), data.dataSIZE );
}

Wave::Wave()
{
    extra_param_length_ = 0;
    extra_param_        = nullptr;
    wave_               = nullptr;
    fmt                 = nullptr;
    fact.samplesNumber  = -1;
}
Wave::Wave( const Wave& w )
{
    init( w );
}
Wave& Wave::operator=( const Wave &w )
{
    init( w );
    return *this;
}
Wave::~Wave()
{
    if( extra_param_ )
        delete[] extra_param_;
    if( wave_ )
        delete[] wave_;
    if( fmt )
        delete[] fmt;
}

Wave Wave::operator+( const Wave &w ) const throw( std::exception )
{
    if( fmt->wFormatTag != w.fmt->wFormatTag )
        throw std::runtime_error( "Can't concatenate waves with different format tags" );

    Wave res;
    res.fmthdr = w.fmthdr;
    res.fmt = reinterpret_cast<FMT*>( new char[w.fmthdr.fmtSIZE] );
    memcpy( res.fmt, w.fmt, w.fmthdr.fmtSIZE );

    res.riff = w.riff;
    res.data = w.data;
    res.data.dataSIZE = data.dataSIZE + w.data.dataSIZE;

    res.extra_param_length_ = w.extra_param_length_;
    if( w.extra_param_length_ )
    {
        res.extra_param_ = new char[w.extra_param_length_];
        memcpy( res.extra_param_, w.extra_param_, w.extra_param_length_ );
    }

    res.wave_ = new char[res.data.dataSIZE];
    memcpy( res.wave_, wave_, data.dataSIZE );
    memcpy( res.wave_ + data.dataSIZE, w.wave_, w.data.dataSIZE );

    return res;
}

void Wave::init( const Wave& w )
{
    fmthdr = w.fmthdr;
    fmt = reinterpret_cast<FMT*>( new char[fmthdr.fmtSIZE] );
    memcpy( fmt, w.fmt, fmthdr.fmtSIZE );
    riff = w.riff;
    data = w.data;
    fact = w.fact;

    extra_param_length_ = w.extra_param_length_;
    if( w.extra_param_length_ )
    {
        extra_param_ = new char[extra_param_length_];
        memcpy( extra_param_, w.extra_param_, extra_param_length_ );
    }
    wave_ = new char[data.dataSIZE];
    memcpy( wave_, w.wave_, data.dataSIZE );
}

void Wave::save( const std::string & filename )
{
    std::ofstream file( filename.c_str(), std::ios_base::binary | std::ios_base::out  );

    file.write( reinterpret_cast<char*>( & riff ), RIFF_SIZE );
    file.write( reinterpret_cast<char*>( & fmthdr ), FMTHDR_SIZE );

    file.write( reinterpret_cast<char*>( fmt ), fmthdr.fmtSIZE );

    if( fmt->wFormatTag > 1 )
    {
        file.write( reinterpret_cast<char*>( &extra_param_length_ ), 2 );
        if( extra_param_length_ > 0 )
            file.write( reinterpret_cast<char*>( extra_param_ ), extra_param_length_ );
    }
    if( fact.samplesNumber > -1 )
    {
        file.write( const_cast<char*>( "fact" ), 4 );
        file.write( reinterpret_cast<char*>( & fact ), FACT_SIZE );
    }

    file.write( reinterpret_cast<char*>( & data ), DATA_SIZE );
    file.write( reinterpret_cast<char*>( wave_) , data.dataSIZE );
}

