#include "plugininterface.h"
#include "CorrelationPlotter.hpp"


namespace isis
{
namespace viewer
{
namespace plugin
{
	
class CorrelationPlotter : public PluginInterface
{
public:
	virtual std::string getName() { return std::string( "CorrelationPlotter" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "C, P" ) ;}
	virtual bool isGUI() { return true; }
	virtual bool call() {
		if( !m_CorrelationPlotterSet ) {	
			m_CorrelationPlotter = new CorrelationPlotterDialog( parentWidget, viewerCore ) ;
			viewerCore->getUI()->getMainWindow()->getUI().topGridLayout->addWidget( m_CorrelationPlotter );
			m_CorrelationPlotterSet = true;
		}
		if( m_Visible ) {
			m_CorrelationPlotter->close();
		} else {
			m_CorrelationPlotter->show();
		}
		m_Visible = !m_Visible;};
	CorrelationPlotter() : m_Visible(false), m_CorrelationPlotterSet(false) {}

	virtual ~CorrelationPlotter() {};
private:
	bool m_Visible;
	bool m_CorrelationPlotterSet;
	CorrelationPlotterDialog *m_CorrelationPlotter;
	
	
};
	
	
	
	
}}}


isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::CorrelationPlotter();
}