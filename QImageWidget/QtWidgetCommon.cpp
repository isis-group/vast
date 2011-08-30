#include "QtWidgetCommon.hpp"

isis::util::ivector4 isis::viewer::qt::get32BitAlignedSize(const isis::util::ivector4& origSize)
{
    util::ivector4 retSize;
    for ( size_t i = 0; i < 4; i++ ) {
	int m = origSize[i] % 4;
	if(m > 0) {
	    retSize[i] = origSize[i] + 4 - m;
	} else {
	    retSize[i] = origSize[i];
	}
    }
    return retSize;
}
