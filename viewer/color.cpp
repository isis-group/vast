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
		} else {
			LOG( Runtime, warning ) << "Unknown lut type " << lutTyp << " !";
			return false;
		}
	}

	if( lutVec.size() != 256 ) {
		LOG( Runtime, warning ) << "The size of the colormap " << lutName
								<< " is " << lutVec.size() << " but has to be 256!";
		return false;
	}

	m_ColormapMap[lutName] = lutVec;
	return true;
}


QIcon Color::getIcon( const std::string &colormapName, size_t w, size_t h, icon_type type, bool flipped ) const
{
	const ColormapType lut = getColormapMap()[colormapName];

	unsigned short index = 0;
	unsigned short start = 0;
	unsigned short end = 0;

	switch( type ) {
	case both:
		start = 0;
		end = 256;
		break;
	case lower_half:
		start = 0;
		end = 128;
		break;
	case upper_half:
		start = 128;
		end = 256;
		break;
	}

	data::ValuePtr<uint8_t> lutImage( ( end - start ) * 3 );

	if( !flipped ) {
		for ( unsigned short i = start; i < end; i++ ) {
			lutImage[index++] = QColor( lut[i] ).red();
			lutImage[index++] = QColor( lut[i] ).green();
			lutImage[index++] = QColor( lut[i] ).blue();
		}
	} else {
		for ( short i = end; i > start - 1; i-- ) {
			lutImage[index++] = QColor( lut[i] ).red();
			lutImage[index++] = QColor( lut[i] ).green();
			lutImage[index++] = QColor( lut[i] ).blue();
		}
	}

	QImage image( static_cast<uint8_t *>( lutImage.getRawAddress().get() ), ( end - start ), 1, QImage::Format_RGB888 );
	QPixmap pixmap( QPixmap::fromImage( image ) );
	return QIcon( pixmap.scaled( w, h ) );
}


bool Color::hasColormap( const std::string &name ) const
{
	return m_ColormapMap.find( name ) != m_ColormapMap.end();
}


color::Color::ColormapType Color::getFallbackColormap() const
{
	ColormapType retColormap;

	for ( unsigned short i = 0; i < 256; i++ ) {
		retColormap.push_back( QColor( i, i, i, 255 ).rgba() );
	}

	return retColormap;
}


color::Color::ColormapType color::Color::adaptColorMapToImage( color::Color::ColormapType colorMap, const boost::shared_ptr< ImageHolder > image, bool split )
{
	LOG_IF( colorMap.size() != 256, Runtime, error ) << "The colormap is of size " << colorMap.size() << " but has to be of size 256!";
	color::Color::ColormapType retMap = colorMap;
	const double extent = image->getPropMap().getPropertyAs<double>( "extent" );
	const double min = image->getMinMax().first->as<double>();
	const double max = image->getMinMax().second->as<double>();
	const double lowerThreshold = image->getPropMap().getPropertyAs<double>( "lowerThreshold" );
	const double upperThreshold = image->getPropMap().getPropertyAs<double>( "upperThreshold" );
	const double offset = image->getPropMap().getPropertyAs<double>( "offset" );
	const double scaling = image->getPropMap().getPropertyAs<double>( "scaling" );
	const double norm = 256.0 / extent;
	const unsigned short mid = norm * fabs( min );
	unsigned short scaledVal;

	for ( unsigned short i = 0; i < 256; i++ ) {
		scaledVal = i * scaling + offset * norm > 255 ? 255 : i * scaling + offset * norm;
		retMap[i] = colorMap[scaledVal];
	}

	//only stuff necessary for colormaps
	if( image->getImageProperties().imageType == ImageHolder::z_map ) {
		if( split ) {
			color::Color::ColormapType negVec( mid );
			color::Color::ColormapType posVec( 256 - mid );
			assert( negVec.size() + posVec.size() == 256 );

			//fill negVec
			if( min < 0 ) {
				const double scaleMin = 1 - fabs( lowerThreshold / min );
				const double normMin = 128.0 / mid;

				for ( unsigned short i = 0; i < mid; i++ ) {
					negVec[i * scaleMin] = retMap[i * normMin];
				}
			}

			if( max > 0 ) {
				const double normMax = 128.0 / ( 256 - mid );
				const double scaleMax = fabs( upperThreshold / max );
				const double offset = ( 256 - mid ) * scaleMax;

				for( unsigned short i = 0; i < 256 - mid; i++ ) {
					posVec[i * ( 1 - scaleMax ) + offset] = retMap[128 + i * normMax];
				}
			}

			retMap = negVec << posVec;
		}
	}

	//kill the zero value
	retMap[mid] = QColor( 0, 0, 0, 0 ).rgba();
	return retMap;
}


}
}
} // end namespace