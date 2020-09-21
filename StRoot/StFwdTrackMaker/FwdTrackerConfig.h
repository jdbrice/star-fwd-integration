#ifndef FWD_TRACKER_CONFIG_H
#define FWD_TRACKER_CONFIG_H

#include "TXMLEngine.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

// Class provides an interface for reading configuration from an XML file
class FwdTrackerConfig {
protected:

    const std::string valDNE = std::string( "<DNE/>" );
    std::map<std::string, std::string> nodes;
    stringstream sstr; // reused for string to numeric conversion

    void mapFile(TXMLEngine &xml, XMLNodePointer_t node, Int_t level, string path = "") {
        using namespace std;
        // add the path delimeter above top level
        if ( "" != path) path += ".";

        // we skip the root node to maintain consistency with original XmlConfig
        if ( level > 1)
            path += xml.GetNodeName(node);

        // get the node name and content if it exists
        const string node_name = xml.GetNodeName(node);
        const string node_content = xml.GetNodeContent(node) != nullptr ? xml.GetNodeContent(node) : valDNE;

        cerr << "[ " << path << " ] count = " << nodes.count( path ) << endl;
        if ( nodes.count( path ) == 0 ) {
            nodes[ path ] = node_content;
        } else {
            cerr << "ERROR: path clash at : " << path << endl;
        }

        // loop through attributes of this node
        XMLAttrPointer_t attr = xml.GetFirstAttr(node);
        while (attr != 0) {

            // get the attribute name and value if exists
            const string attr_name = xml.GetAttrName(attr);
            const string attr_val = xml.GetAttrValue(attr) != nullptr ? xml.GetAttrValue(attr) : valDNE;

            nodes[ (path + ":" + attr_name) ] = attr_val;
            attr = xml.GetNextAttr(attr);
        }

        // recursively get child nodes
        XMLNodePointer_t child = xml.GetChild(node);
        while (child != 0) {
            mapFile(xml, child, level + 1, path);
            child = xml.GetNext(child);
        }
    } // mapFile
public:

    // dump config to a basic string representation - mostly for debugging
    void dump() {
        using namespace std;
        for ( auto kv : nodes ){
            cout << "[" << kv.first << "] = " << kv.second << endl;
        }
    }

    // Does a path exist
    // Either node or attribute - used to determine if default value is used
    bool exists( std::string path ){
        if ( 0 == nodes.count( path ) )
            return false;

        if ( valDNE == nodes[path] )
            return false;
        
        return true;
    }


    // template function for getting any type that can be converted from string via stringstream
    template <typename T>
    T get( std::string path, T dv = 0 )  {
        using namespace std;
        sstr.str("");
        T rv;
        if ( false == exists( path ) )
            return dv;
        sstr << nodes[ path ];
        sstr >> rv;
        return rv;
    }

    // template <>
    // bool get( string path, bool dv = false );


    std::vector<std::string> childrenOf( std::string path ){
        std::vector<std::string> result;

        auto is_attribute = [&](std::string str){
            return ( str.find( ":" ) != std::string::npos );
        };

        for ( auto kv : nodes ){

            // get the first n characters of this path
            string parent = (kv.first).substr( 0, path.length() );

            // dont add self as a child
            if ( parent == kv.first ) continue;

            // if parent path matches query path then it is a child.
            if ( parent == path && false == is_attribute( kv.first )){
                result.push_back( kv.first );
            }
        }
        return result;
    }

    // Constructor is noop, use load(...)
    FwdTrackerConfig() {}

    // constructor that immediately loads an xml file
    FwdTrackerConfig(std::string filename) {
        load( filename );
    }

    // Main setup routine.
    // Loads the given XML file and maps it
    void load( std::string filename ) {
        using namespace std;

        // empty the map of nodes
        nodes.clear();

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