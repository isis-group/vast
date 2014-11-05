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
	LOG( Dev, verbose_info ) << "Adding colormap " << path;
	QFile lutFile( path.c_str() );
	lutFile.open( QIODevice::ReadOnly );

	if( !lutFile.isReadable() ) {
		LOG( Dev, warning ) << "The QResource \"" << path << "\" is not readable!";
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
		LOG( Dev, warning ) << "Can not read colormap " << path << " because header is missing or has wrong syntax (name:type)!";
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
	LOG_IF( image->getImageProperties().colorMap.size() != 256, Runtime, error ) << "The colormap is of size "
			<< image->getImageProperties().colorMap.size() << " but has to be of size " << 256 << "!";
	ColormapType retMap ;
	retMap.resize( 256 );
	ColormapType tmpMap = util::Singletons::get<Color, 10>().getColormapMap().at( image->getImageProperties().lut );
	const double extent = image->getImageProperties().extent;
	const double min = image->getImageProperties().minMax.first->as<double>();
	const double max = image->getImageProperties().minMax.second->as<double>();
	const double offset = image->getImageProperties().offset;
	const double scaling = image->getImageProperties().scaling;
	const double norm = 256.0 / extent;

	short scaledVal;

	for ( unsigned short i = 0; i < 256; i++ ) {
		scaledVal = scaling * ( i - ( offset * norm ) );

		if( scaledVal < 0 ) scaledVal = 0;

		if( scaledVal > 255 ) scaledVal = 255;

		retMap[i] = tmpMap[scaledVal];
	}

	retMap[0] = QColor( 0, 0, 0, 0 ).rgba();
	image->getImageProperties().alphaMap[0] = 0;

	//only stuff necessary for colormaps
	if( image->getImageProperties().imageType == ImageHolder::statistical_image ) {

		unsigned short mid = ( norm * fabs( min ) );
		ColormapType negVec( mid );
		AlphamapType negAlphas( mid );
		ColormapType posVec( 256-mid );
		AlphamapType posAlphas( 256-mid );
		const double lowerThreshold = image->getImageProperties().lowerThreshold;
		const double upperThreshold = image->getImageProperties().upperThreshold;

		double imageScale=1;

		switch(image->getImageProperties().majorTypeID)
		{
			case data::ValueArray<float>::staticID:
			case data::ValueArray<double>::staticID:
			break;
			default:
				if(extent<256)
					imageScale=128/extent;
			break;
		}

		
		//fill negVec
		if( min < 0 ) {
			//where the negative colormap should end (index for "lowest" color) / rest until mid will be black
			const double negMapEnd = (1 - lowerThreshold / min) * mid; // lowerThreshold < min => (1 - lowerThreshold / min) < 1 => negMapEnd < mid
			for(unsigned short i=1;i<negMapEnd;i++){ //first entry in the colormap is reserved for background
				int scaledVal = i/(scaling*imageScale);
				
				if( scaledVal < 1 ) scaledVal = 1;
				if( scaledVal > 127 ) scaledVal = 127;
				
				negVec[i]=tmpMap[scaledVal];
				negAlphas[i] = 1;
			}
		}

		if( max > 0 ) {
			//where the positive colormap should start (index for "lowest" color) / rest from 128 will be black
			const double posMapStart = (upperThreshold / max) * (256-mid); // lowerThreshold < min => (1 - lowerThreshold / min) < 1 => negMapEnd < mid
			for(unsigned short i=std::max<unsigned short>(1,posMapStart);i<(256-mid-1);i++){ //first entry in the colormap is reserved for background
				int scaledVal = i*scaling*imageScale+128;
				
				if( scaledVal <= 128 ) scaledVal = 128;
				if( scaledVal >= 256 ) scaledVal = 255;
				
				posVec[i]=tmpMap[scaledVal];
				posAlphas[i] = 1;
			}
		}

		retMap = negVec << posVec;
		image->getImageProperties().alphaMap = negAlphas << posAlphas;

	}
	image->getImageProperties().colorMap = retMap;
}


}
}
} // end namespace
