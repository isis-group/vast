#include "color.hpp"



namespace isis
{
namespace viewer
{

Color::Color()
	: m_NumberOfElements( 256 ),
	  m_LutType( standard_grey_values ),
	  m_OffsetScaling( std::make_pair<double, double>( 0.0, 1.0 ) ),
	  m_OmitZeros( false )
{

}

void Color::update()
{
	QVector<QRgb> tmpTable( m_NumberOfElements );
	double extent = m_ImageHolder->getPropMap().getPropertyAs<double>( "extent" );
	double norm =  ( double )m_NumberOfElements / extent;
	m_ColorTable.resize( m_NumberOfElements );

	switch( m_LutType ) {
	case Color::standard_grey_values: {
		int value = m_OffsetScaling.first;
		int sum = ( m_NumberOfElements * m_OffsetScaling.second ) / m_NumberOfElements;

		for ( unsigned short i = 0; i < m_NumberOfElements; i++ ) {
			tmpTable[i] = QColor( value, value, value, 255 ).rgba();
			value + sum > 255 ? value : value += sum;
		}

		break;
	}
	case Color::zmap_standard: {
		for( unsigned short i = 0; i < ( m_NumberOfElements / 2 ); i++ ) {
			if( i < ( m_NumberOfElements / 4 ) ) {
				tmpTable[( m_NumberOfElements - 1 ) - ( ( m_NumberOfElements / 2 ) - i - 1 )] = QColor( 255, i * 4, i * 2, 255 ).rgba();
				tmpTable[( m_NumberOfElements - 1 ) - ( i + ( m_NumberOfElements / 2 ) )] = QColor( i * 2, i * 4, 255, 255 ).rgba();
			} else {
				tmpTable[( m_NumberOfElements - 1 ) - ( ( m_NumberOfElements / 2 ) - i - 1 )] = QColor( 255, 255, i * 2, 255 ).rgba();
				tmpTable[( m_NumberOfElements - 1 ) - ( i + ( m_NumberOfElements / 2 ) )] = QColor( i * 2, 255, 255, 255 ).rgba();
			}

		}

		break;
	}
	case Color::colormap1: {
		const float val = 0.664921;

		for ( unsigned short i = 0; i < 128; i++ ) {
			if( i < 95 ) {
				tmpTable[255 - ( i )] = QColor( 255, ( int )rint( i * 0.664921 * 4 ), ( int )rint( i * 0.664921 * 2 ), 255 ).rgba();
				tmpTable[255 - ( 127 - i + 128 )] = QColor( ( int )rint( i * 0.664921 * 2 ), ( int )rint( i * 0.664921 * 4 ), 255, 255 ).rgba();
			} else {
				tmpTable[255 - ( i )] = QColor( 255, 255, int( i * 1.953846 * 2 - 245.184601 ), 255 ).rgba();
				tmpTable[255 - ( 127 - i + 128 )] = QColor( int( i * 1.953846 * 2 - 245.184601 ), 255, 255, 255 ).rgba();
			}
		}

		break;
	}
	case colormap2: {
		for ( unsigned short i = 0; i < m_NumberOfElements; i++ ) {
			if( i < m_NumberOfElements / 2 ) {
				tmpTable[255 - ( 127 - i )] = QColor::fromHsv( int( ( 360 / 60 ) * ( i / 6.4 ) ), 255,  255 ).rgba();
			} else {
				tmpTable[255 - ( 383 - i )] = QColor::fromHsv( int( ( 360 / 60 ) * ( i / 6.4 ) ), 255,  255 ).rgba();
			}
		}

		break;
	}
	case colormap3: {
		for ( unsigned short i = 0; i < m_NumberOfElements; i++ ) {
			if( i < m_NumberOfElements / 2 ) {
				tmpTable[255 - i] = QColor::fromHsv( int( ( 360 / 60 ) * ( i / 6.4 ) ), 255,  255 ).rgba();
			} else {
				tmpTable[255 - ( ( i - 128 ) + 128 )] = QColor::fromHsv( int( ( 360 / 60 ) * ( i / 6.4 ) ), 255,  255 ).rgba();
			}
		}

		break;
	}
	case colormap4: {
		for ( unsigned short i = 0; i < 128; i++ ) {
			if ( i < 32 ) {
				tmpTable[128 + i] = qRgba( i * 4, i * 8, 255, 255 );
				tmpTable[255 - ( 128 + i )] = qRgba( i * 4, i * 8, 255, 255 );
			} else  if ( i < 64 ) {
				tmpTable[128 + i] = qRgba( i * 4, 255, 255, 255 );
				tmpTable[255 - ( 128 + i )] = qRgba( i * 4, 255, 255, 255 );
			} else if ( i < 96 ) {
				tmpTable[128 + i] = qRgba( 255, 255, 1023 - 4 * i, 255 );
				tmpTable[255 - ( 128 + i )] = qRgba( 255, 255, 1023 - 4 * i, 255 );
			} else {
				tmpTable[128 + i] = qRgba( 255, 2047 - 8 * i, 1023 - 4 * i, 255 );
				tmpTable[255 - ( 128 + i )] = qRgba( 255, 2047 - 8 * i, 1023 - 4 * i, 255 );
			}
		}

		break;
	}
	case colormap5: {
		for ( unsigned short i = 0; i < 128; i++ ) {
			tmpTable[255 - i] = QColor::fromHsv( ( int )( i * 300 / 127 ), 255,  255 ).rgba();
			tmpTable[i] = QColor::fromHsv( ( int )( i * 300 / 127 ), 255,  255 ).rgba();
		}

		break;
	}
	case colormap6: {
		for ( int i = 0; i < 256; i++ ) {
			int h = ( 360 / 20 ) * ( i / 6.4 );
			tmpTable[255 - i] =  QColor::fromHsv( h > 359 ? h - 360 : h, 255,  255 ).rgba();
		}

		break;
	}
	case colormap7: {
		for ( unsigned short i = 0; i < 256; i++ ) {
			tmpTable[255 - i] = QColor::fromHsv(  int( ( 360 / 40 ) * ( i / 6.4 ) ), 255,  255 ).rgba();
		}

		break;
	}
	case colormap8: {
		for ( unsigned short i = 0; i < 256; i++ ) {
			if ( i < 128 ) {
				int h = 40 + int( 320 * i / 127 );
				tmpTable[127 - i] = QColor::fromHsv( h > 359 ? h - 360 : h , 255,  255 ).rgba();
			} else {
				int h = 40 + int( 320 * ( i - 128 ) / 127 );
				tmpTable[i] = QColor::fromHsv(  h > 359 ? h - 360 : h, 255,  255 ).rgba();
			}
		}

		break;
	}
	case colormap9: {
		for ( unsigned short i = 0; i < 256; i++ ) {
			tmpTable[255 - i] = QColor( 255 - i, 255 - i, 255 - i ).rgba();
		}

		break;
	}

	case colormap10: {
		for ( unsigned short i = 0 ; i < 256; i++ ) {
			int h = int( ( 360 / 10 ) * ( i / 6.4 ) );
			int diff = h / 360;
			tmpTable[255 - i] = QColor::fromHsv( h > 359 ? h - diff * 360 : h, 255,  255 ).rgba();
		}

		break;
	}
	case colormap11: {
		for ( unsigned short i = 0 ; i < 256; i++ ) {
			srand( ( unsigned int )i );
			int m1 = ( int ) ( 256.0 * rand() / ( RAND_MAX + 1.0 ) );
			srand( ( unsigned int )i + 100 );
			int m2 = ( int ) ( 256.0 * rand() / ( RAND_MAX + 1.0 ) );
			srand( ( unsigned int )i + 200 );
			int m3 = ( int ) ( 256.0 * rand() / ( RAND_MAX + 1.0 ) );
			tmpTable[i] = QColor( m1, m2, m3 ).rgba();
		}

		break;
	}
	case colormap12: {
		tmpTable[0] = QColor( 0, 0, 131 ).rgba();
		tmpTable[1] = QColor( 0, 0, 135 ).rgba();
		tmpTable[2] = QColor( 0, 0, 139 ).rgba();
		tmpTable[3] = QColor( 0, 0, 143 ).rgba();
		tmpTable[4] = QColor( 0, 0, 147 ).rgba();
		tmpTable[5] = QColor( 0, 0, 151 ).rgba();
		tmpTable[6] = QColor( 0, 0, 155 ).rgba();
		tmpTable[7] = QColor( 0, 0, 159 ).rgba();
		tmpTable[8] = QColor( 0, 0, 163 ).rgba();
		tmpTable[9] = QColor( 0, 0, 167 ).rgba();
		tmpTable[10] = QColor( 0, 0, 171 ).rgba();
		tmpTable[11] = QColor( 0, 0, 175 ).rgba();
		tmpTable[12] = QColor( 0, 0, 179 ).rgba();
		tmpTable[13] = QColor( 0, 0, 183 ).rgba();
		tmpTable[14] = QColor( 0, 0, 187 ).rgba();
		tmpTable[15] = QColor( 0, 0, 191 ).rgba();
		tmpTable[16] = QColor( 0, 0, 195 ).rgba();
		tmpTable[17] = QColor( 0, 0, 199 ).rgba();
		tmpTable[18] = QColor( 0, 0, 203 ).rgba();
		tmpTable[19] = QColor( 0, 0, 207 ).rgba();
		tmpTable[20] = QColor( 0, 0, 211 ).rgba();
		tmpTable[21] = QColor( 0, 0, 215 ).rgba();
		tmpTable[22] = QColor( 0, 0, 219 ).rgba();
		tmpTable[23] = QColor( 0, 0, 223 ).rgba();
		tmpTable[24] = QColor( 0, 0, 227 ).rgba();
		tmpTable[25] = QColor( 0, 0, 231 ).rgba();
		tmpTable[26] = QColor( 0, 0, 235 ).rgba();
		tmpTable[27] = QColor( 0, 0, 239 ).rgba();
		tmpTable[28] = QColor( 0, 0, 243 ).rgba();
		tmpTable[29] = QColor( 0, 0, 247 ).rgba();
		tmpTable[30] = QColor( 0, 0, 251 ).rgba();
		tmpTable[31] = QColor( 0, 0, 255 ).rgba();
		tmpTable[32] = QColor( 0, 4, 255 ).rgba();
		tmpTable[33] = QColor( 0, 8, 255 ).rgba();
		tmpTable[34] = QColor( 0, 12, 255 ).rgba();
		tmpTable[35] = QColor( 0, 16, 255 ).rgba();
		tmpTable[36] = QColor( 0, 20, 255 ).rgba();
		tmpTable[37] = QColor( 0, 24, 255 ).rgba();
		tmpTable[38] = QColor( 0, 28, 255 ).rgba();
		tmpTable[39] = QColor( 0, 32, 255 ).rgba();
		tmpTable[40] = QColor( 0, 36, 255 ).rgba();
		tmpTable[41] = QColor( 0, 40, 255 ).rgba();
		tmpTable[42] = QColor( 0, 44, 255 ).rgba();
		tmpTable[43] = QColor( 0, 48, 255 ).rgba();
		tmpTable[44] = QColor( 0, 52, 255 ).rgba();
		tmpTable[45] = QColor( 0, 56, 255 ).rgba();
		tmpTable[46] = QColor( 0, 60, 255 ).rgba();
		tmpTable[47] = QColor( 0, 64, 255 ).rgba();
		tmpTable[48] = QColor( 0, 68, 255 ).rgba();
		tmpTable[49] = QColor( 0, 72, 255 ).rgba();
		tmpTable[50] = QColor( 0, 76, 255 ).rgba();
		tmpTable[51] = QColor( 0, 80, 255 ).rgba();
		tmpTable[52] = QColor( 0, 84, 255 ).rgba();
		tmpTable[53] = QColor( 0, 88, 255 ).rgba();
		tmpTable[54] = QColor( 0, 92, 255 ).rgba();
		tmpTable[55] = QColor( 0, 96, 255 ).rgba();
		tmpTable[56] = QColor( 0, 100, 255 ).rgba();
		tmpTable[57] = QColor( 0, 104, 255 ).rgba();
		tmpTable[58] = QColor( 0, 108, 255 ).rgba();
		tmpTable[59] = QColor( 0, 112, 255 ).rgba();
		tmpTable[60] = QColor( 0, 116, 255 ).rgba();
		tmpTable[61] = QColor( 0, 120, 255 ).rgba();
		tmpTable[62] = QColor( 0, 124, 255 ).rgba();
		tmpTable[63] = QColor( 0, 128, 255 ).rgba();
		tmpTable[64] = QColor( 0, 131, 255 ).rgba();
		tmpTable[65] = QColor( 0, 135, 255 ).rgba();
		tmpTable[66] = QColor( 0, 139, 255 ).rgba();
		tmpTable[67] = QColor( 0, 143, 255 ).rgba();
		tmpTable[68] = QColor( 0, 147, 255 ).rgba();
		tmpTable[69] = QColor( 0, 151, 255 ).rgba();
		tmpTable[70] = QColor( 0, 155, 255 ).rgba();
		tmpTable[71] = QColor( 0, 159, 255 ).rgba();
		tmpTable[72] = QColor( 0, 163, 255 ).rgba();
		tmpTable[73] = QColor( 0, 167, 255 ).rgba();
		tmpTable[74] = QColor( 0, 171, 255 ).rgba();
		tmpTable[75] = QColor( 0, 175, 255 ).rgba();
		tmpTable[76] = QColor( 0, 179, 255 ).rgba();
		tmpTable[77] = QColor( 0, 183, 255 ).rgba();
		tmpTable[78] = QColor( 0, 187, 255 ).rgba();
		tmpTable[79] = QColor( 0, 191, 255 ).rgba();
		tmpTable[80] = QColor( 0, 195, 255 ).rgba();
		tmpTable[81] = QColor( 0, 199, 255 ).rgba();
		tmpTable[82] = QColor( 0, 203, 255 ).rgba();
		tmpTable[83] = QColor( 0, 207, 255 ).rgba();
		tmpTable[84] = QColor( 0, 211, 255 ).rgba();
		tmpTable[85] = QColor( 0, 215, 255 ).rgba();
		tmpTable[86] = QColor( 0, 219, 255 ).rgba();
		tmpTable[87] = QColor( 0, 223, 255 ).rgba();
		tmpTable[88] = QColor( 0, 227, 255 ).rgba();
		tmpTable[89] = QColor( 0, 231, 255 ).rgba();
		tmpTable[90] = QColor( 0, 235, 255 ).rgba();
		tmpTable[91] = QColor( 0, 239, 255 ).rgba();
		tmpTable[92] = QColor( 0, 243, 255 ).rgba();
		tmpTable[93] = QColor( 0, 247, 255 ).rgba();
		tmpTable[94] = QColor( 0, 251, 255 ).rgba();
		tmpTable[95] = QColor( 0, 255, 255 ).rgba();
		tmpTable[96] = QColor( 4, 255, 251 ).rgba();
		tmpTable[97] = QColor( 8, 255, 247 ).rgba();
		tmpTable[98] = QColor( 12, 255, 243 ).rgba();
		tmpTable[99] = QColor( 16, 255, 239 ).rgba();
		tmpTable[100] = QColor( 20, 255, 235 ).rgba();
		tmpTable[101] = QColor( 24, 255, 231 ).rgba();
		tmpTable[102] = QColor( 28, 255, 227 ).rgba();
		tmpTable[103] = QColor( 32, 255, 223 ).rgba();
		tmpTable[104] = QColor( 36, 255, 219 ).rgba();
		tmpTable[105] = QColor( 40, 255, 215 ).rgba();
		tmpTable[106] = QColor( 44, 255, 211 ).rgba();
		tmpTable[107] = QColor( 48, 255, 207 ).rgba();
		tmpTable[108] = QColor( 52, 255, 203 ).rgba();
		tmpTable[109] = QColor( 56, 255, 199 ).rgba();
		tmpTable[110] = QColor( 60, 255, 195 ).rgba();
		tmpTable[111] = QColor( 64, 255, 191 ).rgba();
		tmpTable[112] = QColor( 68, 255, 187 ).rgba();
		tmpTable[113] = QColor( 72, 255, 183 ).rgba();
		tmpTable[114] = QColor( 76, 255, 179 ).rgba();
		tmpTable[115] = QColor( 80, 255, 175 ).rgba();
		tmpTable[116] = QColor( 84, 255, 171 ).rgba();
		tmpTable[117] = QColor( 88, 255, 167 ).rgba();
		tmpTable[118] = QColor( 92, 255, 163 ).rgba();
		tmpTable[119] = QColor( 96, 255, 159 ).rgba();
		tmpTable[120] = QColor( 100, 255, 155 ).rgba();
		tmpTable[121] = QColor( 104, 255, 151 ).rgba();
		tmpTable[122] = QColor( 108, 255, 147 ).rgba();
		tmpTable[123] = QColor( 112, 255, 143 ).rgba();
		tmpTable[124] = QColor( 116, 255, 139 ).rgba();
		tmpTable[125] = QColor( 120, 255, 135 ).rgba();
		tmpTable[126] = QColor( 124, 255, 131 ).rgba();
		tmpTable[127] = QColor( 128, 255, 128 ).rgba();
		tmpTable[128] = QColor( 131, 255, 124 ).rgba();
		tmpTable[129] = QColor( 135, 255, 120 ).rgba();
		tmpTable[130] = QColor( 139, 255, 116 ).rgba();
		tmpTable[131] = QColor( 143, 255, 112 ).rgba();
		tmpTable[132] = QColor( 147, 255, 108 ).rgba();
		tmpTable[133] = QColor( 151, 255, 104 ).rgba();
		tmpTable[134] = QColor( 155, 255, 100 ).rgba();
		tmpTable[135] = QColor( 159, 255, 96 ).rgba();
		tmpTable[136] = QColor( 163, 255, 92 ).rgba();
		tmpTable[137] = QColor( 167, 255, 88 ).rgba();
		tmpTable[138] = QColor( 171, 255, 84 ).rgba();
		tmpTable[139] = QColor( 175, 255, 80 ).rgba();
		tmpTable[140] = QColor( 179, 255, 76 ).rgba();
		tmpTable[141] = QColor( 183, 255, 72 ).rgba();
		tmpTable[142] = QColor( 187, 255, 68 ).rgba();
		tmpTable[143] = QColor( 191, 255, 64 ).rgba();
		tmpTable[144] = QColor( 195, 255, 60 ).rgba();
		tmpTable[145] = QColor( 199, 255, 56 ).rgba();
		tmpTable[146] = QColor( 203, 255, 52 ).rgba();
		tmpTable[147] = QColor( 207, 255, 48 ).rgba();
		tmpTable[148] = QColor( 211, 255, 44 ).rgba();
		tmpTable[149] = QColor( 215, 255, 40 ).rgba();
		tmpTable[150] = QColor( 219, 255, 36 ).rgba();
		tmpTable[151] = QColor( 223, 255, 32 ).rgba();
		tmpTable[152] = QColor( 227, 255, 28 ).rgba();
		tmpTable[153] = QColor( 231, 255, 24 ).rgba();
		tmpTable[154] = QColor( 235, 255, 20 ).rgba();
		tmpTable[155] = QColor( 239, 255, 16 ).rgba();
		tmpTable[156] = QColor( 243, 255, 12 ).rgba();
		tmpTable[157] = QColor( 247, 255, 8 ).rgba();
		tmpTable[158] = QColor( 251, 255, 4 ).rgba();
		tmpTable[159] = QColor( 255, 255, 0 ).rgba();
		tmpTable[160] = QColor( 255, 251, 0 ).rgba();
		tmpTable[161] = QColor( 255, 247, 0 ).rgba();
		tmpTable[162] = QColor( 255, 243, 0 ).rgba();
		tmpTable[163] = QColor( 255, 239, 0 ).rgba();
		tmpTable[164] = QColor( 255, 235, 0 ).rgba();
		tmpTable[165] = QColor( 255, 231, 0 ).rgba();
		tmpTable[166] = QColor( 255, 227, 0 ).rgba();
		tmpTable[167] = QColor( 255, 223, 0 ).rgba();
		tmpTable[168] = QColor( 255, 219, 0 ).rgba();
		tmpTable[169] = QColor( 255, 215, 0 ).rgba();
		tmpTable[170] = QColor( 255, 211, 0 ).rgba();
		tmpTable[171] = QColor( 255, 207, 0 ).rgba();
		tmpTable[172] = QColor( 255, 203, 0 ).rgba();
		tmpTable[173] = QColor( 255, 199, 0 ).rgba();
		tmpTable[174] = QColor( 255, 195, 0 ).rgba();
		tmpTable[175] = QColor( 255, 191, 0 ).rgba();
		tmpTable[176] = QColor( 255, 187, 0 ).rgba();
		tmpTable[177] = QColor( 255, 183, 0 ).rgba();
		tmpTable[178] = QColor( 255, 179, 0 ).rgba();
		tmpTable[179] = QColor( 255, 175, 0 ).rgba();
		tmpTable[180] = QColor( 255, 171, 0 ).rgba();
		tmpTable[181] = QColor( 255, 167, 0 ).rgba();
		tmpTable[182] = QColor( 255, 163, 0 ).rgba();
		tmpTable[183] = QColor( 255, 159, 0 ).rgba();
		tmpTable[184] = QColor( 255, 155, 0 ).rgba();
		tmpTable[185] = QColor( 255, 151, 0 ).rgba();
		tmpTable[186] = QColor( 255, 147, 0 ).rgba();
		tmpTable[187] = QColor( 255, 143, 0 ).rgba();
		tmpTable[188] = QColor( 255, 139, 0 ).rgba();
		tmpTable[189] = QColor( 255, 135, 0 ).rgba();
		tmpTable[190] = QColor( 255, 131, 0 ).rgba();
		tmpTable[191] = QColor( 255, 128, 0 ).rgba();
		tmpTable[192] = QColor( 255, 124, 0 ).rgba();
		tmpTable[193] = QColor( 255, 120, 0 ).rgba();
		tmpTable[194] = QColor( 255, 116, 0 ).rgba();
		tmpTable[195] = QColor( 255, 112, 0 ).rgba();
		tmpTable[196] = QColor( 255, 108, 0 ).rgba();
		tmpTable[197] = QColor( 255, 104, 0 ).rgba();
		tmpTable[198] = QColor( 255, 100, 0 ).rgba();
		tmpTable[199] = QColor( 255, 96, 0 ).rgba();
		tmpTable[200] = QColor( 255, 92, 0 ).rgba();
		tmpTable[201] = QColor( 255, 88, 0 ).rgba();
		tmpTable[202] = QColor( 255, 84, 0 ).rgba();
		tmpTable[203] = QColor( 255, 80, 0 ).rgba();
		tmpTable[204] = QColor( 255, 76, 0 ).rgba();
		tmpTable[205] = QColor( 255, 72, 0 ).rgba();
		tmpTable[206] = QColor( 255, 68, 0 ).rgba();
		tmpTable[207] = QColor( 255, 64, 0 ).rgba();
		tmpTable[208] = QColor( 255, 60, 0 ).rgba();
		tmpTable[209] = QColor( 255, 56, 0 ).rgba();
		tmpTable[210] = QColor( 255, 52, 0 ).rgba();
		tmpTable[211] = QColor( 255, 48, 0 ).rgba();
		tmpTable[212] = QColor( 255, 44, 0 ).rgba();
		tmpTable[213] = QColor( 255, 40, 0 ).rgba();
		tmpTable[214] = QColor( 255, 36, 0 ).rgba();
		tmpTable[215] = QColor( 255, 32, 0 ).rgba();
		tmpTable[216] = QColor( 255, 28, 0 ).rgba();
		tmpTable[217] = QColor( 255, 24, 0 ).rgba();
		tmpTable[218] = QColor( 255, 20, 0 ).rgba();
		tmpTable[219] = QColor( 255, 16, 0 ).rgba();
		tmpTable[220] = QColor( 255, 12, 0 ).rgba();
		tmpTable[221] = QColor( 255, 8, 0 ).rgba();
		tmpTable[222] = QColor( 255, 4, 0 ).rgba();
		tmpTable[223] = QColor( 255, 0, 0 ).rgba();
		tmpTable[224] = QColor( 251, 0, 0 ).rgba();
		tmpTable[225] = QColor( 247, 0, 0 ).rgba();
		tmpTable[226] = QColor( 243, 0, 0 ).rgba();
		tmpTable[227] = QColor( 239, 0, 0 ).rgba();
		tmpTable[228] = QColor( 235, 0, 0 ).rgba();
		tmpTable[229] = QColor( 231, 0, 0 ).rgba();
		tmpTable[230] = QColor( 227, 0, 0 ).rgba();
		tmpTable[231] = QColor( 223, 0, 0 ).rgba();
		tmpTable[232] = QColor( 219, 0, 0 ).rgba();
		tmpTable[233] = QColor( 215, 0, 0 ).rgba();
		tmpTable[234] = QColor( 211, 0, 0 ).rgba();
		tmpTable[235] = QColor( 207, 0, 0 ).rgba();
		tmpTable[236] = QColor( 203, 0, 0 ).rgba();
		tmpTable[237] = QColor( 199, 0, 0 ).rgba();
		tmpTable[238] = QColor( 195, 0, 0 ).rgba();
		tmpTable[239] = QColor( 191, 0, 0 ).rgba();
		tmpTable[240] = QColor( 187, 0, 0 ).rgba();
		tmpTable[241] = QColor( 183, 0, 0 ).rgba();
		tmpTable[242] = QColor( 179, 0, 0 ).rgba();
		tmpTable[243] = QColor( 175, 0, 0 ).rgba();
		tmpTable[244] = QColor( 171, 0, 0 ).rgba();
		tmpTable[245] = QColor( 167, 0, 0 ).rgba();
		tmpTable[246] = QColor( 163, 0, 0 ).rgba();
		tmpTable[247] = QColor( 159, 0, 0 ).rgba();
		tmpTable[248] = QColor( 155, 0, 0 ).rgba();
		tmpTable[249] = QColor( 151, 0, 0 ).rgba();
		tmpTable[250] = QColor( 147, 0, 0 ).rgba();
		tmpTable[251] = QColor( 143, 0, 0 ).rgba();
		tmpTable[252] = QColor( 139, 0, 0 ).rgba();
		tmpTable[253] = QColor( 135, 0, 0 ).rgba();
		tmpTable[254] = QColor( 131, 0, 0 ).rgba();
		tmpTable[255] = QColor( 128, 0, 0 ).rgba();
		break;
	}


	}

	if( m_ImageHolder->getImageProperties().imageType == ImageHolder::z_map ) {
		double normMin = fabs( m_ImageHolder->getMinMax().first->as<double>() ) / ( extent / 2 );
		if ( normMin == 0 ) { normMin = 1; }

		if( m_ImageHolder->getMinMax().first->as<double>() < 0 ) {
			double scaleMin = 1 - fabs( m_ImageHolder->getPropMap().getPropertyAs<double>( "lowerThreshold" )  / m_ImageHolder->getMinMax().first->as<double>() );

			for ( unsigned short i = 0; i < m_NumberOfElements / 2; i++ ) {
				m_ColorTable[i * scaleMin] = tmpTable[i / normMin];
			}
		}

		if( m_ImageHolder->getMinMax().second->as<double>() > 0 ) {
			double scaleMax = fabs( m_ImageHolder->getPropMap().getPropertyAs<double>( "upperThreshold" ) / m_ImageHolder->getMinMax().second->as<double>() );
			double offset = 0;

			for ( unsigned short i = (m_NumberOfElements / 2) - 1; i < m_NumberOfElements; i++ ) {
				offset = ( ( m_NumberOfElements / 2 ) * scaleMax ) * ( ( m_NumberOfElements - i ) / ( 0.5 * m_NumberOfElements ) );
				m_ColorTable[i + offset] = tmpTable[i / normMin];
			}
		}
	} else {
		m_ColorTable = tmpTable;
	}


	m_ColorTable[0] = QColor( 0, 0, 0, 0 ).rgba();



	int elemZero = norm * fabs( m_ImageHolder->getMinMax().first->as<double>() );

	if( m_OmitZeros && m_ImageHolder.get() ) {
		m_ColorTable[elemZero] = QColor( 0, 0, 0, 0 ).rgba();
	}

	//killing the values which are below/above the lowerThreshold/upperThreshold
	if( m_ImageHolder->getImageProperties().imageType == ImageHolder::z_map ) {
		for( int i = 0; i < m_NumberOfElements; i++ ) {
			if( i >= elemZero && i < ( elemZero + norm * m_ImageHolder->getPropMap().getPropertyAs<double>( "upperThreshold" ) ) ) {
				m_ColorTable[i] =  QColor( 0, 0, 0, 0 ).rgba();
			} else if( i < elemZero && i > ( elemZero - fabs( norm * m_ImageHolder->getPropMap().getPropertyAs<double>( "lowerThreshold" ) ) ) ) {
				m_ColorTable[i] = QColor( 0, 0, 0, 0 ).rgba();
			}
		}
	}




}

void Color::resetOffsetAndScaling()
{
	m_OffsetScaling = std::make_pair<double, double>( 0.0, 1.0 );
}



}
} // end namespace