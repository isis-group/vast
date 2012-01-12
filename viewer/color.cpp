/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik Türke, tuerke@cbs.mpg.de
 *
 * color.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "color.hpp"
#include <QResource>
#include <QFile>
#include <fstream>
#include <boost/iostreams/filter/newline.hpp>


namespace isis
{
namespace viewer
{
namespace color
{

void Color::initStandardColormaps()
{
	addColormap( std::string( ":/colormap/lut/colormap1" ) );
	addColormap( std::string( ":/colormap/lut/colormap2" ) );
	addColormap( std::string( ":/colormap/lut/colormap3" ) );
	addColormap( std::string( ":/colormap/lut/colormap4" ) );
	addColormap( std::string( ":/colormap/lut/colormap5" ) );
	addColormap( std::string( ":/colormap/lut/colormap6" ) );
	addColormap( std::string( ":/colormap/lut/colormap7" ) );
	addColormap( std::string( ":/colormap/lut/colormap8" ) );
	addColormap( std::string( ":/colormap/lut/colormap9" ) );
	addColormap( std::string( ":/colormap/lut/colormap10" ) );
	addColormap( std::string( ":/colormap/lut/colormap11" ) );
	addColormap( std::string( ":/colormap/lut/standard_zmap" ) );
	addColormap( std::string( ":/colormap/lut/standard_grey_values" ) );
	addColormap( std::string( ":/colormap/lut/standard_grey_values_inverted" ) );
	m_ColormapMap["fallback"] = getFallbackColormap();
}

bool Color::addColormap( const std::string &path, const boost::regex &separator )
{
	QFile lutFile( path.c_str() );
	lutFile.open( QIODevice::ReadOnly );

	if( !lutFile.isReadable() ) {
		LOG( Runtime, warning ) << "The QResource \"" << path << "\" is not readable!";
		return false;
	}

	std::string data( lutFile.readAll() );
	lutFile.close();
	std::list<std::string > lines = util::stringToList<std::string>( data, boost::regex( "\n" ) );
	ColormapType lutVec;
	//get the name and colorspace
	boost::regex descriptionRegex( "^([[:word:]]+):([[:word:]]{3,4})$" );
	boost::cmatch results;

	if( !boost::regex_match( lines.front().c_str(), results, descriptionRegex ) ) {
		LOG( Runtime, warning ) << "Can not read colormap " << path << " because header is missing or has wrong syntax (name:type)!";
		return false;
	}

	std::string lutTyp = boost::lexical_cast< std::string>( results.str( 2 ) );
	std::string lutName = boost::lexical_cast< std::string>( results.str( 1 ) );
	lines.pop_front();
	BOOST_FOREACH( std::list<std::string>::const_reference lineRef, lines ) {
		std::list< unsigned short > values = util::stringToList< unsigned short >( lineRef, separator );
		std::vector<unsigned short> colorVec( values.begin(), values.end() );

		if( lutTyp == std::string( "rgb" ) ) {
			lutVec.push_back( QColor( colorVec[0], colorVec[1], colorVec[2], 255 ).rgba() );
		} else if ( lutTyp == std::string( "rgba" ) ) {
			lutVec.push_back( QColor( colorVec[0], colorVec[1], colorVec[2], colorVec[3] ).rgba() );
		} else if ( lutTyp == std::string( "hsv" ) ) {
			lutVec.push_back( QColor::fromHsv( colorVec[0], colorVec[1], colorVec[2] ).rgba() );
		} else if ( lutTyp == std::string( "hsva" ) ) {
			lutVec.push_back( QColor::fromHsv( colorVec[0], colorVec[1], colorVec[2], colorVec[3] ).rgba() );
		} else if ( lutTyp == std::string( "cmyk" ) ) {
			lutVec.push_back( QColor::fromCmyk( colorVec[0], colorVec[1], colorVec[2], colorVec[3] ).rgba() );
        }
#if QT_VERSION >= 0x040600            
		 else if ( lutTyp == std::string( "hsl" ) ) {
			lutVec.push_back( QColor::fromHsl( colorVec[0], colorVec[1], colorVec[2] ).rgba() );
		} else if ( lutTyp == std::string( "hsla" ) ) {
			lutVec.push_back( QColor::fromHsl( colorVec[0], colorVec[1], colorVec[2], colorVec[3] ).rgba() );
		} 
#endif
		else {
			LOG( Runtime, warning ) << "Unknown lut type " << lutTyp << " !";
			return false;
		}
	}

	if( lutVec.size() != (getInternalImageRange() + 1) ) {
		LOG( Runtime, warning ) << "The size of the colormap " << lutName
								<< " is " << lutVec.size() << " but has to be 256!";
		return false;
	}

	m_ColormapMap[lutName] = lutVec;
	return true;
}


QIcon Color::getIcon( const std::string &colormapName, size_t w, size_t h, icon_type type, bool flipped ) const
{
	const ColormapType lut = getColormapMap().at( colormapName );
    const size_t internalImageRange = getInternalImageRange();
    
	unsigned short start = 0;
	unsigned short end = 0;

	switch( type ) {
	case both:
		start = 0;
		end = internalImageRange + 1;
		break;
	case lower_half:
		start = 0;
		end = (internalImageRange + 1) / 2;
		break;
	case upper_half:
		start = (internalImageRange + 1) / 2;
		end = (internalImageRange + 1);
		break;
	}

	QImage tmpImage( end - start, 1, QImage::Format_RGB888 );
	uint8_t *line = tmpImage.scanLine( 0 );

	unsigned short index = 0;

	if( !flipped ) {
		for ( unsigned short i = start; i < end; i++ ) {
			line[index++] = qRed( lut[i] );
			line[index++] = qGreen( lut[i] );
			line[index++] = qBlue( lut[i] );
		}
	} else {
		for ( short i = end; i > start - 1; i-- ) {
			line[index++] = qRed( lut[i] );
			line[index++] = qGreen( lut[i] );
			line[index++] = qBlue( lut[i] );
		}
	}

	QPixmap pixmap( QPixmap::fromImage( tmpImage ) );
	return QIcon( pixmap.scaled( w, h ) );
}


bool Color::hasColormap( const std::string &name ) const
{
	return m_ColormapMap.find( name ) != m_ColormapMap.end();
}


Color::ColormapType Color::getFallbackColormap() const
{
	ColormapType retColormap;

	for ( unsigned short i = 0; i < (getInternalImageRange() + 1); i++ ) {
		retColormap.push_back( QColor( i, i, i, getInternalImageRange() ).rgba() );
	}

	return retColormap;
}


void Color::adaptColorMapToImage( ImageHolder *image, bool split )
{
    const size_t internalImageRange = getInternalImageRange();
	LOG_IF( image->colorMap.size() != (internalImageRange + 1), Runtime, error ) << "The colormap is of size " 
        << image->colorMap.size() << " but has to be of size " << (internalImageRange + 1 ) << "!";
	ColormapType retMap ;
	retMap.resize( (internalImageRange + 1 ) );
	ColormapType tmpMap = util::Singletons::get<Color, 10>().getColormapMap().at( image->lut );
	const double extent = image->extent;
	const double min = image->minMax.first->as<double>();
	const double max = image->minMax.second->as<double>();
	const double lowerThreshold = image->lowerThreshold;
	const double upperThreshold = image->upperThreshold;
	const double offset = image->offset;
	const double scaling = image->scaling;
	const double norm = (internalImageRange + 1) / extent;
	const unsigned short mid = norm * fabs( min );
	unsigned short scaledVal;

	for ( unsigned short i = 0; i <= internalImageRange; i++ ) {
		scaledVal = i * scaling + offset * norm > internalImageRange ? internalImageRange : i * scaling + offset * norm;
		retMap[i] = tmpMap[scaledVal];
	}

	ColormapType negVec( mid );
	ColormapType posVec( (internalImageRange + 1) - mid );

	//only stuff necessary for colormaps
	if( image->imageType == ImageHolder::z_map ) {

		if( split ) {
			assert( negVec.size() + posVec.size() == (internalImageRange + 1) );

			//fill negVec
			if( min < 0 ) {
				const double scaleMin = 1 - fabs( lowerThreshold / min );
				const double normMin = ( (internalImageRange + 1 ) / 2 ) / mid;

				for ( unsigned short i = 0; i < mid; i++ ) {
					negVec[i * scaleMin] = retMap[i * normMin];
				}
			}

			if( max > 0 ) {
				const double normMax = ( (internalImageRange + 1 ) / 2 ) / ( (internalImageRange + 1 ) - mid );
				const double scaleMax = fabs( upperThreshold / max );
				const double offset = ( (internalImageRange + 1 ) - mid ) * scaleMax;

				for( unsigned short i = 0; i < ((internalImageRange + 1 ) - mid); i++ ) {
					posVec[( i * ( 1 - scaleMax ) + offset )] = retMap[( (internalImageRange + 1 ) / 2 ) + i * normMax];
				}
			}

			retMap = negVec << posVec;
		}
	}

	//kill the zero value
// 	retMap[mid] = QColor( 0, 0, 0, 0 ).rgba();
	image->colorMap = retMap;
}


}
}
} // end namespace
