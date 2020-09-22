#ifndef FWD_TRACKER_CONFIG_H
#define FWD_TRACKER_CONFIG_H

#include "TXMLEngine.h"
#include "TString.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

// Class provides an interface for reading configuration from an XML file
class FwdTrackerConfig {
protected:

    static const std::string valDNE;// = std::string( "<DNE/>" );
    static const std::string pathDelim;
    static const std::string attrDelim;

    bool errorParsing = false;
    std::map<std::string, std::string> nodes;
    std::stringstream sstr; // reused for string to numeric conversion

    void mapFile(TXMLEngine &xml, XMLNodePointer_t node, Int_t level, std::string path = "") {
        using namespace std;
        // add the path delimeter above top level
        if ( "" != path) path += FwdTrackerConfig::pathDelim;

        // we skip the root node to maintain consistency with original XmlConfig
        if ( level > 1)
            path += xml.GetNodeName(node);

        // get the node name and content if it exists
        const string node_name = xml.GetNodeName(node);
        const string node_content = xml.GetNodeContent(node) != nullptr ? xml.GetNodeContent(node) : FwdTrackerConfig::valDNE;

        // be careful about repeated nodes
        if ( nodes.count( path ) == 0 ) {
            this->nodes[ path ] = node_content;
        } else { // add an array index if more than one
            path += TString::Format( "[%zu]", nodes.count( path ) ).Data();
            this->nodes[ path ] = node_content;
        }

        // loop through attributes of this node
        XMLAttrPointer_t attr = xml.GetFirstAttr(node);
        while (attr != 0) {

            // get the attribute name and value if exists
            const string attr_name = xml.GetAttrName(attr);
            const string attr_val = xml.GetAttrValue(attr) != nullptr ? xml.GetAttrValue(attr) : FwdTrackerConfig::valDNE;
            // save attributes with the ":" 
            this->nodes[ (path + FwdTrackerConfig::attrDelim + attr_name) ] = attr_val;
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

    // the copy and assignment are needed bc stringstream copy ctor is private;
    // copy ctor
    FwdTrackerConfig (const FwdTrackerConfig &cfg) {
        this->errorParsing = cfg.errorParsing;
        this->nodes = cfg.nodes;
        this->sstr.str(""); // this is a reused obj, no need to copy
    }

    // assignment 
    FwdTrackerConfig& operator=( const FwdTrackerConfig& cfg ) {
        this->errorParsing = cfg.errorParsing;
        this->nodes = cfg.nodes;
        this->sstr.str(""); // this is a reused obj, no need to copy
        return *this;
    }

    // sanitizes a path to its canonical form 
    static void canonize( std::string &path ){
        // remove whitespace
        path.erase(std::remove_if(path.begin(), path.end(), ::isspace), path.end());

        // removes "[0]" found in paths, so that the first element in a list can be accessed by index 0 or bare path
        size_t pos = path.find( "[0]" );

        // branchless version avoids using if to catch found or not
        size_t len = 3 * (pos != std::string::npos); // 3 if true, 0 if false
        pos = pos * (pos != std::string::npos); // pos if true, 0 if false
        path.erase( pos, len ); // does nothing if "[0]" not found
        return;
    }

    // dump config to a basic string representation - mostly for debugging
    std::string dump() {
        using namespace std;
        sstr.str("");
        sstr.clear();
        for ( auto kv : this->nodes ){
            sstr << "[" << kv.first << "] = " << kv.second << endl;
        }
        return sstr.str();
    }

    // Does a path exist
    // Either node or attribute - used to determine if default value is used
    bool exists( std::string path ){
        FwdTrackerConfig::canonize( path );
        if ( 0 == this->nodes.count( path ) )
            return false;

        if ( FwdTrackerConfig::valDNE == this->nodes[path] )
            return false;
        
        return true;
    }

    // generic conversion to type T from std::string
    // override this for special conversions
    template <typename T>
    T convert( std::string s ){
        T rv;
        sstr.str("");
        sstr.clear();
        sstr << s;
        sstr >> rv;
        return rv;
    }


    // template function for getting any type that can be converted from string via stringstream
    template <typename T>
    T get( std::string path, T dv )  {
        // return default value if path DNE
        if ( false == exists( path ) )
            return dv;
        FwdTrackerConfig::canonize( path );
        // convrt from string to type T and return
        return convert<T>( this->nodes[ path ] );
    }

    template <typename T>
    std::vector<T> getVector( std::string path, std::vector<T> dv ){
        if ( false == exists( path ) )
            return dv;
        
        FwdTrackerConfig::canonize( path );
        std::string val = this->nodes[ path ];
        // remove whitespace
        val.erase(std::remove_if(val.begin(), val.end(), ::isspace), val.end());
        std::vector<std::string> elems;

        // split the string by commas
        [&]() {
            std::stringstream  ss(val);
            std::string str;
            while (std::getline(ss, str, ',')) {
                elems.push_back(str);
            }
            return;
        }();

        // for each element, convert to type T and push into vector
        std::vector<T> result;
        for ( auto sv : elems ){
            result.push_back( convert<T>( sv ) );
        }
        return result;
    }

    std::vector<std::string> childrenOf( std::string path ){
        using namespace std;
        vector<string> result;

        // test a path to see if it is an attribute
        auto is_attribute = [&](string str){
            return ( str.find( FwdTrackerConfig::attrDelim ) != string::npos );
        };

        for ( auto kv : this->nodes ){

            // get the first n characters of this path
            string parent = (kv.first).substr( 0, path.length() );

            // dont add self as a child
            if ( parent == kv.first ) continue;

            // if parent path matches query path then it is a child.
            if ( parent == path && false == is_attribute( kv.first )){
                result.push_back( kv.first );
            }
        } // loop over all nodes

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

        // Create XML engine for parsing file
        TXMLEngine xml;

        // Now try to parse xml file
        XMLDocPointer_t xmldoc = xml.ParseFile(filename.c_str());
        if (!xmldoc) { // parse failed, TODO inform of error
            this->errorParsing = true;
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
