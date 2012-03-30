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
 * geomhandler.cpp
 *
 * Description:
 *
 *  Created on: Mar 27, 2012
 *      Author: tuerke
 ******************************************************************/

#include "geomhandler.hpp"
#include "geometrical.hpp"
#include <imageholder.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <Adapter/qmatrixconversion.hpp>
#include <DataStorage/common.hpp>

namespace isis {
namespace viewer {
namespace widget {
namespace _internal {

util::fvector4 getPhysicalBoundingBox ( const ImageHolder::Vector images, const PlaneOrientation &orientation )
{
	const geometrical::BoundingBoxType currentBoundingBox = geometrical::getPhysicalBoundingBox(images);

	util::fvector4 ret;
	switch(orientation ) {
		case axial:
			ret = util::fvector4( 	currentBoundingBox[0].first,
									currentBoundingBox[1].first,
									fabs( currentBoundingBox[0].second - currentBoundingBox[0].first ),
									fabs( currentBoundingBox[1].second - currentBoundingBox[1].first ) );
			break;
		case sagittal:
			ret = util::fvector4(	currentBoundingBox[1].first,
									currentBoundingBox[2].first,
									fabs(  currentBoundingBox[1].second - currentBoundingBox[1].first ),
									fabs(  currentBoundingBox[2].second - currentBoundingBox[2].first ) );
			break;
		case coronal:
			ret = util::fvector4(	currentBoundingBox[0].first,
									currentBoundingBox[2].first,
									fabs(  currentBoundingBox[0].second - currentBoundingBox[0].first ),
									fabs(  currentBoundingBox[2].second - currentBoundingBox[2].first ) );
			break;
	}
	return ret * rasteringFac;
}

QTransform getQTransform ( const ImageHolder::Pointer image, const PlaneOrientation& orientation )
{
	using namespace boost::numeric::ublas;
	const matrix<float> mat = extract2DMatrix(image, orientation, false );
	const util::fvector4 mapped_voxelSize = mapCoordsToOrientation(image->getImageProperties().voxelSize, image->getImageProperties().latchedOrientation, orientation, false, true) * rasteringFac;
	const uint16_t slice = mapCoordsToOrientation(image->getImageProperties().voxelCoords, image->getImageProperties().latchedOrientation, orientation, false, true)[2];
	const util::ivector4 mappedCoords = mapCoordsToOrientation(util::ivector4(0,0,slice), image->getImageProperties().latchedOrientation, orientation, true, true);

	vector<float> vc = vector<float>(2);
	vc[0] = mapped_voxelSize[0] / 2.; vc[1] = mapped_voxelSize[1] / 2.;
		
	vector<float> _vc = prod( mat, vc);
	
	const util::fvector4 _io = image->getISISImage()->getPhysicalCoordsFromIndex(mappedCoords) * rasteringFac ;
		
	switch(orientation) {
		case axial:{
			QTransform tr1;
			return QTransform(QTransform(mat(0,0), mat(0,1), mat(1,0), mat(1,1),0,0) * tr1.translate(_io[0] - _vc[0], _io[1] - _vc[1])).scale(mapped_voxelSize[0], mapped_voxelSize[1]);
			break;
		}
		case sagittal:{
			QTransform tr1;
			return QTransform(QTransform(mat(0,0), mat(0,1), mat(1,0), mat(1,1),0,0) * tr1.translate(_io[1] - _vc[0], _io[2] - _vc[1])).scale(mapped_voxelSize[0], mapped_voxelSize[1]);
			break;
		}
		case coronal:{
			QTransform tr1;
			return QTransform(QTransform(mat(0,0), mat(0,1), mat(1,0), mat(1,1),0,0) * tr1.translate(_io[0] - _vc[0], _io[2] - _vc[1])).scale(mapped_voxelSize[0], mapped_voxelSize[1]);
			break;
		}
	}
	return QTransform();
}

QTransform getTransform2ISISSpace ( const PlaneOrientation& orientation, const util::fvector4 &bb )
{
	QTransform retTransform;
	switch ( orientation ) {
		case axial:
			retTransform = QTransform( -1, 0, 0, 1, bb[2] + (bb[0]*2), 0 );
			break;
		case sagittal:{
			retTransform = QTransform( 1, 0, 0, -1, 0, bb[3] + (bb[1]*2) );
			break;
		}
		case coronal:
			retTransform = QTransform( -1, 0, 0, -1, bb[2] + (bb[0]*2), bb[3] + (bb[1]*2) );
			break;
	}
	return retTransform;
}

boost::numeric::ublas::matrix< float > extract2DMatrix ( const boost::shared_ptr<ImageHolder> image, const PlaneOrientation& orientation, bool inverse )
{
	using namespace boost::numeric::ublas;
	
	matrix<float> retMatrix = matrix<float>(2,2);

	matrix<float> latchedOrientation = image->getImageProperties().latchedOrientation;
	matrix<float> latchedOrientation_abs = matrix<float>(4,4);
	
	for( unsigned short i = 0; i<3; i++ ) {
		for( unsigned short j = 0; j<3; j++ ) {
			latchedOrientation_abs(i,j) = fabs(latchedOrientation(i,j));
		}
	}
	matrix<float> invLatchedOrientation = matrix<float>(4,4);
	bool invOk;
	if( inverse ) {
		invOk = data::_internal::inverseMatrix<float>(latchedOrientation_abs, invLatchedOrientation);
		if( !invOk ) {
			LOG( Dev, warning ) << "Failed to usee inverse to extract 2D matrix for image " << image->getImageProperties().filePath
						<< "! Falling back to transposed.";
		}
	}
	if( !inverse || !invOk ) {
		invLatchedOrientation = trans(latchedOrientation_abs);
	}

	const matrix<float> mat = prod( image->getImageProperties().orientation, invLatchedOrientation );
	switch(orientation) {
		case axial:
			retMatrix(0,0) = mat(0,0);
			retMatrix(0,1) = mat(1,0);
			retMatrix(1,0) = mat(0,1);
			retMatrix(1,1) = mat(1,1);
			break;
		case sagittal:
			retMatrix(0,0) = mat(1,1);
			retMatrix(1,0) = mat(1,2);
			retMatrix(0,1) = mat(2,1);
			retMatrix(1,1) = mat(2,2);
			break;
		case coronal:
			retMatrix(0,0) = mat(0,0);
			retMatrix(1,0) = mat(0,2);
			retMatrix(0,1) = mat(2,0);
			retMatrix(1,1) = mat(2,2);
			break;
	}
	return retMatrix;
	
}

util::fvector4 mapPhysicalCoords2Orientation ( const util::fvector4& coords, const PlaneOrientation& orientation )
{
	util::fvector4 retCoords = coords;
	switch( orientation ) {
		case axial:
			retCoords = coords;
			break;
		case sagittal:
			retCoords[0] = coords[1];
			retCoords[1] = coords[2];
			retCoords[2] = coords[0];
			break;
		case coronal:
			retCoords[1] = coords[2];
			retCoords[2] = coords[1];
			break;
	}
	return retCoords;
}



}}}}