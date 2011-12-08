#include "plugininterface.h"
#include "MaskEdit.hpp"

namespace isis {
namespace viewer {
namespace plugin {

	
class MaskEdit : public PluginInterface
{
public:
	virtual std::string getName() { return std::string( "MaskEdit" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "Modifying and creating masks" ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "M, E" ) ;}
	virtual bool isGUI() { return true; }
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/maskEdit.png" ); }
	MaskEdit() : m_Visible(false), m_MaskEditSet(false) {}
	
	virtual bool call() {
		if( !m_MaskEditSet ) {	
			m_MaskEdit = new MaskEditDialog( parentWidget, viewerCore ) ;
			viewerCore->getUICore()->getMainWindow()->getInterface().topGridLayout->addWidget( m_MaskEdit );
			m_MaskEditSet = true;
		}
		if( m_Visible ) {
			m_MaskEdit->close();
		} else {
			m_MaskEdit->show();
		}
		m_Visible = !m_Visible;
	};
	
	virtual ~MaskEdit() {};
private: 
	MaskEditDialog *m_MaskEdit;
	bool m_Visible;
	bool m_MaskEditSet;
	
};
	
	
}}}


isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::MaskEdit();
}