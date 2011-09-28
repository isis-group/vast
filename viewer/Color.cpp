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
	m_ColorTable.resize( m_NumberOfElements );
	switch( m_LutType ) {
		case Color::standard_grey_values: {
			int value = m_OffsetScaling.first;
			int sum = (m_NumberOfElements * m_OffsetScaling.second) / m_NumberOfElements;
			for ( size_t i = 0; i < m_NumberOfElements; i++ ) {
				m_ColorTable[i] = QColor( value, value, value, 255 ).rgba();
				value+sum > 255 ? value : value+=sum;
			}
			break;
		}
		case Color::zmap_standard: {
			for( size_t i = 0; i < ( m_NumberOfElements / 2 ); i++ ) {
				if( i < ( m_NumberOfElements / 4 ) ) {
					m_ColorTable[( m_NumberOfElements - 1 ) - ( ( m_NumberOfElements / 2 ) - i - 1 )] = QColor( 255, i * 4, i * 2, 255 ).rgba();
					m_ColorTable[( m_NumberOfElements - 1 ) - ( i + ( m_NumberOfElements / 2 ) )] = QColor( i * 2, i * 4, 255, 255 ).rgba();
				} else {
					m_ColorTable[( m_NumberOfElements - 1 ) - ( ( m_NumberOfElements / 2 ) - i - 1 )] = QColor( 255, 255, i * 2, 255 ).rgba();
					m_ColorTable[( m_NumberOfElements - 1 ) - ( i + ( m_NumberOfElements / 2 ) )] = QColor( i * 2, 255, 255, 255 ).rgba();
				}
			}
			m_ColorTable[0] = QColor(0,0,0,0).rgba();
			break;
		}
	}
	if( m_OmitZeros && m_ImageHolder.get() ) {
		double extent = m_ImageHolder->getMinMax().second->as<double>() - m_ImageHolder->getMinMax().first->as<double>();
		int elemZero = (m_NumberOfElements / extent) * fabs(m_ImageHolder->getMinMax().first->as<double>());
		m_ColorTable[elemZero] = QColor(0,0,0,0).rgba();
	}
	
}




}
} // end namespace