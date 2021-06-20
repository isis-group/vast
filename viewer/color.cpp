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
 * Author: Erik Tuerke, tuerke@cbs.mpg.de
 *
 * color.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "color.hpp"
#include "imageholder.hpp"
#include <QResource>
#include <QFile>
#include <fstream>
#include <regex>


namespace isis::viewer::color
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

bool Color::addColormap( const std::string &path, const std::regex &separator )
{
	LOG( Dev, verbose_info ) << "Adding colormap " << path;
	QFile lutFile( path.c_str() );
	lutFile.open( QIODevice::ReadOnly );

	if( !lutFile.isReadable() ) {
		LOG( Dev, warning ) << "The QResource \"" << path << "\" is not readable!";
		return false;
	}

	std::string data( lutFile.readAll() );
	lutFile.close();
	auto lines = util::stringToList<std::string>( data, '\n' );
	ColormapType lutVec;
	//get the name and colorspace
	std::regex descriptionRegex( "^([[:word:]]+):([[:word:]]{3,4})$" );
	std::cmatch results;

	if( !std::regex_match( lines.front().c_str(), results, descriptionRegex ) ) {
		LOG( Dev, warning ) << "Can not read colormap " << path << " because header is missing or has wrong syntax (name:type)!";
		return false;
	}

	auto lutTyp = results.str( 2 );
	auto lutName = results.str( 1 );
	lines.pop_front();
	for( const auto &lineRef: lines ) {
		auto values = util::stringToList< unsigned short >( lineRef, separator );
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

#else
		LOG( Dev, info ) << "QT_VERSION < 0x040600 QColor::fromHsl is not supported";
#endif
		else {
			LOG( Runtime, warning ) << "Unknown lut type " << lutTyp << " !";
			return false;
		}
	}

	if( lutVec.size() != 256 ) {
		LOG( Runtime, warning ) << "The size of the colormap " << lutName
								<< " is " << lutVec.size() << " but has to be 256!";
		LOG( Dev, warning ) << "lutVec.size() != 256";
		return false;
	}

	m_ColormapMap[lutName] = lutVec;
	LOG( Dev, verbose_info ) << "Added colormap " << path;
	return true;
}


