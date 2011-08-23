#include "QGLWidgetImplementation.hpp"
#include <QVBoxLayout>
#include <QDockWidget>
#include <QMouseEvent>
#include "GLShaderCode.hpp"
#include <sys/stat.h>


namespace isis
{
namespace viewer
{
namespace GL
{


QGLWidgetImplementation::QGLWidgetImplementation( QViewerCore *core, QWidget *parent, QGLWidget *share, PlaneOrientation orientation )
	: QGLWidget( parent, share ),
	  WidgetImplenentationBase( core, parent, orientation ),
	  m_ShareWidget( share )
	  
{
	//( new QVBoxLayout( parent ) )->addWidget( this ); implemented now in base class
	commonInit();
}

QGLWidgetImplementation::QGLWidgetImplementation( QViewerCore *core, QWidget *parent, PlaneOrientation orientation )
	: QGLWidget( parent ),
	  WidgetImplenentationBase( core, parent, orientation )
{
// 	( new QVBoxLayout( parent ) )->addWidget( this ); implemented in base class
	commonInit();
}


void QGLWidgetImplementation::commonInit()
{
// 	setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored ) );
	setFocusPolicy( Qt::StrongFocus );
	setMouseTracking( true );
	connectSignals();
	m_ScalingType = static_cast<ScalingType>( m_ViewerCore->getSettings()->value("UserProfile/scaling", 0).toUInt());
	m_InterplationType = static_cast<GLTextureHandler::InterpolationType>( m_ViewerCore->getSettings()->value("UserProfile/interpolation", 0).toUInt());
	m_ScalingPair = std::make_pair<double, double>( 0.0, 1.0 );
	//flags
	m_Flags.zoomEvent = false;
	m_Flags.leftButtonPressed = false;
	m_Flags.rightButtonPressed = false;
	m_Flags.strgKeyPressed = false;
	m_Flags.init = true;
	m_Flags.glInitialized = false;
	m_ShowLabels = m_ViewerCore->getSettings()->value("UserProfile/labels", false).toBool();
	
}

WidgetImplenentationBase *QGLWidgetImplementation::createSharedWidget( QWidget *parent, PlaneOrientation orientation )
{
	return new QGLWidgetImplementation( m_ViewerCore, parent, this, orientation );
}

void QGLWidgetImplementation::addImage( const boost::shared_ptr<ImageHolder> image )
{
	m_ImageStates.insert( std::make_pair<boost::shared_ptr<ImageHolder>, State>
						  ( image  , State() ) );
}

bool QGLWidgetImplementation::removeImage( const boost::shared_ptr<ImageHolder> image )
{
}


void QGLWidgetImplementation::connectSignals()
{
	connect( this, SIGNAL( redraw() ), SLOT( updateGL() ) );
	connect( this, SIGNAL( voxelCoordsChanged( util::ivector4 ) ), m_ViewerCore, SLOT( voxelCoordsChanged ( util::ivector4 ) ) );
	connect( this, SIGNAL( physicalCoordsChanged( util::fvector4 ) ), m_ViewerCore, SLOT( physicalCoordsChanged ( util::fvector4 ) ) );
	connect( this, SIGNAL( zoomChanged(float)), m_ViewerCore, SLOT( zoomChanged(float)));
	connect( m_ViewerCore, SIGNAL( emitVoxelCoordChanged( util::ivector4 ) ), this, SLOT( lookAtVoxel( util::ivector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( lookAtPhysicalCoords( util::fvector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitTimeStepChange( unsigned int ) ), this, SLOT( timestepChanged( unsigned int ) ) );
	connect( m_ViewerCore, SIGNAL( emitShowLabels( bool ) ), this, SLOT( setShowLabels( bool ) ) );
	connect( m_ViewerCore, SIGNAL( emitUpdateScene( bool ) ), this, SLOT( updateScene( bool ) ) );
	connect( m_ViewerCore, SIGNAL( emitSetAutomaticScaling( bool ) ), this, SLOT( setAutomaticScaling( bool ) ) );
	connect( m_ViewerCore, SIGNAL( emitZoomChanged(float)), this, SLOT( setZoom(float)));

}

void QGLWidgetImplementation::initializeGL()
{
	LOG( Debug, info ) << "Using OpenGL version " << glGetString( GL_VERSION ) << " .";
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	m_ScalingShader.createContext();
	m_LUTShader.createContext();
	m_ScalingShader.addShader( "scaling", scaling_shader_code, GLShader::fragment );
	m_LUTShader.addShader( "lut", colormap_shader_code, GLShader::fragment );
	checkAndReportGLError( "initializeGL" );
	m_Flags.glInitialized = true;

}


void QGLWidgetImplementation::resizeGL( int w, int h )
{
	makeCurrent();
	LOG( Debug, verbose_info ) << "resizeGL " << objectName().toStdString();
	if( m_ImageStates.size() ) {
		if( m_Flags.init ) {
			util::ivector4 size = m_ImageStates.begin()->first->getImageSize();
			lookAtPhysicalCoords( m_ImageStates.begin()->first->getImage()->getPhysicalCoordsFromIndex( util::ivector4( size[0] / 2, size[1] / 2, size[2] / 2 ) ) );
			m_Flags.init = false;
		} else {
			updateScene();
		}

	}
}

void QGLWidgetImplementation::updateStateValues( boost::shared_ptr<ImageHolder> image, const util::ivector4 &voxelCoords )
{
	LOG( Debug, verbose_info ) << "Updating state values for widget " << objectName().toStdString();
	State &state = m_ImageStates.at( image );
	unsigned int timestep = state.voxelCoords[3];
	state.voxelCoords = voxelCoords;
	state.physicalCoords = image->getImage()->getPhysicalCoordsFromIndex(voxelCoords);
	state.voxelCoords[3] = timestep;

	//if not happend already copy the image to GLtexture memory and return the texture id
	if(image->getImageSize()[3] > m_ViewerCore->getCurrentImage()->getImageState().timestep ) {
		state.textureID = util::Singletons::get<GLTextureHandler, 10>().copyImageToTexture( image, m_ViewerCore->getCurrentImage()->getImageState().timestep, false, m_InterplationType );
	}

	//update the texture matrix.
	//The texture matrix holds the orientation of the image and the orientation of the current widget. It does NOT hold the scaling of the image.
	if( ! state.set ) {
		state.planeOrientation =
			GLOrientationHandler::transformToPlaneView( image->getNormalizedImageOrientation(), m_PlaneOrientation );
		GLOrientationHandler::boostMatrix2Pointer( GLOrientationHandler::addOffset( state.planeOrientation ), state.textureMatrix );
		state.mappedVoxelSize = GLOrientationHandler::transformVector<float>( image->getPropMap().getPropertyAs<util::fvector4>( "voxelSize" ) + image->getPropMap().getPropertyAs<util::fvector4>( "voxelGap" ) , state.planeOrientation );
		state.mappedImageSize = GLOrientationHandler::transformVector<int>( image->getImageSize(), state.planeOrientation );
		state.set = true;
		state.lutID =  m_LookUpTable.getLookUpTableAsTexture( image->getImageState().lookUpTableType );
	}

	state.mappedVoxelCoords = GLOrientationHandler::transformVector<int>( state.voxelCoords, state.planeOrientation );
	//to visualize with the correct scaling we take the viewport
	unsigned short border = 0;

	if( m_ShowLabels ) {
		border = 30;
	}

	GLOrientationHandler::recalculateViewport( width(), height(), state.mappedVoxelSize, state.mappedImageSize, state.viewport, border );

	if( !m_Flags.leftButtonPressed || m_Flags.zoomEvent  ) {
		m_Flags.zoomEvent = false;
		calculateTranslation( );
	}

	util::dvector4 objectCoords = GLOrientationHandler::transformVoxel2ObjectCoords( state.voxelCoords, image, state.planeOrientation );
	state.crosshairCoords = object2WindowCoords( objectCoords[0], objectCoords[1], image );
	state.normalizedSlice = objectCoords[2];
	image->setCurrentVoxelCoords( voxelCoords );
	image->setCurrentPhysicalCoords( image->getImage()->getPhysicalCoordsFromIndex( voxelCoords ));

}

std::pair<GLdouble, GLdouble> QGLWidgetImplementation::window2ObjectCoords( int16_t winx, int16_t winy, const boost::shared_ptr<ImageHolder> image ) const
{
	const State &stateValue = m_ImageStates.at( image );
	GLdouble pos[3];
	gluUnProject( winx, winy, 0, stateValue.modelViewMatrix, stateValue.projectionMatrix, stateValue.viewport , &pos[0], &pos[1], &pos[2] );
	return std::make_pair<GLdouble, GLdouble>( pos[0], pos[1] );

}

std::pair<int16_t, int16_t> QGLWidgetImplementation::object2WindowCoords( GLdouble objx, GLdouble objy, const boost::shared_ptr<ImageHolder> image ) const
{
	const State &stateValue = m_ImageStates.at( image );
	GLdouble win[3];
	GLdouble pro[16];
	gluProject( objx, objy, 0, stateValue.modelViewMatrix, stateValue.projectionMatrix, stateValue.viewport, &win[0], &win[1], &win[2] );
	return std::make_pair<int16_t, int16_t>( ( win[0] - stateValue.viewport[0] ), win[1] - stateValue.viewport[1] );
}


bool QGLWidgetImplementation::calculateTranslation(  )
{
	State state = m_ImageStates.at(getOptimalImage());
	
	std::pair<int16_t, int16_t> center = std::make_pair<int16_t, int16_t>( abs( state.mappedImageSize[0] ) / 2, abs( state.mappedImageSize[1] ) / 2 );
	float shiftX = center.first - ( state.mappedVoxelCoords[0] < 0 ? abs( state.mappedImageSize[0] ) + state.mappedVoxelCoords[0] : state.mappedVoxelCoords[0] );
	float shiftY =  center.second - ( state.mappedVoxelCoords[1] < 0 ? abs( state.mappedImageSize[1] ) + state.mappedVoxelCoords[1] : state.mappedVoxelCoords[1] );
	shiftX = ( 1.0 / abs( state.mappedImageSize[0] ) ) * shiftX ;
	shiftY = ( 1.0 / abs( state.mappedImageSize[1] ) ) * shiftY ;
	float zoomDependentShift = 1.0 - ( 2.0 / m_Zoom.currentZoom );
	BOOST_FOREACH( StateMap::reference stateRef, m_ImageStates ) {
		stateRef.second.modelViewMatrix[12] = shiftX + zoomDependentShift * shiftX + 0.02 * shiftX;
		stateRef.second.modelViewMatrix[13] = shiftY + zoomDependentShift * shiftY + 0.02 * shiftY;
	}
}


bool QGLWidgetImplementation::lookAtPhysicalCoords( const isis::util::fvector4 &physicalCoords )
{
	BOOST_FOREACH( StateMap::const_reference state, m_ImageStates ) {
		updateStateValues(  state.first, state.first->getImage()->getIndexFromPhysicalCoords( physicalCoords ) );
	}

	if( m_ImageStates.size() ) {
		redraw();
	}
}


bool QGLWidgetImplementation::lookAtVoxel( const isis::util::ivector4 &voxelCoords )
{
	LOG( Debug, verbose_info ) << "Looking at voxel: " << voxelCoords;
	//someone has told the widget to paint all available images.
	//So first we have to update the state values for each image
	BOOST_FOREACH( StateMap::const_reference state, m_ImageStates ) {
		updateStateValues(  state.first, voxelCoords );
	}

	if( m_ImageStates.size() ) {
		redraw();
	}
}


void QGLWidgetImplementation::paintGL()
{
	glClearColor( 0, 0, 0, 0 );
	glClear( GL_COLOR_BUFFER_BIT );
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	//paint anatomical images
	BOOST_FOREACH( StateMap::const_reference state, m_ImageStates ) {
		if( state.first.get() != m_ViewerCore->getCurrentImage().get() && state.first->getImageState().visible && state.first->getImageState().imageType == ImageHolder::anatomical_image) {
			paintImage( state );
		}
	}
	
	//paint zmaps		
	BOOST_FOREACH( StateMap::const_reference state, m_ImageStates ) {
		if( state.first.get() != m_ViewerCore->getCurrentImage().get() && state.first->getImageState().visible && state.first->getImageState().imageType == ImageHolder::z_map) {
			paintImage( state );
		}
	}
	if(m_ImageStates.find(m_ViewerCore->getCurrentImage() ) != m_ImageStates.end() && m_ViewerCore->getCurrentImage()->getImageState().visible ) {
		paintImage( std::make_pair<boost::shared_ptr<ImageHolder>, State >( m_ViewerCore->getCurrentImage(), m_ImageStates.at( m_ViewerCore->getCurrentImage() )) );
	}
	glFlush();
	checkAndReportGLError( "painting the scene" );

	if( !m_ImageStates.empty() ) {
		paintCrosshair();
	}
}

void QGLWidgetImplementation::paintImage( const std::pair< boost::shared_ptr<ImageHolder>, State> &state) 
{
	double scaling, bias;
	if( m_ScalingType == automatic_scaling ) {
		scaling = state.first->getOptimalScalingPair().second;
		bias = state.first->getOptimalScalingPair().first;
	} else if ( m_ScalingType == manual_scaling ) {
		scaling = m_ScalingPair.second;
		bias = m_ScalingPair.first;
	} else {
		scaling = 1.0;
		bias = 0.0;
	}

	glViewport( state.second.viewport[0], state.second.viewport[1], state.second.viewport[2], state.second.viewport[3] );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glLoadMatrixd( state.second.projectionMatrix );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glLoadMatrixd( state.second.modelViewMatrix );

	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();
	glLoadMatrixd( state.second.textureMatrix );
	//shader

	//if the image is declared as a zmap
	if( state.first->getImageState().imageType == ImageHolder::z_map ) {
		m_LUTShader.setEnabled( true );
		glEnable( GL_TEXTURE_1D );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_1D, state.second.lutID );
		m_LUTShader.addVariable<float>( "lut", 1, true );
		m_LUTShader.addVariable<float>( "max", state.first->getMinMax().second->as<float>() );
		m_LUTShader.addVariable<float>( "min", state.first->getMinMax().first->as<float>() );
		m_LUTShader.addVariable<float>( "upper_threshold", state.first->getImageState().zmapThreshold.second );
		m_LUTShader.addVariable<float>( "lower_threshold", state.first->getImageState().zmapThreshold.first );
		m_LUTShader.addVariable<float>( "bias", 0.0 );
		m_LUTShader.addVariable<float>( "scaling", 1.0 );
		m_LUTShader.addVariable<float>( "opacity", state.first->getImageState().opacity );
		glDisable( GL_TEXTURE_1D );
	} else if ( state.first->getImageState().imageType == ImageHolder::anatomical_image ) {
		m_ScalingShader.setEnabled( true );
		m_ScalingShader.addVariable<float>( "max", state.first->getMinMax().second->as<float>() );
		m_ScalingShader.addVariable<float>( "min", state.first->getMinMax().first->as<float>() );
		m_ScalingShader.addVariable<float>( "upper_threshold",  state.first->getImageState().threshold.second );
		m_ScalingShader.addVariable<float>( "lower_threshold", state.first->getImageState().threshold.first );
		m_ScalingShader.addVariable<float>( "scaling", scaling );
		m_ScalingShader.addVariable<float>( "bias", bias );
		m_ScalingShader.addVariable<float>( "opacity", state.first->getImageState().opacity );
		m_ScalingShader.addVariable<float>( "killZeros", 1.0 );
	}

	glEnable( GL_TEXTURE_3D );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_3D, state.second.textureID );
	glBegin( GL_QUADS );
	glTexCoord3f( 0, 0, state.second.normalizedSlice );
	glVertex2f( -1.0, -1.0 );
	glTexCoord3f( 0, 1, state.second.normalizedSlice );
	glVertex2f( -1.0, 1.0 );
	glTexCoord3f( 1, 1, state.second.normalizedSlice );
	glVertex2f( 1.0, 1.0 );
	glTexCoord3f( 1, 0, state.second.normalizedSlice );
	glVertex2f( 1.0, -1.0 );
	glEnd();
	glDisable( GL_TEXTURE_3D );
	m_ScalingShader.setEnabled(false);
	m_LUTShader.setEnabled(false);
}


void QGLWidgetImplementation::paintCrosshair()
{
	if( m_LUTShader.isEnabled() ) {
		m_LUTShader.setEnabled( false );
	}

	if( m_ScalingShader.isEnabled() ) {
		m_ScalingShader.setEnabled( false );
	}

	glUseProgram( 0 );
	//paint crosshair
	const State &currentState = m_ImageStates.at(getOptimalImage());
	glDisable( GL_TEXTURE_1D );

	glLineWidth( 1.0 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glOrtho( 0, currentState.viewport[2], 0, currentState.viewport[3], -1, 1 );
	glBegin( GL_LINES );
	glColor3f( 1.0, 0.4, 0.0 );
	unsigned short gap = height() / 20;
	glVertex3i( currentState.crosshairCoords.first , 0, 1 );
	glVertex3i( currentState.crosshairCoords.first, currentState.crosshairCoords.second - gap , 1 );
	glVertex3i( currentState.crosshairCoords.first, currentState.crosshairCoords.second + gap, 1 );
	glVertex3i( currentState.crosshairCoords.first, height(), 1 );

	glVertex3i( 0, currentState.crosshairCoords.second, 1 );
	glVertex3i( currentState.crosshairCoords.first - gap, currentState.crosshairCoords.second, 1 );
	glVertex3i( currentState.crosshairCoords.first + gap, currentState.crosshairCoords.second, 1 );
	glVertex3i( width(), currentState.crosshairCoords.second, 1 );
	glEnd();
	glPointSize( 2.0 );
	glBegin( GL_POINTS );
	glVertex3d( currentState.crosshairCoords.first, currentState.crosshairCoords.second, 1 );
	glEnd();
	glLoadIdentity();

	if( m_ShowLabels ) {
		viewLabels();
	}

	checkAndReportGLError( "painting the crosshair" );
	glColor3f( 1, 1, 1 );
}



void QGLWidgetImplementation::viewLabels()
{
	QFont font;
	font.setPointSize( 15 );
	font.setPixelSize( 15 );
	glColor4f( 0.0, 1.0, 1.0, 1.0 );
	glViewport( 0, 0, width(), height() );

	switch( m_PlaneOrientation ) {
	case axial:
		glColor4f( 0.0, 1.0, 0.0, 1.0 );
		renderText( width() / 2 - 7, 25, QString( "A" ), font );
		renderText( width() / 2 - 7, height() - 10, QString( "P" ), font );
		glColor4f( 1.0, 0.0, 0.0, 1.0 );
		renderText( 5, height() / 2, QString( "L" ), font );
		renderText( width() - 15, height() / 2, QString( "R" ), font );
		break;
	case sagittal:
		glColor4f( 0.0, 1.0, 0.0, 1.0 );
		renderText( 5, height() / 2, QString( "A" ), font );
		renderText( width() - 15, height() / 2, QString( "P" ), font );
		glColor4f( 0.0, 0.0, 1.0, 1.0 );
		renderText( width() / 2 - 7, 25, QString( "S" ), font );
		renderText( width() / 2 - 7, height() - 10, QString( "I" ), font );
		break;
	case coronal:
		glColor4f( 0.0, 0.0, 1.0, 1.0 );
		renderText( width() / 2 - 7, 25, QString( "S" ), font );
		renderText( width() / 2 - 7, height() - 10, QString( "I" ), font );
		glColor4f( 1.0, 0.0, 0.0, 1.0 );
		renderText( 5, height() / 2, QString( "L" ), font );
		renderText( width() - 15, height() / 2, QString( "R" ), font );
		break;
	}

	checkAndReportGLError( "painting labels" );
}


void QGLWidgetImplementation::mouseMoveEvent( QMouseEvent *e )
{
	if ( m_Flags.rightButtonPressed || m_Flags.leftButtonPressed ) {
		emitMousePressEvent( e );
	}
}

void QGLWidgetImplementation::mousePressEvent( QMouseEvent *e )
{
	if( e->button() == Qt::LeftButton ) {
		m_Flags.leftButtonPressed = true;
	}

	if( e->button() == Qt::RightButton ) {
		m_Flags.rightButtonPressed = true;
	}

	emitMousePressEvent( e );

}

void QGLWidgetImplementation::emitMousePressEvent( QMouseEvent *e )
{
	
	std::pair<float, float> objectCoords = window2ObjectCoords( e->x(), height() - e->y(), getOptimalImage());
	util::ivector4 voxelCoords = GLOrientationHandler::transformObject2VoxelCoords( util::fvector4( objectCoords.first, objectCoords.second, m_ImageStates.at( getOptimalImage() ).normalizedSlice ), getOptimalImage(), m_PlaneOrientation );
	physicalCoordsChanged( getOptimalImage()->getImage()->getPhysicalCoordsFromIndex( voxelCoords ) );
}

bool QGLWidgetImplementation::timestepChanged( unsigned int timestep )
{
	physicalCoordsChanged( m_ImageStates.begin()->first->getImageState().physicalCoords );
	updateScene();

}

void QGLWidgetImplementation::setZoom(float zoomFactor)
{
	BOOST_FOREACH( StateMap::reference state, m_ImageStates ) {
		glMatrixMode( GL_PROJECTION );
		glLoadMatrixd( state.second.projectionMatrix );
		glScalef( zoomFactor, zoomFactor, 1 );
		glGetDoublev( GL_PROJECTION_MATRIX, state.second.projectionMatrix );
		glLoadIdentity();
	}
	m_Zoom.currentZoom *= zoomFactor;
	updateScene();
}


void QGLWidgetImplementation::wheelEvent( QWheelEvent *e )
{
	float zoomFactor = 1;
	if( e->delta() < 0 ) {
		zoomFactor = m_Zoom.zoomFactorOut;
	} else if ( e->delta() > 0 ) { zoomFactor = m_Zoom.zoomFactorIn; }

	if( m_Zoom.currentZoom * zoomFactor < 64 && m_Zoom.currentZoom * zoomFactor >= 1) {
#warning change this!!!!!!!!!
		if(m_ViewerCore->getOption()->propagateZooming) {
			zoomChanged( zoomFactor );
		} else {
			setZoom(zoomFactor);
		}
		m_Flags.zoomEvent = true;
	}
}

void QGLWidgetImplementation::mouseReleaseEvent( QMouseEvent *e )
{
	if( e->button() == Qt::LeftButton ) {
		m_Flags.leftButtonPressed = false;
	}

	if( e->button() == Qt::RightButton ) {
		m_Flags.rightButtonPressed = false;
	}

	redraw();
}


void QGLWidgetImplementation::keyPressEvent( QKeyEvent *e )
{
	if( e->key() == Qt::Key_Space ) {
		BOOST_FOREACH( StateMap::reference ref, m_ImageStates ) {
			GLOrientationHandler::makeIdentity( ref.second.modelViewMatrix );
			GLOrientationHandler::makeIdentity( ref.second.projectionMatrix );
		}
		m_Zoom.currentZoom = 1.0;
		util::ivector4 size = m_ImageStates.begin()->first->getImageSize();
		lookAtPhysicalCoords( m_ImageStates.begin()->first->getImage()->getPhysicalCoordsFromIndex( util::ivector4( size[0] / 2, size[1] / 2, size[2] / 2 ) ) );
	}

	if( e->key() == Qt::Key_Control ) {
		m_Flags.strgKeyPressed = true;
	}
}

void QGLWidgetImplementation::keyReleaseEvent( QKeyEvent *e )
{
	if( e->key() == Qt::Key_Control ) {
		m_Flags.strgKeyPressed = false;
	}
}


void  QGLWidgetImplementation::setShowLabels( bool show )
{
	m_ShowLabels = show;
	updateScene();

}

void QGLWidgetImplementation::setInterpolationType( const isis::viewer::GL::GLTextureHandler::InterpolationType interpolation )
{
	m_InterplationType = interpolation;
	updateScene();
}

void QGLWidgetImplementation::updateScene( bool center )
{
	if( !m_ImageStates.empty() ) {
		util::ivector4 voxelCoords;
		if(m_Flags.init) { 
		    center = true;
		    m_Flags.init = false; 
		}
		if( !center ) {
			voxelCoords =  m_ImageStates.begin()->second.voxelCoords ;
		} else {
			util::ivector4 size = m_ImageStates.begin()->first->getImageSize();
			voxelCoords = util::ivector4( size[0] / 2, size[1] / 2, size[2] / 2 );
		}
		lookAtPhysicalCoords( m_ImageStates.begin()->first->getImage()->getPhysicalCoordsFromIndex( voxelCoords ) );
	}
}

boost::shared_ptr< ImageHolder > QGLWidgetImplementation::getOptimalImage() const
{
	if( m_ImageStates.find( m_ViewerCore->getCurrentImage()) != m_ImageStates.end()  ) {
		return m_ViewerCore->getCurrentImage();
	} else {
		return m_ImageStates.begin()->first;
	}
}


}
}
} // end namespace