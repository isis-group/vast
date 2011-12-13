#include "plugininterface.h"
#include "OrientationCorrection.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{

class OrientationCorrection : public PluginInterface
{
public:
	OrientationCorrection() : isInitialized( false ) {}
	virtual std::string getName() { return std::string( "OrienationCorrection" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "O, C" ) ;}
	virtual bool isGUI() { return true; }
	virtual bool call() {
		if( !isInitialized ) {
			m_OrienatationCorrectionDialog = new OrientatioCorrectionDialog( parentWidget, viewerCore );
			isInitialized = true;
		}

		m_OrienatationCorrectionDialog->show();
		return true;
	};

	virtual ~OrientationCorrection() {};
private:
	OrientatioCorrectionDialog *m_OrienatationCorrectionDialog;
	bool isInitialized;
};

}
}
}

isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::OrientationCorrection();
}