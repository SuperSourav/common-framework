#include "OfflineDileptonVariables/DileptonClassBDT.h"
#include "TTHbbToolManager/ToolManager.h"
#include "TTHbbObjects/TTHbbUtils.h"
#include "TTHbbConfiguration/GlobalConfiguration.h"

namespace TTHbb{
    
//____________________________________________________

    DileptonClassBDT::DileptonClassBDT() {


    }
//____________________________________________________
    
    DileptonClassBDT::~DileptonClassBDT() {
    
    }
//____________________________________________________

    EventSampling DileptonClassBDT::initEventSampling(unsigned int modulo, unsigned int rest, std::string pattern) {
        EventSampling evtS;
        evtS.modulo = modulo;
        evtS.rest.push_back(rest);
        evtS.patternName = pattern;

        return evtS;
    }
//____________________________________________________

    void DileptonClassBDT::fillBDTMap(std::vector<std::string> vecParameters) {

        // Flush the content of current map:
        if (!m_mapBDTs.empty()) m_mapBDTs.clear();

        // parameterString = BDTNAME1 XMLRADICALBDT1 BDTNAME2 XMLRADICAL
        if (vecParameters.size()%2 != 0) {
            
            std::cout << "ERROR in DileptonClassBDT: Unequal number of BDT names and file names." << std::endl;
            exit(2);
        }

        // Patch for now:
        std::string str = "${TTHbbAnalysis_DIR}";
        if (m_xmlPath.find(str) != std::string::npos)
            m_xmlPath.replace(m_xmlPath.find(str), str.length(), "");
        m_xmlPath.insert(0, std::string(getenv("TTHbbAnalysis_DIR")));
        
        // For every two arguments, add the first as BDTmap key, second to BDTContainer xml
        for (unsigned int p = 0; p < vecParameters.size() ; p++) {

            std::string BDTID  = vecParameters.at(p);
            p++;
            std::string baseFile = m_xmlPath + "/" + vecParameters.at(p);

            // Open txt file with output name (1st line), region (2nd line) and variables (3rd line, comma separated)
            std::vector<std::string> vecBDTInfo = TTHbb::util::fileToVector(baseFile + ".txt");
	    
	    std::cout << p << " _ " << BDTID << " - " << baseFile << std::endl;

            // Temp: check vector is not empty (method TTHbb:util::fileToVector has not check (2017.07.02)
            if (vecBDTInfo.size() < 3) {
                std::cout << "ERROR in DileptonClassBDT: No output name and/or region and/or variables provided for BDTs. File path issue?" << std::endl;
                exit(2);
            }

            // Create map entry
            BDTContainer BDT;
            BDT.outputName      = vecBDTInfo.at(0);
            BDT.XMLBaseFileName = vecParameters.at(p);
            BDT.regionFlag      = vecBDTInfo.at(1);
            BDT.vars            = TTHbb::util::vectoriseString(vecBDTInfo.at(2));

            m_mapBDTs.insert(std::make_pair(BDTID, BDT));
        } 

    }

//____________________________________________________

    void DileptonClassBDT::initialise() {

        auto* config = TTHbb::GlobalConfiguration::get();
        
        m_xmlPath = (*config)("DileptonClassBDT.XMLpath"); 
        
        std::vector<std::string> paramTokens = TTHbb::util::vectoriseString((*config)("DileptonClassBDT.AddBDTs"), " ");
        
        // Info BDTs stored in m_mapBDTs[BDTName] -> BDTContainer 
        fillBDTMap(paramTokens);

        // Test: print map:
        printBDTInfo();

        // Initializing sampling convention
        EventSampling OddSampling  = initEventSampling(2, 1, "modulo_2_rest_0"); // training is Even
        EventSampling EvenSampling = initEventSampling(2, 0, "modulo_2_rest_1"); // training is Odd
       
        // Looping over BDTs
        for (auto bdt : m_mapBDTs) {

            TMVAConfiguration tmvaConfig;
            tmvaConfig.samplings.push_back(OddSampling);
            tmvaConfig.samplings.push_back(EvenSampling);
            tmvaConfig.name = bdt.second.outputName;
            tmvaConfig.setCalibFilePattern(m_xmlPath + "/" + bdt.second.XMLBaseFileName);
            tmvaConfig.setVariables(bdt.second.vars);
            tmvaConfig.bookReader();

            // Connect this tmvaConfig to RegionInterface for ClassBDTCalculatorTool
            RegionInterface::RegionDefinition regionInterfaceBDT;

            // Applying cuts (hardcoded for the moment)
	        //  SR1:"DilepRegion_A3 == 1",SR2:"DilepRegion_A3 == 3",SR3:"DilepRegion_A3 == 2"

	        if (bdt.second.regionFlag == "SR1") {
	            regionInterfaceBDT.intLowerCut("DilepRegion_A3", 1);
	            regionInterfaceBDT.intUpperCut("DilepRegion_A3", 1);
            }
            else if (bdt.second.regionFlag == "SR2") {
	            regionInterfaceBDT.intLowerCut("DilepRegion_A3", 3);
	            regionInterfaceBDT.intUpperCut("DilepRegion_A3", 3);
            }
            else if (bdt.second.regionFlag == "SR3") {
	            regionInterfaceBDT.intLowerCut("DilepRegion_A3", 2);
	            regionInterfaceBDT.intUpperCut("DilepRegion_A3", 2);
            }
            else
	            std::cout << "WARNING in DileptonClassBDT: unknown region flag: " << bdt.second.regionFlag << std::endl;
	    
            regionInterfaceBDT.tmvaConfiguration = tmvaConfig;

            m_classBDTCalculatorTool.addRegion(regionInterfaceBDT);

        } 
    std::cout << "INFO in DileptonClassBDT: BDTs added to TMVAConfiguration." << std::endl;
    }
//____________________________________________________

    void DileptonClassBDT::printBDTInfo() {

        if (m_mapBDTs.empty()) 
            std::cout << "INFO in DileptonClassBDT: no information available on BDTs." << std::endl;
        else {
            std::cout << "INFO in DileptonClassBDT: list of classification BDTs with variables: " << std::endl;
            for (auto bdt : m_mapBDTs) {
                std::cout << "\nBDT ID\t: " << bdt.first << std::endl;
                std::cout << "\tOutput name:\t" << bdt.second.outputName << std::endl;
                std::cout << "\tRegion flag:\t" << bdt.second.regionFlag << std::endl;
                std::cout << "\tXML/info file:\t" << bdt.second.XMLBaseFileName << std::endl;
                std::cout << "\tVariables:" << std::endl;
                for (auto var : bdt.second.vars)
                    std::cout << "\t\t" << var << std::endl;
            }
        }

    }
//____________________________________________________

    void DileptonClassBDT::apply(TTHbb::Event* event) {
        
        defaultValues(event);
        
        m_classBDTCalculatorTool.computeBDT(event, 0, 0, 0, event->m_info->eventNumber);

    }
//____________________________________________________

    void DileptonClassBDT::defaultValues(TTHbb::Event* event) const {
       
        if (!m_mapBDTs.empty()) { 
            for (auto bdt : m_mapBDTs) {

                if (true || !event->checkFloatVariable(bdt.second.outputName)) {
                    event->floatVariable(bdt.second.outputName) = -9.;
                }
            }
        }
    }
//____________________________________________________
    
    void DileptonClassBDT::finalise() {
    }
//____________________________________________________
}