QIcon Color::getIcon( const std::string &colormapName, size_t w, size_t h, icon_type type, bool flipped ) const
{
	LOG( Dev, verbose_info ) << "Color::getIcon of " << colormapName << "; w: " << w << " h: " << h;
	const ColormapType lut = getColormapMap().at( colormapName );

	unsigned short start = 0;
	unsigned short end = 0;

	switch( type ) {
	case both:
		start = 0;
		end = 256;
		break;
	case lower_half:
		start = 0;
		end = 127;
		break;
	case upper_half:
		start = 128;
		end = 256;
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
	LOG( Dev, verbose_info ) << "Created Icon " << colormapName;
	return QIcon( pixmap.scaled( w, h ) );
}


bool Color::hasColormap( const std::string &name ) const
{
	LOG( Dev, verbose_info ) << "Color::hasColormap";
	return m_ColormapMap.find( name ) != m_ColormapMap.end();
}


Color::ColormapType Color::getFallbackColormap() const
{
	ColormapType retColormap;

	for ( unsigned short i = 0; i < 256; i++ ) {
		retColormap.push_back( QColor( i, i, i, 255 ).rgba() );
	}

	return retColormap;
}


void Color::adaptColorMapToImage( ImageHolder *image )
{
	LOG_IF( image->getImageProperties().colorMap.size() != 256, Runtime, error )
			<< "The colormap is of size "
			<< image->getImageProperties().colorMap.size()
			<< " but has to be of size " << 256 << "!";

	ColormapType retMap( 256 ) ; // implicitely 0 initialised

	ColormapType tmpMap  = util::Singletons::get<Color, 10>().getColormapMap().at( image->getImageProperties().lut );

	const double extent         = image->getImageProperties().extent;         // total value range, i.e. max - min
	const double minVal         = image->getImageProperties().minMax.first.as<double>();
	const double maxVal         = image->getImageProperties().minMax.second.as<double>();
	const double offset         = image->getImageProperties().offset;         // color map offset
	const double scaling        = image->getImageProperties().scaling;        // color map "scaling"
	const double lowerThreshold = image->getImageProperties().lowerThreshold; // minVal<=lowerThreshold<=0, slider
	const double upperThreshold = image->getImageProperties().upperThreshold; // 0<=upperThreshold<=maxVal, slider

	const double norm           = 255.0 / extent;

	if( image->getImageProperties().imageType == ImageHolder::statistical_image ) {
		// Program options "-stats" / "-zmap" leads us here.
		// Overlay statistical attribute values on top of the boring grey level images

		if( ( minVal < 0 ) && ( maxVal > 0 ) ) { // the usual case for "regular" zmaps
			ColormapType negVec( 128 ); // the "zmap's" lower (usually negative) part
			ColormapType posVec( 128 ); // the upper half

			const float negMapEnd   = 127.0 * ( 1.0 - lowerThreshold / minVal );
			const float negzMapIncr = 128.0 / floor( negMapEnd );
			const float posMapStart = 1.0 + ( upperThreshold / maxVal ) * 128.0;
			const float poszMapIncr = 128.0 / ( 128.0 - posMapStart );

			float zMapIndex = 0;

			for( short i = 0; i < floor( negMapEnd ); i++ ) { // starting with zero, using [i+1] below
				if( zMapIndex >= 128.0 ) {
					// sanity check, never happens ...
					std::cout << __FILE__ << __LINE__ << ": adjusting zMap index from "
							  << zMapIndex << " to 127." << std::endl;
					zMapIndex = 127.0;
				}

				negVec[1 + i] = tmpMap[ round( zMapIndex ) ]; // first colormap entry is reserved for the background
				zMapIndex += negzMapIncr;
			}

			// std::cout << "neg: i=0-" << negMapEnd << " -> zMapIndex=" << zMapIndex << std::endl;

			zMapIndex = 128; // explicitely set to the upper half start index

			for( short i = round( posMapStart ); i < 128; i++ ) {
				if( zMapIndex >= 256.0 ) {
					// sanity check, we should not see output from here
					std::cout << __FILE__ << __LINE__ << ": adjusting zMapIndex from "
							  << zMapIndex << " to 255." << std::endl;
					zMapIndex = 255;
				}

				posVec[i] = tmpMap[zMapIndex];
				zMapIndex += poszMapIncr;
			}

			// std::cout << "pos: " << posMapStart << " - " << zMapIndex << std::endl;

			// combine into the final tables
			retMap = negVec << posVec;
		} else if ( minVal >= 0 && maxVal > 0 ) { // all stat/"zmap" values > 0
			const float tgtStartIndex = 1.0 + upperThreshold * norm; // tgt lut index
			const float zMapIndexIncr = 127.0 / ( 256.0 - floor( tgtStartIndex ) ) ;

			float zMapIndex = 128.0; // zmap style ! Positive values start in the "middle"

			for( short tgtMapIndex = floor( tgtStartIndex ); tgtMapIndex < 256; tgtMapIndex++ ) {
				if( zMapIndex >= 256.0 ) { // sanity check
					// virtually dead code
					std::cout << __FILE__ << ": @tgtStartIndex=" << tgtStartIndex << ": adjusting statMapIndex from "
							  << zMapIndex << " to " << 255
							  << ", tgtMapIndex=" << tgtMapIndex << std::endl;
					zMapIndex = 255.0;
				}

				retMap[tgtMapIndex] = tmpMap[zMapIndex];

				zMapIndex += zMapIndexIncr;
			}
		} else {
			std::cout << __FILE__ << __LINE__ << ", min and max < 0, unhandled case ..."
					  << std::endl;
		}
	} else {
		// the "nonstatistical case", a regular MR dataset
		for ( unsigned short i = 1; i < 256; i++ ) {
			short scaledVal = scaling * ( i - ( offset * norm ) );

			if( scaledVal < 0 ) {
				scaledVal = 0;
			} else if( scaledVal > 255 ) {
				scaledVal = 255;
			}

			retMap[i] = tmpMap[scaledVal];
		}
	}

	// assure background reservation
	retMap[0] = QColor( 0, 0, 0, 0 ).rgba();
	image->getImageProperties().alphaMap[0] = 0;

	image->getImageProperties().colorMap = retMap;
}


} // end namespace
