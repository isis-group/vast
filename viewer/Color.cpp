#include "Color.hpp"



namespace isis
{
namespace viewer
{
	
Color::Color()
	: m_NumberOfElements( 256 ),
	m_LutType( standard_grey_values ),
	m_OffsetScaling( std::make_pair<double, double>(0.0, 1.0) ),
	m_OmitZeros( false )
{

}
	
void Color::update()
{
	QVector<QRgb> tmpTable( m_NumberOfElements);
	double extent = m_ImageHolder->getPropMap().getPropertyAs<double>("extent");
	double norm =  (double)m_NumberOfElements / extent;
	m_ColorTable.resize( m_NumberOfElements );
	switch( m_LutType ) {
		case Color::standard_grey_values: {
			int value = m_OffsetScaling.first;
			int sum = (m_NumberOfElements * m_OffsetScaling.second) / m_NumberOfElements;
			for ( size_t i = 0; i < m_NumberOfElements; i++ ) {
				tmpTable[i] = QColor( value, value, value, 255 ).rgba();
				value+sum > 255 ? value : value+=sum;
			}
			break;
		}
		case Color::zmap_standard: {
			for( size_t i = 0; i < ( m_NumberOfElements / 2 ); i++ ) {
				if( i < ( m_NumberOfElements / 4 ) ) {
					tmpTable[( m_NumberOfElements - 1 ) - ( ( m_NumberOfElements / 2 ) - i - 1 )] = QColor( 255, i * 4, i * 2, 255 ).rgba();
					tmpTable[( m_NumberOfElements - 1 ) - ( i + ( m_NumberOfElements / 2 ) )] = QColor( i * 2, i * 4, 255, 255 ).rgba();
				} else {
					tmpTable[( m_NumberOfElements - 1 ) - ( ( m_NumberOfElements / 2 ) - i - 1 )] = QColor( 255, 255, i * 2, 255 ).rgba();
					tmpTable[( m_NumberOfElements - 1 ) - ( i + ( m_NumberOfElements / 2 ) )] = QColor( i * 2, 255, 255, 255 ).rgba();
				}
				
			}
			tmpTable[0] = QColor(0,0,0,0).rgba();
			break;
		}
	}
	
	if( m_ImageHolder->getMinMax().first->as<double>() < 0 ) {
		
		float normMin = fabs(extent / m_ImageHolder->getMinMax().first->as<double>()) / 2.0;
		for(size_t i = 0; i < m_NumberOfElements / 2 / normMin ; i++ ) 
		{
			m_ColorTable[i] = tmpTable[i * normMin];
		}
		float normMax = fabs(extent / m_ImageHolder->getMinMax().second->as<double>()) / 2.0;
		for(size_t i = m_NumberOfElements - (m_NumberOfElements / 2 / normMax); i < m_NumberOfElements ; i++ ) 
		{
			m_ColorTable[i] = tmpTable[i * normMax];
		}
		
	}else {
		m_ColorTable = tmpTable;
	}
	
// 	m_ColorTable = tmpTable;
	//do some additional stuff
	
	
	int elemZero = norm * fabs(m_ImageHolder->getMinMax().first->as<double>());
	if( m_OmitZeros && m_ImageHolder.get() ) {
		m_ColorTable[elemZero] = QColor(0,0,0,0).rgba();
	}
	if( m_ImageHolder->getImageProperties().imageType == ImageHolder::z_map ) {
		for( size_t i = 0; i < m_NumberOfElements; i++ ){
			if( i >= elemZero && i < (elemZero + norm * m_ImageHolder->getPropMap().getPropertyAs<double>("upperThreshold")) )
			{
				m_ColorTable[i] =  QColor(0,0,0,0).rgba();
			} else if( i < elemZero && i > (elemZero - fabs(norm * m_ImageHolder->getPropMap().getPropertyAs<double>("lowerThreshold"))) )
			{
				m_ColorTable[i] = QColor(0,0,0,0).rgba();
			}
		}
	}
	
	
	
	
}

void Color::resetOffsetAndScaling()
{
	m_OffsetScaling = std::make_pair<double, double>(0.0, 1.0);
}



}
} // end namespace