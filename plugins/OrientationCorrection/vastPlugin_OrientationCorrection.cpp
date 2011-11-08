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
	virtual std::string getName() { return std::string( "OrienationCorrection" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual bool isGUI() { return true; }
	virtual bool call() {
		m_OrienatationCorrectionDialog = new OrientatioCorrectionDialog( parentWidget, viewerCore );
		m_OrienatationCorrectionDialog->show();
	};

	virtual ~OrientationCorrection() {};
private:
	OrientatioCorrectionDialog *m_OrienatationCorrectionDialog;
};

}
}
}

isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::OrientationCorrection();
}