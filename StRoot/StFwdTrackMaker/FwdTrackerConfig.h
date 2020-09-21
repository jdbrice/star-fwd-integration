#ifndef FWD_TRACKER_CONFIG_H
#define FWD_TRACKER_CONFIG_H

#include "TXMLEngine.h"

#include <string>
#include <map>

class FwdTrackerConfig {
protected:

    const std::string valDNE = std::string( "<DNE/>" );
    std::map<std::string, std::string> nodes;

    void mapFile(TXMLEngine &xml, XMLNodePointer_t node, Int_t level, string path = "") {
        if ( "" != path)
            path += ".";
        path += xml.GetNodeName(node);

        const std::string node_name = xml.GetNodeName(node);
        const std::string node_content = xml.GetNodeContent(node) != nullptr ? xml.GetNodeContent(node) : valDNE;

        nodes[ path ] = node_content ;

        XMLAttrPointer_t attr = xml.GetFirstAttr(node);
        while (attr != 0) {
            const std::string attr_name = xml.GetAttrName(attr);
            const std::string attr_val = xml.GetAttrValue(attr) != nullptr ? xml.GetAttrValue(attr) : valDNE;
            nodes[ (path + ":" + attr_name) ] = attr_val;
            attr = xml.GetNextAttr(attr);
        }

        // recursively get all child nodes
        XMLNodePointer_t child = xml.GetChild(node);
        while (child != 0) {
            mapFile(xml, child, level + 2, path);
            child = xml.GetNext(child);
        }
    } // mapFile
public:

    FwdTrackerConfig() {}
    void load( std::string filename ) {
        using namespace std;
        // First create engine
        TXMLEngine xml;

        // Now try to parse xml file
        XMLDocPointer_t xmldoc = xml.ParseFile(filename.c_str());
        if (!xmldoc) { // parse failed, TODO inform of error
            return;
        }

        // access to root node (should be "config")
        XMLNodePointer_t root_node = xml.DocGetRootElement(xmldoc);
        // build the file map for config access
        mapFile(xml, root_node, 1);

        // Release memory before finishing
        xml.FreeDoc(xmldoc);
    }
};

#endif
