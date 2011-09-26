#include "Color.hpp"



namespace isis
{
namespace viewer
{
	
Color::Color()
	: m_NumberOfElements( 256 ),
	m_LutType( standard_grey_values ),
	m_OffsetScaling( std::make_pair<double, double>(0.0, 1.0) )
{

}
	
void Color::update()
{
	m_ColorTable.resize( m_NumberOfElements );
	switch( m_LutType ) {
		case Color::standard_grey_values: {
			for ( size_t i = 0; i < m_NumberOfElements; i++ ) {
				m_ColorTable[i] = QColor( i, i, i, 255 ).rgba();
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

			break;
		}
	}

}




}
} // end namespace